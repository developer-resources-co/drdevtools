#!/usr/bin/env python3
"""spike_gdbstub_gen.py — GEN gdbstub spike (Phase 2 plan checklist).

Expects MAME already running:
    mame genesis -cart drmon-test.md -debug -debugger gdbstub -video none -sound none -nothrottle

Answers the plan checklist:
  1. Port / initial notification
  2. g-packet register count/order (m68k layout)
  3. target.xml register description
  4. Z0 bp in cartridge ROM (NOP sled at $200)
  5. s advances exactly one instruction (NOP = 2 bytes → PC+2)
  6. Break-in byte 0x03 while running
  7. R0 / vRun restart support
  8. Hybrid: is Lua periodic still running alongside gdbstub?
"""

import socket, time, sys, re

HOST = '127.0.0.1'
PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 23946


def csum(data):
    if isinstance(data, (str,)):
        data = data.encode()
    return sum(data) % 256


class RSP:
    def __init__(self, host, port):
        self.sock = socket.socket()
        self.sock.connect((host, port))
        self.buf = b''

    # ── low-level recv ────────────────────────────────────────────────────────

    def _fill(self, timeout):
        self.sock.settimeout(timeout)
        chunk = self.sock.recv(4096)
        if not chunk:
            raise EOFError("connection closed")
        self.buf += chunk

    def _drain_ack(self, timeout=5):
        """Read past any leading +/- ack bytes (sent by server to our previous packet)."""
        deadline = time.monotonic() + timeout
        while True:
            i = 0
            while i < len(self.buf) and self.buf[i:i+1] in (b'+', b'-'):
                i += 1
            self.buf = self.buf[i:]
            if self.buf or time.monotonic() >= deadline:
                break
            try:
                self._fill(deadline - time.monotonic())
            except (socket.timeout, OSError):
                break

    def _extract_packet(self):
        """Return decoded data if a complete $..#xx packet is in buf, else None."""
        i = self.buf.find(b'$')
        if i < 0:
            self.buf = b''
            return None
        self.buf = self.buf[i:]
        h = self.buf.find(b'#', 1)
        if h < 0 or len(self.buf) < h + 3:
            return None
        data = self.buf[1:h]
        self.buf = self.buf[h+3:]
        self.sock.sendall(b'+')
        return data.decode(errors='replace')

    def recv_any(self, timeout=10):
        deadline = time.monotonic() + timeout
        while True:
            pkt = self._extract_packet()
            if pkt is not None:
                return pkt
            rem = deadline - time.monotonic()
            if rem <= 0:
                return None
            try:
                self._fill(min(rem, 1.0))
            except (socket.timeout, OSError):
                pass

    # ── high-level send/recv ─────────────────────────────────────────────────

    def cmd(self, data, wait_reply=True, timeout=10):
        """Send a packet and optionally wait for a reply."""
        raw = data.encode() if isinstance(data, str) else data
        cs = csum(raw)
        pkt = b'$' + raw + b'#' + f'{cs:02x}'.encode()
        self.sock.sendall(pkt)
        # Drain the ack for our send
        self._drain_ack()
        if not wait_reply:
            return None
        return self.recv_any(timeout)

    def send_break(self):
        self.sock.sendall(b'\x03')


def log(label, val):
    print(f"[{label}] {val!r}" if not isinstance(val, str) else f"[{label}] {val}")


def hex_to_regs(hexstr, reg_width_bytes=4):
    """Parse a hex register dump into a list of integer values."""
    regs = []
    for i in range(0, len(hexstr), reg_width_bytes * 2):
        chunk = hexstr[i:i + reg_width_bytes * 2]
        if len(chunk) == reg_width_bytes * 2:
            # GDB uses little-endian for little-endian targets, big-endian for BE.
            # m68k is big-endian.
            regs.append(int(chunk, 16))
    return regs


