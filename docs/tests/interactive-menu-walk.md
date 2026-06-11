# Interactive menu-walk — snesmon connected to MAME

Three menu paths that require a live connected session and cannot be driven headlessly.
All underlying functions are proven by `spikes/verify_features.py` (8/8 pass); this
test confirms the on-screen menu navigation reaches them.

**Linked plan:** [docs/plans/2026-06-11-lift-snes-out-of-scope-stubs.md](../plans/2026-06-11-lift-snes-out-of-scope-stubs.md) (verification items 2, 3/4, 5)

---

## Prerequisites

- MAME 0.277 installed (`/usr/games/mame` — already present on this machine).
- Docker image built: `task image` (one-time, cached).
- snesmon binary built: `task build` (skip if already done and no changes).
- Two terminals open side-by-side; run MAME in one, snesmon in the other.

---

## Session setup — do this once before all three tests

**Terminal 1 — start MAME with the test ROM:**

```bash
task mame SYS=snes CART=devsys/tools/drmon/linux/test-roms/drmon-test.sfc
```

MAME opens a windowed SNES. You'll see a blank/diagnostic screen (the test ROM runs
a `BRK` / halt loop — that's normal). Dismiss MAME's initial "game is not working" warning
with **OK** if it appears.

> **Test 1 requires a real game ROM.** The test ROM halts before running any SNES PPU
> init code, so its VRAM is all zeros by design — indistinguishable from the old stub.
> Use a game that draws to screen instead:
> `CART="roms/snes/Donkey Kong Country (U) (V1.2).smc"`

**Terminal 2 — start snesmon:**

```bash
task run SYS=snes
```

The TUI comes up. The status bar at the top-right should show **Running** (not `Slave Dead`).
If it shows `Slave Dead`, the bridge isn't connected — check that `task mame` started without
errors and is still running.

---

## Test 1 — PPU window (Memory → Type → PPU)

**Goal:** Open an MTYPE_PPU memory window that reads live SNES VRAM instead of all zeros.

**Steps:**

1. Press **Alt+M** to open (or focus) the Memory window.
2. Press **Ctrl+F10** to open the Memory window's local menu.
3. Arrow down to **Memory Type >>** → press **Enter**.
4. Arrow down to **PPU** → press **Enter**.

The window title changes to show `PPU`, and the content switches to a hex dump of
SNES VRAM (`$0000`–`$FFFF`, 64 KB tile data).

**What to look for:**

The PPU window should show non-zero tile pixel data in the first few rows. Press **F2**
(Run) and watch bytes change as the title screen animates; press **F3** (Stop) to freeze and
compare. Let the title screen run a few seconds first so the PPU has time to load tiles.

To cross-check: press **Tab** in the MAME window to open MAME's internal menu →
**Video Options** → enable the tilemap viewer. The tile patterns there are drawn from the
same `m_vram` array the PPU window reads.

**Pass:** PPU window shows non-zero bytes that change while running. All-zero content with a
game ROM means the `RP` command is broken (not a VRAM-is-empty false negative).

---

## Test 2 — mem-search (MemOps → Search…)

**Goal:** Search work-RAM for a known value; confirm hit addresses match a manual read.

**Setup — find a known byte in live work-RAM:**

The game (DKC or similar) already has non-zero data in work-RAM. Use that instead of
planting a value.

1. Press **Alt+M** to focus the Memory window.
2. Press **Ctrl+F10** → **Memory Type >>** → **Byte** (switch back from PPU mode if needed).
3. Press **Ctrl+F10** → **Memory Address >>** → **Enter** an address → type `$7E0000` and Enter.
   You are now looking at SNES work-RAM.
4. Note a non-zero byte and its address. For example, if address `$7E0032` shows `$2C`,
   record `$2C` as your search target. Pick a value that appears at least once (not `$00`).

**Run the search:**

1. Stay in the Memory window (or press **Alt+M** to focus it).
2. Press **Ctrl+F10** → arrow down to **MemOps >>** → **Enter**.
3. Arrow down to **Search…** → **Enter**.
4. A prompt sequence appears — fill in:
   - *Enter fill start address:* `$7E0000`
   - *Enter # of bytes to search:* `$10000`  (64 KB of work-RAM)
   - *Enter value to search for:* the byte value you noted above (e.g. `$2C`)
5. The search runs client-side (drmon reads the range in chunks and scans for the value).

**What to look for:**

Results appear in the **status bar at the very bottom** of the screen (same line that
shows `Running`/`Stopped`) — not a separate window. It looks like:

```
Search: 3 hits @ 7E0032 7E0A10 7E1234
```

The address you noted in step 4 above should appear in that list. (A dedicated results
window is TODO; for now the status bar is the only output.)

**Pass:** Hit list includes the address you observed. No crash or hang during the scan.

---

## Test 3 — Break on ROM Write (F10 → Control)

**Goal:** Arm the MAME write-watchpoint via the menu; confirm snesmon halts when a ROM
write is attempted.

For this test, use the **writer ROM** so the CPU loops writing to ROM automatically:

```bash
# In Terminal 1, stop MAME (Ctrl-C or close the window) then restart with the writer ROM:
task mame SYS=snes CART=devsys/tools/drmon/linux/test-roms/drmon-test-writer.sfc
```

The writer ROM executes `STA $8250` in a tight loop — it tries to write to ROM address
`$8250` repeatedly. MAME drops the write (ROM is read-only), but a watchpoint can detect
the attempt.

**Steps:**

1. In snesmon, press **F3** to stop execution first (status → `Stopped`).
2. Press **F10** to open the global menu bar. The top bar highlights.
3. Press **→** (right arrow) until **Control** is highlighted → press **Enter**.
4. Arrow down to **Break on ROM Write** → **Enter** → select **On** → **Enter**.
   The menu closes; the status bar should show a brief confirmation or just return to normal.
5. Press **F2** to run.

**What to look for:**

Within a fraction of a second the writer ROM's `STA $8250` loop trips the watchpoint.
snesmon should halt automatically:

- Status bar switches to **Stopped** (or shows a stop reason).
- The Memory/Register windows update to the CPU state at the moment of the halt.
- PC in the Register window (`Alt+R`) points to or just after the `STA $8250` instruction.

To clean up: **F10 → Control → Break on ROM Write → Off** to disarm, then **F2** to resume.

**Pass:** snesmon halts on its own (without you pressing F3) within ~1 second of pressing
F2. Stopping a second time and examining the PC confirms it's stuck near `$8250`.

---

## Teardown

Close snesmon: **Alt+X** or press **F10 → File → Exit**.  
Close MAME: **Ctrl+C** in Terminal 1, or close the MAME window.

---

## Recording results

Paste the evidence in the linked plan's verification section (items 2, 3, 4, 5).
A tmux screenshot (`task shot SYS=snes`) captured while snesmon is showing the PPU window
or halted state is good supporting evidence.
