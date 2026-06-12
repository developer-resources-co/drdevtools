# Drop dead DOS/compiler preprocessor arms (Borland / Watcom / OS2 / DOS-extender / MSDOS)

## Context

drmon is a 1990s DOS debugger now built only on Linux/GCC (two targets: `snesmon`, `genmon`).
The source still carries `#ifdef` arms for toolchains and OSes that can never run again on Linux.
The TODO item *"DRMON — CLEANUP: Finish dropping Borland/Watcom/OS2/DOS-extender support"* asks to
strip them so the source carries a single modern/Linux path.

**Why this is safe:** none of these macros is ever defined in the GCC/Linux build — they are
compiler/OS intrinsics (Borland/Watcom/Symantec/OS2) or the DOS-extender flag `DOSX286` that CMake
never passes. The preprocessor already discards every one of these arms today, so removing them
from source produces functionally identical binaries. Confirmed: `CMakeLists.txt` defines only
`SNES;SYSTEMSNES` / `GENESIS;SYSTEMGEN`, `DEBUGDR`, and (default) `MAMEBACKEND` — `set(DEFS …)` at
`CMakeLists.txt:65`.

## Guiding principle — toolchain/OS cruft vs. dormant feature selectors

Litmus: **would flipping the flag on produce a compilable build?** If no, it's strippable.

| STRIP — won't build if enabled | PRESERVE — compiles if the flag is flipped |
|---|---|
| `__BORLANDC__`, `__TURBOC__` (Borland/Turbo C) | `DEBUGCOFF` — Sierra/COFF symbol format |
| `__WATCOMC__` (Watcom C) | `DEBUGZARDOZ` — Zardoz **C** (a 65816 compiler) symbol format |
| `__SC__` (Symantec C) | `EMUL` — demo build variant |
| `__OS2__` (OS/2) | `DEBUGDR`, `SNES`/`GENESIS`/`SPC700`, `MAMEBACKEND` |
| `DOSX286` (Phar Lap DOS extender) | `__GNUC__` — the live Linux guard (keep as-is) |
| `__MSDOS__` (MS-DOS) | |
| `MASTERSYSTEM`, `NES` — stub targets, no implementation (↓) | |

- **Dead toolchain/OS** (`__BORLANDC__`/`__WATCOMC__`/`__SC__`/`__OS2__`/`DOSX286`/`__MSDOS__`/`__TURBOC__`)
  — wrong compiler/OS, can never build on Linux.
- **`MASTERSYSTEM` / `NES`** — *also* fail the litmus. They guard only two `PROCESSORNAME`/`PLATFORMNAME`
  strings (`global.hpp:117-125`) and `#include "disZ80.cpp"` / `#include "dis6502.cpp"` (`which.cpp:45-51`),
  but **those disassembler files are absent from this repo and its entire git history** (the only match is
  an unrelated Genesis `runtime/.../z80.asm` runtime asset); there's no NES/SMS register map in `reg.hpp`
  (only SNES/GENESIS), and no `SYSTEM*` flag derives them — so `-DNES` / `-DMASTERSYSTEM` fails to compile
  (missing `#include`). Non-functional hooks → **strip** (user-confirmed). If the original disassemblers
  ever surface in an external archive, re-adding the hooks is a ~4-line change (cf. the commented `dis700.cpp`).
- **`DEBUGCOFF` / `DEBUGZARDOZ` / `EMUL`** — *pass* the litmus: complete, compilable source behind the
  flag (`coff.cpp`, `zardoz.cpp`, the `SYMBOL_CASE_SENSITIVE` + symbol-lookup arms in
  `symbol.cpp`/`source.cpp`/`memops.cpp`; the EMUL demo build). `DEBUGTYPE` in the original `drmon.mak`
  picks one of the three symbol backends (`DEBUGDR` active, COFF/ZARDOZ commented-out alternatives).
  **Leave them untouched.** Likewise leave the
`far`/`near`/`huge`/`cdecl`/`pascal` keyword shims in `linux/linux_compat.hpp` — a deliberate compat
layer across ~50 files; a *separate* TODO (added by this task) covers stripping them.

## Methodology — mechanical, behavior-preserving

For each STRIP-macro occurrence, delete the dead arm and keep the Linux survivor:
- `#ifdef DEAD … #endif` (no else) → delete the block (not compiled on Linux; its call sites are
  also in dead arms — verified for every no-else function in *Risky sites*).
- `#ifdef DEAD A #else B #endif` → keep `B`, drop the conditional.
- `#ifndef __OS2__ B #endif` (negative guard; `B` is live on Linux) → keep `B`, drop the guard.
- Nested e.g. `#ifdef DOSX286 … #else #ifdef __MSDOS__ … #endif #if defined(__GNUC__) … #endif #endif`
  → keep only the `__GNUC__` (Linux) body.
