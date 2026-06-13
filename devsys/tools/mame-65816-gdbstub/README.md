# mame-65816-gdbstub — a 65816 GDB Remote Serial Protocol endpoint for MAME

Adds **65816 (`5a22` / `w65c816` / `w65c802`)** support to MAME's GDB stub so XML‑aware RSP clients
(IDA Pro, custom tooling, CI harnesses) can attach to a running SNES / Apple IIgs / Foenix‑class
65816 target. Decoupled from drmon — drmon keeps its own custom Lua bridge (RSP can't express
PPU/SPC700/secondary address spaces); this is a general, upstream‑oriented contribution.

Background and rationale: [`docs/investigations/2026-06-11-65816-gdbstub.md`](../../../docs/investigations/2026-06-11-65816-gdbstub.md).
Plans: [register map](../../../docs/plans/2026-06-13-mame-65816-gdbstub-map.md) ·
[IDA/Ghidra clients](../../../docs/plans/2026-06-13-65816-rsp-clients-ida-ghidra.md).

## Two tiers

- **Tier 1 — Lua plugin (no rebuild, works on stock apt MAME 0.277).** `plugins/gdbstub/init.lua`
  is a fork of MAME's shipped gdbstub plugin with a `5a22` register map, a hand‑authored
  `target.xml` (`qSupported` + `qXfer:features:read`), `Z0` software breakpoints, and a single
  clean stop notifier. **14/14 verified live** (registers, target.xml, memory r/w, breakpoint
  set/clear/fire, continue, single‑step) — `./verify.sh`.
- **Tier 2 — C++ module patch (the upstream contribution).**
  `tier2/0001-add-65816-w65c816-register-map.patch` adds the same map to
  `src/osd/modules/debugger/debuggdbstub.cpp` (`-debugger gdbstub`). Applies clean against
  `mame0277`; build‑verification deferred (disk). See [`tier2/PR-NOTES.md`](tier2/PR-NOTES.md).

## Register layout (canonical; both tiers agree)

`g`‑packet order = `target.xml` `<reg>` order: **A X Y P E S D DB PB PC**, little‑endian, widths
`16 16 16 8 8 16 16 8 8 32` bits (18 bytes / 36 hex). PC is the 24‑bit linear value (bank in the
high byte), zero‑extended to 32. Verified ground truth (MAME 0.277 + DKC): `PC=0x80c0d5`,
`PB=0x80`.

## Run / verify

```sh
./run-mame.sh          # headless MAME (snes + DKC) with the gdbstub Lua; RSP on 127.0.0.1:2159
python3 test_65816.py  # drive the endpoint and assert the protocol
./verify.sh            # the above, with retry around the rare MAME boot race
```
No `$DISPLAY` needed (`-video none` + `-autoboot_script`). To use the **installable plugin form**
instead, drop `plugins/gdbstub/` onto MAME's `-pluginspath` and run with `-plugin gdbstub` (needs a
video target / display).

## Files

| path | what |
|------|------|
| `plugins/gdbstub/init.lua` | the 65816 register‑map plugin (Tier 1) |
| `plugins/gdbstub/plugin.json` | plugin manifest |
| `gdbstub_autoboot.lua` | headless shim that loads `init.lua` via `-autoboot_script` |
| `run-mame.sh` | launch MAME + the gdbstub, headless |
| `test_65816.py` | RSP client + protocol test |
| `verify.sh` | run the test with retry; reproducible verification |
| `clients.md` | IDA / Ghidra / gdb client recipes + what actually works |
| `tier2/` | the C++ `debuggdbstub.cpp` patch + PR notes |

## Clients

gdb is **not** a client (no 65816 gdbarch). Ghidra's gdb‑based connector inherits that gap (not
turnkey). IDA's GDB plugin consumes the `target.xml` given a 65816 processor module. Details +
recipes: [`clients.md`](clients.md).
