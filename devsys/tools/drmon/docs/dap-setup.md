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

No extension is required for basic use. Add a launch configuration to
`.vscode/launch.json`:

```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Attach to MAME (SNES)",
      "type": "drmon",
      "request": "attach"
    }
  ]
}
```

Then register the adapter type in `.vscode/settings.json` (or a workspace extension):

```json
{
  "debug.extensionHost": {
    "drmon": {
      "type": "executable",
      "command": "/tmp/drmon-build/drmon-dap-snes"
    }
  }
}
```

> A minimal VS Code extension (~3 files: `package.json`, `extension.ts`, binary)
> that registers `"type": "drmon"` as a first-class debug type would remove the
> `debug.extensionHost` workaround. The workaround above works today without
> installing anything from the marketplace.

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