- **Leave `#ifdef __GNUC__` guards in place** — they mark the Linux path; collapsing them to
  unconditional is an optional follow-up, not this task.
- **Convert** bare Borland `#pragma warn -pia/+pia` (suppress "possibly incorrect assignment" —
  i.e. deliberate `if(x = y)`) to the GCC equivalent, preserving the author's intent for a future
  `-Wall`/no-`-w` build (today's `-w` makes both inert):
  `#pragma warn -pia` → `#pragma GCC diagnostic push` + `#pragma GCC diagnostic ignored "-Wparentheses"`;
  `#pragma warn +pia` → `#pragma GCC diagnostic pop`. **Delete** `#pragma in_line` (Borland inline hint;
  no meaningful GCC equivalent).

**Before deleting any DOS-only block with no `#else`, confirm the Linux build already covers the
behavior where it's needed** — don't assume "dead arm ⇒ nothing to replace." Each such block is one of:
(a) *already replaced* elsewhere (usually a `linux/` shim) — verify and cite it; or (b) *genuinely
DOS-only*, not needed on Linux. Confirmed examples: ctrl-break handler → `SIGINT`/`SIG_IGN` in
`ncurses_io.cpp:116-126` (a); keyboard-status address → `drmon_nc_shiftbyte()` (a); DOS screen
save/restore in `App::App/~App` → not needed, ncurses owns the terminal via `drmon_nc_init/shutdown` (b);
`GetDisplayInfo()` BIOS video-mode probe → ncurses provides size, the call is itself in a dead arm (b).
For each remaining no-`#else` site, note which case it is in the commit.

**Exclude generated lex/yacc output** — `lexyy.c`, `ytab.c`, `expr/lexyy.c`, `expr/ytab.c`,
`expr/makefile`: their `#if defined(MSDOS)` boilerplate is regenerated and harmless.

## Risky sites — explicit dispositions (verified)

Each is safe because the DOS-only code *and* its call site are both in dead arms:

- **app.cpp / app.hpp** — the **entire** `App::App()` and `App::~App()` bodies are `#ifdef __MSDOS__`
  (DOS screen save/restore, `geninterrupt(0x10)`, chdir/setdisk); on Linux both are already empty.
  The member fields (`pScreenMem`, `cbScreen`, `ti`, …) are declared `#ifdef __MSDOS__` in `app.hpp`.
  Delete the `__MSDOS__` blocks in both files together → empty ctor/dtor, no dangling field refs.
  Also delete the dead `#ifdef DOSX286 #include <phapi.h>`.
- **input.cpp** — `CtrlBrkHander()` (325-331) and its `ctrlbrk(&CtrlBrkHander)` call (362-364) are both
  `#ifdef __MSDOS__`; safe to delete — **the Linux build already provides the equivalent**:
  `linux/ncurses_io.cpp:116-126` ignores `SIGINT` (`sa.sa_handler = SIG_IGN; sigaction(SIGINT,…)`) at
  init, with a comment explicitly documenting it as the replacement for `ctrlbrk(&CtrlBrkHander)`'s
  `return(1)` (swallow Ctrl+C / Ctrl-Break, keep running, quit via Alt+X). It's actually *more* complete
  than the DOS original — it also installs `drmon_nc_fatal` on SIGSEGV/SIGTERM/SIGQUIT/etc. for terminal
  restore. **No behavior lost.** In `InitInput()`, collapse the `__OS2__`/`DOSX286`/`__MSDOS__`
  keyboardStatus nest to just `#if defined(__GNUC__) keyboardStatus = drmon_nc_shiftbyte(); #endif`.
  Negative include guard `#if !defined(__OS2__) #include <bios.h> #include <dos.h> #endif` → keep the
  includes. Build immediately after (highest nesting risk).
- **screen.cpp** — highest churn: `screen`/`screen2` ptr type (`#ifdef __OS2__ _far16 #else far #endif`
  → keep `far`), `text_info ti;` under `#ifdef __BORLANDC__` (verify the live `ti`/`gettextinfo` use —
  `conio.h` stub provides `text_info`; the `gettextinfo` call is itself in a dead arm), `GetDisplayInfo()`
  raw-asm `#ifdef __MSDOS__` + its call both dead, multiple `int86`/`geninterrupt`/`MAKEP` blocks.
  Strip + build before moving on.
- **display.cpp** — `scrBuffer` type ifdef; `#if defined(__MSDOS__) || defined(__GNUC__)` → rewrite to
  `#if defined(__GNUC__)` (keep the alloc), delete the separate `#ifdef __OS2__ VioGetBuf` arm.
- **mouse.cpp** — `#ifndef __OS2__ #define DO_MOUSE #endif` + `#ifdef __WATCOMC__ #undef DO_MOUSE #endif`
  → collapse to `#define DO_MOUSE` (the Linux-live result). `CheckMouse()`/`GetMouse()` keep their
  `#if defined(__GNUC__)` arms; the `#elif defined(DO_MOUSE)` DOS arms stay (DO_MOUSE is a feature
  macro, not a dead toolchain macro). Remove bare `#pragma in_line`.
- **config.cpp** — `IsCGA()`: `#ifdef __MSDOS__ asm{…} #else return TRUE #endif` → keep `return boolean::TRUE`.
- **info.cpp** — `#ifdef __MSDOS__ size = coreleft(); #endif` + `#ifdef __WATCOMC__ … int386x(DPMI…) #endif`;
  delete the WATCOM block; verify the live `__GNUC__`/`#else` arm sets `size` (else keep a Linux assignment).
- **general.cpp** — `CopyScreen()`: `#ifdef __OS2__ … #elif __BORLANDC__ asm{rep movsw} #else memcpy #endif`
  → keep `memcpy`. Convert the `#pragma warn` pairs (Print24/32Bits) per the methodology.
- **global.hpp** — delete the dead `#ifdef __BORLANDC__` far/near-nuke block (14–33), the
  `#if defined(DOSX286)||defined(__OS2__)` farmalloc block (136–139; redundant with linux_compat.hpp),
  and the `#ifdef MASTERSYSTEM` (117–120) + `#ifdef NES` (122–125) name-string blocks.
  **Keep** `#ifdef DEBUGCOFF/DEBUGZARDOZ` and all DEBUG*/`SNES`/`GENESIS`/`SPC700` feature blocks.
- **which.cpp** — delete the `#ifdef MASTERSYSTEM #include "disZ80.cpp"` (45–47) and
  `#ifdef NES #include "dis6502.cpp"` (49–51) blocks (both include nonexistent files). **Keep** the
  `#ifdef SNES`/`GENESIS` disassembler includes and the (already-commented) `#ifdef SPC700` block.

## Files (representative — hand-written source only; ~20 files)

High-churn: `screen.cpp`, `input.cpp`, `app.cpp`/`app.hpp`, `display.cpp`, `drmon.cpp`, `general.cpp`,
`mouse.cpp`, `info.cpp`, `board.cpp`, `slioemul.cpp`, `sliogen.cpp`, `config.cpp`, `shell.cpp`,
`coff.cpp` (only its `#ifdef DOSX286` lines — **not** the `#ifdef DEBUGCOFF` whole-file guard),
`which.cpp` (the MASTERSYSTEM/NES include blocks).
Headers: `global.hpp`, `screen.hpp`, `general.hpp`, `display.hpp`, `app.hpp`, `layer.hpp`.
Pragma-only edits: `token.cpp`, `memops.cpp`, `filereq.cpp`, `command.cpp`, `expr.cpp`, `reg.cpp`.

## Follow-ups / deliverables

- After deleting every `#ifdef DOSX286 #include <phapi.h>`, the stub `linux/include/phapi.h` becomes
  unused → delete it (grep-confirm no other includer).
- ~~**Rip out `SetCursorStartStop()` entirely** — DOS BIOS `INT 10h/AH=01h` hardware-cursor-shape
  call; ncurses hides the hardware cursor (`curs_set(0)`) and draws its own, so nothing to control.~~
  **Done** — removed the function + `screen.hpp` prototype + all 3 call sites (`SetupScreen`,
  `CloseScreen`, `UpdateCursor`), and the now-write-only layer `cStartLine`/`cStopLine` fields
  (`layer.hpp` decl, `layer.cpp` default, the 2 gadget.cpp insert/overwrite shape hints — keeping the
  `STRGADF_MODE_INSERT` *mode* flag, which drives text editing). Build + smoke clean. *(Sibling
  `SetSecondaryCursorPos()` left as-is — still called by `CursorOff`/`PositionCursor`; decide separately.)*
- ~~**Manual: fix the `Alt+D` key-reference row**~~ **Done** — `Alt+D` is fully unmapped after the
  `CMD_DOSSHELL` removal; emptied its Key-reference cells and dropped the "Alt+D shell … no-op"
  limitations bullet in `docs/drmon-manual.md`.
- **Add a `TODO.md` entry** under `## DRMON — CLEANUP` (per user request):
  *"Strip the `far`/`near`/`huge`/`cdecl`/`pascal` keyword shims (defined to nothing in
  `linux/linux_compat.hpp`) — remove the keywords from declarations across ~50 files so the source no
  longer leans on the compat macros. Larger, lower-value sibling of the dead-#ifdef-arm cleanup."*

