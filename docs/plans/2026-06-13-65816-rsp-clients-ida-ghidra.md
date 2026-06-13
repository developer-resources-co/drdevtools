# IDA Pro / Ghidra as 65816 RSP clients against MAME's gdbstub — get real answers

**Date:** 2026-06-13
**Status:** wip
**TODO item:** "IDA Pro + Ghidra 65816 RSP against MAME gdbstub — get real answers"
(DRMON — INVESTIGATIONS).
**Source investigation:** [2026-06-11-65816-gdbstub.md](../investigations/2026-06-11-65816-gdbstub.md)
(answers get written back into its "Open questions" section).
**Prerequisite:** the Tier‑1 `target.xml`‑serving endpoint from
[2026-06-13-mame-65816-gdbstub-map.md](2026-06-13-mame-65816-gdbstub-map.md).

---

## Goal

Replace the investigation's open questions with **real, evidenced answers**:

- Does a 65816 processor module exist for IDA, and does IDA's GDB client consume a MAME‑style
  `target.xml` for 65816 (the way [idapro_m6502](https://github.com/LucienMP/idapro_m6502)
  does for m6502)?
- Does Ghidra's RSP/debugger path connect to the endpoint without a custom agent?
- Is the on‑the‑wire protocol (qXfer XML, `g` layout, stop replies) actually well‑formed and
  client‑consumable?

## What can and cannot be run here (honest scope)

| Client | Installed? | Verifiable here? |
|--------|-----------|------------------|
| Python RSP client | yes (forked) | **Fully** — drives the real endpoint; validates XML + `g` + bp + step + mem. This is the substantive wire‑protocol proof. |
| `gdb` | yes (`/usr/bin/gdb`) | **The 65816 gap is directly demonstrable** — `set architecture` has no 65816; confirms gdb is a non‑client and *why* (the Ghidra dependency below). |
| Ghidra | no (1 GB + JDK; disk at 98%) | **Analytically, with gdb evidence.** Ghidra's "GDB" debugger spawns a local `gdb`; since gdb has no 65816 gdbarch, that path can't disassemble/step a 65816 target. Substantiated, not just asserted. |
| IDA Pro | no (commercial license) | **Recipe + wire‑format proof.** Produce the exact connection recipe and prove the `target.xml`/`g` format matches what IDA's GDB plugin consumes (m6502 precedent). Final IDA‑in‑the‑loop step is license‑gated → handed to the user with a documented procedure. |

This is a *findings* deliverable. The disk (98% full) and licensing realities are stated, not
papered over — the verifiable core (wire protocol) is verified; the unverifiable tail (IDA GUI,
Ghidra GUI) is reduced to a single documented user step each.

---

## Method

1. **Stand up the Tier‑1 endpoint** (sibling plan) against `mame snes -cart DKC.smc`.
2. **Wire‑protocol verification** with `test_65816.py`: fetch `target.xml`, assert it parses as
   XML and the `<reg>` list (names/bitsizes/order) matches the canonical table; read `g`,
   decode per the description, cross‑check PC/A/X against MAME‑side reads; set a `Z0` bp, run,
   confirm the stop reply; single‑step; memory read/round‑trip. This is exactly the byte stream
   IDA's GDB plugin and any XML‑aware client consume — if it's well‑formed and self‑describing,
   the client‑side gap reduces to "does the client ship a 65816 disassembler."
3. **gdb evidence** — show `gdb` cannot select a 65816 architecture (`set architecture ?` /
   connecting and observing the description rejection). This is the root cause for Ghidra.
4. **IDA recipe** — document: IDA has no stock 65816 processor module (ships 6502, not 65816);
   options are (a) a third‑party 65816 procmod + IDA's GDB debugger pointed at `:2159`, or
   (b) treat as data + manual, using the `target.xml` for register names. The MAME `target.xml`
   we serve is the same shape IDA consumes for m6502 (idapro_m6502 precedent).
5. **Ghidra recipe/verdict** — Ghidra has no stock 65816 language either; its gdb‑based
   debugger inherits gdb's no‑65816 limitation; a raw‑RSP connection without gdb is not turnkey
   (needs a custom connector/Trace‑RMI work). Verdict: not turnkey for 65816.
6. **Write answers** into the investigation's "Open questions" + a `clients.md` under
   `devsys/tools/mame-65816-gdbstub/` with the concrete recipes.

---

## Verification

1. **`target.xml` is valid XML** and its `<reg>` list matches the canonical 10‑register table
   (parsed by `xml.etree` in `test_65816.py`).
2. **`g` decodes against the description** — every register's width from `target.xml` lines up
   with the `g`‑packet byte offsets; PC/A match independent MAME‑side reads.
3. **gdb 65816 gap demonstrated** — raw output showing gdb has no 65816 architecture (root
   cause for Ghidra's gdb‑connector path).
4. **Findings written** — investigation "Open questions" replaced with evidenced answers;
   `clients.md` recipes present.

### Results — 2026-06-13

1. **PASS** — `target.xml` parses (`xml.etree`) and its `<reg>` list matches the canonical
   10-register table (names/bitsizes/order), via `test_65816.py` (14/14 — see the
   [gdbstub-map plan](2026-06-13-mame-65816-gdbstub-map.md) for raw output).
2. **PASS** — `g` decodes against the description; PC `0x80c0d5` with `PB=0x80` (24-bit linear PC
   consistent); E ∈ {0,1}; memory reads match.
3. **PASS** — gdb has no 65816 architecture (stock gdb 16.3):

   ```
   $ gdb -nx -batch -ex "set architecture w65c816"  -> Undefined item: "w65c816".
   $ gdb -nx -batch -ex "set architecture 65816"    -> Undefined item: "65816".
   $ gdb -nx -batch -ex "set architecture m6502"    -> Undefined item: "m6502".
   # valid arches: i386, i386:x86-64, i8086, ...  (x86 only — no 65xx/w65/spc700)
   ```
   This is the root cause: Ghidra's GDB connector drives a local gdb → can't disassemble/step a
   65816 → no turnkey Ghidra path. IDA's GDB plugin reads the (verified well-formed) `target.xml`
   like idapro_m6502 does for m6502, so with a 65816 procmod it attaches; the GUI step is
   license-gated. Recipes in `clients.md`.
4. **PASS** — investigation "Open questions" replaced with evidenced answers; `clients.md` present.

**Honest scope:** IDA Pro and Ghidra GUIs were not installed (commercial license / ~1 GB + JDK on a
98%-full disk), so the GUI-in-the-loop attach wasn't exercised. What's verified is the on-the-wire
contract those clients consume (target.xml + g + m/M + bp), plus the gdb gap that gates Ghidra.

**Licensing & cost (why the GUI attach is license-gated, and what unblocks it):** two IDA features stack
here, both **IDA Pro**-only — loading a **third-party 65816 processor module** (no 65xx CPU ships with
IDA) and the **remote GDB debugger** backend that reads `target.xml`. IDA **Free** ($0) cannot load
custom processor modules at all; IDA **Home** ($365/yr) is a curated, fixed-architecture set with no
custom-module/SDK support — neither can disassemble 65816. The cheapest edition that can is **IDA Pro
"Essential", $1,099/yr** (Hex-Rays went subscription-only on 2024-10-01; perpetual licenses are no
longer sold). The higher Pro tiers (Expert 2/4/6 ≈ $2,999/$4,999/$6,899, Ultimate $8,599) only add
**decompilers** — which 65816 has none of and this RSP workflow doesn't use — so Essential is the price
of admission, *provided* it includes the SDK/custom-procmod loading + the gdb debugger backend (verify
on the current [pricing/feature matrix](https://hex-rays.com/pricing) before quoting; checked 2026-06-13).
The **free** route to the same end is Ghidra — but **stock Ghidra has no 65816 processor** (it ships
6502, not 65816). Static RE for $0 means adding a *third-party* SLEIGH module
([achan1989/ghidra-65816](https://github.com/achan1989/ghidra-65816)): disassembly mostly works, but
its decompiler is "probably unusable" per the module's own README. Only the *live* attach needs the
custom connector (TraceRMI / bespoke RSP agent → that SLEIGH) noted in the TODO.
