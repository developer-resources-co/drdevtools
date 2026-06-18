# drmon-dap — editor integration

`drmon-dap-snes` and `drmon-dap-gen` are standalone DAP adapters that let any
DAP-capable editor debug SNES and Genesis code running in MAME. They communicate
with the editor over **stdin/stdout** (DAP stdio transport) and with MAME over the
same TCP bridge as the TUI binaries (`127.0.0.1:41816` by default).

The TUI binaries (`snesmon`/`genmon`) continue to work alongside the DAP adapters;
the two are independent.

## Prerequisites

- `task build` has been run — produces `drmon-dap-snes` and `drmon-dap-gen` in
  `/tmp/drmon-build/` alongside the TUI binaries.
- MAME is running with the bridge script:
  ```bash
  task mame SYS=snes CART=path/to/game.sfc
  ```

## Capabilities

| Feature | Support |
|---------|---------|
| Attach to MAME | ✓ |
| Continue / Pause | ✓ |
| Step (next / stepIn) | ✓ — correct branch targets for 65816 |
| Instruction breakpoints | ✓ |
| Registers | ✓ — full register set with Memory Inspector links |
| Read memory | ✓ — hex address in Memory Inspector |
| Evaluate expression | ✓ — `$addr` byte; register name; symbol name (with `--symbols`) |
| Disassembly view | ✓ — 65816 / 68k mnemonics; label annotations (with `--symbols`) |
| Source breakpoints | ✓ — file:line → address via `--symbols` |
| Symbol names | ✓ — binary `.sld` + Sierra COFF via `--symbols <path>` |

---

## VS Code

VS Code registers a debug `"type"` only through an extension's `contributes.debuggers`
(there is **no** `settings.json` key that points a debug type at an arbitrary executable —
unlike nvim-dap / dap-mode below, which do accept a bare `command`). A tiny, no-build
extension is shipped in the repo for exactly this: **`devsys/tools/drmon/vscode-drmon/`**
(`package.json` + `extension.js`, plain JS — it registers `"type": "drmon"` and launches the
adapter, passing `host`/`port`/`symbols` as CLI args).

Install it by symlinking the folder into your extensions dir, then reload VS Code:

```bash
ln -sfn "$PWD/devsys/tools/drmon/vscode-drmon" ~/.vscode/extensions/drmon-dap
# Command Palette → "Developer: Reload Window"
```

(Or, for a throwaway session, `code --extensionDevelopmentPath=devsys/tools/drmon/vscode-drmon`.)

Then add a launch configuration to `.vscode/launch.json` (one is committed at the repo root):

```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Attach drmon (SNES + DWARF)",
      "type": "drmon",
      "request": "attach",
      "symbols": "${workspaceFolder}/devsys/tools/drmon/linux/test-roms/a16local.sfc.elf"
    }
  ]
}
```

The `symbols` path accepts an ELF+DWARF (e.g. an llvm-mos `<rom>.sfc.elf` companion), a
Developer Resources `.sld`, Sierra COFF, ca65 `.dbg`, or WLA `.sym`; omit it to attach
without symbols. `binary` (default `/tmp/drmon-build/drmon-dap-snes`) selects the adapter —
point it at `drmon-dap-gen` for Genesis. Press **F5**, then continue/step/break as usual.

Verified end-to-end in VS Code (2026-06-19) against a `-g` llvm-mos build (`a16local.sfc` + its
`a16local.sfc.elf` DWARF companion): the adapter attaches, a **source breakpoint** set in the editor
**fires**, the CPU **halts**, and VS Code **highlights the line** — the CALL STACK frame is mapped to
`a16local.c:17`, PC = `0x8074` (the line's DWARF address), and the Registers scope shows live state
(`A = 0x1122`). VS Code sends the file's absolute path and the adapter resolves it through DWARF (by
basename):

<img src="../../../docs/plans/screenshots/dap-vscode-halt-line17.png" width="720">

> **Loading the extension:** dropping the folder into `~/.vscode/extensions/` does **not** work — VS Code's
> extension manager marks a hand-placed folder as removed. Load it as a *development* extension instead:
> `code --extensionDevelopmentPath=<repo>/devsys/tools/drmon/vscode-drmon <repo>/devsys`, which opens an
> "[Extension Development Host]" window with the `drmon` type registered.

Once attached, open the **Memory Inspector** (Debug toolbar → hex icon) and type a
hex address such as `0x7e0000` to view SNES work-RAM. Registers in the Variables
pane each carry a memory-reference link — click the hex icon next to SP or PC to
jump straight to that address in the inspector.

