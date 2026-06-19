---
title: SPC700 IPL boot ROM — clean-room reimplementation for legal MAME use + public release
date: 2026-06-19
status: draft / decisions-pending
---

# SPC700 IPL boot ROM — clean-room reimplementation

> **Goal.** Produce an *independently created* 64-byte SPC700 IPL boot ROM that is
> behaviourally compatible with the SNES audio subsystem, so that (1) it can be
> dropped into MAME in place of the copyrighted Nintendo IPL — letting **anyone**
> run the SNES driver legally, not just owners of real hardware — and (2) **both
> the source and the assembled bytes can be committed to a public repository.**
>
> The assembler is [`tools/spasm`](../../tools/spasm) (the in-house multi-CPU macro
> assembler; we own it). Verification rides the existing drmon MAME Lua bridge.

> [!IMPORTANT]
> **This document is engineering planning, not legal advice.** The "public
> distribution" goal is where the risk concentrates, and Nintendo is an
> aggressive litigant. **Have qualified IP counsel review §3 + the provenance
> bundle before anything is pushed public.** Nothing below is a guarantee of
> non-infringement.

---

## 1. Scope & terminology

- **The "SPC700 BIOS" = the 64-byte IPL boot ROM** mapped at `$FFC0–$FFFF` in the
  SNES APU (S-SMP) address space, with the reset vector at `$FFFE/$FFFF`. It is a
  tiny bootloader: at reset it signals readiness on the CPU↔APU mailbox ports and
  runs an upload handshake that lets the main 65816 stream code/data into the
  64 KB of APU RAM, then transfers control to the uploaded program. **That is the
  whole artifact** — 64 bytes.
- **Out of scope:** the S-DSP, BRR audio, and any game's audio engine. Those are
  uploaded by cartridges at runtime; they are not the boot ROM and not Nintendo's
  to begin with.
- **Naming hygiene:** the deliverable is an *"independent SPC700 IPL-compatible
  boot ROM."* Do **not** market it as "Nintendo's", "SNES BIOS", or with any
  Nintendo trademark. Trademark ≠ copyright; keep both clean.

---

## 2. Tooling verification — REQUIRED FIRST CHECK ✅ DONE

The user asked to **first verify an SPC700 assembler exists in `../drdevtools/`.**
It does. Evidence gathered 2026-06-19:

| Question | Finding | Evidence |
|---|---|---|
| Is there an SPC700 assembler? | **Yes** — `tools/spasm`, a multi-CPU macro assembler with a dedicated SPC700 backend. | `tools/spasm/opcode70.asm`, `tools/spasm/cam700.asm` |
| Is the backend really SPC700 (not 6502)? | **Yes** — implements SPC700-unique mnemonics `TCALL XCN CBNE DBNZ BBS/BBC MUL DIV DAA DAS CLRC SETC MOV`. | `opcode70.asm:13–87` |
| Other targets in the same tool | 65816 (`opcode81`/`cam816`), 68000 (`opcode68`/`cam68`). | makefile, `spaz.asm:3` |
| How is the SPC700 target selected? | **Compile-time** of the assembler: `#ifdef SPC700` / `-DSPC700`. There is no runtime `processor` directive — you build a *separate* SPC700 binary. | `spasm.cpp:71,218,468`, `spaz.asm:91,123` |
| Canonical SPC700 build recipe | `m7.bat` → `nmake "SYSTEM=SPC700" "EXT=700" spasm700.exe`. The SPC700 binary is **`spasm700.exe`**. | `tools/spasm/m7.bat` |
| Who owns it? | **We do.** © "Developer Resources" (Anderson, Seghetti, **Norris IV**); `spasm.cpp:311` credits *"SPC700 additions by William B. Norris IV."* GitHub org `developer-resources-co`. | `makefile`, `spasm.cpp:311` |

**The catch (→ Phase 0):** `spasm` is a 1994 **16-bit DOS** program (TASM + Borland
C `bcc`/`tlink`, `nmake`/`wmake`). This Linux host currently has **no** DOSBox /
dosemu / OpenWatcom / TASM. So a *runnable* SPC700 assembler is not yet in hand —
standing one up is the first phase, not a freebie.

