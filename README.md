# drdevtools — Developer Resources console dev tools

Development systems and game-development tools for the **Sega Genesis / Mega Drive**
and the **Super Nintendo (SNES)**, built by **Developer Resources** in the early 1990s:
a source-level debugger, assembler, graphics tools, runtime game libraries, sample
games, and the hardware/dev-pod tooling that went with them.

## Provenance

This repository was migrated from the original [SourceForge project](https://sourceforge.net/projects/drdevtools/),
which held the code in **CVS** (read-only since SourceForge retired CVS write access).
The full `,v` history was converted to git with [cvs-fast-export](http://www.catb.org/~esr/cvs-fast-export/)
on 2026-06-10, preserving the original 2003-08-15 import. Background and the verified
migration steps are in [docs/investigations](docs/investigations/2026-06-10-hosting-sourceforge-vs-github.md)
and [docs/plans](docs/plans/2026-06-10-migrate-cvs-to-github.md).

Background on the company and its tools: [Kevin Seghetti interview (Sega-16)](https://www.sega-16.com/2013/03/interview-kevin-seghetti/).

## Layout

| Path | What |
|------|------|
| `tools/` | Host-side tools: assemblers, graphics/IFF/ABM converters, ROM utilities, font tools |
| `devsys/` | Development-system tooling — the dev pod, monitor (`drmon`), diagnostics, cart viewer |
| `runtime/` | On-console runtime libraries and sample games (Genesis/SNES) |
| `libs/` | Shared host libraries (e.g. `pclib`) |

## License

Per the [SourceForge project](https://sourceforge.net/projects/drdevtools/) metadata, the
code is released under the **[GNU GPL v2](COPYING)** and the **[GNU Library GPL v2](COPYING.LESSER)**.
See those files for the full terms.
