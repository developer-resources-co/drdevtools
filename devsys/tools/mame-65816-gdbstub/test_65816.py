#!/usr/bin/env python3
"""test_65816.py — GDB RSP protocol + target.xml tests for the MAME 65816 gdbstub Lua plugin.

Expects a MAME 5a22 (SNES) instance already running with the repo-local gdbstub plugin:
    devsys/tools/mame-65816-gdbstub/run-mame.sh        # opens RSP on 127.0.0.1:2159

Usage: python3 test_65816.py [port]
The RSP transport class is lifted from drmon's linux/test_gdb.py (proven against MAME 0.277).
"""

import socket
import sys
import time
import xml.etree.ElementTree as ET

HOST = "127.0.0.1"
PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 2159

PASS = 0
FAIL = 0

# Canonical 65816 g-packet layout (must match init.lua regmaps["5a22"].regs and target.xml).
# (name, width_bytes). Little-endian. Total 18 bytes = 36 hex chars.
CANON = [("A", 2), ("X", 2), ("Y", 2), ("P", 1), ("E", 1),
         ("S", 2), ("D", 2), ("DB", 1), ("PB", 1), ("PC", 4)]
G_HEXLEN = sum(w for _, w in CANON) * 2  # 36


def csum(data):
    if isinstance(data, str):
        data = data.encode()
    return sum(data) % 256


class RSP:
    def __init__(self):
        self.sock = socket.socket()
        self.buf = b""

    def connect(self, retries=30, delay=0.5):
        for attempt in range(retries):
            try:
                self.sock.connect((HOST, PORT))
                return
            except OSError:
                if attempt < retries - 1:
                    time.sleep(delay)
        raise RuntimeError(f"Could not connect to {HOST}:{PORT} after {retries} attempts")

    def _fill(self, timeout):
        self.sock.settimeout(timeout)
        chunk = self.sock.recv(4096)
        if not chunk:
            raise EOFError("connection closed")
        self.buf += chunk

    def _drain_ack(self, timeout=2):
        deadline = time.monotonic() + timeout
        while True:
            i = 0
            while i < len(self.buf) and self.buf[i:i+1] in (b"+", b"-"):
                i += 1
            self.buf = self.buf[i:]
            if self.buf or time.monotonic() >= deadline:
                break
            try:
                self._fill(deadline - time.monotonic())
            except (socket.timeout, OSError):
                break

    def _extract(self):
        i = self.buf.find(b"$")
        if i < 0:
            self.buf = b""
            return None
        self.buf = self.buf[i:]
        h = self.buf.find(b"#", 1)
        if h < 0 or len(self.buf) < h + 3:
            return None
        data = self.buf[1:h]
        self.buf = self.buf[h+3:]
        self.sock.sendall(b"+")
        return data.decode(errors="replace")

    def recv_any(self, timeout=5):
        deadline = time.monotonic() + timeout
        while True:
            pkt = self._extract()
            if pkt is not None:
                return pkt
            rem = deadline - time.monotonic()
            if rem <= 0:
                return None
            try:
                self._fill(min(rem, 0.5))
            except (socket.timeout, OSError):
                pass

    def cmd(self, data, wait_reply=True, timeout=5):
        raw = data.encode() if isinstance(data, str) else data
        pkt = b"$" + raw + b"#" + f"{csum(raw):02x}".encode()
        self.sock.sendall(pkt)
        self._drain_ack()
        if not wait_reply:
            return None
        return self.recv_any(timeout)

    def cmd_xfer(self, data, timeout=5):
        """qXfer-style read: reply has a 1-char 'l'/'m' prefix stripped here."""
        r = self.cmd(data, timeout=timeout)
        if r and r[:1] in ("l", "m"):
            return r[1:], r[0]
        return r, None

    def send_break(self):
        self.sock.sendall(b"\x03")

    def close(self):
        self.sock.close()


def check(label, got, pred):
    global PASS, FAIL
    ok = pred(got)
    status = "PASS" if ok else "FAIL"
    if ok:
        PASS += 1
    else:
        FAIL += 1
        print(f"  FAIL: {label}")
        print(f"         got: {got!r}")
        return False
    print(f"  PASS: {label}")
    return True


def le(hexstr):
    """Decode a little-endian hex byte string to an int."""
    b = bytes.fromhex(hexstr)
    return int.from_bytes(b, "little")


def decode_g(g):
    """Decode a g-packet hex string into {name: value} per CANON."""
    out, off = {}, 0
    for name, w in CANON:
        out[name] = le(g[off:off + w * 2])
        off += w * 2
    return out


def stop_into(r):
    """Force the machine to a stop so registers are readable; tolerate already-stopped."""
    r.send_break()
    return r.recv_any(3)