---

## Emacs (dap-mode)

Install [dap-mode](https://github.com/emacs-lsp/dap-mode) (requires lsp-mode),
then add to your config:

```elisp
(require 'dap-mode)

(dap-register-debug-provider
 "drmon-snes"
 (lambda (conf)
   (plist-put conf :program "/tmp/drmon-build/drmon-dap-snes")))

(dap-register-debug-provider
 "drmon-gen"
 (lambda (conf)
   (plist-put conf :program "/tmp/drmon-build/drmon-dap-gen")))

(dap-register-debug-template
 "MAME SNES attach"
 (list :type    "drmon-snes"
       :request "attach"
       :name    "MAME SNES attach"))

(dap-register-debug-template
 "MAME Genesis attach"
 (list :type    "drmon-gen"
       :request "attach"
       :name    "MAME Genesis attach"))
```

Start a session with `M-x dap-debug` → pick "MAME SNES attach".

Useful commands once attached:

| Command | Action |
|---------|--------|
| `M-x dap-continue` | Resume execution |
| `M-x dap-next` | Step over |
| `M-x dap-step-in` | Step into |
| `M-x dap-breakpoint-toggle` | Add/remove breakpoint at point |
| `M-x dap-ui-repl` | Open expression evaluator (`$7e0032`, `PC`, etc.) |
| `M-x dap-ui-locals` | Show Registers scope |

---

## Neovim (nvim-dap)

Install [nvim-dap](https://github.com/mfussenegger/nvim-dap) via your plugin manager,
then add to your config:

```lua
local dap = require('dap')

dap.adapters.drmon_snes = {
  type    = 'executable',
  command = '/tmp/drmon-build/drmon-dap-snes',
}

dap.adapters.drmon_gen = {
  type    = 'executable',
  command = '/tmp/drmon-build/drmon-dap-gen',
}

dap.configurations.drmon = {
  {
    type    = 'drmon_snes',
    request = 'attach',
    name    = 'Attach to MAME (SNES)',
  },
  {
    type    = 'drmon_gen',
    request = 'attach',
    name    = 'Attach to MAME (Genesis)',
  },
}
```

Key mappings (add to your config):

```lua
vim.keymap.set('n', '<F5>',  dap.continue)
vim.keymap.set('n', '<F10>', dap.step_over)
vim.keymap.set('n', '<F11>', dap.step_into)
vim.keymap.set('n', '<F9>',  dap.toggle_breakpoint)
```

Start a session with `:DapContinue` → pick "Attach to MAME (SNES)".

**Instruction breakpoints** — nvim-dap sets address breakpoints via
`setInstructionBreakpoints` when the adapter declares the capability (which
drmon-dap does). Use `:DapToggleBreakpoint` on any line in a disassembly buffer,
or set one programmatically:

```lua
dap.set_breakpoint(nil, nil, nil, { instructionReference = '0x80a000' })
```

---

## Symbol file (`--symbols`)

Pass a Developer Resources binary `.sld` or Sierra COFF (`.cof`) file to enable:

- **Disassembly labels** — instruction addresses with a matching symbol show the
  label name in the disassembly pane.
- **Symbol evaluation** — type a symbol name in the Watch or Debug Console (`RESET`,
  `GameLoop`, etc.); the adapter resolves it to its address.
- **Source breakpoints** — set breakpoints by file and line number (requires a `.sld`
  file with file:line→address records from the assembler).

```bash
/tmp/drmon-build/drmon-dap-snes --symbols /path/to/game.sld
# or
/tmp/drmon-build/drmon-dap-snes --symbols /path/to/game.cof
```

The adapter tries `.sld` format first; if the header doesn't match it falls back to
Sierra COFF. Pass the flag in a wrapper script so the editor picks it up:

```bash
#!/bin/sh
exec /tmp/drmon-build/drmon-dap-snes \
  --host "${DRMON_HOST:-127.0.0.1}" \
  --port "${DRMON_PORT:-41816}" \
  --symbols "${DRMON_SYMBOLS:-}"
```

---

## Host and port

By default `drmon-dap-snes` connects to `127.0.0.1:41816`. Override with flags:

```bash
/tmp/drmon-build/drmon-dap-snes --host 192.168.1.10 --port 41816
```

Point your editor's adapter `command` at a wrapper script if you need to pass
arguments:

```bash
#!/bin/sh
exec /tmp/drmon-build/drmon-dap-snes --host "$DRMON_HOST" --port "${DRMON_PORT:-41816}"
```
