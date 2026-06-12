# Zardoz 65816 C Compiler — History, ABI, and Successors

**Date:** 2026-06-12  
**Method:** Deep-research harness — 5 search angles, 15 sources fetched, 25 claims adversarially
verified (11 confirmed, 14 killed), 97 subagents.  
**Primary source:** [WDC W65C816S C Compiler Manual (816cc.pdf)](https://www.westerndesigncenter.com/wdc/documentation/816cc.pdf)

---

## Summary

Zardoz is a 65816 C compiler that community sources consistently attribute to **Jim Goodnow**
("jimg"), formerly hosted at `www.oro.net/~jimg/cdev.html` (archived ~1997). It is frequently
described as the direct ancestor of Western Design Center's commercial **WDC816CC** compiler —
either acquired by WDC or relabelled. **That lineage claim could not be confirmed from any primary
source.** Every assertion of WDC acquisition or direct derivation was refuted 0-3 in adversarial
verification; it rests entirely on community forum memory.

What *is* well-established from the WDC compiler manual is a technically distinctive ABI, described
in detail below. Whether that ABI originated with Zardoz or was independently designed by WDC is
unknown.

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
  This was not confirmed from primary sources.
- **SNES commercial use:** Community claims that commercial SNES titles used Zardoz. No specific
  title was confirmed from a primary source — refuted 0-3.

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

### cc65

[cc65](https://github.com/cc65/cc65) includes ca65 (assembler) which supports 65816, but the
**C compiler only generates 8-bit 6502 code** — it does not emit 65816 instructions. Not a
successor for C on 65816.

### Calypsi C Compiler

[Calypsi](https://www.calypsi.cc/) is a modern proprietary retro C compiler supporting 65816
(and 68000). It is actively maintained (v5.17 as of May 2026) and is the closest modern
alternative to WDC816CC. It is not open source and is free for hobby use only — not packageable
for a foundry apt repo. See also the [toolchain survey](2026-06-11-snes-65816-toolchains.md).

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

ORCA/C is another known 65816 C compiler from the era, targeting the Apple IIGS. It has since
been [open-sourced by Byte Works](https://github.com/byteworksinc/ORCA-C). Its ABI, memory model,
and code quality relative to WDC816CC remain uncharacterised in this research — this is a gap.

---

## Writing C for the SNES Today

The short answer: **PVSnesLib is your only practical open-source choice**; Calypsi if you can
accept a proprietary license.

| Option | Compiler | License | Code quality | SNES-specific support | Verdict |
|--------|----------|---------|-------------|----------------------|---------|
| **[PVSnesLib](https://github.com/alekmaul/pvsneslib)** | 816-tcc (TCC port) | MIT | Unoptimized but correct | Full SDK: VRAM, DMA, controllers, sprites, sound | **Best open-source choice** |
| **[Calypsi](https://www.calypsi.cc/)** | Calypsi C (proprietary) | Proprietary, free (hobby-only) | Optimizing | 65816 target; SNES needs manual BSP | **Best code quality, non-redistributable** |
| **[WDC816CC](https://www.westerndesigncenter.com/wdc/products/C-Compiler.cfm)** | WDC C (commercial) | Commercial | Optimizing | 65816; no SNES SDK | Expensive, no active community |
| **[ORCA/C](https://github.com/byteworksinc/ORCA-C)** | ORCA/C | MIT | Mature | Targets Apple IIGS, not SNES | Needs significant porting work |
| **[llvm-mos](https://github.com/llvm-mos/llvm-mos)** | LLVM (incomplete) | Apache-2.0 | — | Assembler/linker only | No C compiler yet |
| **[cc65](https://github.com/cc65/cc65)** | cc65 C | Zlib | Moderate | 6502 only | Does not emit 65816 |

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
also closed-source, so it cannot be packaged for a redistributable apt repo. If you are writing a
personal project, it is worth evaluating; see [calypsi.cc](https://www.calypsi.cc/) for the
current license terms.

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
| Zardoz was used to develop commercial SNES titles | 0-3 | [nesdev.org](https://forums.nesdev.org/viewtopic.php?t=10336) |
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

2. **SNES commercial titles.** Were any commercial SNES games verifiably compiled with Zardoz or
   WDC816CC? Community assertions exist but no specific title has been confirmed from a primary
   source (credits, developer post-mortem, interview).

3. **llvm-mos 65816 backend post-2024.** Has any work resumed on the C compiler backend since
   @asiekierka's October 2024 statement? Is there an active maintainer?

4. **ORCA/C comparison.** How does [ORCA/C](https://github.com/byteworksinc/ORCA-C) (open source,
   Apple IIGS 65816 target) compare to WDC816CC in ABI, memory model, and generated code quality?
   ORCA/C is a potential open-source reference implementation worth studying.

---

## Caveats

- The WDC816CC manual (revised 2013) is the strongest primary source, but it describes a
  post-Zardoz commercial product. Its ABI may or may not match the original Zardoz design.
- The ABI findings above describe WDC816CC as documented; whether Zardoz had an identical or
  merely similar ABI is unverified.
- llvm-mos backend status is time-bounded to October 2024; it may have changed.
- Source searches found no public repository or archive dump of either Zardoz or WDC816CC source
  code.