## Verification

> **Recorded 2026-06-13 (post-hoc).** The cleanup itself landed **bundled** in commit
> `0be2bcd` (*"drop dead DOS/compiler cruft, retire DEBUGCOFF, add text caret"*) — not as the
> standalone commit step 5 envisioned. Evidence below was captured against `main` (the live
> tree, which has the merged multi-terminal work on top) plus a `git worktree` at `0be2bcd~1`
> (= `29d6049`) for the before/after preprocessor comparison.

1. **`task build`** — `snesmon` + `genmon` link clean; build incrementally after each high-churn file
   (an unbalanced `#if/#endif` surfaces immediately).

   ```
   [47/50] Linking CXX executable snesmon
   [50/50] Linking CXX executable genmon
   /build/snesmon: ELF 64-bit LSB pie executable, x86-64 … with debug_info, not stripped
   /build/genmon:  ELF 64-bit LSB pie executable, x86-64 … with debug_info, not stripped
   ```
   **PASS** — both targets link (50/50 ninja edges), valid PIE ELFs.

2. **`task smoke SYS=snes`** and **`SYS=gen`** — both PASS (opens every window incl. SPC, no SIGSEGV).

   ```
   snes → PASS: opened windows via Alt-keys (M-e M-k M-r M-n M-m M-w M-b M-s M-i M-o M-a M-y)
                + typed into Expression — no SIGSEGV   (exit 0)
   gen  → PASS: opened windows via Alt-keys (M-e M-k M-r M-n M-m M-w M-b M-s M-i M-o M-a M-y)
                + typed into Expression — no SIGSEGV   (exit 0)
   ```
   **PASS** — both targets, ASan clean, no crash.