---

## 3. Legal strategy — why this can be public

Four independent doctrines stack in our favour. We rely on **all** of them, not any
single one.

1. **Independent creation is a complete defense to copyright.** Copyright forbids
   *copying*, not arriving at the same result on your own. If two authors
   independently write identical works, neither infringes. The clean room exists
   to produce **documentary proof** we created our bytes without copying Nintendo's.

2. **Merger doctrine / idea–expression merger.** Where a function admits only one
   or a few expressions, expression "merges" with the idea and is unprotectable (or
   protection is "thin"). A **64-byte** boot ROM is the textbook case: every byte is
   forced by (a) the SPC700 instruction set, (b) the **fixed** mailbox port map, (c)
   the upload protocol cartridges depend on, and (d) the 64-byte ceiling. There is
   essentially one sensible way to write it.

3. **Scènes à faire / Altai abstraction-filtration-comparison.** Elements dictated
   by external constraints, efficiency, and standard technique are filtered out
   before any infringement comparison. For this ROM, *almost everything* filters
   out — the fixed addresses, the handshake, the reset vector are all external
   constraints, not creative choices.

4. **Thin copyright on a 64-byte functional artifact.** The originality threshold
   for 64 bytes of maximally-constrained boot code is at or below the floor of
   protectability.

### The "the bytes will probably be identical" reality — read this twice

Because the constraints leave near-zero freedom, a *correct* clean-room
implementation is **very likely to be byte-for-byte identical** to Nintendo's ROM.
Two consequences, pulling in opposite directions:

- **Legally, identical output is fine** — *if* independently created (doctrine 1)
  and *because* the constraints force it (doctrines 2–4). Identity is actually
  **evidence** that expression merged with function.
- **Practically, a bit-identical file invites hash-matching takedowns.** GitHub's
  DMCA process does not adjudicate merger; a claimant can match a CRC/SHA and file,
  and the file comes down pending a counter-notice. **Our provenance bundle (the
  spec, the role attestations, the build logs, the two-toolchain reproduction) is
  exactly the counter-notice ammunition.** Plan for the takedown even though we
  believe we're right.

### Hard rules that protect the position

- **Never look at Nintendo's ROM image or any disassembly of it during spec or
  implementation.** Not to "check," not "just the tricky part." That is the line
  between independent creation and copying.
- **Never commit Nintendo's ROM bytes or a disassembly of them to any repo** —
  including as a test fixture or in this plan. Correctness is judged
  *behaviourally* (Phase 4), never by diffing toward Nintendo's bytes.
- **Do not feed any byte-level comparison back into the implementation.** Observing,
  after the fact, that our bytes coincide is fine as forensic notes by the
  verification lead; *editing our code to match Nintendo's* converts independent
  creation into copying. We don't do it.

---

## 4. Roles & the taint problem (including the LLM)

A clean room is a **Chinese wall**: the people who may study behaviour are walled
off from the people who write code, and the two sides communicate only through an
approved specification.

| Role | May see | May NOT see | Output |
|---|---|---|---|
| **Spec author** (Person A) | Public docs of the CPU↔APU *interface/protocol*; black-box port behaviour; the SPC700 ISA. | Nintendo's ROM image / any disassembly of it. | A behavioural spec (§Phase 1) with **no** Nintendo code. |
| **Clean implementer** (Person B) | Only Person A's spec; the SPC700 ISA; `spasm` docs. | Nintendo's ROM bytes/disassembly; **must attest non-exposure.** | SPC700 source → 64-byte image. |
| **Verification lead** (Person C) | Everything behavioural; may run MAME; may do *forensic-only* byte comparison after Phase 3 is frozen. | — (but must not relay Nintendo bytes back to B). | Behavioural PASS/FAIL + provenance bundle. |

**A and B must be different people.** One person cannot wall themselves off.

**Two honest taint warnings:**

