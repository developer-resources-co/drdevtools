# SNES/65816 Toolchain Survey: Packaging Value for Foundry Linux

**Date:** 2026-06-11  
**Context:** Evaluating which SNES/65816 development tools are worth packaging for the foundry Linux apt repo, as a complement to drmon (the source-level 65816 debugger being ported).

---

## Summary Table

| Tool | Upstream Version | Last Activity | License | In Debian/Ubuntu | Primary Use | Packaging Rec |
|------|-----------------|---------------|---------|-----------------|-------------|---------------|
| **ca65** (cc65) | 2.19 (Nov 2020); active git | May 2026 commit | Zlib | Yes — 2.19 in Jammy+ | Homebrew ASM + C (6502 only) | Low value — already packaged |
| **WLA-DX** | 10.6 (Nov 2023); active git | June 2026 commit | GPL-2.0-or-later | No | Homebrew ASM, multi-target | **High value** |
| **64tass** | 1.60.3243 (May 2025) | May 2025 | GPL-2.0 | Yes — 1.60 in Resolute/Forky | C64/6502/65816 hobbyist | Low value — already packaged |
| **asar** | 1.91 (Mar 2024) | Sep 2025 | GPL-3.0/LGPL-3.0 | No (AUR + SlackBuilds only) | ROM hacking / patching | **Medium–high value** |
| **PVSnesLib** | 4.5.0 (Dec 2025) | Mar 2026 | MIT | No | SNES homebrew C SDK | **High value** (complex to package) |
| **Calypsi** | 5.17 (May 2026) | May 2026 | Proprietary (hobby-free) | No | Retro C dev (65816/68000) | Not packageable |

---

## Per-Tool Details

### 1. ca65 / cc65

