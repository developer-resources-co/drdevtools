# Memory Index

- [Links belong in documents](feedback_links_in_docs.md) — proactively write links/diffs/references into the relevant markdown doc; don't wait to be asked
- [Features not platforms](features-not-platforms.md) — gate code by feature/capability flags (`#ifdef SPC700`), not platform macros (`#ifdef SNES`); the platform block defines which features it has
- [User background](user_background.md) — Will is a commercial SNES dev; used Zardoz compiler on Demolition Man and Izzy's Olympic Quest; firsthand primary source on 1990s 65816 tooling
- [Default base: Foundry/Kubuntu 26.04](default-base-foundry-2604.md) — target 26.04 (ubuntu:26.04 for containers), not 24.04; 26.04 ships libcppdap-dev first-class
- [MAME Lua debugger control flow](mame-lua-debugger-control-flow.md) — for MAME 0.277 `-debugger none` Lua: reuse `mame_bridge.lua`; emu.pause/unpause (not execution_state), bp pseudo-holds, bpset cond/act gotchas, lazy-init under autoboot
- [$HOME is a git repo](home-is-a-git-repo.md) — `/home/will` is itself a git repo; a new repo under `~/SRC` must be cleanly `git init`'d standalone (verify `rev-parse --show-toplevel`) or git commands fall through to the home repo
