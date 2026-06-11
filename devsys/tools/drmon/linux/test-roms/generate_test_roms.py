#!/usr/bin/env python3
"""generate_test_roms.py — generate minimal test ROMs for drmon bridge tests.

Outputs:
  drmon-test.sfc         — 32 KB SNES LoROM; NOP sled $8000–$FFBB; JML $8000 at $FFBC (loop); reset → $8000
  drmon-test-writer.sfc  — 32 KB SNES LoROM that loops STA $8250 (writes into the ROM window) — fires WP+/BW+ watchpoints
  drmon-test.md          — 4 KB Genesis ROM (recreate/verify); NOP sled from $200; BRA.w $200 at $0FFC

Run from any directory:
  python3 devsys/tools/drmon/linux/test-roms/generate_test_roms.py
"""

import struct
import os

OUT_DIR = os.path.dirname(os.path.abspath(__file__))


# ── SNES LoROM (32 KB = 0x8000 bytes) ─────────────────────────────────────────
# LoROM bank 0: ROM maps to $008000–$00FFFF (32 KB page).
# Reset vector lives at $00FFFC–$00FFFD (file offset 0x7FFC–0x7FFD).
# Header region: $00FFC0–$00FFDF (file offset 0x7FC0–0x7FDF).
# NOP sled: $8000–$FFBB (file 0x0000–0x7FBB).
# Loop:     JML $008000 at $FFBC–$FFBF (file 0x7FBC–0x7FBF) — keeps CPU in sled forever,
#           never reaching the header area with its non-NOP bytes.
def make_snes():
    SIZE = 0x8000   # 32 KB
    rom = bytearray(b'\xEA' * SIZE)   # NOP sled (0xEA = NOP on 65816)

    # JML $008000 at offset 0x7FBC (SNES $FFBC) — loop back before the header.
    # JML abs24 opcode 0x5C followed by 24-bit little-endian address.
    rom[0x7FBC] = 0x5C  # JML abs24
    rom[0x7FBD] = 0x00  # low  byte of $008000
    rom[0x7FBE] = 0x80  # high byte of $008000
    rom[0x7FBF] = 0x00  # bank byte

    # Minimal LoROM header at offset 0x7FC0
    hdr_off = 0x7FC0
    title = b"DRMON TEST ROM       "[:21]  # 21 bytes, space-padded
    rom[hdr_off:hdr_off+21] = title
    rom[hdr_off+21] = 0x20   # ROM makeup: LoROM, no FastROM
    rom[hdr_off+22] = 0x00   # ROM type: ROM only
    rom[hdr_off+23] = 0x08   # ROM size: 1<<8 = 256 Kbit... close enough for 32KB
    rom[hdr_off+24] = 0x00   # RAM size: none
    rom[hdr_off+25] = 0x01   # destination: NTSC
    rom[hdr_off+26] = 0x33   # fixed value (extended header marker) or maker code
    rom[hdr_off+27] = 0x00   # version
    # Checksum complement + checksum (0xFFFF, not validated but MAME needs it plausible)
    rom[hdr_off+28] = 0xFF; rom[hdr_off+29] = 0xFF   # checksum complement
    rom[hdr_off+30] = 0x00; rom[hdr_off+31] = 0x00   # checksum

    # Native-mode vectors at 0x7FE0–0x7FFF; emulation-mode vectors at 0x7FF4–0x7FFF
    # RESET (emulation) at 0x7FFC–0x7FFD → $8000
    rom[0x7FFC] = 0x00
    rom[0x7FFD] = 0x80

    path = os.path.join(OUT_DIR, "drmon-test.sfc")
    with open(path, "wb") as f:
        f.write(rom)
    print(f"Wrote {len(rom)} bytes → {path}")