def main():
    print(f"=== spike_gdbstub_gen.py  connecting to {HOST}:{PORT} ===")
    r = RSP(HOST, PORT)
    print("Connected.")

    # 1. Initial unsolicited notification (machine starts stopped under -debug)
    init = r.recv_any(2)
    log("1. initial notification", init)

    # 2. qSupported
    reply = r.cmd("qSupported:multiprocess+;xmlRegisters+;qRelocInsn+;swbreak+;hwbreak+")
    log("2. qSupported", reply)

    # 3. target.xml
    target_xml = ''
    offset = 0
    while True:
        reply = r.cmd(f"qXfer:features:read:target.xml:{offset:x},800")
        if reply is None:
            print("[3. target.xml] no reply")
            break
        if reply.startswith('l'):
            target_xml += reply[1:]
            break
        elif reply.startswith('m'):
            target_xml += reply[1:]
            offset += len(reply) - 1
        else:
            log("3. target.xml unexpected", reply)
            break
    log("3. target.xml length", str(len(target_xml)))
    print(target_xml[:2000])

    # 4. g packet while stopped at reset PC ($200)
    g = r.cmd("g")
    log("4. g (raw hex)", g)
    n_regs = len(g) // 8  # assuming 4 bytes per register
    log("4. register count (assuming 4B each)", n_regs)
    regs = hex_to_regs(g)
    for i, v in enumerate(regs):
        print(f"  reg[{i:2d}] = 0x{v:08x}")

    # Determine which register index is PC (should be $200)
    pc_idx = None
    for i, v in enumerate(regs):
        if v == 0x200:
            print(f"  → PC is likely reg[{i}] (value = 0x{v:08x})")
            pc_idx = i

    # 5. Z0 bp at $202 (second NOP, 2 bytes past reset)
    reply = r.cmd("Z0,202,2")
    log("5. Z0,202,2 (set bp)", reply)

    # 6. c (continue), wait for stop reply
    print("[6. c] sending continue...")
    r.cmd("c", wait_reply=False)
    stop = r.recv_any(10)
    log("6. stop after c", stop)

    # Parse stop reason and PC from stop reply
    if stop:
        m = re.search(r'(?:^T05|T05).*?(?:(\d+):([0-9a-fA-F]+);)*', stop)
        print(f"  stop reply fields: {dict(re.findall(r'([0-9a-fA-F]+):([0-9a-fA-F]+)', stop))}")

    # 7. g after bp hit (verify PC = $202)
    g2 = r.cmd("g")
    regs2 = hex_to_regs(g2)
    if pc_idx is not None:
        log("7. PC after bp hit", f"0x{regs2[pc_idx]:08x} (expected 0x202)")
    else:
        log("7. g after bp (first 160 chars)", g2[:160])

    # 8. s (single step from $202) — expect PC → $204
    print("[8. s] single step...")
    r.cmd("s", wait_reply=False)
    stop2 = r.recv_any(10)
    log("8. stop after s", stop2)

    g3 = r.cmd("g")
    regs3 = hex_to_regs(g3)
    if pc_idx is not None:
        log("8. PC after step", f"0x{regs3[pc_idx]:08x} (expected 0x204, NOP=2 bytes)")

    # 9. z0 (clear bp)
    reply = r.cmd("z0,202,2")
    log("9. z0 (clear bp)", reply)

    # 10. c then break-in 0x03
    print("[10. c+break] continue then 0x03 break-in...")
    r.cmd("c", wait_reply=False)
    time.sleep(0.3)
    r.send_break()
    stop3 = r.recv_any(5)
    log("10. stop after break-in", stop3)

    # 11. R0 restart
    reply = r.cmd("R0", timeout=3)
    log("11. R0 restart", reply)

    # 12. vRun restart
    reply = r.cmd("vRun;", timeout=3)
    log("12. vRun restart", reply)

    # 13. Summary
    print("\n=== SUMMARY ===")
    print(f"Port: {PORT}")
    print(f"Register count: {n_regs} × 4 bytes = {n_regs*4} bytes ({n_regs*8} hex chars)")
    print(f"PC reg index: {pc_idx}")
    print(f"Z0 bp in ROM: {'OK' if stop and 'T05' in (stop or '') else 'UNKNOWN: ' + repr(stop)}")
    step_ok = (pc_idx is not None and len(regs3) > pc_idx and
               regs3[pc_idx] == regs2[pc_idx] + 2) if pc_idx is not None else False
    print(f"s advances 2 bytes (NOP): {'YES' if step_ok else 'NO'}")
    print(f"Break-in 0x03: {'OK' if stop3 else 'FAILED'}")
    print("=== END SPIKE ===")


if __name__ == '__main__':
    main()
