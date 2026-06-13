# llvm-mos 65816 C backend — contribution roadmap

## Context

This is a **roadmap for upstream contribution to [llvm-mos](https://github.com/llvm-mos/llvm-mos)**,
not a change to drdevtools. The work lands in `llvm-mos/llvm-mos` (compiler) and
`llvm-mos/llvm-mos-sdk` (platform target). drdevtools' stake: an optimizing open-source 65816 C
compiler that emits DWARF (spec landed Dec 2025) closes the loop *compile → optimized SNES ROM →
DWARF symbols → source-level debug in drmon* on fully-open tooling. See the
[investigation](../investigations/2026-06-13-llvm-mos-65816-backend.md) for the full status,
players, and rival/dead efforts; this plan is the **execution order** distilled from it.

**Current state (verified 2026-06-13):**

- 65816 **assembler + linker**: shipped. `FeatureW65816` subtarget exists in `MOSDevices.td`.
- 65816 **C code generation**: does not exist. Two open, design-only issues:
  [#320](https://github.com/llvm-mos/llvm-mos/issues/320) (24-bit address space) and
  [#321](https://github.com/llvm-mos/llvm-mos/issues/321) (16-bit register mode).
- **SNES SDK target**: does not exist — open issue
  [llvm-mos-sdk#415](https://github.com/llvm-mos/llvm-mos-sdk/issues/415). SDK ships 46 platforms
  (8 NES variants, C64, …) but no SNES.
- **No active implementer** since @asiekierka stepped away (his design notes in #320/#321 stand).

## Goal

A working, then optimizing, open-source 65816 C compiler with an SNES target, built **code-first**
(the maintainers bless ABI decisions only behind a running implementation).

## Approach — one track, three milestones (NOT two phases)

The naive framing is "land a cheap intermediate, *then* implement codegen." That's a false
dichotomy: the bulk of the cheap intermediate is the **SNES SDK target + an emulator/CI test loop**,
which is mandatory infrastructure for testing *any* codegen — you can't validate one far-pointer
lowering rule until a target exists to emit into and run. The only throwaway part is the generated
8-bit machine code, and that is **free** (the existing 6502 backend writes it). So it's one track:
build the bench, ship a validated unoptimized compiler, then layer optimization on top with a
regression baseline already in hand.

### M0 — toy + test bench (no new codegen)

Stand up the infrastructure every later milestone needs, using the *existing* 6502 backend.

- Create the SNES SDK target in llvm-mos-sdk ([#415](https://github.com/llvm-mos/llvm-mos-sdk/issues/415)):
  crt0, LoROM (and later HiROM) linker scripts, interrupt/reset vectors, memory map, ROM header.
- crt0 initializes the native-mode stack pointer to `SP = $01FF` (vs the 6502's `$FF`).
- Wire the 65816 subtarget triple so `clang` selects it; emit a `.sfc`/`.smc` ROM.
- Emulator smoke loop: a bank-0 C "hello world" (e.g. write a tilemap / known value) **boots and
  runs correctly in Mesen and bsnes**, driven from CI.
- Seed a **regression corpus**: a handful of small C programs with known-correct output.

Deliverable: a merged (or PR-ready) `mos-platform/snes` target + CI smoke. Risk: low — no compiler
changes, only SDK + build glue. This is the credibility artifact that pulls @asiekierka/@mysterymath
into guiding the #320 design.

### M1 — the cheap intermediate, useful form (first real codegen)

Add **[#320](https://github.com/llvm-mos/llvm-mos/issues/320)** — 24-bit address space / far
pointers — keeping registers 8-bit. Follow asiekierka's proposed five-address-space LLVM data layout
(addrspace `0`=32-bit far, `1`=8-bit direct page, `2`=16-bit absolute, `3`=24-bit packed far,
`4`=16-bit zero-bank). Default pointer 32-bit (cheaper than 24-bit on the 65816 — extra mode switch),
24-bit "packed" as a size option. Track near/far calls (JSR vs JSL).

Deliverable: a **working, multi-bank, unoptimized** 65816 C compiler. The corpus from M0 now exercises
real far-pointer codegen and becomes the regression baseline for M2.

### M2 — the optimizing payoff

Add **[#321](https://github.com/llvm-mos/llvm-mos/issues/321) stage 1** — 16-bit accumulator:
model X/Y/C as 16-bit and A as 8-bit; insert `REP`/`SEP` at a late codegen stage from the register
widths each instruction needs. X/Y stay 16-bit (xy16). Build on the
[jackoalan REP/SEP POC](https://github.com/jackoalan/llvm-mos/commit/ec070a70ba8d8b3d3a9da24b4216435f9575f6bb).
Then: hardware-stack ABI (16-bit SP + stack-relative addressing) and the calling convention.

**Later / out of initial scope:** #321 stage 2 (xy8/xy16 switching — asiekierka: "may well be a
pipe dream with our current resources").

## Calling-convention decision (open, blocks the ABI)

Undecided in #320/#321 and gating. Three candidates:

- **PHD/TCD direct-page frame** — what Zardoz / WDC816CC / ORCA-C all did
  ([Zardoz ABI](../investigations/2026-06-12-zardoz-65816-compiler.md#the-wdc816cc-abi-high-confidence)):
  fast (8-bit DP offsets) but hard 256-byte frame cap.
- **Hardware-stack-relative frame** — now viable on the 65816 (16-bit SP, S-indexed modes).
- **llvm-mos soft static stack** — carried over from the 6502.

The prior-art reference for this decision is the **documented** WDC816CC/ORCA-C ABI (from the WDC
compiler manual + ORCA/C source, captured in the
[Zardoz investigation](../investigations/2026-06-12-zardoz-65816-compiler.md#the-wdc816cc-abi-high-confidence))
— a manual, not anyone's memory. Zardoz shipped real commercial SNES titles (Will Norris among its
users), which establishes the ABI *worked in production*; but the codegen internals were the
compiler author's domain, not the game developers', so don't expect first-hand recall of the frame
layout. **A low-effort, no-code contribution: surface that documented prior art in #320/#321.**

## Verification

Acceptance test per milestone — each step is the bar that milestone must clear. Paste raw evidence
(build log, emulator screenshot/value dump) under each step as it's met, mark PASS/FAIL.

1. **M0 — target boots.** `mos-snes-clang hello.c -o hello.sfc` produces a valid ROM; the ROM boots
   in Mesen **and** bsnes and produces the known-correct output (value/tilemap). CI runs the smoke
   headless. (Evidence: build log + emulator dump.)

2. **M0 — bench reproducible.** The regression corpus (≥5 programs) builds and runs green in CI from
   a clean checkout. (Evidence: CI run.)

3. **M1 — far pointers work.** A C program that reads/writes data across ≥2 banks (far pointer
   dereference, a `>64 KB` data table) compiles and produces correct output in both emulators.
   8-bit-register codegen still passes the full M0 corpus (no regression). (Evidence: program output
   + corpus green.)

4. **M1 — address-space model honored.** Spot-check disassembly: near calls emit `JSR`, far calls
   emit `JSL`; direct-page vs absolute vs long accesses match the addrspace of the pointer.
   (Evidence: `llvm-objdump` excerpt.)

5. **M2 — 16-bit A + REP/SEP.** A 16-bit arithmetic kernel (e.g. fixed-point multiply-add loop)
   compiles with correct `REP`/`SEP` placement, produces correct results, and is **smaller/faster**
   than the M1 8-bit-mode output for the same source. The M0+M1 corpus stays green. (Evidence:
   size/cycle comparison + corpus green.)

6. **DWARF round-trip (drmon tie-in).** A `-g` build emits llvm-mos DWARF that a source-level
   debugger loads with correct line/variable mapping. (Evidence: drmon or `llvm-dwarfdump` against
   the ROM's symbols.)

## Dependencies & sequencing

- M0 → M1 → M2 strictly (each is the test bench for the next).
- #320 is shared by the "cheap intermediate" and "implement codegen" framings — unavoidable, first
  real codegen, do it at M1.
- Engage on the llvm-mos **Discord** before large PRs (design discussion lives there; the Feb 2024
  codegen breakdown originated there). Coordinate with @asiekierka (design owner) and @mysterymath
  (maintainer / ABI gate).

## Links

- [Investigation: status, players, rivals](../investigations/2026-06-13-llvm-mos-65816-backend.md)
- [Zardoz / WDC816CC ABI (calling-convention prior art)](../investigations/2026-06-12-zardoz-65816-compiler.md)
- Upstream: [#32 umbrella](https://github.com/llvm-mos/llvm-mos/issues/32) ·
  [#320 24-bit addr](https://github.com/llvm-mos/llvm-mos/issues/320) ·
  [#321 16-bit regs](https://github.com/llvm-mos/llvm-mos/issues/321) ·
  [sdk#415 SNES target](https://github.com/llvm-mos/llvm-mos-sdk/issues/415)
