#!/usr/bin/env python3
"""test_dap.py — live-MAME verifications for drmon-dap (Phase 3, V3–V6).

Unlike test_symbols.py (symbol-loading only, no MAME), this drives the full
DAP↔MAME round-trip against a *running* headless MAME + Lua bridge:

    test_dap.py  --(DAP stdio)-->  drmon-dap-snes  --(TCP :41816)-->  MAME + mame_bridge.lua

Launched by test_dap.sh, which brings MAME up headless on the host and runs this
inside the build container (--network=host so the in-container DAP reaches the
host bridge). The reference ROM is test-roms/drmon-test.sfc — a NOP sled
$8000–$FFBB that JML $008000-loops forever, so an instruction breakpoint set
anywhere in the sled is reached within one pass.

Covers the previously-unrecorded live-MAME items:
  V3  attach → connected session (data round-trips to MAME)
  V4  instruction breakpoint fires; PC lands at the breakpoint address
  V5  variables (Registers scope) read back; PCL == breakpoint; cross-checked
      against a direct bridge `G`
  V6  readMemory matches the ROM (NOP sled = 0xEA); cross-checked against bridge `R`
"""

import base64
import json
import os
import queue
import socket
import subprocess
import sys
import threading
import time

BINARY  = os.environ.get("DRMON_DAP_SNES", "/build/drmon-dap-snes")
ADDR    = os.environ.get("DRMON_MAME_ADDR", "127.0.0.1:41816")
HOST, PORT = (ADDR.rsplit(":", 1) + ["41816"])[:2]
PORT = int(PORT)

# A NOP-sled address well inside drmon-test.sfc ($8000–$FFBB). Reached every loop.
BP_ADDR = 0x8100
ROM_PROBE_ADDR = 0x8000   # start of the sled — 16 bytes here are all 0xEA (NOP)

PASS = 0
FAIL = 0

def check(label, ok, detail=""):
    global PASS, FAIL
    if ok:
        PASS += 1
        print(f"  PASS: {label}")
    else:
        FAIL += 1
        print(f"  FAIL: {label}" + (f"  ({detail})" if detail else ""))
    return ok

# ---------------------------------------------------------------------------
# DAP transport: a background reader thread frames messages onto a queue so the
# main thread can wait for responses *or* async events with a real timeout.
# ---------------------------------------------------------------------------

class Dap:
    def __init__(self, args):
        # Capture stderr to a file (readable on the host at /tmp/drmon-build/) so a
        # crash or bridge error in the adapter is diagnosable, not silently swallowed.
        self._errf = open(os.environ.get("DRMON_DAP_STDERR", "/build/dap-stderr.log"), "wb")
        self.proc = subprocess.Popen(
            [BINARY] + args,
            stdin=subprocess.PIPE, stdout=subprocess.PIPE,
            stderr=self._errf,
        )
        self.q = queue.Queue()
        self.seq = 1
        self._reader = threading.Thread(target=self._read_loop, daemon=True)
        self._reader.start()

    def _read_loop(self):
        f = self.proc.stdout
        try:
            while True:
                hdr = b""
                while b"\r\n\r\n" not in hdr:
                    ch = f.read(1)
                    if not ch:
                        return
                    hdr += ch
                length = int(hdr.split(b"Content-Length:")[1].split(b"\r\n")[0])
                body = f.read(length)
                self.q.put(json.loads(body))
        except Exception:
            return

    def send(self, method, params=None):
        # Always include "arguments" (even when empty): cppdap requires the field
        # for requests like attach/configurationDone and silently drops the request
        # — no response — if it is missing.
        msg = {"seq": self.seq, "type": "request", "command": method,
               "arguments": params if params is not None else {}}
        self.seq += 1
        body = json.dumps(msg).encode()
        self.proc.stdin.write(f"Content-Length: {len(body)}\r\n\r\n".encode() + body)
        self.proc.stdin.flush()

    def wait(self, predicate, timeout=12.0):
        """Drain queued messages until predicate(msg) is true; return that msg."""
        deadline = time.monotonic() + timeout
        while True:
            remaining = deadline - time.monotonic()
            if remaining <= 0:
                raise TimeoutError("no matching DAP message in time")
            try:
                msg = self.q.get(timeout=remaining)
            except queue.Empty:
                raise TimeoutError("no matching DAP message in time")
            if predicate(msg):
                return msg

    def request(self, method, params=None, timeout=12.0):
        """Send a request and return its matching response (events are skipped)."""
        self.send(method, params)
        return self.wait(lambda m: m.get("type") == "response"
                         and m.get("command") == method, timeout)

    def close(self):
        try:
            self.send("disconnect")
            time.sleep(0.2)
        except Exception:
            pass
        self.proc.terminate()
        self.proc.wait()

