#!/usr/bin/env python3
"""test_genesis_bridge.py — protocol-level tests for mame_genesis_bridge.lua.

Asserts the full command set against a running bridge server.
Expects MAME to be running with -debugger gdbstub -autoboot_script mame_genesis_bridge.lua.

Usage:
    python3 linux/test_genesis_bridge.py [host:port]

Environment:
    DRMON_GEN_BRIDGE_ADDR   override host:port (default 127.0.0.1:41817)
"""

import os
import socket
import sys
import time

addr_env = os.environ.get("DRMON_GEN_BRIDGE_ADDR", "127.0.0.1:41817")
if len(sys.argv) > 1:
    addr_env = sys.argv[1]

_parts = addr_env.rsplit(":", 1)
HOST = _parts[0]
PORT = int(_parts[1]) if len(_parts) > 1 else 41817

PASS = 0
FAIL = 0

def connect():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(5.0)
    s.connect((HOST, PORT))
    return s

def send(s, cmd):
    s.sendall((cmd + "\n").encode())

def recv_line(s):
    buf = b""
    while True:
        ch = s.recv(1)
        if not ch:
            raise EOFError("connection closed")
        if ch == b"\n":
            return buf.decode().rstrip("\r")
        buf += ch

def check(label, got, expected_fn, expected_desc):
    global PASS, FAIL
    if expected_fn(got):
        print(f"  PASS  {label}")
        PASS += 1
    else:
        print(f"  FAIL  {label}: got {got!r}, expected {expected_desc}")
        FAIL += 1

def is_hex(s, nbytes):
    return len(s) == nbytes * 2 and all(c in "0123456789abcdefABCDEF" for c in s)

# ── connect ───────────────────────────────────────────────────────────────────
try:
    s = connect()
except Exception as e:
    print(f"FATAL: cannot connect to {HOST}:{PORT}: {e}")
    sys.exit(1)

print(f"Connected to {HOST}:{PORT}")
print()

# ── V: version handshake ──────────────────────────────────────────────────────
print("=== V: version handshake ===")
send(s, "V")
resp = recv_line(s)
check("V response starts with 'ok drmon-genesis-bridge'",
      resp,
      lambda r: r.startswith("ok drmon-genesis-bridge"),
      "'ok drmon-genesis-bridge ...'")

# ── RV: VDP VRAM read ─────────────────────────────────────────────────────────
print("\n=== RV: VDP VRAM read ===")
send(s, "RV 0 10")   # 16 bytes from start of VRAM
resp = recv_line(s)
check("RV 0 10 → 32 hex chars (16 bytes)",
      resp,
      lambda r: is_hex(r, 16),
      "32 hex chars")

send(s, "RV 0 1")
resp = recv_line(s)
check("RV 0 1 → 2 hex chars (1 byte)",
      resp,
      lambda r: is_hex(r, 1),
      "2 hex chars")

send(s, "RV ffff 1")
resp = recv_line(s)
check("RV ffff 1 → 2 hex chars (top of 64K VRAM)",
      resp,
      lambda r: is_hex(r, 1),
      "2 hex chars")

# ── RC: CRAM read ─────────────────────────────────────────────────────────────
print("\n=== RC: CRAM read (64 × u16 = 128 bytes) ===")
send(s, "RC")
resp = recv_line(s)
check("RC → 256 hex chars (128 bytes)",
      resp,
      lambda r: is_hex(r, 128),
      "256 hex chars")

# ── RS: VSRAM read ────────────────────────────────────────────────────────────
print("\n=== RS: VSRAM read (40 × u16 = 80 bytes) ===")
send(s, "RS")
resp = recv_line(s)
check("RS → 160 hex chars (80 bytes)",
      resp,
      lambda r: is_hex(r, 80),
      "160 hex chars")

# ── RZ: Z80 program space read ────────────────────────────────────────────────
print("\n=== RZ: Z80 program space read ===")
send(s, "RZ 0 10")   # 16 bytes from start of Z80 space
resp = recv_line(s)
check("RZ 0 10 → 32 hex chars (16 bytes)",
      resp,
      lambda r: is_hex(r, 16),
      "32 hex chars")

send(s, "RZ 1fff 1")
resp = recv_line(s)
check("RZ 1fff 1 → 2 hex chars",
      resp,
      lambda r: is_hex(r, 1),
      "2 hex chars")

# ── unknown command ───────────────────────────────────────────────────────────
print("\n=== unknown command → err ===")
send(s, "BOGUS")
resp = recv_line(s)
check("unknown command → 'err ...'",
      resp,
      lambda r: r.startswith("err"),
      "'err ...'")

# ── BYE: graceful disconnect + reconnect ──────────────────────────────────────
print("\n=== BYE: disconnect + reconnect ===")
send(s, "BYE")
s.close()
time.sleep(0.3)
bye_ok = False
bye_err = ""
try:
    s2 = connect()
    send(s2, "V")
    resp = recv_line(s2)
    bye_ok = resp.startswith("ok drmon-genesis-bridge")
    s2.close()
except Exception as e:
    bye_err = str(e)

check("reconnect after BYE + V handshake",
      bye_ok,
      lambda r: r is True,
      f"True (got err: {bye_err})" if bye_err else "True")

# ── summary ───────────────────────────────────────────────────────────────────
print()
print(f"Results: {PASS} passed, {FAIL} failed")
sys.exit(0 if FAIL == 0 else 1)
