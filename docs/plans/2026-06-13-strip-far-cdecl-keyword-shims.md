# Strip the `far` / `near` / `huge` / `cdecl` / `pascal` keyword shims

## Context

drmon is a 1990s DOS debugger now built only on Linux/GCC. The DOS memory-model
(`far`/`near`/`huge`) and calling-convention (`cdecl`/`pascal`) keywords — plus the `_`/`__`
variants and `_seg` — were `#define`d to **nothing** in `linux/linux_compat.hpp:11–29` and
force-included into every TU (`-include …/linux_compat.hpp`). ~318 `far *` pointer-qualifier
declarations across 44 files leaned on these macros. This is the larger, lower-value **sibling**
of the dead-`#ifdef`-arm cleanup ([plan](2026-06-12-drop-dead-dos-compiler-ifdef-arms.md),
closed out in `1bb766a`); that cleanup explicitly deferred this one.

**Goal:** remove the keyword tokens from declarations and delete the shim macros, so the source
is plain modern C++ with no compat lean. **Behavior-neutral** — the macros already expanded to
nothing, so the compiled translation units are unchanged.

**Why safe + self-verifying:** once `#define far` is gone, any surviving `far` keyword
(`char far *p`) is a hard **parse error** even under `-fpermissive -w` (unknown identifier at
parse time, before semantic relaxation). So the dual build (snesmon=SNES, genmon=GEN, + the two
DAP adapters) mechanically finds every miss; comments/strings never error.

## Scope (what changed)

- **Deleted** the keyword-shim block `linux/linux_compat.hpp:11–29` (16 `#define`s: `far near
  huge _far _near _huge __far __near __huge _seg` + `cdecl _cdecl __cdecl pascal _pascal
  __pascal`), replaced with a short explanatory comment. **Kept** the function macros below
  (`farmalloc`/`farfree`/`farcalloc`/`farrealloc`/`farcoreleft`/`coreleft`/`_fmemcpy`) — real
  DOS→libc mappings, a different concern.
- **Bulk strip** of the pointer-qualifier patterns across 46 files, two `*`-anchored sed
  regexes (can't touch English "far" in prose, which has no adjacent `*`):
  `s/\b_{0,2}far[ \t]*\*/*/g` (`far`/`_far`/`__far` before `*`, incl. no-space `far*` and the 3
  `_far` in chargfx) and `s/\*[ \t]*far\b/*/g` (the 10 reverse `char * far` return-types in
  general.cpp/.hpp).
- **Surgical edits** (7 non-pointer sites): `express.hpp:86` (`far` function qualifier),
  `mouse.hpp:34–35` (`far` array qualifier), `layer.hpp:169`+`layer.cpp:405` (`cdecl` on
  `FillRectLayer`, decl+def together), `send.hpp:7` (`_cdecl SwapLong`, reached via
  board.hpp/moninc.hpp), `geneio.cpp:5` (`_Cdecl SwapLong`; dead file, stripped for tidiness).
- `near`/`huge`/`pascal`/`_seg`/all `__`-variants had **0** code uses → only the `#define`
  deletion.

Net: **48 files changed, 293(+)/308(−)**. Slave-I/O (`slioemul`/`sliosnes`/`sliogen.cpp`) and
disassemblers (`dis816`/`dis68000.cpp`) are `#include`d by `which.cpp`, so both build targets
cover them transitively.

## Verification

1. **`task build`** — snesmon + genmon (+ DAP adapters) link clean; the keyword self-check
   (any missed keyword = parse error).

   ```
   [123/124] Linking CXX executable drmon-dap-snes
   [124/124] Linking CXX executable drmon-dap-gen
   /build/snesmon: ELF 64-bit LSB pie executable, x86-64 … not stripped
   /build/genmon:  ELF 64-bit LSB pie executable, x86-64 … not stripped
   build exit: 0   (zero error: lines)
   ```
   **PASS** — 124/124 targets, zero parse errors → zero surviving keyword uses anywhere reachable.

2. **`task smoke SYS=snes` / `SYS=gen`** — both open every window, no SIGSEGV.

   ```
   snes → PASS: opened windows via Alt-keys (M-e M-k M-r M-n M-m M-w M-b M-s M-i M-o M-a M-y)
                + typed into Expression — no SIGSEGV   (exit 0)
   gen  → PASS: … same …                                (exit 0)
   ```
   **PASS**.

3. **gcc -E no-op proof** — preprocess representative **compiled** TUs at pre-change
   (`git worktree` @ `1bb766a`) vs current, exact CMake flags, intra-line whitespace stripped
   (keeping newlines, so the token stream per line is compared and differences localize):

   ```
   screen.cpp   : IDENTICAL token stream (14984 lines) → PASS
   board.cpp    : IDENTICAL token stream (16141 lines) → PASS
   which.cpp    : IDENTICAL token stream (18610 lines) → PASS   (pulls in slio + dis)
   general.cpp  : IDENTICAL token stream (15689 lines) → PASS   (the reverse '* far' cases)
   dis816.cpp   : IDENTICAL token stream ( 2426 lines) → PASS
   sliosnes.cpp : IDENTICAL token stream (15353 lines) → PASS
   ```
   **PASS** — removing keywords that already expanded to nothing leaves the token stream
   byte-identical; the only raw difference is non-semantic inter-token whitespace. Provably
   behavior-neutral. (First attempt collapsed newlines too and mis-reported a diff; corrected to
   per-line whitespace stripping.)

4. **Residual keyword sweep** — `grep -rnwE 'far|near|huge|cdecl|pascal|_far|_seg|…'` over
   hand-written source, minus the kept `far*`/`coreleft` function-macro names.

   **PASS** — zero live keyword-position uses. The only remaining whole-word `far` is one
   **commented-out** prototype (`dis68.hpp:112 //unsigned int far Disassem(…)`) plus the new
   explanatory note + "far heap" section header in `linux_compat.hpp` — all comments, left as-is.

5. **Commit + promote** — one focused commit
   `refactor(drmon): strip far/near/huge/cdecl/pascal keyword shims`; `TODO.md` item `[ ]`→`[x]`.