# ---------------------------------------------------------------------------
# Direct bridge client (independent source of truth for V5/V6 cross-checks).
# ---------------------------------------------------------------------------

class Bridge:
    def __init__(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.settimeout(5.0)
        self.buf = b""

    def connect(self, retries=20, delay=0.5):
        for attempt in range(retries):
            try:
                self.sock.connect((HOST, PORT)); return
            except OSError:
                if attempt < retries - 1:
                    time.sleep(delay)
        raise RuntimeError(f"cannot connect bridge {HOST}:{PORT}")

    def cmd(self, c, timeout=5.0):
        self.sock.sendall((c + "\n").encode())
        deadline = time.monotonic() + timeout
        while b"\n" not in self.buf:
            self.sock.settimeout(max(0.01, deadline - time.monotonic()))
            chunk = self.sock.recv(4096)
            if not chunk:
                raise EOFError("bridge closed")
            self.buf += chunk
        line, _, self.buf = self.buf.partition(b"\n")
        return line.decode().strip()

    def close(self):
        try: self.sock.sendall(b"BYE\n")
        except Exception: pass
        self.sock.close()

# ---------------------------------------------------------------------------

def regs_from_variables(dap):
    """variables(ref=1) → {NAME: intvalue}."""
    resp = dap.request("variables", {"variablesReference": 1})
    out = {}
    for v in resp["body"]["variables"]:
        try:
            out[v["name"]] = int(v["value"], 0)
        except (ValueError, KeyError):
            out[v["name"]] = v.get("value")
    return out

def run_phasec(elf):
    """Phase C — end-to-end DWARF round-trip: MAME runs a16local.sfc; the DAP loads
    the matching debug ELF and sets a SOURCE breakpoint on a16local.c:17 (the for(;;)
    loop, where the CPU spins forever). The breakpoint must resolve via DWARF to the
    line's address, fire, and stop the CPU at that PC — proving compile→DWARF→drmon→
    live-MAME source debugging on fully-open tooling."""
    # Use an ABSOLUTE path like a real editor sends, and mimic VS Code's live-attach
    # order exactly: attach -> configurationDone -> set the breakpoint, with NO
    # explicit continue. This exercises the two adapter fixes the GUI run surfaced:
    #   - configurationDone issues the bridge "go" (else nothing fires on attach);
    #   - stackTrace maps the stopped PC back to source file:line for the highlight.
    SRC, LINE, WANT = "/anywhere/on/disk/a16local.c", 17, 0x8074
    dap = Dap(["--host", HOST, "--port", str(PORT), "--symbols", elf])
    dap_pc = None
    try:
        check("initialize succeeds", dap.request("initialize",
              {"clientID": "test", "adapterID": "drmon"}).get("success"))
        check("attach succeeds", dap.request("attach", {}).get("success"))
        dap.request("configurationDone", {})

        # Source breakpoint resolved from the loaded DWARF line table (absolute path
        # → basename match), set live AFTER configurationDone — the user's flow.
        sb = dap.request("setBreakpoints",
                         {"source": {"path": SRC}, "breakpoints": [{"line": LINE}]})
        b = sb["body"]["breakpoints"][0]
        ref = b.get("instructionReference", "")
        check(f"PhaseC source bp (abs path):{LINE} resolves via DWARF to {hex(WANT)}",
              b.get("verified") and ref and int(ref, 0) == WANT, f"bp={b}")

        # It must fire live in MAME with NO explicit continue (the #5 fix): the
        # configurationDone "go" keeps the CPU running into the just-armed breakpoint.
        try:
            dap.wait(lambda m: m.get("type") == "event" and m.get("event") == "stopped"
                     and m.get("body", {}).get("reason") == "breakpoint", timeout=15.0)
            check("PhaseC breakpoint fires (no explicit continue — attach flow)", True)
        except TimeoutError as e:
            check("PhaseC breakpoint fires (no explicit continue — attach flow)", False, str(e))

        st = dap.request("stackTrace", {"threadId": 1})
        fr = st["body"]["stackFrames"][0]
        ip = fr.get("instructionPointerReference", "")
        dap_pc = int(ip, 0) if ip else -1
        check(f"PhaseC PC at/just past {hex(WANT)} (DWARF-mapped source line)",
              WANT <= dap_pc <= WANT + 16, f"ip={ip!r}")
        # #6: stackTrace carries source + line so the editor highlights it, and the
        # path is the exact one the editor used (so it matches the open document).
        src = fr.get("source") or {}
        check("PhaseC stackTrace maps PC -> source line 17",
              fr.get("line") == LINE, f"line={fr.get('line')} source={src}")
        check("PhaseC stackTrace source.path == the editor's path",
              src.get("path") == SRC, f"source={src}")
    finally:
        dap.close()

    # Independent confirmation: a direct bridge read reports the same stopped PC.
    try:
        br = Bridge(); br.connect()
        br.cmd("REGS A,X,Y,FLAGS,EMUL,D,DB,PB,SP,PC,PCL")
        g = br.cmd("G").split()
        bpcl = int(g[10], 16) if len(g) == 11 else None
        check("PhaseC cross-check: bridge PCL == drmon PC",
              bpcl is not None and bpcl == dap_pc, f"bridge={bpcl} drmon={dap_pc}")
        br.close()
    except Exception as e:
        check("PhaseC bridge cross-check", False, f"bridge error: {e}")

    print(f"\nResults: {PASS} passed, {FAIL} failed")
    sys.exit(0 if FAIL == 0 else 1)

def main():
    if not os.path.exists(BINARY):
        print(f"FAIL  {BINARY} not found (run: task build)"); sys.exit(1)

    # Phase C mode: DRMON_DAP_SYMBOLS points at the debug ELF (set by test_dap.sh phasec).
    elf = os.environ.get("DRMON_DAP_SYMBOLS", "")
    if elf:
        run_phasec(elf)
        return

    dap = Dap(["--host", HOST, "--port", str(PORT)])
    dap_pcl = None
    dap_mem = None
    try:
        # initialize → expect InitializedEvent + capabilities
        init = dap.request("initialize", {"clientID": "test", "adapterID": "drmon"})
        check("initialize succeeds", init.get("success"), str(init))

        # ---- V3: attach → connected session -------------------------------
        att = dap.request("attach", {})
        check("V3 attach succeeds", att.get("success"), str(att))
        dap.request("configurationDone", {})

        # A successful instruction-breakpoint round-trip proves the DAP is
        # actually talking to MAME (B+ goes over the bridge).
        sib = dap.request("setInstructionBreakpoints",
                          {"breakpoints": [{"instructionReference": hex(BP_ADDR)}]})
        bps = sib["body"]["breakpoints"]
        check("V3 connected: instruction breakpoint accepted",
              sib.get("success") and bps and bps[0].get("verified"), str(sib))
        check("V3 breakpoint reports its address",
              bps and bps[0].get("instructionReference", "").lower() == hex(BP_ADDR),
              str(bps))

        # ---- V4: breakpoint fires -----------------------------------------
        dap.send("continue", {"threadId": 1})
        try:
            ev = dap.wait(lambda m: m.get("type") == "event"
                          and m.get("event") == "stopped"
                          and m.get("body", {}).get("reason") == "breakpoint",
                          timeout=15.0)
            check("V4 StoppedEvent(reason=breakpoint) fires", True)
        except TimeoutError as e:
            check("V4 StoppedEvent(reason=breakpoint) fires", False, str(e))
            ev = None

        # PC at the stop, via stackTrace.instructionPointerReference.
        # The Tier-1 Lua bridge (-debugger none) pseudo-holds some NOPs PAST the
        # breakpoint: its stop is marker-detected on a later periodic tick, by which
        # point the free-running CPU has stepped on. Since configurationDone now arms
        # the bridge "go" before the bp (so attach-flow breakpoints fire at all), the
        # sled CPU is already running when the bp arms, widening the hold to ~a couple
        # dozen NOPs (the tight for(;;) loop in phasec still lands within a few). The
        # Tier-2 C++ gdbstub freezes exactly. So assert AT-or-past, still in the sled.
        SLED_HOLD = 64
        st = dap.request("stackTrace", {"threadId": 1})
        ipref = st["body"]["stackFrames"][0].get("instructionPointerReference", "")
        pc = int(ipref, 0) if ipref else -1
        check(f"V4 PC at/just past breakpoint {hex(BP_ADDR)} (Tier-1 bridge holds some NOPs past)",
              BP_ADDR <= pc <= BP_ADDR + SLED_HOLD, f"ipref={ipref!r}")

        # ---- V5: registers -------------------------------------------------
        regs = regs_from_variables(dap)
        want = {"A","X","Y","FLAGS","EMUL","D","DB","PB","SP","PC","PCL"}
        check("V5 Registers scope has all 11 SNES registers",
              want.issubset(set(regs.keys())), f"got {sorted(regs.keys())}")
        dap_pcl = regs.get("PCL")
        check(f"V5 PCL at/just past breakpoint {hex(BP_ADDR)} (matches stackTrace PC)",
              isinstance(dap_pcl, int) and BP_ADDR <= dap_pcl <= BP_ADDR + SLED_HOLD,
              f"PCL={dap_pcl}")

        # ---- V6: memory ----------------------------------------------------
        rm = dap.request("readMemory", {"memoryReference": hex(ROM_PROBE_ADDR), "count": 16})
        dap_mem = base64.b64decode(rm["body"]["data"])
        check("V6 readMemory($8000,16) == NOP sled (0xEA×16)",
              dap_mem == b"\xEA" * 16, dap_mem.hex())
    finally:
        dap.close()

    # ---- V5/V6 independent cross-check via the bridge (MAME still halted) ---
    # After disconnect, MAME stays stopped at the breakpoint, so a fresh bridge
    # connection must report the *same* PC and the *same* ROM bytes drmon read.
    try:
        b = Bridge(); b.connect()
        b.cmd("REGS A,X,Y,FLAGS,EMUL,D,DB,PB,SP,PC,PCL")
        g = b.cmd("G").split()
        bridge_pcl = int(g[10], 16) if len(g) == 11 else None
        check("V5 cross-check: bridge PCL == drmon PCL",
              bridge_pcl is not None and bridge_pcl == dap_pcl,
              f"bridge={bridge_pcl} drmon={dap_pcl}")
        rmem = b.cmd(f"R {ROM_PROBE_ADDR:x} 10")
        check("V6 cross-check: bridge R($8000,16) == drmon readMemory",
              bytes.fromhex(rmem) == dap_mem, f"bridge={rmem}")
        b.close()
    except Exception as e:
        check("V5/V6 bridge cross-check", False, f"bridge error: {e}")

    print(f"\nResults: {PASS} passed, {FAIL} failed")
    sys.exit(0 if FAIL == 0 else 1)

if __name__ == "__main__":
    main()