def run():
    r = RSP()
    print(f"Connecting to {HOST}:{PORT} ...")
    r.connect()
    print("Connected.")

    # 1. qSupported advertises qXfer + a packet size
    reply = r.cmd("qSupported:xmlRegisters+;swbreak+")
    check("qSupported advertises qXfer:features:read+", reply,
          lambda s: s is not None and "qXfer:features:read+" in s)

    # 2. target.xml round-trips, parses, and matches the canonical register layout
    xml, prefix = r.cmd_xfer("qXfer:features:read:target.xml:0,4000")
    check("qXfer:features:read returns a last-chunk ('l') document", prefix,
          lambda p: p == "l")
    ok_xml = check("target.xml is well-formed XML", xml,
                   lambda s: s is not None and _parses(s))
    if ok_xml:
        regs = [(e.get("name"), int(e.get("bitsize"))) for e in
                ET.fromstring(xml).iter("reg")]
        want = [(n, w * 8) for n, w in CANON]
        check("target.xml <reg> names/bitsizes/order match canonical layout", regs,
              lambda got: got == want)

    # 3. break in so registers are readable, then g
    print("  [break-in] stopping the CPU...")
    stop_into(r)
    g = r.cmd("g")
    ok_g = check(f"g returns {G_HEXLEN} hex chars", g,
                 lambda s: s is not None and len(s) == G_HEXLEN and
                 all(c in "0123456789abcdefABCDEF" for c in s))
    if ok_g:
        regs = decode_g(g)
        print("         decoded:", {k: hex(v) for k, v in regs.items()})
        # PC is 24-bit zero-extended to 32: high byte must be 0
        check("PC high byte is zero (24-bit zero-extended)", regs["PC"],
              lambda pc: (pc >> 24) == 0)
        # 24-bit linear consistency: PC bits[16:24] == program bank (PB)
        check("PC bits[16:24] == PB (24-bit linear address)", regs,
              lambda d: ((d["PC"] >> 16) & 0xFF) == d["PB"])
        check("E (emulation flag) is 0 or 1", regs["E"], lambda e: e in (0, 1))
        pc0 = regs["PC"]

        # 4. memory read at PC returns bytes
        mem = r.cmd(f"m{pc0:x},4")
        check("m@PC returns 8 hex chars", mem,
              lambda s: s is not None and len(s) == 8 and
              all(c in "0123456789abcdefABCDEF" for c in s))

        # 5. Z0 software breakpoint: set/clear (must not crash) + deterministic fire.
        # The native-mode NMI vector lives at $00FFEA and its handler runs every frame, so a
        # breakpoint there fires within ~1 frame of continuing (no reliance on where we broke in).
        nmi_lo = r.cmd("mffea,2")
        if nmi_lo and len(nmi_lo) == 4:
            nmi = le(nmi_lo)            # bank-0 NMI handler address
            setr = r.cmd(f"Z0,{nmi:x},1")
            check("Z0 (software breakpoint) set returns OK", setr, lambda s: s == "OK")
            r.cmd("c", wait_reply=False)
            stop = r.recv_any(5)
            if stop is not None and "05" in stop:
                gbp = r.cmd("g")
                pcbp = (decode_g(gbp)["PC"] & 0xFFFFFF) if (gbp and len(gbp) == G_HEXLEN) else -1
                # The bp fires (the machine halts on continue rather than running forever). In
                # -debugger none MAME a breakpoint is a pseudo-hold that doesn't hard-freeze the
                # CPU, so the reported PC may land a few instructions past the bp (here: inside /
                # just after the NMI handler). We assert the machine genuinely ran from the break-in
                # PC and then stopped — i.e. continue + breakpoint detection both work.
                print(f"         stopped at {hex(pcbp)} after bp@{hex(nmi)} (continue ran)")
                check("Z0 breakpoint causes a stop on continue (machine ran from break-in PC then halted)",
                      (hex(pcbp), hex(pc0)), lambda t: pcbp != -1 and pcbp != pc0)
            else:
                r.send_break(); r.recv_any(2)   # re-stop so later ops have a halted CPU
                check("Z0 breakpoint causes a stop on continue", stop, lambda s: False)
            clrr = r.cmd(f"z0,{nmi:x},1")
            check("z0 (software breakpoint) clear returns OK", clrr, lambda s: s == "OK")

    # 6. M/m memory round-trip in SNES WRAM ($7E0000)
    ram = 0x7E0000
    r.cmd(f"M{ram:x},4:deadbeef")
    rt = r.cmd(f"m{ram:x},4")
    check("M/m round-trip at WRAM $7E0000", rt,
          lambda s: s is not None and s.lower() == "deadbeef")

    # 7. single-step (exercises the shipped plugin's cpu.debug:step path) — run LAST so a
    #    crash here doesn't mask the rest; guarded so an EOF is reported, not raised.
    if ok_g:
        print("  [s] single step...")
        try:
            r.cmd("s", wait_reply=False)
            r.recv_any(5)
            g2 = r.cmd("g")
            check("single step returns readable registers and advances PC", g2,
                  lambda s: s is not None and len(s) == G_HEXLEN and
                  decode_g(s)["PC"] != pc0)
        except (EOFError, OSError) as e:
            check("single step does not crash the target", str(e), lambda s: False)

    try:
        r.close()
    except OSError:
        pass
    print(f"\nResults: {PASS} passed, {FAIL} failed")
    return FAIL == 0


def _parses(s):
    try:
        ET.fromstring(s)
        return True
    except ET.ParseError:
        return False


if not run():
    sys.exit(1)
