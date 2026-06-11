#!/usr/bin/env python3
"""test_bridge.py — protocol-level tests for the drmon MAME TCP bridge (SNES).

Asserts the full command set against a running mame_bridge.lua server.
Expects a MAME instance with the bridge already listening; launched by test_bridge.sh.

Usage: python3 linux/test_bridge.py [snes]
"""

import os
import socket
import sys
import time

SYS = sys.argv[1] if len(sys.argv) > 1 else "snes"

addr_env = os.environ.get("DRMON_MAME_ADDR", "127.0.0.1:41816")
host, port_str = (addr_env.rsplit(":", 1) + ["41816"])[:2]
PORT = int(port_str)
HOST = host

SNES_REGS = ["A", "X", "Y", "FLAGS", "EMUL", "D", "DB", "PB", "SP", "PC", "PCL"]
REGS = SNES_REGS

# SNES WRAM starts at bus address 0x7E0000.
WRAM_ADDR = 0x7E0000

PASS = 0
FAIL = 0


class Bridge:
    def __init__(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.settimeout(5.0)
        self.buf = b""

    def connect(self, retries=10, delay=0.5):
        for attempt in range(retries):
            try:
                self.sock.connect((HOST, PORT))
                return
            except OSError:
                if attempt < retries - 1:
                    time.sleep(delay)
        raise RuntimeError(f"Could not connect to {HOST}:{PORT} after {retries} attempts")

    def send(self, cmd):
        self.sock.sendall((cmd + "\n").encode())

    def recv_line(self, timeout=5.0):
        deadline = time.monotonic() + timeout
        while b"\n" not in self.buf:
            remaining = deadline - time.monotonic()
            if remaining <= 0:
                raise TimeoutError(f"No response within {timeout} s (buf={self.buf!r})")
            self.sock.settimeout(remaining)
            chunk = self.sock.recv(4096)
            if not chunk:
                raise EOFError("Connection closed by server")
            self.buf += chunk
        line, _, self.buf = self.buf.partition(b"\n")
        return line.decode().strip()

    def cmd(self, c, timeout=5.0):
        self.send(c)
        return self.recv_line(timeout)

    def close(self):
        try:
            self.send("BYE")
        except Exception:
            pass
        self.sock.close()


def check(label, got, pred):
    global PASS, FAIL
    ok = pred(got)
    status = "PASS" if ok else "FAIL"
    if ok:
        PASS += 1
    else:
        FAIL += 1
        print(f"  {status}: {label}")
        print(f"         got: {got!r}")
    if ok:
        print(f"  {status}: {label}")
    return ok


def run():
    b = Bridge()
    print(f"Connecting to {HOST}:{PORT} ...")
    b.connect()
    print(f"Connected.  SYS={SYS}")

    # V — version handshake
    r = b.cmd("V")
    check("V returns ok drmon-bridge 1 <cpu>", r, lambda s: s.startswith("ok drmon-bridge 1 "))
    shortname = r.split()[-1] if r.startswith("ok") else "unknown"
    check("V cpu shortname is 5a22", shortname, lambda s: s == "5a22")

    # REGS — announce register order
    r = b.cmd("REGS " + ",".join(REGS))
    check("REGS returns ok", r, lambda s: s == "ok")

    # G — get registers
    r = b.cmd("G")
    reg_vals = r.split()
    check(f"G returns {len(REGS)} hex values", r,
          lambda s: len(s.split()) == len(REGS) and all(
              all(c in "0123456789abcdefABCDEF" for c in v) for v in s.split()))

    # P — round-trip: write back the same values we just read
    r = b.cmd("P " + " ".join(reg_vals if len(reg_vals) == len(REGS)
                               else ["0"] * len(REGS)))
    check("P returns ok", r, lambda s: s == "ok")

    # H — halt before memory tests
    r = b.cmd("H")
    check("H returns stopped ...", r, lambda s: s.startswith("stopped "))

    # ? — verify stopped
    r = b.cmd("?")
    check("? returns stopped after H", r, lambda s: s.startswith("stopped "))

    # R — memory read (16 bytes from WRAM)
    r = b.cmd(f"R {WRAM_ADDR:x} 10")
    check("R returns 32 hex chars (16 bytes)", r,
          lambda s: len(s) == 32 and all(c in "0123456789abcdefABCDEF" for c in s))

    # W + R round-trip
    pattern = "deadbeef"
    r = b.cmd(f"W {WRAM_ADDR:x} {pattern}")
    check("W returns ok", r, lambda s: s == "ok")
    r = b.cmd(f"R {WRAM_ADDR:x} 4")
    check(f"R returns written pattern {pattern}", r,
          lambda s: s.lower() == pattern.lower())

    # B+ / B- — breakpoint set/clear
    BP_ADDR = 0x8000
    r = b.cmd(f"B+ {BP_ADDR:x}")
    check("B+ returns ok", r, lambda s: s == "ok")
    r = b.cmd(f"B- {BP_ADDR:x}")
    check("B- returns ok", r, lambda s: s == "ok")

    # S <next> — single step.
    # Machine is halted; get current PCL so we can send the correct next-PC.
    pcl_idx = REGS.index("PCL")
    current_pcl = int(reg_vals[pcl_idx], 16) if len(reg_vals) == len(REGS) else 0
    # PC+1 is a safe fall-through candidate (NOP sled); bridge will stop there.
    next_pc = (current_pcl + 1) & 0xFFFFFF
    r = b.cmd(f"S {next_pc:x}", timeout=3.0)
    step_ok = check("S <next> returns stopped ... step", r,
                    lambda s: s.startswith("stopped ") and "step" in s)

    if step_ok:
        # Parse PC from stop reply and verify it advanced
        parts = r.split()
        stopped_pc = int(parts[1], 16) if len(parts) >= 2 else 0
        check("S advanced PC by at least 1 byte", stopped_pc,
              lambda v: v != current_pcl)

    # H — re-halt before bp-while-running test
    b.cmd("H")

    # B+ at a known address ahead of current PC, C to continue, poll ? until bp fires.
    # Use a ROM address that the NOP sled will reach.
    r = b.cmd("?")
    parts = r.split()
    pc_now = int(parts[1], 16) if len(parts) >= 2 else 0x8000
    bp_ahead = (pc_now + 4) & 0xFFFFFF
    r = b.cmd(f"B+ {bp_ahead:x}")
    check("B+ ahead returns ok", r, lambda s: s == "ok")

    r = b.cmd("C")
    check("C returns ok", r, lambda s: s == "ok")

    # Poll ? up to 2 s for bp hit
    bp_hit = False
    for _ in range(40):
        time.sleep(0.05)
        r = b.cmd("?")
        if r.startswith("stopped") and "bp" in r:
            bp_hit = True
            break
    check("bp fires after C (stopped ... bp)", bp_hit, lambda v: v)

    r = b.cmd(f"B- {bp_ahead:x}")
    check("B- ahead returns ok", r, lambda s: s == "ok")

    # C — continue again then RESET
    b.cmd("C")

    # RESET — soft reset
    r = b.cmd("RESET")
    check("RESET returns ok", r, lambda s: s == "ok")

    b.close()

    print(f"\nResults: {PASS} passed, {FAIL} failed")
    return FAIL == 0


if not run():
    sys.exit(1)
