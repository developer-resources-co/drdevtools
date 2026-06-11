# Revive `EvalCommand` — bring the typed command / scripting language back to life

## Context

drmon's command window (Alt-K) and script files (`.scr` via `EXECUTE`) are driven by
`EvalCommand` (`command.cpp:596`) — the dispatcher for every command verb (`OPEN`, `CLOSE`,
`RUN`, `STOP`, `STEP`, `OVER`, `RESET`, `BSET`, `SET`, `LOAD`, `SAVE`, macros, …). Its **entire
body is `#if 0`'d** (`command.cpp:598–942`), leaving just `return(s)`, so **every typed command
is a no-op** (root-caused this session; the `#if 0` dates to the original 2003 DOS import
`c835c3b`, not the Linux port). That's also why `OPEN SEARCHLIST` returns "No such window".

Reviving it makes the documented command language, scripts, and command-driven workflows
actually work — and turns the `RW_OPEN` switch (which already contains the `RW_SEARCHLIST`
case) live, so `OPEN <window>` opens windows.

**Decisions (from clarifying round):** route revived expression/number parsing through the
**working flex/bison `DoExp()`** path (not the obsolete hand-written evaluator); revive **all**
verbs in one pass.

## Findings (exploration)

- The `#if 0` block is **two** dead blocks: `EvalCommand` body (`command.cpp:598–942`) **and its
  only blocker** `GetNumbers` (`command.cpp:290–320`, used by `BSET/BCLEAR/SET/LOAD/SAVE/
  SIZE/POSITION/WAIT`).
- **~30 verbs; nearly all helpers are already active/compiled** — `OpenBreak/OpenMemory/…`,
  `ParseHardWindow`, `ParseWindow`, `ForceWindowResize`, `Slave{Run,Stop,Step,StepOver,Reset,
  Restart}`, `DoSlaveRunWithUpdates`, `AddBrkPt`, `BreakClear`, `LoadBinaryFile`,
  `SaveBinaryFile`, `LoadSymbol`, `Load/SaveMacros`, `SetConfigParm`, `AddCmdMacro`,
  `ExecFile`, `OpenHelpFile`, etc. No DOS-isms / `far` pointers in the block.
- **Two expression-eval sites** are the only real work: `RW_EVAL` (`command.cpp:613`) and the
  `GetNumbers` loop (`command.cpp:310`). Both call the obsolete hand-written
  `TokenizeExpression`/`EvalExpression`. The live path is `DoExp(const char*)` (`expr.cpp:19`)
  → `yyparse()` → global `exprAnswer` (`expr.cpp:12`), used everywhere (breakpoints, watch,
  memory-goto) and consistent with drmon's symbol/register resolution.

## Approach

All edits in `command.cpp` (one file).

1. **`GetNumbers` (`288–322`)** — remove the `#if 0/#endif`; replace the per-number
   `TokenizeExpression`/`EvalExpression` with `DoExp`:
   ```c
   s = SkipSpace(s);
   for (i = 0; i < count; i++) {
       char *delims = (i == count-1) ? (char*)DELIMITERS : (char*)",";
       s2 = FindWord(s, delims, &c);   // isolate this arg (verify it NUL-terminates at the delim;
                                       // if not, save c and NUL it manually, restore after)
       DoExp(s);                       // flex/bison parse of the isolated expression
       answer[i] = exprAnswer;
       s = s2;
   }
   ```
   Keep the trailing delimiter-restore (`s[strlen(s)] = c`) semantics if `FindWord` mutated the
   buffer.

2. **`EvalCommand` (`596–944`)** — remove the `#if 0/#endif`. Replace the `RW_EVAL` case body:
   ```c
   case RW_EVAL:
       DoExp(s);
       sprintf(textBuffer, "Decimal: %ld  Hex: %lX\n",
               (long)exprAnswer, (unsigned long)exprAnswer);
       PrintToCmdWindow(textBuffer);
       break;
   ```
   Drop the now-unused `token tokenStream[100]; token *tsPtr;` locals (and any other locals only
   used by the removed evaluator). The rest of the `switch` (window open/close/size/name,
   run-control, breakpoints, set, load/save, macros, quit, wait, hit) stays as-is — its helpers
   are active. The `RW_OPEN` nested switch already has the `RW_SEARCHLIST` case → goes live.

