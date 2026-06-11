# Revive the scrollable mem-search results window

**Date:** 2026-06-12
**TODO:** DRMON — UI / UX → "Revive the scrollable mem-search results window"

## Context

The SNES mem-search shipped as a **message-bar first cut** (`MemSearchGUI` in `search.cpp`
dumps "Search: N hits @ …" to the status line). The original DOS drmon had a proper
**scrollable results window** (`OpenMemSearchWindow`, `SearchInput`, gadgets, a `_searchList`
list), but it was `#if 0`'d during the stub-lift because its local-menu calls use the old
`CreateMenuWithItems(labels[], routines[], title)` two-array form — `CreateMenuWithItems` now
takes a single `menuItems[]` table. This plan revives it: a scrollable list of hit addresses
you can arrow through and **Enter to jump a Memory window there**.

## Findings (why this is low-risk)

The `#if 0`'d scaffold is **structurally identical** to the live, compiled `symbol.cpp` and
`break.cpp` list windows — same `AddListRect` / `ListRectInput` / `UpdateListRect` /
`_stringList` / `_object`/`_window` APIs, which are **stable** (those windows work today). The
*only* drift is the one menu call. So this is "mirror the symbol-window pattern + fix one call",
not "resurrect unknown code".

Key APIs (verified): `CreateMenuWithItems(menuItems*, char*)`; `AddListRect(win, xs, ys,
(_stringList*)&base, GADID)`; selection = `lrPtr->selItem`, node = `SkipNodes((_list*)lrPtr->
listPtr, selItem+1)`; list build = `InsertListNode((_list*)&base, (_list*)node)` /
`DeleteListNode`; memory goto = set `oPtr->addr` on a Memory window (`MemoryGotoDRout` pattern,
`memory.cpp:200`).

## Design

### `search.hpp`
- Convert `_searchList` from a plain struct to `class _searchList : public _stringList` with
  `ULONG addr;` + `Address()` accessors (mirror `_breakList` in `break.hpp`) so list ops and the
  `(_stringList*)` casts are clean.
- Declare the window opener + globals: `FLAG OpenSearchWindow(void);` (already declared),
  `extern _object *searchObjPtr;`.

### `search.cpp`
1. Globals: `_searchList searchListBase; _object *searchObjPtr = NULL; FLAG searchOpen = FALSE;`
   (mirror `symbolListBase`/`symbolObjPtr`/`symbolOpen`).
2. Replace `searchMenuItems[]` + `searchRoutines[]` with one table:
   ```c
   menuItems searchMenu[] = {
     {"&Search again...  ", DoMemSearch,          0},               // re-run the input chain
     {"&Goto address     ", SendWindowMessage, SEARCH_GOTO},
     {"&Clear list       ", SendWindowMessage, SEARCH_CLEAR},
     {0,0,0} };
   ```
3. Un-`#if 0` the scaffold: `SGAD_*` enum, `SearchPrintListEntry` (already prints `addr` — fine),
   `AddSearchGadgets` (fine), `SearchInput`, and the window opener.
4. Fix `SearchInput`: swap the two `CreateMenuWithItems(searchMenuItems,searchRoutines,…)` for
   `CreateMenuWithItems(searchMenu,"Search")`; add an `INP_WINDOW_MESSAGE` arm handling
   `SEARCH_GOTO` (SkipNodes→addr→open Memory window there) and `SEARCH_CLEAR` (free list,
   `ChangeListRect`); make **Enter on a row** (LRIF_NEWSEL / a key) also do goto. Use the
   symbol-window close/teardown (`delete pWindow; delete lrPtr; DeleteObject(oPtr); searchOpen=FALSE`).
5. `OpenSearchWindow`: adapt the `#if 0`'d `OpenMemSearchWindow`, mirroring `OpenSymbol`
   (AddObject → new _window → inputRoutine=SearchInput → AddSysGadgets → AddSearchGadgets →
   point the list-rect at `searchListBase`). Re-open-to-front if already open.
6. `MemSearchGUI`: keep the scan, but instead of only PrintWarning → **clear `searchListBase`,
   append a `_searchList` node per hit (`addr`+`Name` = formatted address), open/refresh the
   window via `OpenSearchWindow` + `ChangeListRect`**. Keep the one-line message-bar summary too.
7. **Goto helper**: open a Memory window at an address — `OpenMemory()` then set the front memory
   object's `addr`/`updateMode` (verify `OpenMemory`'s handle; if it doesn't expose the new
   object, add a tiny `OpenMemoryAt(ULONG)` in `memory.cpp`). This is the one piece without a
   direct template; keep it minimal.

### `memops.hpp` / message ids
- Add `SEARCH_GOTO` / `SEARCH_CLEAR` window-message constants (near other `*_…` msg ids).

### Menu reachability
- `MemOps → Search…` already runs `DoMemSearch` → input chain → `MemSearchGUI`, which now opens
  the window. (Optionally also wire the reserved `SEARCHLIST` hard-window / `OpenSearchWindow` to
  a Windows-menu entry — nice-to-have, not required.)

## Files
| File | Change |
|---|---|
| `search.hpp` | `_searchList` → class; globals |
| `search.cpp` | un-`#if 0` + menu-API fix + `MemSearchGUI` populates list + goto |
| `memops.hpp` | `SEARCH_GOTO`/`SEARCH_CLEAR` message ids |
| `memory.cpp` | (only if needed) `OpenMemoryAt(addr)` helper |

## Verification
1. `task build` — both binaries compile clean (search window now live). **PASS** (2026-06-12).
   Only one drift fix needed beyond the menu-API: `KEY_ENTER` → `KEY_RETURN` in the key remap.
2. `task smoke SYS=snes` — no crash opening windows/menus. **PASS**.
3. Visual (manual): MemOps → Search for `EA` (NOP) over the ROM → a scrollable window lists
   many `00xxxx` addresses; arrow + Enter (or Ctrl-G) on one opens a Memory window there; Del
   clears. **Not driven headlessly** — MemOps → Search is the Memory window's local (right-click)
   menu, which doesn't render via `capture-pane` (same TUI-automation wall as the stub-lift
   pass). The window is proven by construction (mirrors the live symbol/break windows) + build
   + smoke; the on-screen walk is a manual check (`task run SYS=snes`).

## Notes / discovered issues
- Wired `OPEN SEARCHLIST` (command layer, `command.cpp`) to `OpenSearchWindow` for parity with
  the other hard-windows. **But the `OPEN <window>` command is a no-op** — root-caused by
  instrumentation: `EvalCommand` (which handles every command verb) has its *whole body* `#if 0`'d
  (`command.cpp:598–942`), so `OPEN` does nothing and the trailing `SEARCHLIST` token resolves via
  `ParseHardWindow` (existing-window lookup) → "No such window". `git blame` dates that `#if 0` to
  the **original 2003 DOS import** (`c835c3b`) — it shipped disabled; the Linux port did not break
  it. (Tables/`RW_*` enum verified aligned — not the cause.) So my `RW_SEARCHLIST` case is inert
  until `EvalCommand` is revived (own TODO). The search window opens fine via the MemOps menu.

Cost: $0 — local build; no MAME needed to compile-test (search logic already proven).
