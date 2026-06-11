# Package drmon as a Debian .deb

**Date:** 2026-06-12
**Status:** In progress

## Goal

Package `snesmon` and `genmon` (the drmon Linux port) as a debhelper-style
`.deb` for the `foundry-apt` repository (`apt.foundrylinux.org`, suite `resolute`).

## Scope

- Package name: `drmon`
- Version: `1.0.1` (native format, no Debian revision suffix — we own the source)
- Source format: `3.0 (native)` — local project, no upstream URL
- Binaries: `/usr/bin/snesmon` (SNES/65816), `/usr/bin/genmon` (Genesis/68000)
- Target distro: Ubuntu 26.04 "questing" (Foundry suite `resolute`)

## Design decisions

1. **Native source format** — drmon is maintained in the sibling `drdevtools` repo.
   There is no "upstream" we're patching; the source is ours. `3.0 (native)` is simpler
   and correct here.
2. **ASan disabled in package** — CMakeLists.txt has ASan ON by default; we pass
   `-DDRMON_ASAN=OFF` in `debian/rules` so the production binary has no debug overhead.
3. **libs/pclib bundled** — `CMakeLists.txt` references `../../../libs` (relative to the
   drmon dir). The build.sh copies `libs/pclib` alongside the source and patches the
   cmake path to `libs` so the build is self-contained.
4. **Two binaries, one package** — `snesmon` and `genmon` share all source; splitting
   into sub-packages adds complexity for no benefit.

## Files created

- `foundry-apt/packages/drmon/build.sh` — assembles source from drdevtools, patches cmake, builds .deb
- `foundry-apt/packages/drmon/debian/control` — package metadata
- `foundry-apt/packages/drmon/debian/changelog` — version `1.0.1 resolute`
- `foundry-apt/packages/drmon/debian/copyright` — DEP-5, GPL-2
- `foundry-apt/packages/drmon/debian/rules` — cmake build: Release, ASan off, MAME backend on
- `foundry-apt/packages/drmon/debian/source/format` — `3.0 (native)`
- `foundry-apt/packages/drmon/debian/watch` — comment-only (no upstream URL stream)
- `foundry-apt/packages/drmon/debian/man/snesmon.1` — man page
- `foundry-apt/packages/drmon/debian/man/genmon.1` — man page
- `foundry-apt/packages/drmon/debian/drmon.manpages` — manpages list

## Verification steps

1. Build the .deb inside a clean ubuntu:26.04 container:

```bash
docker run --rm \
    -v "$HOME/SRC/drdevtools":/drdevtools \
    -v "$HOME/SRC/foundrylinux.org/foundry-apt":/foundry-apt \
    ubuntu:26.04 bash -c '
set -euo pipefail
DEBIAN_FRONTEND=noninteractive apt-get update -qq 2>/dev/null
DEBIAN_FRONTEND=noninteractive apt-get install -y -qq \
    build-essential debhelper dpkg-dev lintian cmake ninja-build libncurses-dev 2>/dev/null
mkdir -p /foundry-apt/dist
DRDEVTOOLS=/drdevtools bash /foundry-apt/packages/drmon/build.sh
lintian /foundry-apt/dist/drmon_*.deb 2>&1
'
```

```
=== Building drmon 1.0.1 ===
=== Copying drmon source from /drdevtools/devsys/tools/drmon ===
=== Copying libs/pclib headers ===
=== Patching CMakeLists.txt: ../../../libs -> libs ===
=== Copying debian/ tree ===
=== Installing Build-Depends ===
=== dpkg-buildpackage -us -uc -b ===
OK   /foundry-apt/dist/drmon_1.0.1_amd64.deb  (151560 bytes)
=== lintian ===
running with root privileges is not recommended!
```

PASS — build succeeds; lintian returns clean (only root-privileges advisory, no E: or W: lines).

2. Inspect the .deb contents:

```bash
dpkg-deb -I dist/drmon_*.deb
dpkg-deb -c dist/drmon_*.deb
```

```
 Package: drmon
 Version: 1.0.1
 Architecture: amd64
 Depends: libc6 (>= 2.38), libgcc-s1 (>= 3.3.1), libncursesw6 (>= 6), libstdc++6 (>= 13.1), libtinfo6 (>= 6)
 Description: SNES/Genesis source-level debugger for game development

./usr/bin/genmon
./usr/bin/snesmon
./usr/share/doc/drmon/changelog.gz
./usr/share/doc/drmon/copyright
./usr/share/man/man1/genmon.1.gz
./usr/share/man/man1/snesmon.1.gz
```

PASS — both binaries in `./usr/bin/`; `${shlibs:Depends}` resolved to `libncursesw6`.

3. Check that binaries are stripped:

```bash
dpkg-deb -x dist/drmon_*.deb /tmp/drmon-verify
file /tmp/drmon-verify/usr/bin/snesmon /tmp/drmon-verify/usr/bin/genmon
```

```
/tmp/drmon-verify/usr/bin/snesmon: ELF 64-bit LSB pie executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, BuildID[sha1]=5d3005bdb179693e41356f548ec8f6d1e8fc7148, for GNU/Linux 3.2.0, stripped
/tmp/drmon-verify/usr/bin/genmon:  ELF 64-bit LSB pie executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, BuildID[sha1]=5441d3ec00bb26c6eda9a336fc2d842a8ccb3991, for GNU/Linux 3.2.0, stripped
```

PASS — both are PIE ELFs, `stripped`.

4. Smoke install in a clean container:

```bash
docker run --rm \
    -v "$HOME/SRC/foundrylinux.org/foundry-apt/dist":/debs \
    ubuntu:26.04 bash -c '
DEBIAN_FRONTEND=noninteractive apt-get update -qq 2>/dev/null
DEBIAN_FRONTEND=noninteractive apt-get install -y -qq /debs/drmon_*.deb
dpkg -s drmon | grep -E "^Version:|^Status:"
'
```

```
Status: install ok installed
Version: 1.0.1
Depends: libc6 (>= 2.38), libgcc-s1 (>= 3.3.1), libncursesw6 (>= 6), libstdc++6 (>= 13.1), libtinfo6 (>= 6)
```

PASS — package installs cleanly; `Status: install ok installed`.