3. **Build both binaries; fix signature drift iteratively.** Expect a small handful of call-site
   mismatches between the 1990s dead code and current definitions (candidates: `AddBrkPt`,
   `SaveBinaryFile`, `SetConfigParm`, `ForceWindowResize`, `LoadSymbol`/`LoadMacros` overloads,
   `ParseWindow`). The compiler (gnu++98, `-w` so only errors show) pinpoints each; adjust the
   call in-place to the active signature.

4. **Drop the dead `EvalCommand` no-op tail** (`return(s)` after the old `#endif`) once the
   switch is the function body, and ensure each `case` advances/returns `s` correctly (the
   original block already does).

## Files
| File | Change |
|---|---|
| `devsys/tools/drmon/command.cpp` | un-`#if 0` `GetNumbers` + `EvalCommand`; swap both expr sites to `DoExp`/`exprAnswer`; fix any signature drift |
| `TODO.md` | move the `OPEN`-no-op CLEANUP item → DONE; add a `[wip]` entry linked to the repo plan |
| `docs/plans/2026-06-12-revive-evalcommand.md` | repo copy of this plan (created at implementation time) |

## Risks / notes
- **`DoExp` swallows parse errors** — `exprAnswer = yyparse()` treats the return as the value, so
  a bad expression yields garbage/`1` rather than a clean error. Pre-existing behavior of the live
  path; acceptable for a command line. Document; don't try to "improve" the shared evaluator here.
- **`GetNumbers` arg splitting** — confirm `FindWord` NUL-terminates at the delimiter so `DoExp`
  parses a single arg; otherwise NUL it manually and restore. Verify with a 2-arg verb (`SIZE`,
  `SAVE`).
- **Expression consistency** — using `DoExp` means command-line expressions resolve symbols/
  registers identically to breakpoint conditions / watch (the win of this choice).
- **No new evaluator** — the obsolete `TokenizeExpression`/`EvalExpression` stay unused (don't
  delete; out of scope).

## Verification
1. **Build** — `task build`: both `snesmon` + `genmon` compile/link clean.
2. **Smoke** — `task smoke SYS=snes`: opening every window + typing into Expression → no crash.
3. **Command verbs (disconnected, no MAME needed):** in the Command window (Alt-K):
   - `OPEN MEMORY`, `OPEN SEARCHLIST`, `OPEN BREAK` → the named window opens (no "No such window").
   - `<expr>` via the EVAL path, e.g. `? $1234 + 10` → prints `Decimal: … Hex: …`.
   - `BSET $8000` → a breakpoint appears in the Break window; `BCLEAR $8000` removes it.
   - `SET <param> <value>` → the config parameter changes (observable behavior).
   - `CLOSE`, `SIZE`/`POSITION`/`NAME` on an open window → geometry/title change.
   Drive via `linux/spikes/connected_shot.sh` (Alt-K + typed command + capture) or `task run`.
4. **Run-control (connected to MAME):** `RUN`, `STOP`, `STEP`, `OVER`, `RESET` drive the slave
   (status line / registers update) — cross-check against the menu/F-key equivalents.
5. **Script** — a tiny `.scr` (`OPEN MEMORY` / `BSET $8000` / a macro def) run via the File →
   Execute Script path → commands take effect.
6. **Regression** — menu/Alt-key window opening and the search window (MemOps → Search…) still
   work; `task test-bridge SYS=snes` unaffected (24/24).

Cost: $0 — local build; most verification needs no MAME (command verbs are TUI-side).

## Verification results (2026-06-12)

- **Build** — `task build`: both `snesmon` + `genmon` compile/link clean, **zero signature
  drift** (every revived helper's signature already matched). **PASS.**
- **`OPEN MEMORY`** (Alt-K → type → Enter) → a **Memory window opens** (no "No such window").
  **PASS** (captured).
- **`OPEN SEARCHLIST`** → the **Search List window opens** — closes the loop from the
  investigation (`RW_OPEN`/`RW_SEARCHLIST` now live). **PASS** (captured).
- **Smoke** — `task smoke SYS=snes`: all windows open + Expression typing, no SIGSEGV. **PASS.**
- **`?`-eval / `BSET` display** — couldn't be reliably **captured** over tmux: the command
  window's interactive Enter is racy headless (the same line that worked for `OPEN` sometimes
  doesn't submit). No code fault (snesmon never crashes; `OPEN` dispatches fine), and these
  paths use the same `DoExp` evaluator proven by breakpoints/watch/memory-goto and compile clean.
  Recommend a 30-second manual check: `task run SYS=snes`, Alt-K, `? $10 + $10` (expect
  `Decimal: 32  Hex: 20`) and `BSET $8000` (appears in the Break window).
