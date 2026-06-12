# Fix DOS 8.3-era filename/path buffer overflows

## Context

drmon is a 1990s DOS debugger now run on Linux. Several filename/path buffers are still sized
for DOS 8.3 names (12–14 bytes) but now receive real Linux paths/names (up to 255 per component),
and the writes into them are **unchecked** `strcpy`/`tmpnam`. With ASan on by default, selecting
or even *listing* a file/dir with a >12-char name in the file requester, setting a log file, or
saving a profile setting **overflows** a static/global/stack buffer — a real, reachable memory
bug (`TODO.md` → DRMON — CLEANUP).

`_MAX_PATH` (= 260, defined in `linux/linux_compat.hpp:36`, force-included) is already the
convention used by `help.cpp`/`sld.hpp`/`filename.hpp`/`app.hpp`. The fix: resize the DOS-era
buffers to `_MAX_PATH`, make the copies bounded, delete a now-dead constant, and ship a
regression guard that catches the overflow.

## The buffers & overflow sites (verified)

| Buffer | Decl | Overflow write(s) | Source (can exceed size) |
|---|---|---|---|
| `fileReqFileName[13]` | `filereq.cpp:55` (static) | `strcpy(gadgText,…Name())` `filereq.cpp:245,488` | a listed/selected filename (`_dos_findfirst`→`fileBlock.name`, ≤255) |
| DIR gadget heap buf (cap 13) | `filereq.cpp:149` `SetGadgString(…)` | `strcpy(gadgText,…Name())` `filereq.cpp:254,439,472` | a listed/selected **directory** name |
| `patternString[13]` | `filereq.cpp:54` (global, extern `filereq.hpp:14`) | gadget-edited in place (`filereq.cpp:180`) | low risk (literal writes) — resize for consistency |
| `logFileName[14]` | `monmenu.cpp:16` (global, extern `monmenu.hpp:22`) | `strcpy(logFileName,fileName)` `monmenu.cpp:58` | the requester's filename → `fopen` at `manager.cpp:186` |
| `szTempName[13]` | `profile.cpp:298` (stack) | `tmpnam(szTempName)` `profile.cpp:303` | `tmpnam` writes up to `L_tmpnam` (~20) |
| `#define FILENAME_SIZE 9` | `monmenu.cpp:14` | — | **dead** (0 refs since DEBUGCOFF `LoadCOFFFile` removed) → delete |

Requester opened via the **File** menu → **&Load…** (`monmenu.cpp:124`, `FileMenuLoad`→
`DoFileReq("Load File",…)`), i.e. **Alt-F then L** headlessly; `ScanDir` (`filereq.cpp:60`)
then strcpy's the first listed file/dir into the gadget buffers — the regression trigger.

## Fix

1. **Resize to `_MAX_PATH`** (keep init strings/semantics, just widen the array):
   - `filereq.cpp:54` `patternString[_MAX_PATH]` + `filereq.hpp:14` extern.
   - `filereq.cpp:55` `fileReqFileName[_MAX_PATH]`.
   - `monmenu.cpp:16` `logFileName[_MAX_PATH]` + `monmenu.hpp:22` extern.
   - `profile.cpp:298` `szTempName[_MAX_PATH]` (makes `tmpnam` safe: `L_tmpnam` ≪ 260; keep
     `tmpnam` — switching to `mkstemp` is a behavior change, out of TODO scope; note in a comment).
2. **DIR gadget gets a real buffer**: add `static char fileReqDirName[_MAX_PATH]` and replace
   `SetGadgString(gPtr,"            ")` at `filereq.cpp:149` with `gPtr->gadgText = fileReqDirName;`
   (mirrors the FILE/PATTERN gadgets, which point at static `_MAX_PATH` buffers — no per-scan
   heap alloc/leak, and the strcpy sites now write into a 260-byte dest).
3. **Bound every copy**: replace the unchecked `strcpy(gPtr->gadgText,X->Name())` at
   `filereq.cpp:245,254,439,472,488` and `strcpy(logFileName,fileName)` at `monmenu.cpp:58`
   with `snprintf(dst,_MAX_PATH,"%s",src)` (truncates safely; standard, no new helper). Verify
   `<stdio.h>` is in scope (it is — `sprintf` already used). Check `LoadFile`'s
   `strcpy(textBuffer,fileName)` (`monmenu.cpp:26`): if `textBuffer` is `_MAX_PATH`-or-larger
   leave it; otherwise bound it the same way.
4. **Delete** `#define FILENAME_SIZE 9` (`monmenu.cpp:14`).

## Regression guard (ship in the same commit)

