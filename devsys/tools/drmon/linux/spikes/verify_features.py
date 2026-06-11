#!/usr/bin/env python3
"""verify_features.py — reproduce the deep feature verifications for the SNES
out-of-scope stub-lift (docs/plans/2026-06-11-lift-snes-out-of-scope-stubs.md).

Self-contained: launches headless MAME (SDL offscreen — no host window) for each
check against its required ROM/autoboot, runs the protocol check, tears down.

  python3 spikes/verify_features.py        # run all three
Requires: mame 0.277 on PATH.

Checks (the things test_bridge.py's RP/WP±/BW± toggle assertions can't cover with
the plain NOP-sled ROM):
  ppu        — seed VRAM via verify_ppu.lua, confirm RP reads the live data back
  watchpoint — writer ROM loops STA into the ROM window; BW+ must halt the CPU
  romwrite   — WRAM write sticks; ROM write is dropped (drives the ROM-write warning)
"""
import os, signal, socket, subprocess, sys, time

HERE  = os.path.dirname(os.path.abspath(__file__))
LINUX = os.path.dirname(HERE)
ROMS  = os.path.join(LINUX, "test-roms")
PORT  = 41816

results = []
def check(label, ok):
    results.append(ok)
    print(f"    {'PASS' if ok else 'FAIL'}: {label}")
    return ok

def launch_mame(rom, autoboot):
    env = dict(os.environ, SDL_VIDEODRIVER="offscreen", SDL_AUDIODRIVER="dummy")
    return subprocess.Popen(
        ["mame", "snes", "-cart", rom, "-debug", "-debugger", "none",
         "-autoboot_script", autoboot, "-video", "none", "-nothrottle"],
        cwd=LINUX, env=env,
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
        preexec_fn=os.setsid)

def kill(proc):
    try: os.killpg(os.getpgid(proc.pid), signal.SIGKILL)
    except Exception: pass

def connect(retries=60):
    for _ in range(retries):
        try: return socket.create_connection(("127.0.0.1", PORT), timeout=1.0)
        except OSError: time.sleep(0.25)
    raise SystemExit("FAIL: could not connect to bridge")

def cmd(s, c, timeout=2.0):
    s.sendall((c + "\n").encode()); s.settimeout(timeout)
    buf = b""
    while b"\n" not in buf:
        ch = s.recv(4096)
        if not ch: break
        buf += ch
    return buf.split(b"\n", 1)[0].decode()

def with_bridge(rom, autoboot, fn):
    os.system(f"fuser -k {PORT}/tcp 2>/dev/null"); time.sleep(0.3)
    proc = launch_mame(rom, autoboot)
    try:
        s = connect()
        try:
            fn(s)
        finally:
            try: cmd(s, "BYE")
            except Exception: pass
            s.close()
    finally:
        kill(proc); time.sleep(0.3)

# ── checks ────────────────────────────────────────────────────────────────────
def check_ppu(s):
    check("V handshake", cmd(s, "V").startswith("ok drmon-bridge"))
    expect = "".join("%02x" % (0xA0 + i) for i in range(16))
    check("RP reads seeded VRAM A0..AF", cmd(s, "RP 0 10") == expect)
    check("RP past 0xFFFF zero-fills", cmd(s, "RP fffe 8").endswith("0000"))

def check_watchpoint(s):
    check("? running pre-arm", cmd(s, "?") == "running")
    check("BW+ returns ok", cmd(s, "BW+") == "ok")
    stopped = None
    for _ in range(50):
        r = cmd(s, "?")
        if r.startswith("stopped"): stopped = r; break
        time.sleep(0.05)
    check(f"watchpoint fires on ROM-window write -> {stopped}", bool(stopped))
    cmd(s, "BW-"); cmd(s, "C")

def check_romwrite(s):
    cmd(s, "H")
    cmd(s, "W 7e0000 c3")
    check("WRAM write sticks", cmd(s, "R 7e0000 1") == "c3")
    before = cmd(s, "R 8000 1")
    cmd(s, "W 8000 c3")
    check(f"ROM write dropped (readback {before} != c3 -> warns)", cmd(s, "R 8000 1") != "c3")
    cmd(s, "C")

def main():
    print("PPU live-data (verify_ppu.lua seeds VRAM A0..AF):")
    with_bridge(os.path.join(ROMS, "drmon-test.sfc"),
                os.path.join(HERE, "verify_ppu.lua"), check_ppu)
    print("Watchpoint fire (drmon-test-writer.sfc loops STA $8250):")
    with_bridge(os.path.join(ROMS, "drmon-test-writer.sfc"),
                os.path.join(LINUX, "mame_bridge.lua"), check_watchpoint)
    print("ROM-write drop (drmon-test.sfc):")
    with_bridge(os.path.join(ROMS, "drmon-test.sfc"),
                os.path.join(LINUX, "mame_bridge.lua"), check_romwrite)

    n_fail = sum(1 for ok in results if not ok)
    print(f"\nResults: {len(results) - n_fail} passed, {n_fail} failed")
    sys.exit(1 if n_fail else 0)

if __name__ == "__main__":
    main()