**Repo:** [cc65/cc65](https://github.com/cc65/cc65)  
**Maintainers:** groepaz (project maintainer), Oliver Schmidt, Christian Groessler, Greg King  
**License:** Zlib  
**Latest release:** V2.19, November 20, 2020  
**Active development:** Yes — commits as recently as May 2026; project accepts PRs continuously  
**Debian/Ubuntu:** cc65 2.19 is in Debian bookworm/trixie/sid (2.19-2) and Ubuntu Jammy through Resolute (2.19-1 or 2.19-2). The packaged version matches upstream.

**What it is:** ca65 is the assembler component of cc65, a complete 6502-family cross-development suite including C compiler, linker, librarian, and utilities. It targets the 65816 for assembly tasks, but the cc65 *C compiler* only generates 6502 (8-bit) code — it does not emit 65816 instructions. This is a hard limitation for SNES C development.

**Typical use:** NES homebrew (dominant use case), 6502-era retro platforms (C64, Apple II, Atari 8-bit, etc.). Used for SNES assembly via ca65 + [libSFX](https://github.com/Optiroc/libSFX), a ca65-based SNES framework. ca65 is [cited as one of the three most popular SNES assemblers](https://snes.nesdev.org/wiki/Tools) alongside 64tass and asar.

**Notable users/projects:** libSFX (ca65-based SNES framework), nesdoug NES tutorials, create-nes-game toolchain, Mesen emulator debugger integration.

**Packaging recommendation:** Low value. Already well-packaged in Debian/Ubuntu at the current upstream version. No gap to fill.

---

### 2. WLA-DX

**Repo:** [vhelin/wla-dx](https://github.com/vhelin/wla-dx)  
**Homepage:** [villehelin.com/wla.html](https://www.villehelin.com/wla.html)  
**Maintainer:** Ville Helin (vhelin)  
**License:** GPL-2.0-or-later  
**Latest release:** v10.6, November 19, 2023  
**Active development:** Yes — commits through June 1, 2026; active bug fixes including eZ80, Mega Drive, and Neo Geo support added recently  
**Debian/Ubuntu:** Not in Debian or Ubuntu. Available on NixOS (10.6), Arch AUR (10.6), GNU Guix (10.5), Slackware (10.6). Confirmed absent from Debian package search.

**What it is:** WLA DX ("Yet Another Multi Platform Cross Assembler") is a mature macro assembler supporting an unusually wide range of targets: GB-Z80, Z80, Z80N, eZ80, 6502, 65C02, 65CE02, 65816, 68000, 6800, 6801, 6809, 8008, 8080, HuC6280, SPC-700, SuperFX, Cx4, SH-2. The 65816 and SPC-700 targets make it directly useful for SNES development (main CPU and audio CPU respectively).

**Typical use:** SNES homebrew assembly — notably it is the assembler used internally by [PVSnesLib](#5-pvsneslib) (see below). Also widely used for Game Boy development. SNESdev Wiki lists it as one of the recommended SNES assemblers.

**Notable users/projects:** PVSnesLib (SNES C SDK), various SNES homebrew projects, Game Boy development community.

**Packaging recommendation:** **High value.** Actively maintained, GPL-licensed, genuinely useful for 65816/SNES development, and completely absent from Debian/Ubuntu. A straightforward cmake build. This is a clear gap to fill.

---

### 3. 64tass

**Repo:** [SourceForge: tass64](https://sourceforge.net/projects/tass64/) (canonical); [irmen/64tass](https://github.com/irmen/64tass) (GitHub mirror)  
**Maintainer:** soci/Singular  
**License:** GPL-2.0  
**Latest release:** 1.60.3243, May 10, 2025  
**Active development:** Yes — 1.60 released 2025; upstream manual at [tass64.sourceforge.net](https://tass64.sourceforge.net/) reflects current version  
**Debian/Ubuntu:** In Debian forky/sid (1.60.3243-1) and Ubuntu Resolute 26.04 LTS (1.60.3243-1). Ubuntu Noble 24.04 has 1.59.3120-1 — one point release behind. Ubuntu Jammy 22.04 has 1.56.2625-1, noticeably older.

**What it is:** 64tass is a multi-pass optimizing macro assembler for the 65xx family: 6502, 65C02, R65C02, W65C02, 65CE02, 65816, DTV, 65EL02, 4510, 45GS02. Despite the name it is fully capable of 65816 SNES work, and [the SNESdev community cites it as one of the top three SNES assemblers](https://snes.nesdev.org/wiki/Tools). It has advanced macro facilities and is considered more featureful than ca65 in some respects.

**Typical use:** Commodore 64/65 homebrew (original home), but heavily used for 65816/SNES homebrew as well. Less common in ROM hacking than asar.

**Notable users/projects:** CSDB (Commodore 64 Scene Database) releases; active SNES homebrew community.

**Packaging recommendation:** Low value for newer Ubuntu (Noble/Resolute already has it). Potentially worth a backport to Jammy if that's a supported target, but the upstream SourceForge hosting makes tracking releases slightly harder than a pure GitHub project. If Jammy is not a priority target, skip.

---

### 4. asar

**Repo:** [RPGHacker/asar](https://github.com/RPGHacker/asar)  
**Original author:** Alcaro (modelled after xkas v0.06 by byuu)  
**Current maintainer:** RPGHacker  
**License:** GPL-3.0 (core), LGPL-3.0 (library/API), WTFPL (some components)  
**Latest release:** v1.91, March 13, 2024  
**Active development:** Yes (slow cadence) — last commit on asar_19 branch: September 21, 2025  
**Debian/Ubuntu:** Not in Debian or Ubuntu. Only packaged in Arch AUR (snes-asar 1.81) and Slackware (1.91 via SlackBuilds). Confirmed absent from Debian.

**What it is:** Asar is an SNES-specific assembler designed for two primary tasks: applying patches to existing ROM images (the dominant ROM hacking workflow) and creating new ROM images from scratch. It supports all three SNES processor architectures: 65c816 (main CPU), SPC-700 (audio CPU), and Super FX (coprocessor). It includes RATS tag support for safe freespace allocation, a built-in testing/patching API, and can be embedded as a shared library (libasar) in other tools.

**Typical use:** ROM hacking — this is the de facto standard assembler for Super Mario World hacking on [SMW Central](https://www.smwcentral.net/), which is the largest SNES ROM hacking community. Also used for scratch SNES homebrew. The library form (libasar) is used by editors and level editors.

**Notable users/projects:** SMW Central — the largest SNES ROM hacking community online, where asar is the standard patch tool. Lunar Magic and related SNES ROM editors use libasar.

**Packaging recommendation:** **Medium–high value.** Not in Debian/Ubuntu, actively used by the SNES hobbyist community, and GPL-licensed. The GPL-3.0/LGPL split is deliberate and clean — the core tool is GPL-3.0, the embeddable library is LGPL-3.0 which allows integration into non-GPL tools. Cmake build is straightforward. Worth packaging for anyone targeting the ROM hacking or scratch SNES dev audience. Complements drmon well: asar patches/builds the ROM, drmon debugs it.

---

### 5. PVSnesLib

**Repo:** [alekmaul/pvsneslib](https://github.com/alekmaul/pvsneslib)  
**Maintainer:** alekmaul; contributor kobenairb (tcc-816 upgrades, Docker, build scripts)  
**License:** MIT  
**Latest release:** 4.5.0, December 28, 2025  
**Active development:** Yes — commits through March 25, 2026  
**Debian/Ubuntu:** Not packaged anywhere in standard distros.

**What it is:** PVSnesLib is a complete SNES development SDK for writing games in C (with inline assembly). Its toolchain stack:

- **816-tcc** — a port of the Tiny C Compiler targeting the 65c816, translates C to 65816 assembly
- **816-opt** — post-processor that optimizes the generated assembly
- **wla-65816 + wlalink** — WLA-DX specialised for 65816, used as assembler and linker
- Runtime library, graphics/sound/input drivers — mainly written in WLA-DX assembly for performance

It supports Windows, Linux, and macOS, and ships Docker images and VS Code integration.

**Typical use:** SNES homebrew C development — developers who want to write SNES games in C rather than pure assembly. The SDK is self-contained enough that a developer only needs this plus an emulator.

**Notable users/projects:** *Yo-Yo Shuriken* and *Keeping SNES Alive!* (by Dr. Ludos) — both written 100% in C using PVSnesLib. Featured at Homebrew Games Summer Showcase 2024.

**Packaging challenges:** PVSnesLib is not a single binary — it is an SDK that bundles multiple tools (tcc-816, wla-dx fork, the library itself, examples, asset converters). Packaging it as a conventional Debian package is non-trivial because: (1) it embeds a custom WLA-DX fork rather than using system WLA-DX; (2) the library paths are baked relative to an install prefix; (3) users typically clone the repo and set `PVSNESLIB_HOME`. A workable approach would be a meta-package that pulls in wla-dx and ships the SDK components under `/opt/pvsneslib` or `/usr/share/pvsneslib`.

**Packaging recommendation:** **High value, high effort.** The only actively maintained SNES C SDK, MIT-licensed, completely absent from all standard distros. The packaging complexity is real but manageable with a non-standard layout. If foundry linux wants to support SNES homebrew C development end-to-end, this and wla-dx together form the C dev stack. That said, given the complexity, packaging wla-dx first (a dependency) and documenting a PVSnesLib manual install is a reasonable phased approach.

---

### 6. Calypsi

**Website:** [calypsi.cc](https://www.calypsi.cc/)  
**Repo (open-source support packages only):** [hth313/Calypsi-tool-chains](https://github.com/hth313/Calypsi-tool-chains)  
**Maintainer:** hth313 (hth313@gmail.com)  
**License:** Proprietary (closed-source); hobby use free of charge; commercial/income use prohibited  
**Latest release:** 5.17, May 6, 2026  
**Active development:** Yes — consistent release cadence (5.7 through 5.17 across 2025–2026)  
**Debian/Ubuntu:** Not packaged anywhere.

**What it is:** Calypsi is a commercial-grade C/C++ compiler toolchain targeting retro and niche processors: MOS 6502, WDC 65816, Motorola 68000, and HP Nut (assembler/debugger only). It is intended for serious homebrew and embedded development on retro hardware. The 65816 target covers the WDC W65C816 directly; SNES development is a valid use case but the tool focuses on the Foenix C256 and F256 retro computers more than SNES specifically. The compiler itself is closed-source; support packages (board support, examples) are open-source on GitHub.

**License details:** Free for hobby use; prohibited for making a living directly or indirectly from the tools. Small side income is permitted. The HP-41 Nut target uniquely carries a BSD license permitting commercial use.

**Typical use:** Foenix retro computer development, 65816 embedded development, 68000 Amiga/retro development. SNESdev Wiki lists it under compilers but notes the non-commercial restriction.

**Notable users/projects:** Foenix computer community; the [GenerationAmiga community](https://www.generationamiga.com/2026/04/18/calypsi-5-16-boosts-amiga-and-68k-retro-development-with-important-fixes/) tracks 68K releases.

**Packaging recommendation:** **Not packageable.** The license explicitly prohibits redistribution for commercial/income use, and the binaries are closed-source. Even hobby-free terms are incompatible with apt repo distribution (Debian policy requires DFSG-free licenses). This tool can only be documented as a manual download option.

---

## Packaging Priority Order

Given the goal of supporting SNES/65816 homebrew development and complementing drmon:

1. **WLA-DX** — package first. Single binary tool, GPL, cmake build, completely absent from Debian/Ubuntu, used by both standalone SNES assemblers and as PVSnesLib's assembler backend. Direct complement to drmon (assemble → debug).

2. **asar** — package second. The standard ROM hacking assembler, GPL, cmake, absent from Debian/Ubuntu. High visibility in the SNES community; a packaged asar lowers the barrier for ROM hackers who might also adopt drmon for debugging.

3. **PVSnesLib** — package eventually, after WLA-DX. High effort non-standard layout, but it is the only maintained SNES C SDK. MIT license. Phased approach: get WLA-DX packaged (a PVSnesLib dependency), then tackle the SDK itself. Could be distributed as a tarball install under `/opt/pvsneslib` with a wrapper `.deb` that sets environment variables.

4. **ca65/cc65** — skip. Already in Debian/Ubuntu at current upstream version. No gap to fill.

5. **64tass** — skip for now. In Debian/Ubuntu (older on Jammy, current on Noble+). Revisit only if a Jammy backport is needed.

6. **Calypsi** — cannot package. Proprietary license incompatible with apt repo distribution.

---

## Notes on drmon Complementarity

drmon is a source-level debugger being ported to SNES/65816. The most useful toolchain pairing depends on which assembler outputs compatible debug info:

drmon natively reads `.sld` (Source-Level Debug) files — the format produced by the original Developer Resources assembler suite — as well as COFF object files. None of the tools surveyed produce `.sld` output today.

- **ca65** outputs `.dbg` files (cc65 debug format) and integrates with [Mesen's](https://snes.nesdev.org/wiki/Tools) debugger. Incompatible with drmon natively; would need a `.dbg` → `.sld` converter or drmon adding `.dbg` import.
- **WLA-DX** produces symbol/listing files in its own format. Same situation — a converter or drmon import would be needed.
- **asar** outputs a `.sym` file and supports per-label defines. Same situation.
- **64tass** produces VICE-compatible label files and DASM-format listings. Same situation.

Until drmon grows support for one of these formats (or a converter exists), the assemble→debug workflow with any of these tools requires manual symbol loading via drmon's `LOADSYM` command or the Symbol window. That said, packaging these tools still has standalone value for the SNES development audience.

The most impactful packaging for drmon's ecosystem is **WLA-DX** (broad SNES use, PVSnesLib dependency) and **asar** (ROM hacking community entry point).

---

## Sources

- [SNESdev Wiki — Tools](https://snes.nesdev.org/wiki/Tools)
- [cc65/cc65 GitHub](https://github.com/cc65/cc65)
- [vhelin/wla-dx GitHub](https://github.com/vhelin/wla-dx)
- [WLA-DX documentation (Read the Docs)](https://wla-dx.readthedocs.io/en/latest/introduction.html)
- [64tass on SourceForge](https://sourceforge.net/projects/tass64/)
- [64tass reference manual](https://tass64.sourceforge.net/)
- [irmen/64tass GitHub mirror](https://github.com/irmen/64tass)
- [RPGHacker/asar GitHub](https://github.com/RPGHacker/asar)
- [asar on Repology](https://repology.org/project/asar-snes-assembler/packages)
- [alekmaul/pvsneslib GitHub](https://github.com/alekmaul/pvsneslib)
- [hth313/Calypsi-tool-chains GitHub](https://github.com/hth313/Calypsi-tool-chains)
- [Calypsi website](https://www.calypsi.cc/)
- [Debian Package Tracker — cc65](https://tracker.debian.org/pkg/cc65)
- [Debian packages — 64tass](https://packages.debian.org/search?keywords=64tass&searchon=names&suite=all&section=all)
- [Ubuntu packages — cc65](https://packages.ubuntu.com/search?keywords=cc65&searchon=names&suite=all&section=all)
- [WLA-DX on Repology](https://repology.org/project/wla-dx/packages)
- [cc65 on Repology](https://repology.org/project/cc65/packages)
- [Calypsi 5.16 — GenerationAmiga](https://www.generationamiga.com/2026/04/18/calypsi-5-16-boosts-amiga-and-68k-retro-development-with-important-fixes/)
- [libSFX — Optiroc/libSFX](https://github.com/Optiroc/libSFX)
- [PVSnesLib v3.1.0 on GBAtemp](https://gbatemp.net/threads/c-library-devkit-for-snes-pvsneslib-v3-1-0-release.579203/)
- [CSDB — 64tass V1.60.3243 by Singular](https://csdb.dk/release/?id=252813)
