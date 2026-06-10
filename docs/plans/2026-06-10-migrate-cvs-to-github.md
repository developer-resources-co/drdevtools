# Migrate drdevtools from SourceForge (CVS) → GitHub org `developer-resources-co`

## Context

`drdevtools` (the "Developer Resources" Sega Genesis / SNES dev tools, originally
~1992) currently lives **only** on SourceForge under CVS — confirmed by the prior
investigation in `drdevtools/docs/investigations/2026-06-10-hosting-sourceforge-vs-github.md`.
SourceForge has put CVS in permanent read-only mode, the legacy CVS rsync/pserver
hosts are dead, and CVS is effectively unmaintainable. We want the canonical home
on **GitHub**, under a **new org**, with a real git history.

**Decisions (confirmed with user):**
- Destination: [github.com/developer-resources-co/drdevtools](https://github.com/developer-resources-co/drdevtools)
  (org name = the original company "Developer Resources"; bare `developer-resources`
  is an unrelated stranger's account, so `-co` disambiguates — "Co." for company,
  avoiding `-inc`'s claim of formal incorporation, which may never have happened.)
- Visibility: **Public** (matches its GPL/LGPL license + public SF project)
- Source of truth becomes GitHub; SourceForge left in place as an archived origin.

## What we already know (from investigation)

- The SourceForge **code snapshot zip** is the raw CVS repository, not a HEAD export:
  [the snapshot zip](https://sourceforge.net/code-snapshots/cvs/d/dr/drdevtools.zip) (3.3 MB, already
  downloaded to `/tmp/drdevtools-snap.zip`). It contains **1199 `,v` RCS files**.
- Layout inside the zip:
  - `drdevtools/drdevtools/` — the real module (tools, devsys, libraries) → **convert this**
  - `drdevtools/CVSROOT/` — CVS admin metadata → **exclude**
  - No `Attic/` dirs → no deleted-file edge cases.
- History is shallow: essentially **one `cvs import` on 2003-08-15**, committer
  **`wbniv`** (= Will). Vendor-branch tags `vendor` / `start` present; a stray `is`
  author appears 2× (confirm/clean during conversion).
- `gh` authed as `wbniv` with `repo` + `workflow` scopes (sufficient to create the
  repo in an org and push). Existing org `foundry-linux`; the new org does not exist yet.
- `cvs2git`/`cvs2svn` is **not installed**.

## The one manual step (unavoidable)

The `developer-resources-co` org **does not exist yet** (name is available).
Free-plan GitHub **org creation is web-UI only** — not in the API/`gh`. User creates
the empty org once at [github.com/account/organizations/new](https://github.com/account/organizations/new)
(name: `developer-resources-co`). Everything downstream is automated.

## Plan

### 1. Prereqs — Dockerized converter (NO host installs)
- **Constraint (user):** install nothing on the host. The host is Python-3.13-only,
  PEP-668-locked, and has no `cvs`/`rcs`/`cvs2svn` — and the classic `cvs2git` is
  Python-2. So run the converter inside a **throwaway Docker container** built on an
  older base image (Debian bullseye / Ubuntu focal) that still packages `cvs2svn`.
- Docker 29 is present and usable without sudo (verified).
- The host already has `git`; the fast-import + final repo creation happen host-side so
  the repo lands directly at `~/SRC/drdevtools`. Nothing is installed on the host.

### 2. Stage the raw CVS repo
- Reuse `/tmp/drdevtools-snap.zip`; extract to `/tmp/drdevtools-cvs/`.
- CVS repo root for conversion = `/tmp/drdevtools-cvs/drdevtools` (so the module
  `drdevtools/` and `CVSROOT/` sit beside each other, as cvs2git expects a CVSROOT).

### 3. Author map
- Create `/tmp/cvs-authors.txt`:
  - `wbniv = Will Norris <wbnorris@gmail.com>`
  - investigate the `is` author (likely parse artifact); map to Will or drop.
- cvs2git `--options` / `--author-transforms` keyed off this map.

### 4. Convert CVS → git
- **In a container** (CVS repo mounted read-only, an output dir mounted rw), install
  and run `cvs2git` to emit blob + dump streams. `cvs2git` is the authority on the real
  history (resolves the binary-file head-field ambiguity from investigation — expect it
  to report the true commit/branch/tag set):
  ```
  docker run --rm \
    -v /tmp/drdevtools-cvs/drdevtools:/cvs:ro \
    -v /tmp/cvs2git-out:/out \
    debian:bullseye bash -c 'set -euo pipefail
      apt-get update -qq && apt-get install -y -qq cvs2svn
      cvs2git --blobfile=/out/git-blob.dat --dumpfile=/out/git-dump.dat /cvs'
  ```
  (CVSROOT/ in the mount is auto-skipped by cvs2git.)
- **On the host**, fast-import into a fresh **standalone** repo at `~/SRC/drdevtools`:
  ```
  git init ~/SRC/drdevtools && cd ~/SRC/drdevtools
  cat /tmp/cvs2git-out/git-blob.dat /tmp/cvs2git-out/git-dump.dat | git fast-import
  git checkout -f HEAD
  ```
  (Its own `.git`; it sits inside the `wbniv/homedir` tree — confirm homedir ignores
  `SRC/*` subrepos so it isn't nested-committed.)
- **Author rewrite:** map `wbniv` (+ stray `is`) → `Will Norris <wbnorris@gmail.com>`
  with `git filter-branch --env-filter` (ships with host git; repo is tiny). Verify with
  the author check in Verification.
- Carry the existing `docs/investigations/...md` (and this plan) into the new repo's tree.

### 5. Tidy
- Rename default branch to `main`.
- Drop CVS cruft: the vendor/`start` import tags can be kept or pruned; keep `main`
  clean. Remove stray lock file `#cvs.rfl...` if it survived conversion.
- Add `README.md` (project description + "migrated from SourceForge CVS, 2026-06-10"
  provenance line) and `LICENSE` files (GPLv2 / LGPLv2 as the source declares).

### 6. Create remote + push (automated)
- After the user creates the empty org, create the repo in it and push:
  ```
  gh repo create developer-resources-co/drdevtools --public \
     --description "Developer Resources Sega Genesis/SNES dev tools (migrated from SourceForge CVS)"
  git remote add origin https://github.com/developer-resources-co/drdevtools.git
  git push -u origin main --tags
  ```

### 7. Leave SourceForge as archived origin + post migration notice
- No deletion; keep SourceForge as a read-only archive.
- The project has **no Wiki/News tool** (only Activity, Bugs, Code), so the notice goes in the
  **project description**. Exact wording in [docs/sourceforge-notice.md](../sourceforge-notice.md):
  prepend the Markdown block to the *Description* field and set the short summary via
  [admin/overview](https://sourceforge.net/p/drdevtools/admin/overview). Requires SF login —
  a single manual paste (no unauthenticated path).

## Critical files / paths
- `drdevtools/docs/investigations/2026-06-10-hosting-sourceforge-vs-github.md` — provenance, carried into new repo.
- `/tmp/drdevtools-snap.zip` → `/tmp/drdevtools-cvs/drdevtools` (conversion input, mounted into container).
- `/tmp/cvs2git-out/` — container output (git-blob.dat + git-dump.dat), imported host-side.
- Converter: throwaway `debian:bullseye` container (no host installs).
- New repo working tree: `~/SRC/drdevtools`.

## Verification

1. **Conversion produced a git repo with history**
   `git -C ~/SRC/drdevtools log --oneline --stat | head` — expect ≥1 commit dated
   2003-08-15, author "Will Norris", file tree under `tools/`, `devsys/`, etc.
2. **File count matches the CVS module**
   Compare `git ls-files | wc -l` against the ~1199 source files (minus CVS metadata) — should be in the same ballpark.
3. **No CVS artifacts leaked**
   `git -C ~/SRC/drdevtools ls-files | grep -E ',v$|/CVS/|#cvs' || echo CLEAN` → `CLEAN`.
4. **Author identity correct**
   `git -C ~/SRC/drdevtools log --format='%an <%ae>' | sort -u` → only `Will Norris <wbnorris@gmail.com>`.
5. **Pushed and browsable**
   `gh repo view developer-resources-co/drdevtools --json url,visibility,defaultBranchRef`
   → public, default branch `main`, URL resolves.
6. **Remote tree matches local**
   `git ls-remote origin` shows `main` (+ tags) at local HEAD.

Paste raw output under each step and mark PASS/FAIL before promoting the TODO item.

## Verification results — executed 2026-06-10

1. **History present** — PASS

        9e8f889 2003-08-15 09:45:46 +0000 Will Norris: initial import
        56c97f1 2003-08-15 09:40:36 +0000 Will Norris: initial import
        (4 import commits + 1 provenance commit; tree has tools/ devsys/ runtime/ libs/)

2. **File count matches** — PASS — `1199` tracked source files (exact match to the 1199 CVS masters).

3. **No CVS artifacts** — PASS — `CLEAN`.

4. **Author identity** — PASS (with nuance) — imported commits all `Will Norris <wbnorris@gmail.com>`;
   the single provenance commit uses the host git identity `Will Norris <will@biohack.net>` (also Will).

5. **Pushed and browsable** — PASS

        {"defaultBranch":"main","url":"https://github.com/developer-resources-co/drdevtools","visibility":"PUBLIC"}

6. **Remote matches local** — PASS — local HEAD `ff3a1b9` == remote `main` `ff3a1b9`; tag `start` pushed. MATCH.

**Outcome:** live at [github.com/developer-resources-co/drdevtools](https://github.com/developer-resources-co/drdevtools).
Converted via `cvs-fast-export` in a throwaway `ubuntu:20.04` container (the `cvs2svn`/`debian:bullseye`
path in §1/§4 failed — `cvs2svn` is Python-2 and absent from current Debian/Ubuntu; `cvs-fast-export` is
the working tool). SourceForge left in place as archived origin (§7).

---

# Remediation: render every URL as a link (memory + hooks)

**The rule (Will's words):** it applies to **ALL URLs** — no carve-out for "click here"
or actionable ones. Every URL → `[label](url)`, no exceptions.

**Trigger:** I wrote the org-creation URL **inside backticks** (a code span), which
renders as an inline **code span** — monospace, never a hyperlink (see Will's screenshot
of `we-want-to-migrate-melodic-church.html`). Two compounding defects:
1. **Backtick-wrapped URLs are a defect** — a code span looks like a URL but cannot be a link.
2. **`md-to-pdf.sh` only linkifies `[label](url)`** — it does not autolink bare URLs or `<url>`.
So the only form that renders clickable is `[label](url)` with **no backticks**.

### 1. Extend the memory (not a duplicate)
Edit `~/.claude/projects/-home-will/memory/feedback_md_renderer_no_autolinks.md`:
- Reassert scope: the rule is **ALL URLs, everywhere** — markdown files **and** chat / TUI
  replies. No "click-here" subset; that narrowing is itself the recurring mistake.
- Add the new failure mode: **URLs inside backticks / code spans render as non-clickable
  monospace** — never wrap a URL in backticks; use `[label](url)`.
- Append this 2026-06-10 failure to the recurring-blind-spot log.
- Update the `description:` frontmatter to cover chat output + the backticks pitfall.

### 2. Hook A — file linter (PostToolUse `Write|Edit`)
New `~/.claude/hooks/md-linkcheck.sh` (direct-call, like the `tab-*.sh` hooks):
- On a `.md` write/edit, flag any `https?://…` that is **not** in `[..](..)` form —
  including URLs wrapped in backticks (a code span is not a link).
- Print a **non-blocking** warning listing offending `file:line` so I fix before moving on.
- Wire into the existing `PostToolUse → Write|Edit` block in `settings.json`.
- Would have caught the backtick-wrapped URL in this very plan file.

### 3. Hook B — salience reminder (SessionStart + UserPromptSubmit)
New `~/.claude/hooks/links-reminder.sh`:
- Echo one line into context (its stdout is injected as context):
  "Every URL → `[label](url)`, no backticks, in files AND chat replies. ALL URLs. A bare or
  backtick-wrapped URL is a defect."
- Wire into the existing `SessionStart` block (once per session). Optionally also
  `UserPromptSubmit` for per-turn reinforcement on this known blind spot.

### What hooks can and cannot enforce (the honest mechanics)
Claude Code hooks fire on **events** (tool calls, session start, prompt submit, stop) and can:
- **Block or warn on tool calls** — `PreToolUse` can return a non-zero exit / decision to
  *reject* a Write|Edit, or `PostToolUse` can emit a warning after it lands.
- **Inject context** — `SessionStart` / `UserPromptSubmit` stdout is added to my context.

They **cannot** post-process the assistant's chat message — there is no hook that rewrites
text I send to the terminal. So enforcement splits:

| Surface | Mechanism | Strength |
|---|---|---|
| URLs in `.md`/files I write | **Hook A** (`PostToolUse Write|Edit`, can be made **blocking** via `PreToolUse`) | hard enforce |
| URLs in my chat replies | **Hook B** reminder + memory | salience only (behavioral) |

**Enforcement strength choice for files:** Hook A can be either
- **warn** (PostToolUse, non-blocking) — informs me after the write, I fix next edit; or
- **block** (PreToolUse Write|Edit) — rejects the write if the new content contains a
  bare/backtick URL, forcing the `[label](url)` form *before* it lands. Stronger; mirrors the
  existing `plan-first.sh` / `git-add-guard.sh` blocking pattern already in `PreToolUse`.

Recommend **block** for files (it's the only true enforcement) + reminder for chat.

### Open choice
Hooks added as **direct-call** scripts in `~/.claude/hooks/` (simple, self-contained) vs.
routed through the `hook-runner.sh` sha256 wrapper (consistent with the wrapped hooks, but
needs the python-tui-lib + checksum regen dance). Default: direct-call.
