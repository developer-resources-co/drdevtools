# 65816 RSP clients against the MAME gdbstub — recipes + what actually works

Companion to [the IDA/Ghidra plan](../../../docs/plans/2026-06-13-65816-rsp-clients-ida-ghidra.md)
and [the gdbstub investigation](../../../docs/investigations/2026-06-11-65816-gdbstub.md). Endpoint:
the Tier‑1 Lua plugin here (RSP on `127.0.0.1:2159`, serves a `w65c816` `target.xml`) or, once
built, the Tier‑2 C++ module (`-debugger gdbstub`, same description). Both expose the identical
register description, so client behaviour is the same.

## Verified wire format (the part that's reproducible here)

`test_65816.py` drives the live endpoint and asserts the exact bytes an XML‑aware RSP client
consumes — all green (14/14, see the plan):

- `qSupported` advertises `qXfer:features:read+`.
- `qXfer:features:read:target.xml` returns a well‑formed description whose `<reg>` list is
  `A X Y P E S D DB PB PC` with bitsizes `16 16 16 8 8 16 16 8 8 32` in that order.
- `g` decodes against that description; PC is the full 24‑bit linear value (e.g. `0x80c0d5`,
  `PB=0x80`), zero‑extended to 32 bits.
- `m`/`M` read/write memory; `Z0` set/clear; continue / single‑step work.

This is the same self‑describing `target.xml` + `g` mechanism IDA's GDB plugin consumes for m6502
([idapro_m6502](https://github.com/LucienMP/idapro_m6502)). So the **client‑side gap is purely
"does the client ship a 65816 disassembler"**, not "can the client parse the registers" — that's
answered, the description is valid and standard.

## gdb — NOT a client (evidenced)

gdb has no 65816 architecture. Stock gdb 16.3:

```
$ gdb -nx -batch -ex "set architecture w65c816"   ->  Undefined item: "w65c816".
$ gdb -nx -batch -ex "set architecture 65816"     ->  Undefined item: "65816".
$ gdb -nx -batch -ex "set architecture m6502"     ->  Undefined item: "m6502".
# valid architectures on this build: i386, i386:x86-64, i8086, ... (x86 only; no 65xx/w65/spc700)
```

binutils' `w65` target was assembler‑only and was removed in 2018; no gdbarch ever replaced it. So
`gdb` can connect to the socket but cannot describe/disassemble/step a 65816 — it is not a usable
client, and this is the root cause for Ghidra below.

## Ghidra — not turnkey (gdb dependency)

Ghidra's "GDB" debugger connector launches a **local `gdb`** and drives the remote target through
it. Since gdb has no 65816 gdbarch (above), that path cannot disassemble or step a 65816 target —
the connection may open but gdb can't make sense of the architecture. A working Ghidra path would
need a **custom connector** (Ghidra's TraceRMI / a bespoke RSP agent) that speaks our `target.xml`
directly and bridges to Ghidra's own 65816 SLEIGH language — i.e. real plugin work, not a
configuration. Verdict: **no turnkey Ghidra path for 65816 today.**

## IDA Pro — works with a 65816 processor module (final step license‑gated)

IDA does **not** ship a stock 65816 processor module (it ships the 6502 family); 65816 disassembly
in IDA comes from third‑party/community processor modules. Given such a module loaded, IDA's GDB
debugger plugin consumes a MAME‑style `target.xml` exactly as it does for m6502 (idapro_m6502
precedent), so it can attach to our endpoint and show registers + memory.

Recipe:
1. Load the 65816 binary in IDA with a 65816 processor module.
2. Debugger → *Remote GDB debugger*; host `127.0.0.1`, port `2159` (Tier‑1 Lua) or your
   `-debugger_port` (Tier‑2 C++).
3. IDA fetches `target.xml` and presents `A/X/Y/P/E/S/D/DB/PB/PC`; memory and stepping use `m`/`M`,
   `Z0`, `c`/`s`.

The wire format is verified here; the only step that needs an IDA license + a 65816 procmod (so it
couldn't be exercised in this environment) is the GUI attach. The endpoint is ready for it.

## Quick start (any client)

```sh
./run-mame.sh            # headless MAME + gdbstub, RSP on 127.0.0.1:2159
python3 test_65816.py    # confirm the endpoint (or ./verify.sh for the retry-wrapped run)
```
