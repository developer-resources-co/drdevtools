# Memory Index

- [Links belong in documents](feedback_links_in_docs.md) — proactively write links/diffs/references into the relevant markdown doc; don't wait to be asked
- [Features not platforms](features-not-platforms.md) — gate code by feature/capability flags (`#ifdef SPC700`), not platform macros (`#ifdef SNES`); the platform block defines which features it has
- [User background](user_background.md) — Will is a commercial SNES dev; used Zardoz compiler on Demolition Man and Izzy's Olympic Quest; firsthand primary source on 1990s 65816 tooling
- [Default base: Foundry/Kubuntu 26.04](default-base-foundry-2604.md) — target 26.04 (ubuntu:26.04 for containers), not 24.04; 26.04 ships libcppdap-dev first-class
- [MAME Lua debugger control flow](mame-lua-debugger-control-flow.md) — for MAME 0.277 `-debugger none` Lua: reuse `mame_bridge.lua`; emu.pause/unpause (not execution_state), bp pseudo-holds, bpset cond/act gotchas, lazy-init under autoboot
- [$HOME is a git repo](home-is-a-git-repo.md) — `/home/will` is itself a git repo; a new repo under `~/SRC` must be cleanly `git init`'d standalone (verify `rev-parse --show-toplevel`) or git commands fall through to the home repo

<!-- BEGIN GLOBAL MEMORY (managed by claude-housekeeping; do not edit) -->

## User (inherited from ~)

- [user_profile.md](user_profile.md) — Will's role, setup, and desktop/dev preferences
- [user_mammouth_subscription.md](user_mammouth_subscription.md) — €20/mo Mammouth.ai Standard: multi-model API (GPT-4o, Claude, Gemini, Mistral, Llama) at api.mammouth.ai/v1

## Project (inherited from ~)

- [home_src_layout.md](home_src_layout.md) — Projects moved ~/SRC/<name> → ~/<name> post-reformat; projects.json + hook-runner still assume ~/SRC (hook-runner patched via symlink)

## Feedback (inherited from ~)

- [feedback_wayland_keybindings.md](feedback_wayland_keybindings.md) — How held modifiers combine with ydotool on GNOME Wayland; architecture for tab switching across apps
- [feedback_wezterm_flatpak.md](feedback_wezterm_flatpak.md) — Use flatpak enter + GUI socket (not flatpak run or mux socket) for WezTerm CLI access
- [feedback_run_task_md.md](feedback_run_task_md.md) — After writing/editing any .md file, run `task md -- {filename}` to preview in browser; never run on non-markdown files
- [feedback_tooling_choices.md](feedback_tooling_choices.md) — Prefer hand-rolled over integration libs when Will already does the pattern manually (e.g., PWA); convert content to Markdown upfront, not "start HTML, migrate later"
- [feedback_bangkok_cost_estimates.md](feedback_bangkok_cost_estimates.md) — Default lower on Bangkok cost estimates; verify against Lalamove/Grab/Makro/local norms, not Western/expat-tier defaults
- [feedback_excluded_providers.md](feedback_excluded_providers.md) — Don't recommend Facebook/Meta (except WhatsApp) or Oracle as providers anywhere; Oracle's "Always Free" ARM tier is mostly fictional (capacity-starved)
- [feedback_no_speculation.md](feedback_no_speculation.md) — Verify before advising: RDAP for domains, file reads for config, the screenshot already on screen — don't list generic "common causes" when state is fetchable
- [feedback_use_task_tracking.md](feedback_use_task_tracking.md) — Reach for TaskCreate/TaskUpdate proactively on multi-step work; don't wait for the auto-reminder
- [feedback_commit_scope.md](feedback_commit_scope.md) — "Commit the others" means the files just enumerated, not everything git status shows; auto-mode doesn't expand scope
- [feedback_md_renderer_no_autolinks.md](feedback_md_renderer_no_autolinks.md) — md-to-pdf.sh silently drops `<url>` autolinks; always use `[url](url)` form
- [feedback_seed_dont_clone.md](feedback_seed_dont_clone.md) — Seeding a new site from an existing one + swapping wordmark/color isn't enough — the source's visual fingerprint carries through. Ship distinctive elements with the seed, not after.
- [feedback_prefer_proper_fix.md](feedback_prefer_proper_fix.md) — When offering fix-scope options, default to the proper/architectural one. Don't lead with the minimal fix as "recommended."
- [feedback_public_vs_internal_surfaces.md](feedback_public_vs_internal_surfaces.md) — Public marketing pages (colophon, homepage) describe visible craft — never internal infra (repo URLs, predecessor projects, deploy pipeline, IaC paths).
- [feedback_node24_everywhere.md](feedback_node24_everywhere.md) — Always use Node 24 on all supported platforms; confirmed: GitHub Actions, Codemagic.io.
- [feedback_always_astro_tailwind.md](feedback_always_astro_tailwind.md) — Always scaffold Astro + Tailwind 4 + @theme tokens even when design is undecided; path choice is infra, not framework.

<!-- END GLOBAL MEMORY -->
