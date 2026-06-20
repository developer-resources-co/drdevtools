---
name: home-is-a-git-repo
description: "$HOME (/home/will) is itself a git repo — new repos under ~/SRC must be cleanly nested, or git commands fall through to the home repo"
metadata: 
  node_type: memory
  type: reference
  originSessionId: e3b2cebe-95eb-41c3-b7c2-8737ce026e54
---

`/home/will` (the user's `$HOME`) is a git repository (dotfiles/home-managed,
remote `origin/main`). `~/SRC` is inside its worktree (the home repo gitignores
much of `SRC/`).

**Why it matters / the gotcha (hit 2026-06-13):** when creating a new project
repo under `~/SRC/<name>`, if `git init` doesn't actually succeed (e.g. it was
bundled with a `git add -A` that a pre-commit hook blocked, so the whole Bash
call was rejected), then later `git add`/`git commit`/`git log` run from that
dir **walk up and operate on the home repo** instead. Symptoms: `git status`
shows `../../.claude/`, `../../TODO.md`, blender config, etc.; `git add` of your
files is refused as "ignored" (the home repo ignores the path).

**How to apply:** when spinning up a new repo under `~/SRC`, run `git init`
**on its own** (never chained with `git add -A`, which the SRC hooks block — see
[[home-is-a-git-repo]] context: stage explicit files only), then immediately
verify isolation with `git rev-parse --show-toplevel` before staging. The new
repo nests cleanly inside the home repo (git won't recurse into a nested
`.git`), and the home repo already ignores `SRC/<name>`.
