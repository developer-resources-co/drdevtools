# WLA-DX issue #619 — ROM use reported over 100% (`.background` + `overwrite`)

**Date:** 2026-06-12
**Issue:** [vhelin/wla-dx#619 — "Assembler reported ROM use can go over 100%"](https://github.com/vhelin/wla-dx/issues/619) (open, label `bug`, reporter maxim-zhao, 2023-12-27, no maintainer replies)
**Method:** Read the upstream source at HEAD (`8e55b08`, 2026-06-02). Traced the reporting code path and the `.background`/`overwrite` write paths by hand. **A build+run repro was not done** — compiling the freshly-cloned external repo was denied by the sandbox, so the exact byte-level interleaving for the OVERWRITE case is reasoned from source, not observed (see *Evidence gap* below).
**Why we care:** WLA-DX is the **#1 packaging target** for foundry linux in [`2026-06-11-snes-65816-toolchains.md`](2026-06-11-snes-65816-toolchains.md) — broad SNES/65816 use and a PVSnesLib dependency. This note answers: does #619 affect that plan?

---

## TL;DR recommendation

**This is a cosmetic statistics-display bug, not a codegen bug. It does not affect the assembled or linked ROM. It should not delay or complicate packaging WLA-DX.**

1. **Package WLA-DX as-is.** The defect lives in a verbose-only (`-v` / `g_verbose_level >= 2`) end-of-run usage summary that prints *after* the object file is already written (`phase_4.c:2560` then `:2569`). Output bytes are untouched.
2. **Do not carry a downstream patch in the `.deb`.** Patching a cosmetic display number is not worth the maintenance drift against upstream. Ship stock.
3. **Optionally upstream a fix** as a goodwill contribution (the project is GPL-2.0, actively maintained, and we want a good relationship as a packager). A drafted patch + regression test are ready in [`wla-dx-619/`](wla-dx-619/) (build-and-verify steps in its README); the issue has sat untouched for ~2.5 years, so a clean PR with a repro test would likely land.
4. **If a number is needed, trust the linker, not the assembler.** `wlalink`'s own report (`wlalink/main.c:628–681`) iterates each bank byte exactly once and **cannot exceed 100%**. Final placement is the linker's job anyway; the assembler's object-mode per-bank percentage is only ever an estimate.

---

## The bug (from the issue)

Minimal repro: a 32×$4000 SMS memory map, `.background "PS1-J.SMS"` (a full 512 KB image), then one 100-byte `overwrite` section at `.org $1000`. The verbose summary prints **bank 0 at 16,484 bytes = 100.61% used** (16,384-byte bank + the 100 section bytes counted again). The reporter saw worse cases — 128.93%, 456.18%, 955.11% — with more/larger sections, and noted "`.background` alone works; `.background` + `overwrite` double-counts."

`100 / 16384 = 0.61%`, so the overage is *exactly the section size* added on top of an already-full bank. That is the signature of double counting.

---

## Root cause — a dual accounting model that can overlap

The offending function is `show_project_information_object()` (`phase_4.c:3344`). Per bank it builds **two independent tallies and sums them** (`phase_4.c:3442`):

```c
total_used_rom += used_rom + used_sections;   /* phase_4.c:3442 */
...
f = ((float)(used_rom + used_sections))/bank_size * 100.0f;   /* :3448 */
```

- **`used_rom`** — a byte scan of the global usage map (`phase_4.c:3385–3388`):
  ```c
  for (j = 0; j < bank_size; j++)
    if (g_rom_banks_usage_table[bank_address + j] > 0)
      used_rom++;
  ```
- **`used_sections`** — a per-section *size* tally walked from the section list (`phase_4.c:3390–3440`).

These two methods describe **the same bytes** whenever a section overlaps something already marked in the byte map. The summary makes no attempt to reconcile them except for a partial guard (below).

**`.background` is what makes the overlap total.** The directive memsets the entire image region of the usage map to `2` up front (`phase_1.c:7415`):

```c
memset(g_rom_banks_usage_table, 2, background_size);   /* phase_1.c:7415 */
```

So with a full-ROM background, `used_rom` alone already equals a full bank — **100% before a single section is counted.** Anything `used_sections` adds is pure overage. That is precisely why "`.background` alone is fine" (only `used_rom`, capped at `bank_size`) but "`.background` + a section" exceeds 100%.

### Defect A — FORCE sections have *no* overlap guard (definite)

The FORCE branch increments `used_sections` for **every** byte, unconditionally, even when that byte is already counted in `used_rom` (`phase_4.c:3398–3409`):

```c
else if (status == SECTION_STATUS_FORCE) {
  for (j = 0; j < s->size; j++) {
    int address = g_bankaddress[s->bank] + s->address + j;
    if (s->address + j < banksize)
      rom_banks_usage_table[address] = 1;   /* local copy only */
    used_sections++;                         /* ALWAYS — no map check */
  }
}
```

A FORCE section overlapping `.background` is therefore **always** double-counted. This is the clearest, unambiguous instance of the bug class and is provable from the source alone.

### Defect B — the FREE-family branch is also unguarded (definite)

The first branch blindly adds the section size with no map check at all (`phase_4.c:3395–3397`):

```c
if (status == SECTION_STATUS_FREE || ... || SEMISUPERFREE)
  used_sections += s->size;   /* no overlap check vs used_rom */
```

A free/semifree section over background double-counts too.

### Defect C — OVERWRITE: the guard exists but is fed a stale/incomplete map (the reported case)

OVERWRITE is the *only* status that guards (`phase_4.c:3416–3436`):

```c
if (rom_banks_usage_table[address] == 0) {   /* local copy */
  rom_banks_usage_table[address] = 1;
  used_sections++;                             /* counted as "new" */
}
else
  uncounted_overwrite_sections++;              /* correctly suppressed */
```

The guard compares against `rom_banks_usage_table`, a **local memcpy** of the global map taken at function entry (`phase_4.c:3359`). For the guard to *miss* — and thus count the 100 reported bytes as "new" — the section's resolved address `g_bankaddress[s->bank] + s->address + j` must point at bytes the local map shows as `0`, i.e. **not** where `.background` actually sits, while `used_rom` separately counts the real background bytes elsewhere. The relevant subtlety: in OBJECT mode (which `.background` *requires*, `phase_1.c:7367`) a section's payload lives in the section's own buffer — `mem_insert*` with `g_section_status == ON` does **not** mark `g_rom_banks_usage_table` (`phase_4.c:3807`) — and final placement is the linker's job. So the report's per-section address arithmetic is resolving the position of a section the assembler has **not actually placed**, and when that math disagrees with where background bytes really are, the guard fails open and the section size lands on top → the exact-section-size overage observed (100 → 100.61%; larger/more sections → the 456%/955% cases).

---

## Evidence gap (stated honestly)

Defects A and B are provable from source: unconditional `used_sections++` against a background-filled map double-counts, full stop. **Defect C — the exact reason the OVERWRITE guard fails open in object mode — is reasoned, not observed.** Confirming the precise interleaving (what `s->bank`/`s->address` hold for an unplaced overwrite section, and which addresses read `0` in the local map) needs a build + `-v` run of the issue's repro, which the sandbox denied as untrusted-code execution. The structural diagnosis (dual overlapping accounting + a background-filled map) is solid regardless of which branch trips in any given repro; it explains every magnitude the reporter saw.

---

## Severity: cosmetic / display-only

- The function only runs at `g_verbose_level >= 2` and **after** `write_object_file()` (`phase_4.c:2560`). It feeds `print_text` only — never the output buffer.
- The emitted object/ROM is correct. The number printed is wrong.
- The linker's parallel report (`wlalink/main.c:_show_rom_ram_information`) is structurally immune: it iterates `a < g_banksizes[r]` once per byte (`main.c:631`), so its percentage is bounded by 100% by construction.

**For drdevtools: zero impact on a packaged toolchain's correctness.** A user assembling with `-v` sees a wrong percentage; the binary is fine.

---

## Proposed upstream fix (if we choose to contribute)

The root issue is summing a byte-map scan and a per-section size tally that describe overlapping bytes. **Single-source-of-truth** is the clean fix: merge every locatable section into the local copy first, then count the map **once**.

```c
/* 1. Start from the global map (already includes .background and out-of-section data). */
memcpy(rom_banks_usage_table, g_rom_banks_usage_table, g_max_address);

/* 2. Stamp every locatable section (FORCE/OVERWRITE/ABSOLUTE) into the local map.
      Never increment a separate counter — just set bytes. */
/*    ... set rom_banks_usage_table[address] = 1 for in-bank bytes ... */

/* 3. Count used bytes ONCE from the merged local map. */
for (j = 0; j < bank_size; j++)
  if (rom_banks_usage_table[bank_address + j] != 0)
    bank_used++;
/* total_used_rom += bank_used;  — no `used_rom + used_sections` sum, so overlaps can't double-count */
```

FREE-family sections (`SECTION_STATUS_FREE`/`SEMIFREE`/…) have **no resolved address in object mode** — the linker places them — so the assembler genuinely cannot know whether they overlap background. Two honest options:

1. **Report them separately and clamp:** keep a "sections (unplaced): N bytes" line but clamp the headline per-bank figure to `bank_size` and the total to `total_rom_size`, since >100% is never physically meaningful.
2. **Defer the percentage to the linker entirely** for object mode and have the assembler print only raw section sizes, with a note that final usage comes from `wlalink`.

A minimal, low-risk PR that resolves the reporter's symptom without restructuring: clamp the printed per-bank figure to the bank size (the summary total is then the sum of clamped per-bank figures and is bounded too). Pair it with a regression test using the issue's repro (a small generated background file + a 100-byte overwrite section) asserting the summary never prints `>100%`.

**A drafted patch + test live in [`wla-dx-619/`](wla-dx-619/):** `0001-Clamp-object-mode-ROM-usage-report-to-bank-capacity-.patch` (`git format-patch` against HEAD `8e55b08`) and `test/` (`main.s` repro + `verify.sh` output check). ⚠️ They are authored from source reading only — **not yet built or run** (the sandbox declined to compile the cloned repo); see that folder's README for the build-and-verify steps before any upstream submission.

---

## References

- Issue: <https://github.com/vhelin/wla-dx/issues/619>
- Reporting fn: `phase_4.c:3344` `show_project_information_object()` — dual tally at `:3442`, `:3448`
- `.background` memset: `phase_1.c:7415`
- Object-mode section writes skip the usage map: `phase_4.c:3807`
- Linker's bounded report (the trustworthy one): `wlalink/main.c:628–681`
- Packaging context: [`docs/investigations/2026-06-11-snes-65816-toolchains.md`](2026-06-11-snes-65816-toolchains.md), `TODO.md` (WLA-DX `.sym` importer)