- **The LLM (Claude) is presumed tainted.** Models of this class may have the
  64-byte IPL (it is widely published) in training data, so **Claude must not be
  the clean implementer (Person B).** Claude's role here is *tooling + scaffolding*:
  stand up the assembler (Phase 0), build the MAME verification harness (Phase 4),
  help Person A draft the spec **from public interface docs only**, and assemble the
  provenance/repo. Claude does **not** write the boot-ROM bytes.
- **The original author is also likely tainted.** You wrote `spasm`'s SPC700 backend
  and have decades of SNES work — you have very probably seen Nintendo's IPL
  disassembly at some point. That makes you an excellent **spec author / verification
  lead / tooling owner**, but a **poor "clean" implementer.** If we want a strong
  independent-creation story, **recruit an untainted Person B** (a competent asm
  programmer who can credibly attest they've never studied the SNES IPL). If no
  untainted implementer is available, we fall back to leaning on the merger doctrine
  (§3.2–3.4) and label the clean-room as "best-effort" — weaker, and worth flagging
  to counsel.

---

## 5. The wall — allowed vs forbidden specification sources

The spec (Phase 1) is derived from the **interface contract**, which is functional
and observable, not from Nintendo's expression:

- **Allowed:** documentation describing *what the main CPU does* to upload to the
  APU (the mailbox ports `$2140–$2143` ↔ `$F4–$F7`, the readiness signal, the
  address/counter handshake, the jump); the SPC700 ISA and timing; black-box
  observation of an APU's port behaviour at reset (treating the chip as a sealed
  box); our own `spasm` docs. Method-of-operation / interface facts are not
  protectable expression (cf. the API-as-functional reasoning in *Google v.
  Oracle*).
- **Forbidden:** Nintendo's ROM image; any disassembly/annotated listing of it;
  any source purporting to reproduce its bytes; "reference implementations" that are
  really transcriptions of the original.