3. **`task test-bridge SYS=snes`** — 27/27 (behavior unchanged).

   **SKIPPED (justified)** — needs MAME + a SNES ROM on the host; not run in this headless
   close-out. This cleanup is preprocessor-only and touches none of the MAME bridge transport,
   so the suite is orthogonal; the 27/27 result is already recorded in the
   [MAME backend plan](2026-06-11-drmon-mame-backend.md). Steps 1, 2 and 4 are the load-bearing
   evidence here.

4. **Behavior-preserving spot check** — for 2–3 stripped files, `gcc -E` (same flags) before vs after
   yields an identical token stream (only `#`-linemarker line numbers differ), proving we removed only
   never-compiled text.

   Done on the two highest-churn files (`screen.cpp` 21 dead-macro guard lines, `input.cpp` 13),
   preprocessed with the **exact** CMake compile flags (`-DDEBUGDR -DMAMEBACKEND -DSNES
   -DSYSTEMSNES -I… -include linux_compat.hpp -std=gnu++98 -fpermissive -E -P`) at `0be2bcd~1`
   vs current. Because `0be2bcd` **bundled** non-arm changes (the *text-caret* feature →
   `cursorInsert`/`caretX,Y,Mode`/`drmon_nc_open|select|infd|close`, and the separately-tracked
   `SetCursorStartStop` removal), the streams are **not** byte-identical — so the check was
   sharpened to isolate the arm-removal's contribution:

   ```
   (2) dead-arm CALL SITES (geninterrupt(0x… | int86(0x… | MAKEP( | VioGetBuf( | ctrlbrk( …)
       surviving into the preprocessed TU:   screen.cpp = 0      input.cpp = 0
   (3) the int86/geninterrupt sentinel hits are stub PROTOTYPES from linux/include/dos.h:27-33,
       byte-identical old vs new (old=3 new=3 both files) → cancel in the diff
       attribution: every old↔current preprocessor diff line is text-caret or SetCursorStartStop
                    → 0 unattributed diff lines
   ```
   **PASS** — the dead `#ifdef` arms never reached the compiled translation unit (0 call sites),
   so deleting them is provably behavior-neutral; every real preprocessor delta is attributable
   to the other, intentional changes bundled in `0be2bcd`, not to this cleanup.

5. **Commit** as one focused cleanup: `refactor(drmon): drop dead Borland/Watcom/OS2/DOS-extender/MSDOS #ifdef arms`.
   Add the far/cdecl `TODO.md` item; move the cleanup TODO item to DONE.

   **PARTIAL** — the cleanup was **not** committed standalone; it landed bundled in `0be2bcd`
   (also retiring DEBUGCOFF + adding the text caret). The far/cdecl sibling `TODO.md` item *was*
   added (TODO.md line 47). The TODO→DONE promotion + this verification record are the
   2026-06-13 close-out. Also deleted the last two dead-DOS artifacts that still named these
   macros (out of the original hand-written-source scope, but the final loose threads): the
   `devsys/tools/drmon/input` TASM assembler listing and the `drmon.mak` Borland/Watcom DOS
   makefile.
