# Per-terminal different sizes

**Date:** 2026-06-13  
**Branch:** `feature/drmon-option-a-multiterm`  
**Status:** In progress

## Problem

In the Option A multi-terminal implementation `screenWidth`/`screenHeight`/`screenSize` are
shared globals — every desktop sees the same values.  When a spawned terminal is a different
size than the primary, its buffers (`screen`, `scrBuffer`) are allocated at the wrong size and
rendering clips incorrectly.

## Fix

Add `screenW`/`screenH`/`screenSz` fields to `Desktop` and include them in the
`desktop_save`/`desktop_load` swap.  No other callers change: `SetupScreen()` already calls
`drmon_nc_size()` on the currently-selected terminal and writes `screenWidth`/`screenHeight`/
`screenSize` fresh, so `InitDesktop()` (called while the new terminal is selected) picks up the
correct dimensions automatically.  `ReSizeViewport()` (called per-desktop in the main loop)
also works unchanged — it calls `SetupScreen()` after `switchDesktop` has already restored the
right terminal.

## Files changed

| File | Change |
|------|--------|
| `desktop.hpp` | Add `unsigned short screenW, screenH, screenSz` to `Desktop` |
| `desktop.cpp` | Add `screenSize` to extern; save/restore all three in `desktop_save`/`desktop_load` |

## Verification

1. ~~**Build** — `task build SYS=snes` links cleanly.~~

   ```
   [44/44] Linking CXX executable genmon
   /build/snesmon: ELF 64-bit LSB pie executable …
   /build/genmon:  ELF 64-bit LSB pie executable …
   ```
   **PASS**

2. ~~**Smoke** — `task smoke SYS=snes` passes (single-desktop behaviour unchanged).~~

   ```
   PASS: opened windows via Alt-keys (M-e M-k M-r M-n M-m M-w M-b M-s M-i M-o M-a M-y)
   + typed into Expression — no SIGSEGV
   ```
   **PASS**

3. **Different sizes** — spawn a second terminal at a different size (resize before or after
   New Window); each desktop renders correctly in its own COLS×LINES without bleed or garble.
   **(Needs live desktop — not yet recorded.)**