The spec describes **requirements** ("at reset the ROM SHALL signal readiness on
the mailbox, then service this upload protocol, then jump"), never a byte listing.

---

## 6. Phases

### Phase 0 — Stand up a runnable SPC700 `spasm` on Linux  *(blocker; decision needed)*

`spasm700.exe` is a 16-bit DOS binary today. Options, cheapest-first:

- **0a — DOSBox-X + vintage toolchain (closest to original).** `apt install
  dosbox-x`; supply TASM + Borland C (you may have them archived as an original
  author); run `m7.bat` under DOSBox-X to build `spasm700.exe`; invoke the
  assembler under DOSBox-X. **Pro:** byte-faithful to the original build. **Con:**
  needs the proprietary Borland/TASM bits; clumsy in CI.
- **0b — OpenWatcom V2 (`m.bat` already uses `wmake`).** Build the DOS binary with
  OpenWatcom (free, scriptable, still emits 16-bit DOS), run under DOSBox-X.
  **Con:** the asm core is **TASM-syntax**; Watcom's `wasm` is MASM-ish — expect
  directive friction (`ifdef`, `OFFSET`, segment model).
- **0c — Native port.** Port the `.cpp` driver + reimplement the asm core's logic
  in portable C++. **Big** effort (the core *is* 16-bit x86 asm); out of scope for v1.
- **0d — Modern SPC700 assembler as a *cross-check only*.** WLA-DX `wla-spc700` or
  byuu's `bass` (both FOSS, SPC700-capable). **Not** the primary per the user's
  "use drdevtools tooling" instruction, but ideal as the **independent second
  toolchain** in Phase 5 (two unrelated assemblers agreeing on the bytes is strong
  evidence).

**Recommendation:** primary = **0a/0b** (own tool, satisfies the instruction);
reserve **0d** for the Phase-5 cross-check. **Exit:** `spasm700` assembles a
trivial `.asm` to expected bytes (Phase-0 verification step 1).

### Phase 1 — Behavioural specification (Person A, untainted)

Write `spec/ipl-behaviour.md` in the new public repo: a numbered, testable
specification of the boot ROM's behaviour derived **only** from §5-allowed sources.
Cover: reset entry/vector requirement; readiness signalling on the mailbox; the
upload handshake (address setup, counter/echo pacing, multi-block, final jump);
register/zero-page init obligations; the 64-byte budget. **No byte listings.** Each
clause gets an ID so Phase 4 tests can cite it.

### Phase 2 — Clean implementation (Person B, untainted)

From the Phase-1 spec **only**, Person B writes `src/ipl.asm` in SPC700 assembly for
`spasm700`. Person B signs `ATTESTATION.md` (never studied the SNES IPL; worked
solely from the spec). Iteration is allowed against the **spec** and against
**behavioural** test failures — never against Nintendo's bytes.

### Phase 3 — Assemble

`spasm700 src/ipl.asm → build/ipl.bin`. Assert **exactly 64 bytes** and the reset
vector lands at `$FFFE/$FFFF`. Record the SHA-256 of our image. (We do **not**
compare it to Nintendo's hash to drive anything; see §3 hard rules.)

### Phase 4 — Behavioural verification in MAME (Person C)

This is the real correctness oracle. Use the existing drmon SNES Lua bridge
(`task drmon-snes…`, the SPC700 register + APU-RAM windows from
[`2026-06-12-spc700-window.md`](2026-06-12-spc700-window.md)):

1. Make MAME use **our** `ipl.bin` instead of its built-in SPC700 IPL (see §7).
2. Boot a cart whose audio engine performs a real APU upload.
3. Via the bridge, confirm: readiness handshake observed → bytes land in APU RAM at
   the intended addresses → SPC700 PC jumps into the uploaded program → audio engine
   runs. Cross-check APU-RAM reads + SPC700 regs against expectations.
4. Regression set: several carts / a purpose-built APU exerciser; multi-block upload;
   edge cases in the counter handshake.

PASS = real software boots and runs its audio through *our* ROM. No byte-diffing.

### Phase 5 — Independent reproduction + provenance bundle

- **Second toolchain (0d):** assemble the *same* `src/ipl.asm` with `wla-spc700`
  and/or `bass`; confirm identical bytes. Two unrelated assemblers agreeing
  documents that the bytes are a function of the source, not a copy.
- **Provenance bundle** (the counter-notice kit): the Phase-1 spec, Person B's
  attestation, dated build logs, the two-toolchain reproduction, this plan, and a
  `PROVENANCE.md` narrating the wall and timeline.

### Phase 6 — Public release  *(GATED on counsel + user go-ahead)*

Stand up the public repo (§8), license it (recommend a permissive code license +
an explicit "independent reimplementation, see PROVENANCE.md" statement). **Do not
push until §3's review happens and the user explicitly approves** — publishing is
irreversible and is the whole risk surface.

---

## 7. MAME integration specifics  *(verify, don't assume)*

- MAME carries the SPC700 IPL internally (commonly the `spc700.rom` region, 64
  bytes, a fixed expected hash). **Action:** confirm the exact region name + hash +
  load path in the MAME version drdevtools drives — **don't guess** (CLAUDE.md:
  "explain the gap"). This is Phase-4 verification step 0.
- **If our bytes equal Nintendo's** (likely): the file satisfies MAME's hash check
  and is a literal drop-in.
- **If our bytes differ:** MAME will reject the hash; we'd either run a MAME built
  to accept an alternate IPL, or pursue upstreaming an "open IPL" option. Note for
  realism: MAME upstream generally only ships bit-exact dumps, so a *differing*
  reimplementation is mainly useful for our own builds + the public repo, with a
  "how to use with MAME" note — not an automatic MAME-distribution win.
- Preservation framing: this directly serves the "everyone, not just SNES owners"
  goal — a freely-distributable IPL removes the "you must dump your own" barrier.

---

## 8. Deliverables — public repo layout

A **separate, single-purpose public repo** (clean provenance, easy to point a
license + DMCA counter-notice at), e.g. `snes-ipl-cleanroom`:

```
README.md            # what it is, what it is NOT (no Nintendo marks), how to use w/ MAME
LICENSE              # permissive code license
PROVENANCE.md        # the wall, roles, timeline — the legal narrative
ATTESTATION.md       # Person B's signed non-exposure statement
spec/ipl-behaviour.md# Phase-1 behavioural spec (interface-derived)
src/ipl.asm          # SPC700 source (assembles under spasm700; cross-checks under wla/bass)
build/ipl.bin        # assembled 64-byte image  ← the "public bytes"
build/ipl.sha256     # our hash
build.sh / Taskfile  # reproducible: source → bytes, both toolchains
tests/               # behavioural MAME harness + exerciser (NO Nintendo bytes)
```

Keep the messy tooling (DOSBox build of `spasm700`, MAME bridge) in **drdevtools**;
ship only the clean artifact + provenance publicly.

---

## 9. Risks & decisions needed before execution

| # | Decision / risk | Default / recommendation |
|---|---|---|
| D1 | **Phase-0 build path** (0a DOSBox+TASM / 0b OpenWatcom / 0c port). | 0a if you have archived TASM+BCC; else 0b. 0d reserved for the Phase-5 cross-check. |
| D2 | **Who is the untainted Person B?** | Recruit one. If impossible, lean on merger doctrine and label clean-room "best-effort" (weaker — flag to counsel). |
| D3 | **Public host / repo name / license.** | New repo `snes-ipl-cleanroom` under a chosen org; permissive license. **User decides.** |
| D4 | **Counsel review before push?** | **Strongly yes** given the public goal. Non-negotiable in my recommendation. |
| R1 | Hash-matching DMCA takedown despite legal merit. | Pre-build the provenance bundle (§5/Phase 5) as counter-notice ammo. |
| R2 | `spasm` won't build/run cleanly on Linux. | Phase 0 is explicitly the de-risking phase; 0d gives a working assembler regardless. |
| R3 | MAME won't accept a differing ROM. | §7; acceptable for own-use + public repo; upstreaming is a stretch goal only. |

---

## 10. Verification  *(steps are the spec; fill raw output + PASS/FAIL during execution)*

> Per `~/SRC/CLAUDE.md`: keep these numbered steps verbatim; paste raw command
> output in a code block under each; mark PASS/FAIL; write results back here.

1. **SPC700 assembler runs.** Build `spasm700` (Phase 0) and assemble a trivial
   SPC700 `.asm` (e.g. a `MOV`/`TCALL`/`DBNZ` smoke file); confirm it emits the
   expected opcodes. *(Output: TBD)*
2. **Image shape.** `spasm700 src/ipl.asm` produces **exactly 64 bytes**; reset
   vector occupies `$FFFE/$FFFF`. *(Output: TBD)*
3. **Wall integrity.** `PROVENANCE.md` + `ATTESTATION.md` exist; Person A ≠ Person
   B; no Nintendo ROM/disassembly anywhere in the repo (`grep`/hash audit). *(Output: TBD)*
4. **Behavioural boot.** MAME using our `ipl.bin` (§7): a real cart's audio upload
   completes, SPC700 jumps into uploaded code, audio runs — confirmed via the drmon
   bridge (APU-RAM + SPC700 regs). *(Output: TBD)*
5. **Independent reproduction.** A second, unrelated assembler (`wla-spc700`/`bass`)
   assembles the same source to identical bytes. *(Output: TBD)*
6. **MAME region facts.** The exact MAME SPC700 IPL region name + expected hash +
   override path are documented (no guesses). *(Output: TBD)*

---

## Appendix — what already exists to build on

- **Assembler:** `tools/spasm` (SPC700 = `opcode70.asm`/`cam700.asm`; build via
  `m7.bat`). Ours.
- **SPC700 visibility in MAME:** drmon's SPC700 register window + `MTYPE_SPC`
  APU-RAM window over the Lua bridge — [`docs/plans/2026-06-12-spc700-window.md`](2026-06-12-spc700-window.md).
- **MAME SNES bridge:** `task drmon-snes` / `mame_cpu_bridge.lua` (CPU + PPU +
  SPC700 at a breakpoint).
