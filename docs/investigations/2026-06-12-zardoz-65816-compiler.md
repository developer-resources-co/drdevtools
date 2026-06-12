# Zardoz 65816 C Compiler — History, ABI, and Successors

**Date:** 2026-06-12  
**Method:** Deep-research harness — 5 search angles, 15 sources fetched, 25 claims adversarially
verified (11 confirmed, 14 killed), 97 subagents; supplemented by primary-source testimony from
Will Norris (commercial SNES developer, Demolition Man and Izzy's Olympic Quest).  
**Primary source:** [WDC W65C816S C Compiler Manual (816cc.pdf)](https://www.westerndesigncenter.com/wdc/documentation/816cc.pdf)

---

## Summary

Zardoz is a 65816 C compiler attributed to **Jim Goodnow** ("jimg"), formerly hosted at
`www.oro.net/~jimg/cdev.html` (archived ~1997). It was used to ship commercial SNES titles in the
1990s — confirmed by Will Norris, who used it to develop *Demolition Man* and *Izzy's Olympic
Quest* for SNES; other commercial titles likely exist.

Zardoz is frequently described as the direct ancestor of Western Design Center's commercial
**WDC816CC** compiler. **That lineage claim could not be confirmed from any primary source** —
every assertion of WDC acquisition or direct derivation was refuted 0-3 in adversarial
verification; it rests entirely on community forum memory.

What *is* well-established from the WDC compiler manual is a technically distinctive ABI, described
below. Whether that ABI originated with Zardoz or was independently designed by WDC is unknown.

---

## What We Know About Zardoz

- **Attribution:** Jim Goodnow / "jimg" — referenced in multiple community threads on
  [nesdev.org](https://forums.nesdev.org/viewtopic.php?t=12330) and
  [assemblergames.org](https://assemblergames.org/viewtopic.php?t=60992).
- **Archived homepage:** [web.archive.org](https://web.archive.org/web/19970720124726/www.oro.net/~jimg/cdev.html)
  — snapshot from ~1997, could not be fetched to verify content.
- **Source code:** Not publicly available. No GitHub repository, archive.org source dump, or
  public release has been identified.
- **Platform scope:** Community claims it targeted the 65816 across platforms (SNES, Apple IIGS).
  Not confirmed from secondary sources, but the SNES use is confirmed (see below).
- **SNES commercial use:** **Confirmed.** Will Norris used Zardoz to develop *Demolition Man*
  (SNES) and *Izzy's Olympic Quest* (SNES, official 1996 Olympics game). Additional commercial
  titles likely exist.

---

## The WDC816CC ABI (High Confidence)

The following is drawn directly from the WDC compiler manual (2013 revision), which is the
strongest surviving primary source in this ecosystem. Whether the design originated in Zardoz is
unknown, but the ABI is shared by whatever the community calls "Zardoz/WDC."

### Stack frame and Direct Page trick

Arguments are pushed onto the **hardware stack** before the `JSR`/`JSL`. On function entry:

1. `PHD` — saves the caller's Direct Page register
2. `TCD` — redirects DP to `SP+1`, i.e. the base of the current stack frame

Every local variable, argument, and temporary is then accessible via **Direct Page addressing**
with an 8-bit offset. This is significantly faster than stack-relative (`S`-indexed) addressing,
which the 65816 only supports with limited instruction coverage.

The design is explicitly speed-motivated: the WDC manual notes that the DP-frame approach is
"partly done for speed."

Sources: [6502.org forum t=4284](https://6502.org/forum/viewtopic.php?f=12&t=4284),
[6502.org forum t=8067](https://6502.org/forum/viewtopic.php?f=2&t=8067),
[816cc.pdf](https://www.westerndesigncenter.com/wdc/documentation/816cc.pdf)

### 256-byte stack frame hard limit

Because DP addressing uses an **8-bit offset**, the entire frame — arguments + return address +
locals + temporaries — is capped at **256 bytes**. The WDC manual states verbatim:

> "the total size of the arguments, return address, local variables and temporary variables may not
> exceed 256 bytes… NOTE: If you require more variables in a function, you must put them in a
> global area."

This is a hard architectural constraint, not a soft warning. Functions with large local state must
spill to globals.

Sources: [816cc.pdf p.21](https://www.westerndesigncenter.com/wdc/documentation/816cc.pdf),
[6502.org forum t=4284](https://6502.org/forum/viewtopic.php?f=12&t=4284)

### Return value convention

Return values are split across two 16-bit registers:

| Part | Register |
|------|----------|
| Low word | Accumulator (A) |
| High word | X register |

Combined, this gives a 32-bit return value. The WDC manual (p. 21) states: "Functions called by a
C function and C functions themselves return values in the X register and the Accumulator. The high
word of the result, if any, is in the X register, while the low word is in the Accumulator."

Sources: [816cc.pdf](https://www.westerndesigncenter.com/wdc/documentation/816cc.pdf),
[6502.org forum t=4284](https://6502.org/forum/viewtopic.php?f=12&t=4284)

### Memory models

The compiler supports four memory models (manual pp. 22–23):

| Model | Code pointers | Data pointers | Notes |
|-------|--------------|---------------|-------|
| **Small** | 16-bit | 16-bit | All globals must be in bank 0 |
| **Compact** | 16-bit | 32-bit | |
| **Medium** | 32-bit | 16-bit | |
| **Large** | 32-bit | 32-bit | |

Source: [816cc.pdf](https://www.westerndesigncenter.com/wdc/documentation/816cc.pdf)

---

## Successors and Modern Alternatives

### llvm-mos (65816 backend — incomplete)

[llvm-mos](https://github.com/llvm-mos/llvm-mos) is an LLVM fork targeting the 6502 family.
A 65816 assembler and linker were contributed, but the C compiler backend was never completed.
Contributor @asiekierka stated on 2024-10-25 in
[issue #454](https://github.com/llvm-mos/llvm-mos/issues/454):

> "The rest was put on hold until I got around to porting the compiler backend proper, which I
> never did. In short — LLVM-MOS ships a 65816 assembler and linker, but not a compiler."

[Issue #32](https://github.com/llvm-mos/llvm-mos/issues/32) (the main 65816 tracking issue)
remained open with no merged C compiler backend PRs through June 2026.

**Status: assembler/linker only; no C.**

### cc65 — extending to 65816

[cc65](https://github.com/cc65/cc65) includes ca65 (assembler, which supports 65816), but the
**C compiler only generates 8-bit 6502 code** — it does not emit 65816 instructions. Adding a
65816 C backend has been raised ([issue #850](https://github.com/cc65/cc65/issues/850), Feb 2019)
but attracted no substantive response and no active effort exists.

**Feasibility: possible but substantial.** The key obstacles, drawn from the cc65 internals docs
and the [FUZIX cc65/65C816 case study](https://github.com/EtchedPixels/FUZIX/wiki/Using-CC65-on-the-65C816):

- **Register width model.** cc65's code generator (`codegen.c`) is built around an 8-bit
  accumulator / AX pseudo-register pair. The 65816's mode-switched 16-bit A, X, Y would require
  a dual-width register model woven throughout the optimizer and emission stages — not a local
  change.
- **Self-modifying code patterns.** The cc65 runtime helpers `callax` and `callptr4` store
  addresses inline (self-modifying code). This breaks under 65816 bank switching: cross-bank
  execution moves the code page, and inline stores become stale. The entire affected runtime
  library would need rewriting.
- **ABI overhaul.** cc65's fastcall convention passes the rightmost argument in A/X/sreg.
  A 65816 ABI using the PHD/TCD Direct Page trick (as WDC816CC does) would require new
  prologue/epilogue sequences, re-mapping of pseudo-registers to real 16-bit registers, and
  updated calling convention docs.
- **Bank-aware linker.** The 65816's segmented bank model requires the linker to distinguish
  near vs. far calls (JSR vs. JSL) and route data references accordingly. cc65's linker has no
  concept of this.

No fork targeting 65816 C has been found. The most realistic path to an open-source optimizing
65816 C compiler remains the llvm-mos backend.

### Calypsi C Compiler

[Calypsi](https://www.calypsi.cc/) is a modern proprietary retro C compiler supporting 65816
(and 68000, 6502). Actively maintained (v5.17 as of May 2026). **Free to download; hobby-only
license** — the license prohibits using it to "make your living… either directly or indirectly"
(small side/hobby income is explicitly permitted; commercial product development is not). It is
closed-source, so it cannot be packaged for redistribution. No paid commercial tier exists.

See also the [toolchain survey](2026-06-11-snes-65816-toolchains.md).

### PVSnesLib / 816-tcc

[PVSnesLib](https://github.com/alekmaul/pvsneslib) is the main actively-maintained SNES C SDK
(MIT, v4.5.0, Dec 2025). Its C compiler is **816-tcc** — a port of Fabrice Bellard's
[Tiny C Compiler](https://bellard.org/tcc/) retargeted to the 65816 by "Mic\_", with ongoing
maintenance by community contributor kobenairb (a significant upgrade landed in v4.0, Oct 2022).

Toolchain stack:

- **816-tcc** — C → 65816 assembly (invoked as `$(PVSNESLIB_HOME)/devkitsnes/bin/816-tcc`)
- **wla-dx** (bundled fork) — assembler and linker
- **bass** (Byuu's assembler) — used for SPC700 audio CPU code

TCC was chosen over GCC for its simplicity and portability as a retarget base; the full GCC
backend infrastructure (RTL, register allocator, peephole optimizer) would require far more
porting work for a 16-bit accumulator/index architecture with the 65816's unusual addressing modes.
The tradeoff is that 816-tcc produces functional but unoptimized code — no loop unrolling,
no inlining, no strength reduction. For SNES homebrew this is usually acceptable; inner loops and
DMA routines are written in inline assembly anyway.

Notable games shipped with PVSnesLib: *Yo-Yo Shuriken* and *Keeping SNES Alive!* (Dr. Ludos),
both written 100% in C. See also the [toolchain survey](2026-06-11-snes-65816-toolchains.md) for
packaging notes.

### ORCA/C (Apple IIGS)

[ORCA/C](https://github.com/byteworksinc/ORCA-C) is a 65816 C compiler from the era targeting the
Apple IIGS, now source-available from Byte Works. **License: source-available, not OSI open
source** — the LICENSE file permits private use and forking but prohibits redistribution without
written permission. Not packageable for an apt repo.

**ABI** (from source code analysis of `Gen.pas`): ORCA/C uses the same fundamental **PHD + TCD**
stack-frame trick as WDC816CC — TSC loads the stack pointer into C, PHD saves the old Direct Page,
then TCD redirects DP to the new frame base, giving direct-page-speed access to locals. Arguments
are pushed left-to-right before JSL.

**Return values**: byte/word in A; 32-bit long in A (low) + X (high) — identical convention to
WDC816CC. 64-bit values are returned via caller-supplied pointer.

**Memory model**: Single "small memory model" (`smallMemoryModel := true`). No equivalent of
WDC816CC's four-model system. An optional `dataBank` flag saves/restores the data bank register
around function calls for multi-bank programs, but this is a pragma, not a full memory model.

**Optimization**: DAG-based + peephole, enabled with `-O`. The peephole window is ~128 bytes.
Supports common subexpression elimination and loop optimizations when enabled. More capable than
816-tcc; comparable to WDC816CC for a 1990s compiler.

**SNES suitability**: Low. ORCA/C is tightly coupled to the Apple IIGS runtime (System 6+,
toolbox bindings). The bank model assumes 64 KB flat-within-bank access, whereas SNES uses 8 KB
ROM windows per bank. Adapting it to SNES would require stripping the IIGS runtime entirely and
replacing it with a SNES BSP — substantial work with no active effort known.

---

## Writing C for the SNES Today

The short answer: **PVSnesLib is your only practical open-source choice**; Calypsi if you can
accept a proprietary license.

| Option | Compiler | License | Code quality | SNES-specific support | Verdict |
|--------|----------|---------|-------------|----------------------|---------|
| **[PVSnesLib](https://github.com/alekmaul/pvsneslib)** | 816-tcc (TCC port) | MIT | Unoptimized but correct | Full SDK: VRAM, DMA, controllers, sprites, sound | **Best open-source choice** |
| **[Calypsi](https://www.calypsi.cc/)** | Calypsi C (proprietary) | Free DL, hobby-only | Optimizing | 65816 target; SNES needs manual BSP | **Best code quality, non-redistributable** |
| **[WDC816CC](https://www.westerndesigncenter.com/wdc/products/C-Compiler.cfm)** | WDC C (commercial) | Commercial | Optimizing | 65816; no SNES SDK | Expensive, no active community |
| **[ORCA/C](https://github.com/byteworksinc/ORCA-C)** | ORCA/C | Source-available | Mature, peephole opt | Targets Apple IIGS, not SNES | Needs heavy porting; not redistributable |
| **[llvm-mos](https://github.com/llvm-mos/llvm-mos)** | LLVM (incomplete) | Apache-2.0 | — | Assembler/linker only | No C compiler yet |
| **[cc65](https://github.com/cc65/cc65)** | cc65 C | Zlib | Moderate | 6502 only | Does not emit 65816; extension is substantial work |

### If you want to write C for SNES homebrew right now

Use **PVSnesLib**. Clone the repo, set `PVSNESLIB_HOME`, and the Makefile-based build system
handles C compilation via 816-tcc → wla-dx → SNES ROM. The SDK includes:

- Hardware abstraction for VRAM, OAM, DMA, HDMA
- Controller input, sprite and tilemap management
- SPC700 sound driver integration
- A working examples directory covering most common use cases

The main limitation to plan around: **no optimization**. 816-tcc is a single-pass compiler with
no backend optimizer. Anything performance-sensitive (scanline effects, physics, audio mixing)
needs inline assembly (`asm` blocks are supported). This is standard practice in SNES homebrew
regardless of compiler.

### If you need optimized output

**Calypsi** produces substantially better code than 816-tcc — it has a real optimizer and
understands the 65816's direct-page and bank-switching model. The download is **free of charge**,
but the license prohibits using it to "make your living… either directly or indirectly" — hobby
and small side-income use is explicitly permitted, commercial product development is not. It is
also closed-source, so it cannot be packaged for a redistributable apt repo. No paid commercial
tier exists. If you are writing a personal project, it is worth evaluating; see
[calypsi.cc](https://www.calypsi.cc/) for the current license terms.

### The gap nobody has filled

There is no open-source *optimizing* C compiler that targets the 65816 and is actively
maintained. llvm-mos is the most plausible path — the infrastructure is there, the assembler
works, and LLVM's backend is already a good optimizer — but the 65816 C compiler backend stalled
in 2024. This is the structural gap Zardoz/WDC816CC occupied for professional SNES development
in the 1990s, and it remains unfilled for open-source toolchains today.

---

## Refuted Claims (Community Lore, Not Confirmed)

These claims appear widely in forum discussions but were refuted 0-3 or 1-2 by independent
adversarial verification — they should not be treated as established fact:

| Claim | Vote | Source |
|-------|------|--------|
| WDC acquired Zardoz from Jim Goodnow | 0-3 | [assemblergames.org](https://assemblergames.org/viewtopic.php?t=60992) |
| WDC816CC is directly based on / identical to Zardoz | 0-3 | [nesdev.org](https://forums.nesdev.org/viewtopic.php?p=115927), [nesdev.org](https://forums.nesdev.org/viewtopic.php?t=10336) |
| WDC816CC license explicitly prohibits use with non-WDC processors (restricting SNES use) | 0-3 | [nesdev.org](https://forums.nesdev.org/viewtopic.php?p=115927) |
| Zardoz targeted Apple IIGS as well as SNES | 0-3 | [assemblergames.org](https://assemblergames.org/viewtopic.php?t=60992) |
| WDC816CC is a full ANSI C / ISO C99 implementation with Plum Hall validation | 0-3 | [816cc.pdf](https://www.westerndesigncenter.com/wdc/documentation/816cc.pdf) |
| Only three 65816 C compilers existed: ORCA/C, Zardoz, and TMorita's lcc retarget | 0-3 | [nesdev.org](https://forums.nesdev.org/viewtopic.php?t=12330) |
| WDC C compiler / Zardoz are the same product at different points in time | 1-2 | [6502.org](http://forum.6502.org/viewtopic.php?f=2&t=8067) |

---

## Open Questions

1. **Zardoz ↔ WDC lineage.** What documentary evidence — purchase agreement, changelog, release
   announcement — links Jim Goodnow's Zardoz to WDC816CC? The archived homepage
   ([1997 snapshot](https://web.archive.org/web/19970720124726/www.oro.net/~jimg/cdev.html))
   could not be fetched; its content may shed light.

2. **llvm-mos 65816 backend post-2024.** Has any work resumed on the C compiler backend since
   @asiekierka's October 2024 statement? Is there an active maintainer?

3. **Zardoz ABI vs. WDC816CC ABI.** Will Norris used Zardoz commercially — does the PHD/TCD
   Direct Page frame trick described in the WDC816CC manual match what Zardoz actually generated?
   If the ABIs differ, drmon's symbol/stack unwinding assumptions may need to account for both.

---

## Caveats

- The WDC816CC manual (revised 2013) is the strongest secondary source, but it describes a
  post-Zardoz commercial product. Its ABI may or may not match the original Zardoz design.
- llvm-mos backend status is time-bounded to October 2024; it may have changed.
- Source searches found no public repository or archive dump of either Zardoz or WDC816CC source
  code.
- ORCA/C source analysis is based on reading `Gen.pas` directly; the ORCA/C manual (not in the
  repo) would be the authoritative ABI reference.