# ── SNES "writer" ROM (32 KB) — for watchpoint / break-on-write tests ──────────
# The plain drmon-test.sfc is a NOP sled and never writes memory, so it can't make
# a write-watchpoint fire.  This variant loops a store into the ROM window so the
# WP+/BW+ watchpoint (over $8000–$FFFF) triggers:
#   8000: A9 42      LDA #$42
#   8002: 8D 50 82   STA $8250      ; write into the watched ROM window
#   8005: 80 F9      BRA $8000      ; loop
# Reset vector → $8000.  Used by spikes/watchpoint_fire.py.
def make_snes_writer():
    SIZE = 0x8000
    rom = bytearray(b'\xEA' * SIZE)                 # NOP fill
    code = bytes([0xA9, 0x42, 0x8D, 0x50, 0x82, 0x80, 0xF9])
    rom[0x0000:0x0000+len(code)] = code

    hdr_off = 0x7FC0
    rom[hdr_off:hdr_off+21] = b"DRMON WP TEST ROM    "[:21]
    rom[hdr_off+21] = 0x20   # LoROM, no FastROM
    rom[hdr_off+22] = 0x00   # ROM only
    rom[hdr_off+23] = 0x08   # ROM size
    rom[hdr_off+24] = 0x00   # no RAM
    rom[hdr_off+25] = 0x01   # NTSC
    rom[hdr_off+26] = 0x33   # extended-header marker
    rom[hdr_off+27] = 0x00   # version
    rom[hdr_off+28] = 0xFF; rom[hdr_off+29] = 0xFF   # checksum complement
    rom[hdr_off+30] = 0x00; rom[hdr_off+31] = 0x00   # checksum
    rom[0x7FFC] = 0x00; rom[0x7FFD] = 0x80           # RESET → $8000

    path = os.path.join(OUT_DIR, "drmon-test-writer.sfc")
    with open(path, "wb") as f:
        f.write(rom)
    print(f"Wrote {len(rom)} bytes → {path}")


# ── Genesis / Mega Drive (4 KB) ────────────────────────────────────────────────
# Matches the vendored drmon-test.md already in test-roms/.
# SSP = 0x00FF8000, PC = 0x00000200.
# NOP (4E71) sled from 0x200 to 0x0FFB.
# BRA.w 0x0200 at 0x0FFC–0x0FFF (loop back to start for continuous running).
def make_genesis():
    SIZE = 0x1000   # 4 KB
    rom = bytearray(SIZE)

    # Vector table: SSP at 0x000–0x003, PC at 0x004–0x007 (big-endian)
    struct.pack_into(">I", rom, 0x000, 0x00FF8000)   # SSP
    struct.pack_into(">I", rom, 0x004, 0x00000200)   # PC

    # "SEGA MEGA DRIVE " header at 0x100 (required by MAME for genesis driver)
    hdr = b"SEGA MEGA DRIVE                                 "
    rom[0x100:0x100+len(hdr)] = hdr

    # NOP sled (4E71) from 0x200 to 0x0FFB (inclusive)
    nop = b'\x4E\x71'
    for off in range(0x200, 0x0FFC, 2):
        rom[off:off+2] = nop

    # BRA.w $0200 at 0x0FFC: opcode 0x60, word displacement.
    # BRA.w format: 60 00 disp16 (3 bytes total, but 4E71 alignment means we use BRA short)
    # BRA short (60 dd): dd = (target - (pc+2)) with pc=0x0FFC → target=0x0200
    # disp = 0x0200 - (0x0FFC + 2) = 0x0200 - 0x0FFE = 0xF202 → -0x0DFE (too far for byte)
    # Use BRA.w (60 00 disp16): 60 00 F2 02 at 0x0FFC
    # disp16 = 0x0200 - (0x0FFC + 2) = 0x0200 - 0x0FFE = -0x0DFE → 0xF202
    rom[0x0FFC] = 0x60
    rom[0x0FFD] = 0x00
    struct.pack_into(">H", rom, 0x0FFE, (0x0200 - (0x0FFC + 2)) & 0xFFFF)

    path = os.path.join(OUT_DIR, "drmon-test.md")
    with open(path, "wb") as f:
        f.write(rom)
    print(f"Wrote {len(rom)} bytes → {path}")


if __name__ == "__main__":
    make_snes()
    make_snes_writer()
    make_genesis()
    print("Done.")
