# BUGS

Genuine bugs that lay dormant for years before something surfaced them — here, mostly the act of bringing the early-1990s DR Monitor (`drmon`) source to Linux. Not TODOs, not feature gaps: bugs that shipped in the DOS product because they *worked anyway*, were masked by a dead path, were never exercised, or were purely cosmetic and nobody bothered.

**Eligibility:** Only bugs whose buggy code was **authored before 2026-01-01** belong here. drmon's source entered version control via the SourceForge CVS import [`c835c3b`](https://github.com/developer-resources-co/drdevtools/commit/c835c3b) (2003-08-15); the code itself is ~1993–1994 vintage (drmon 2.1.30, 1994; many files carry `kts` date markers like `06-15-93`). Anything written in 2026+ as part of the Linux port is fresh-author error, not a dormant bug, and does not qualify. Verify origin with `git log --diff-filter=A -- <file>` / `git blame` before adding an entry.

**Ordering:** Entries are sorted **reverse-chronologically by the date the bug was finally surfaced/fixed** (the date in each entry's title — newest first, ending at the `## Template` section). Insert a new entry at the position its surface-date dictates; do not append blindly. Same-date entries group together.

Format per entry:
- **Title** with the date it was finally surfaced (`YYYY-MM-DD`, used for sorting)
- **Status:** FIXED `<sha>` | OPEN | INVESTIGATING
- **Symptom**, **Root cause**, **Why dormant**, **Fix**, **Diff** (the actual patch), **Commit/Investigation** (link)

---

## `ExpWinInput` read `oPtr->dataPtr` after `delete oPtr` — Expression-window close use-after-free — 2026-06-11

**Status:** FIXED [`0680e84`](https://github.com/developer-resources-co/drdevtools/commit/0680e84) (`devsys/tools/drmon/expr.cpp`).

**Symptom:** Closing the Expression window read freed memory — AddressSanitizer `heap-use-after-free` (READ) in `ExpWinInput` ([`expr.cpp:344`](../devsys/tools/drmon/expr.cpp)), the object having been `delete`d one line earlier at `expr.cpp:343`. No visible failure (the freed `_object` wasn't reused yet), so it never crashed in practice.

**Root cause:** The window-close branch deleted the object, then dereferenced it:
```cpp
delete oPtr;
if(!oPtr->dataPtr)        // read of freed _object
    { exprOpen = boolean::FALSE; exprObjPtr = NULL; }
```
`oPtr->dataPtr` distinguishes a pop-up from the normal window, but it's read *after* `oPtr` is freed.

**Why dormant:** Use-after-free of a just-freed heap block is silently fine until that memory is recycled; the glibc allocator hands the freed `_object` back later, not immediately, so the stale read returned the old value and behaved correctly for ~30 years. Only ASan (now the default build) flags it.

**Fix:** Capture the flag into a local before the deletes:
```diff
+	FLAG notPopup = !oPtr->dataPtr;	// capture BEFORE delete
	delete pWindow;
	delete oPtr;
-	if(!oPtr->dataPtr)
+	if(notPopup)
```

**Origin:** CVS import [`c835c3b`](https://github.com/developer-resources-co/drdevtools/commit/c835c3b) (2003-08-15), ~1993 code. **Commit / verification:** [`0680e84`](https://github.com/developer-resources-co/drdevtools/commit/0680e84); `task smoke` (opens every window + types into Expression + closes) is ASan-clean after the fix.

---

## Mouse-pointer backdrop dangled across a `scrBuffer` realloc — use-after-free on terminal resize — 2026-06-11

**Status:** FIXED [`2038358`](https://github.com/developer-resources-co/drdevtools/commit/2038358) (`devsys/tools/drmon/input.cpp` + `input.hpp`, `display.cpp`).

**Symptom:** Resizing the terminal triggered an AddressSanitizer `heap-use-after-free` (WRITE) in `ErasePointer` ([`input.cpp:86`](../devsys/tools/drmon/input.cpp)) via `UpdateScreen`, writing into a `scrBuffer` that `SetupDisplay` had just freed. Surfaced by the 2026 viewport-fill feature (live `KEY_RESIZE` re-layout); never reachable before, since `scrBuffer` was only ever (re)allocated at startup/mode-change, not at runtime with a pointer already drawn.

**Root cause:** `DrawPointer` caches the framebuffer base in a file-global `pointerScreen` (and the saved cell) so `ErasePointer` can restore it; `ErasePointer` writes through that cached pointer whenever `pointerDrawn` is set. `SetupDisplay` does `free(scrBuffer); scrBuffer = farmalloc(...)`. When a resize calls `SetupDisplay` while a pointer backdrop is live, `pointerScreen` is left dangling at the freed block, and the next `ErasePointer` writes through it.

**Why dormant:** Pre-port, `SetupDisplay`'s free+realloc of `scrBuffer` only ran at startup (no backdrop yet) or on a config display-mode change (rare); the realloc-while-pointer-drawn path didn't exist until live terminal resize was added.

**Fix:** Add `InvalidatePointer()` (clears `pointerDrawn`) and call it in `SetupDisplay` immediately before `free(scrBuffer)`, so a stale backdrop can never be written back through the freed pointer; the trailing `DisplayPointer()` re-establishes it against the new buffer.

**Origin:** `input.cpp`/`display.cpp` from CVS import [`c835c3b`](https://github.com/developer-resources-co/drdevtools/commit/c835c3b) (2003-08-15); latent until the resize feature exercised it. **Commit / verification:** [`2038358`](https://github.com/developer-resources-co/drdevtools/commit/2038358); resize churn 80→120→70→100→80 is ASan-clean.

---

## `CopyScreen`/`CopyMem` copied 2× their length on 64-bit — `len /= 4` assumed a 4-byte `long` — 2026-06-11

**Status:** FIXED [`adc92e2`](https://github.com/developer-resources-co/drdevtools/commit/adc92e2) (`devsys/tools/drmon/general.cpp`).

**Symptom:** With the Linux viewport-fill change (size the CGA screen to the terminal instead of a hardcoded 80×25), drmon rendered correctly at exactly 80×25 but **garbled the entire desktop at any width ≥ 83** — menu-structure text (Rate/Save/Index/Command/About items, hotkeys) and raw memory splattered across the body, two menu bars, etc. — and **`SIGABRT`'d when the terminal was shrunk**. Width-triggered (height was fine); the framebuffer drmon handed to the blit was already garbage, so it was drmon-side, not ncurses. AddressSanitizer pinned it exactly:

```
ERROR: AddressSanitizer: heap-buffer-overflow ... READ of size 8
    #0 CopyScreen(char*, char*, unsigned int)  general.cpp:319
    #1 UpdateScreen()                           display.cpp:117
    ... 0 bytes after 9600-byte region allocated by SetupDisplay (scrBuffer)
```

**Root cause:** `CopyScreen` (and its twin `CopyMem`) copied the framebuffer with a `long`-at-a-time loop, sizing it as:

```cpp
long far *source = (long far *)sBuff, *dest = (long far *)dBuff;
len /= 4;                       // # of 4-byte longs
for (i = 0; i < len; ++i) *dest++ = *source++;
```

`len /= 4` is correct only where `sizeof(long) == 4` (16-bit DOS / Win16). On 64-bit Linux `long` is **8 bytes**, so the loop runs `len/4` iterations copying 8 bytes each = **2 × `len` bytes** — every `UpdateScreen` it read `screenSize` bytes *past* `scrBuffer` and wrote `screenSize` bytes *past* `screen`. The over-read dragged whatever heap followed `scrBuffer` (menu `_menuItem` structures) into the visible framebuffer; the over-write corrupted the heap after `screen` (the shrink-time `SIGABRT`).

**Why dormant:** drmon ran at exactly **80×25** for its entire ~30-year life (the DOS text mode, and the hardcoded Linux Phase-1.5 size). At 80×25 the 2× copy ran 4000 bytes past each buffer onto spare heap that happened to be harmless, and — crucially — the blit only reads the first `screenSize` bytes of `screen`, which *are* a correct copy, so nothing visibly broke. The bug only bites once the screen is a different size: a larger framebuffer's 2× over-copy reaches live allocations (menu data, then heap metadata). Nothing exercised a non-80×25 screen until the viewport-fill work, so the 64-bit `long` mismatch stayed invisible from the 2003 CVS import (≈1993 code) until now.

**Fix:** Replace the hand-rolled `long`-copy with `memcpy(dBuff, sBuff, len)` in both `CopyScreen` and `CopyMem` — copies exactly `len` bytes regardless of `sizeof(long)`, and lets the compiler vectorize.

**Diff** (`devsys/tools/drmon/general.cpp`):
```diff
-	long far *source,far *dest;
-	source = (long far *)sBuff;
-	dest = (long far *)dBuff;
-	len /= 4;
-	for(i=0;i<len;++i)
-		*dest++ = *source++;
+	memcpy(dBuff, sBuff, len);
```

**Origin:** Imported via CVS commit [`c835c3b`](https://github.com/developer-resources-co/drdevtools/commit/c835c3b) (2003-08-15); the `len/=4` long-copy is ~1993-vintage DOS code, unchanged since. ~30 years dormant.

**Commit / verification:** Fix [`adc92e2`](https://github.com/developer-resources-co/drdevtools/commit/adc92e2). Found by enabling AddressSanitizer (now the default build, `-DDRMON_ASAN=ON`). Verified: clean render + **0 ASan errors** at 120×40 startup, live resize 80→120→70→100→80, and no shrink crash. The same ASan sweep also surfaced two overlapping-`strcpy` accelerator-strip bugs in `menu.cpp` (`CreateItems`, `_menu::_menu`) — `strcpy(p, p+1)` to delete the `&`, UB on overlap — fixed with `memmove` in the same commit.

---

## String gadgets stored their editable text in a read-only string literal — opening the Expression window (Alt+E) segfaulted — 2026-06-10

**Status:** FIXED [`0406bef`](https://github.com/developer-resources-co/drdevtools/commit/0406bef) (`devsys/tools/drmon/gadget.cpp` + `gadget.hpp`, `expr.cpp`, `command.cpp`, `filereq.cpp`).

**Symptom:** Pressing **Alt+E** (Open Expression window) crashed drmon with `SIGSEGV` (exit 139). gdb backtrace:

```
#0  PadString(char*, int)
#1  ActivateStrGadget(_window*, _gadget*)
#2  OpenExp(int, int)
#3  OpenExpWindow()
#4  GlobInput(_input*, _object*)
```

The same fault lurked in the Command window (Alt+K) and the file requester.

**Root cause:** String gadgets are edited **in place** — [`PadString`](../devsys/tools/drmon/general.cpp) (`general.cpp:368`) and the `StrGadgInput` key handlers ([`gadget.cpp:443-517`](../devsys/tools/drmon/gadget.cpp)) write directly into `gPtr->gadgText`, padding spaces out to `xSize` and inserting/deleting typed characters. But `AddExpWinGadgets` initialized that text with a **string literal**:

```cpp
gPtr->gadgText = "                                                     ";   // expr.cpp:120
...
pWindow->activeGadget = FindGadget(&pWindow->gadgBase,EXPGAD_EXP);
ActivateStrGadget(pWindow,pWindow->activeGadget);   // -> PadString(gadgText, xSize)
```

`PadString` walks to the terminator, then `*buffer = 0` — a **write into the literal**, which on a modern toolchain lives in read-only `.rodata`. Write to a read-only page → `SIGSEGV`. (Even if `PadString` were skipped, the first keystroke would fault on the same buffer.)

**Why dormant:** The 1990s DOS compilers this code targets (where `char* = "literal"` was even legal, not just `const char*`) placed string literals in **writable** data, so editing them worked and shipped fine for ~30 years. `g++` puts literals in read-only `.rodata`, so the long-standing idiom faults the instant the gadget is activated. The sibling builder `AddExpGadgets` (`expr.cpp:86`) happened to use the writable module buffer `expString` for the *same* gadget, which is why the GetExpr prompt path never crashed — the two paths were inconsistent and only the literal one was reachable via Alt+E.

**Fix:** Add [`SetGadgString(gPtr, text)`](../devsys/tools/drmon/gadget.cpp) — copies the initial text into a heap buffer sized for in-place editing (`max(strlen+1, xSize+1)`) — and use it for every **editable** string gadget: the Expression input + 4 result gadgets (`expr.cpp`), the Command input (`command.cpp`), and the file-requester dir/drive fields (`filereq.cpp`). The file field switches to the existing writable module buffer `fileReqFileName`, dropping a dead duplicate assignment. Read-only buttons/labels (`[Ok]`, `[Cancel]`, the close/zoom/scroll glyphs) keep their literals — they are drawn, never written.

**Diff** (representative — `expr.cpp` input gadget + the new helper in `gadget.cpp`):
```diff
-	gPtr->gadgText = "                                                     ";
+	SetGadgString(gPtr,"                                                     ");
```
```diff
+void
+SetGadgString(_gadget *gPtr,const char *text)
+{
+	int len = (int)strlen(text);
+	int cap = (int)gPtr->xSize + 1;		// room for PadString's pad-to-xSize + NUL
+	if(len + 1 > cap)
+		cap = len + 1;
+	gPtr->gadgText = (char *)malloc(cap);
+	if(gPtr->gadgText)
+		memcpy(gPtr->gadgText,text,len + 1);
+}
```

**Origin:** Imported to version control via CVS commit [`c835c3b`](https://github.com/developer-resources-co/drdevtools/commit/c835c3b) (2003-08-15); `git blame` puts both `PadString` and the literal assignment in that initial import (≈1993 code, never modified since). ~30 years dormant.

**Commit / verification:** Fix [`0406bef`](https://github.com/developer-resources-co/drdevtools/commit/0406bef). Regression guard: [`linux/smoke-test.sh`](../devsys/tools/drmon/linux/smoke-test.sh) / `task smoke` opens every window via its Alt-key under gdb and fails on any `SIGSEGV` (and types into the Expression gadget to exercise the edit path). Confirmed it **FAILS** with the backtrace above on the pre-fix line and **PASSES** after.

---

## `_window` ctor painted the auto-assigned window number before menus could opt out — dropdowns showed a stray "1" — 2026-06-10

**Status:** FIXED [`5f679ff`](https://github.com/developer-resources-co/drdevtools/commit/5f679ff) (`devsys/tools/drmon/window.cpp` ctor + `window.hpp` + `menu.cpp` `_menu::Render`).

**Symptom:** Every dropdown menu rendered a stray `1` embedded in the top border of its title bar, e.g. the File menu drew `╔═File═════════════════1═╗` instead of `╔═File═══════════════════╗`. The digit is meaningless on a menu — you cannot hot-key-switch to a transient dropdown — and changes with whatever window number happened to be free.

**Root cause:** The number is a legitimate feature on *real* windows: [`PrintTitle`](../devsys/tools/drmon/window.cpp) (`window.cpp:272-279`) draws each window's "hot-key number" ([`layer.hpp:107`](../devsys/tools/drmon/layer.hpp)) in the top-right of the title bar, so the user can jump to numbered debugger windows. The `_window` constructor ([`window.cpp:98-100`](../devsys/tools/drmon/window.cpp)) ran, in order:

```cpp
DrawBorder(this,border,xSize,ySize);
windowNum = GetAvailWindowNum();        // auto-assigns the next free number → 1
PrintTitle(this,windowTitleAttr);       // paints "1" into the title-bar buffer
```

Menus tried to opt out — but only *after_ construction had already painted the digit:

```cpp
pWindow = new _window(...);   // ctor already drew "1"
pWindow->windowNum = 0;       // too late — only suppresses future repaints
```

`PrintTitle`'s guard `if(pWindow->windowNum)` does respect `0`, and `windowNum==0` correctly excludes menus from the switch list (`GetAvailWindowNum`/`FindNumberedWindow`, `window.cpp:38-39,62`). But the `0` landed one step too late: the constructor had already auto-assigned `GetAvailWindowNum()` (→1, since no numbered debugger windows exist at startup) and rendered it into the buffer. `menu.cpp` never repainted the title, so the stale `1` persisted.

**Why dormant:** Purely cosmetic — the digit never affected behaviour, so it shipped unnoticed in the DOS product for ~30 years. This is shared `window.cpp`/`menu.cpp` logic with no platform `#ifdef`, so the DOS build rendered the same stray `1`; on a CGA text screen amid a dense debugger UI it was easy to overlook. The constructor's own `// won't work since oPtr is not yet valid` comment on the `PrintTitle` call shows the authors already knew constructor-time title painting was half-baked — they just never noticed it also leaked the window number onto menus. Surfaced now only because the Linux/ncurses port put fresh eyes on the rendering and captured it in a clean 80×25 screenshot.

**Fix:** Give the caller control of the number at construction instead of patching it afterward. Add an `int newWindowNum = -1` parameter to the `_window` ctor: `-1` (default) auto-assigns via `GetAvailWindowNum()` exactly as before for real switchable windows; `0` means unnumbered; `>0` is explicit. Set `windowNum` from the parameter *before* `PrintTitle`, so the digit is never drawn for menus. `menu.cpp` passes `0` at construction and drops the now-redundant post-construction `windowNum = 0`. No behaviour change for any real window (all keep the `-1` default).

**Diff** (`devsys/tools/drmon/window.cpp` + `menu.cpp`; `window.hpp` gets the matching default param):
```diff
-_window::_window( ..., char newBorder[8]) :
+_window::_window( ..., char newBorder[8], int newWindowNum) :
 ...
     DrawBorder(this,border,xSize,ySize);
-    windowNum = GetAvailWindowNum();
+    windowNum = (newWindowNum < 0) ? GetAvailWindowNum() : newWindowNum;
     PrintTitle(this,windowTitleAttr);
```
```diff
-    pWindow = new _window(xPos,yPos,xSize,ySize,text,(unsigned char)menuAttr,(char)menuChar,"\xC9\xCD\xBB\xBA\xBA\xC8\xCD\xBC");
-    pWindow->windowNum = 0;
+    pWindow = new _window(xPos,yPos,xSize,ySize,text,(unsigned char)menuAttr,(char)menuChar,"\xC9\xCD\xBB\xBA\xBA\xC8\xCD\xBC",0);
```

**Origin:** Imported to version control via CVS commit [`c835c3b`](https://github.com/developer-resources-co/drdevtools/commit/c835c3b) (2003-08-15); the windowing code is ~1993-vintage (`menu.cpp` carries `kts 06-15-93` markers). ~30 years dormant.

**Commit / verification:** Fix [`5f679ff`](https://github.com/developer-resources-co/drdevtools/commit/5f679ff). Verified headlessly: keyboard-opened File dropdown renders `╔═File═══════════════════╗` with no digit. (Mouse-opened menus share the same `_menu::Render` path; the digit was independent of how the menu was opened.)

---

## Template

```
## <Title with what failed> — YYYY-MM-DD

**Status:** FIXED `<sha>` | OPEN | INVESTIGATING

**Symptom:** What the user (or a test) sees.
**Root cause:** What's actually wrong, with file:line citations.
**Why dormant:** Which exercise path was missing, or what masked it.
**Fix:** Minimal description; link the commit / plan.
**Diff** (`path/to/file.cc`):
```diff
- buggy line
+ fixed line
```
**Commit / Investigation:** Link to the fix commit and/or docs/investigations/<date>-<slug>.md.
```
