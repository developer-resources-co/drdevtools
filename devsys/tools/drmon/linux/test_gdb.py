#!/usr/bin/env python3
"""test_gdb.py — GDB RSP protocol tests for drmon Genesis (MAME -debugger gdbstub).

Expects a MAME genesis instance already running with:
    mame genesis -cart drmon-test.md -debug -debugger gdbstub -debugger_port 23946

Usage: python3 linux/test_gdb.py [port]
Launched automatically by test_bridge.sh SYS=gen.
"""

import os
import socket
import sys
import time
import re

HOST = "127.0.0.1"
PORT = int(sys.argv[1]) if len(sys.argv) > 1 else int(
    (os.environ.get("DRMON_GDB_ADDR", "127.0.0.1:23946").rsplit(":", 1) + ["23946"])[1])

PASS = 0
FAIL = 0

# m68k GDB RSP register layout (MAME gdbstub, confirmed by spike):
#   D0-D7: 4B BE each, hex offsets 0-63
#   A0-A7: 4B BE each, hex offsets 64-127
#   SR:    2B BE,      hex offsets 128-131
#   PC:    4B BE,      hex offsets 132-139
#   Total: 140 hex chars
G_HEXLEN = 140
PC_HEX_OFF = 132   # byte offset in g-packet hex string for PC


def csum(data):
    if isinstance(data, str):
        data = data.encode()
    return sum(data) % 256


class RSP:
    def __init__(self):
        self.sock = socket.socket()
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
        print(f"  {status}: {label}")
        print(f"         got: {got!r}")
    if ok:
        print(f"  {status}: {label}")
    return ok


def be32(hex4):
    """Parse a big-endian 32-bit value from 8 hex chars."""
    return int(hex4, 16)


def run():
    r = RSP()
    print(f"Connecting to {HOST}:{PORT} ...")
    r.connect()
    print("Connected.")

    # Drain initial stop notification (MAME sends T05 on connect under -debug)
    init = r.recv_any(2)
    check("Initial notification present (T05 or None)", init,
          lambda s: s is None or "05" in (s or ""))

    # qSupported
    reply = r.cmd("qSupported:xmlRegisters+;swbreak+")
    check("qSupported returns non-empty reply", reply,
          lambda s: s is not None and len(s) > 0)

    # g — register dump
    g = r.cmd("g")
    check(f"g returns {G_HEXLEN} hex chars", g,
          lambda s: s is not None and len(s) == G_HEXLEN and
          all(c in "0123456789abcdefABCDEF" for c in s))

    if g and len(g) == G_HEXLEN:
        pc_at_reset = be32(g[PC_HEX_OFF:PC_HEX_OFF+8])
        print(f"  PC at reset: 0x{pc_at_reset:08x} (expected 0x200)")
        check("PC at reset is 0x200", pc_at_reset, lambda v: v == 0x200)
    else:
        pc_at_reset = 0x200

    # m — memory read (16 bytes from ROM at 0x200)
    mem = r.cmd("m200,10")
    check("m200,10 returns 32 hex chars", mem,
          lambda s: s is not None and len(s) == 32 and
          all(c in "0123456789abcdefABCDEF" for c in s))

    # Z0 — set software breakpoint at $202 (second NOP in the sled)
    reply = r.cmd("Z0,202,2")
    check("Z0,202,2 returns OK", reply, lambda s: s == "OK")

    # c — continue until bp fires
    print("  [c] continuing to bp at 0x202...")
    r.cmd("c", wait_reply=False)
    stop = r.recv_any(5)
    check("stop reply after c (T05)", stop,
          lambda s: s is not None and "T05" in (s or "").upper() or
          (s is not None and s.startswith("T") and "05" in s[:3]))

    if stop:
        g2 = r.cmd("g")
        if g2 and len(g2) == G_HEXLEN:
            pc_at_bp = be32(g2[PC_HEX_OFF:PC_HEX_OFF+8])
            check("PC at bp is 0x202", pc_at_bp, lambda v: v == 0x202)

    # s — single step from 0x202 (NOP = 4e71 = 2 bytes → PC should → 0x204)
    print("  [s] single step from 0x202...")
    r.cmd("s", wait_reply=False)
    stop2 = r.recv_any(5)
    check("stop reply after s", stop2, lambda s: s is not None)

    g3 = r.cmd("g")
    if g3 and len(g3) == G_HEXLEN:
        pc_after_step = be32(g3[PC_HEX_OFF:PC_HEX_OFF+8])
        check("s advances exactly 2 bytes (NOP=4e71)", pc_after_step,
              lambda v: v == 0x204)

    # z0 — clear breakpoint
    reply = r.cmd("z0,202,2")
    check("z0,202,2 returns OK", reply, lambda s: s == "OK")

    # M — memory write round-trip in Genesis RAM (0xFF0000)
    r.cmd("c", wait_reply=False)   # let it run briefly
    time.sleep(0.05)
    r.send_break()
    r.recv_any(3)

    ram_addr = 0xFF0000
    r.cmd(f"M{ram_addr:x},4:deadbeef")
    mem2 = r.cmd(f"m{ram_addr:x},4")
    check("M/m round-trip at Genesis RAM", mem2,
          lambda s: s is not None and s.lower() == "deadbeef")

    # c + break-in (0x03)
    print("  [c+break] continue then break-in...")
    r.cmd("c", wait_reply=False)
    time.sleep(0.2)
    r.send_break()
    stop3 = r.recv_any(3)
    check("break-in 0x03 returns stop reply", stop3, lambda s: s is not None)

    r.close()

    print(f"\nResults: {PASS} passed, {FAIL} failed")
    return FAIL == 0


if not run():
    sys.exit(1)