New `linux/overflow-test.sh <binary>` (mirrors `smoke-test.sh`/`signal-test.sh`) + `task overflow`
Taskfile target (`deps:[build]`, `SYS=snes|gen`):
- Make a scratch dir containing a long-named file (e.g. `0000_overflow_regression_name.bin`,
  sorts first so `ScanDir` picks it) **and** a long-named subdir.
- Run drmon in that dir under tmux with `ASAN_OPTIONS=…:log_path=<dir>/asan:abort_on_error=0`,
  send `M-f` then `l` to open the Load File requester, let `ScanDir` run, quit.
- `grep -q 'buffer-overflow' <dir>/asan.*` → **FAIL if found**.

Prove the guard works: run it against the **pre-fix** binary (git worktree @ current HEAD
`6c51085`) → expect FAIL (ASan global/heap-buffer-overflow report); against the fixed tree →
PASS. Both recorded in the Verification section below.

## Verification (recorded 2026-06-13)

> **Reachability note found during execution:** `_dos_findfirst` is a no-op stub
> (`linux/dos_stubs.cpp:125` returns 1), so the requester currently *lists nothing* and typed
> input is capped at the gadget's `xSize`=12 — the `fileReqFileName`/`logFileName` overflows are
> **latent** (resized + bounded as defense-in-depth, and correct the moment `_dos_findfirst` is
> implemented). The **reachable** overflow is `tmpnam(szTempName[13])`, hit in-app via
> Settings→Save (`SaveConfig`→`WritePrivateProfileInt`, `config.cpp:395`) — that's what the
> regression test exercises. `textBuffer` (`monmenu.cpp` load path) is already `[160+_MAX_PATH]`
> (`drmon.cpp:23`) — left as-is.

1. **`task build`** — snesmon + genmon link clean (`snprintf` resolves; all edits compile).
   ```
   /build/snesmon: ELF 64-bit LSB pie executable … not stripped
   /build/genmon:  ELF 64-bit LSB pie executable … not stripped   (build exit 0)
   ```
   **PASS**.

2. **Regression guard — `task overflow`** (compiles real `profile.cpp` + `linux/overflow-test.cpp`
   under ASan, calls the real `WritePrivateProfileString`):
   ```
   (A) PRE-FIX  (worktree @ 6c51085, szTempName[13]):
       ERROR: AddressSanitizer: stack-buffer-overflow … WRITE of size 16
       [64, 77) 'szTempName' (line 298) <== Memory access … overflows this variable
       SUMMARY: … profile.cpp:303 in WritePrivateProfileString
       FAIL: AddressSanitizer flagged a buffer overflow (rc=1)        ← catches the bug
   (B) FIXED    (szTempName[_MAX_PATH]):
       overflow-test: WritePrivateProfileString completed, no buffer overflow
       PASS: … no buffer overflow under ASan                          (exit 0)
   ```
   **PASS** — fails before the fix, passes after. Regression guard proven.

3. **`task smoke SYS=snes` + `SYS=gen`** — both PASS (every window opens, no SIGSEGV; no
   regression). *(Running the two in parallel once raced on the shared `/build` dir → a spurious
   `ninja: failed recompaction` / exit 201; run sequentially both are clean — a harness race, not
   a code issue.)*
   ```
   snes → PASS … no SIGSEGV   (exit 0)
   gen  → PASS … no SIGSEGV   (exit 0)
   ```
   **PASS**.

4. **Residual checks** — `grep -rnw FILENAME_SIZE` → **0 hits**; the 5 buffers
   (`patternString`/`fileReqFileName`/`fileReqDirName`/`logFileName`/`szTempName`) are all
   `[_MAX_PATH]`; the user-data copies (`filereq.cpp:245/254/439/472/488`, `monmenu.cpp:58`) are
   `snprintf`-bounded; remaining `strcpy(patternString,"*.xxx       ")` sites are fixed 12-char
   literals into the 260-byte buffer (safe). No leftover `[13]`/`[14]` filename buffers. **PASS**.

5. Commit `fix(drmon): size DOS 8.3 filename/path buffers to _MAX_PATH + bound the copies`;
   `TODO.md` item `[ ]` → `[x]`. **Done.**

## Files

- `devsys/tools/drmon/filereq.cpp` (buffers 54/55, new `fileReqDirName`, gadget 149, copies
  245/254/439/472/488) + `filereq.hpp:14`.
- `devsys/tools/drmon/monmenu.cpp` (14 delete, 16 resize, 58 bound) + `monmenu.hpp:22`.
- `devsys/tools/drmon/profile.cpp:298` (resize).
- New `devsys/tools/drmon/linux/overflow-test.{cpp,sh}` + `Taskfile.yml` (`overflow` target).
- `TODO.md` (promote on completion).
