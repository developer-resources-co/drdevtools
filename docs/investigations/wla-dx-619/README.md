# WLA-DX issue #619 — draft fix + regression test

Companion artifacts for [`../2026-06-12-wla-dx-rom-overreport-issue-619.md`](../2026-06-12-wla-dx-rom-overreport-issue-619.md).
Upstream issue: <https://github.com/vhelin/wla-dx/issues/619>.

## What's here

| File | Purpose |
| --- | --- |
| `0001-Clamp-object-mode-ROM-usage-report-to-bank-capacity-.patch` | `git format-patch` against wla-dx HEAD (`8e55b08`). Upstream-ready. |
| `test/main.s` | Minimal repro: full-ROM `.background` + a 100-byte `overwrite` section. |
| `test/verify.sh` | Assembles with `-v` and fails if any usage line reports >100%. |

## The fix

`show_project_information_object()` (`phase_4.c:3344`) prints per-bank ROM usage as
`used_rom + used_sections`, where `used_rom` is a byte-map scan that **already includes
the entire `.background` image** and `used_sections` is a separate per-section size
tally. When a section overlaps background-initialised ROM the two count the same bytes,
so the printed figure exceeds 100% (reporters saw 100.61%, 456.18%, 955.11%).

The patch clamps the reported per-bank usage to the bank size. A bank is never more than
full, and the summary total is the sum of the clamped per-bank figures, so it is bounded
by the ROM size too. This is a **verbose-only display statistic computed after the object
file is written** — the change cannot affect emitted output.

### Why clamp rather than re-architect the tally

The accounting is two overlapping methods (byte-map scan + per-section size). A full
single-source-of-truth rewrite (merge all *locatable* sections into one map, count once,
add only the genuinely-unplaceable FREE-family section sizes) is the "proper" fix and is
sketched in the parent investigation. But in OBJECT mode the assembler does not do final
placement — the linker does — so the assembler's per-bank number is inherently an
estimate. The clamp respects every existing per-section semantic (including the explicit
"every byte in a FORCE .SECTION counts" intent), is correct by inspection, and directly
resolves the titled symptom with minimal risk. The linker's own report
(`wlalink/main.c:628`) already counts each byte once and is bounded by construction, so
the trustworthy final number is available there.

## Verification status — NOT yet built or run

⚠️ The patch and test are **drafts authored from source reading only.** Building the
freshly-cloned upstream repo was declined by the sandbox in the session that produced
these, so neither the patch's compilation nor `verify.sh` has been executed here. Before
sending upstream:

```sh
git clone https://github.com/vhelin/wla-dx && cd wla-dx
git am < /path/to/0001-Clamp-object-mode-ROM-usage-report-to-bank-capacity-.patch
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j

# pre-patch the test should FAIL (prints >100%), post-patch it should PASS:
WLA="$PWD/build/binaries/wla-z80" /path/to/test/verify.sh
```

Expected: `verify.sh` exits 1 on stock HEAD (bug present), exits 0 with the patch applied.
Wiring `test/` into upstream `run_tests.sh` requires extending that harness to assert on
verbose output (today it only checks that `make` succeeds) — note this in the PR.
