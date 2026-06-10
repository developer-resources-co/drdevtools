# BUGS

Genuine bugs that lay dormant for years before something surfaced them — here, mostly the act of bringing the early-1990s DR Monitor (`drmon`) source to Linux. Not TODOs, not feature gaps: bugs that shipped in the DOS product because they *worked anyway*, were masked by a dead path, were never exercised, or were purely cosmetic and nobody bothered.

**Eligibility:** Only bugs whose buggy code was **authored before 2026-01-01** belong here. drmon's source entered version control via the SourceForge CVS import [`c835c3b`](https://github.com/developer-resources-co/drdevtools/commit/c835c3b) (2003-08-15); the code itself is ~1993–1994 vintage (drmon 2.1.30, 1994; many files carry `kts` date markers like `06-15-93`). Anything written in 2026+ as part of the Linux port is fresh-author error, not a dormant bug, and does not qualify. Verify origin with `git log --diff-filter=A -- <file>` / `git blame` before adding an entry.

**Ordering:** Entries are sorted **reverse-chronologically by the date the bug was finally surfaced/fixed** (the date in each entry's title — newest first, ending at the `## Template` section). Insert a new entry at the position its surface-date dictates; do not append blindly. Same-date entries group together.

Format per entry:
- **Title** with the date it was finally surfaced (`YYYY-MM-DD`, used for sorting)
- **Status:** FIXED `<sha>` | OPEN | INVESTIGATING
- **Symptom**, **Root cause**, **Why dormant**, **Fix**, **Diff** (the actual patch), **Commit/Investigation** (link)

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
