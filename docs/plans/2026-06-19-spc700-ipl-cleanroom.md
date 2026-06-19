---
title: SPC700 IPL boot ROM — clean-room reimplementation for legal MAME use + public release
date: 2026-06-19
status: draft / decisions-pending
toolchain: WLA-DX (wla-spc700) — primary; bass — independent cross-check
revised: 2026-06-19 (research-grounded; switched assembler spasm → WLA-DX)
---

# SPC700 IPL boot ROM — clean-room reimplementation

> **Goal.** Produce an *independently created* 64-byte SPC700 IPL boot ROM that is
> behaviourally compatible with the SNES audio subsystem, so that (1) it can be
> dropped into MAME in place of the copyrighted Nintendo IPL — letting **anyone**
> run the SNES driver legally, not just owners of real hardware — and (2) **both
> the source and the assembled bytes can be committed to a public repository.**
>
> **Assembler: WLA-DX (`wla-spc700`)** — FOSS, Linux-native, reproducible by anyone
> (chosen over the in-house `spasm`; see §2). Verification rides the existing drmon
> MAME Lua bridge.

> [!IMPORTANT]
> **This document is engineering planning, not legal advice.** The "public
> distribution" goal is where the risk concentrates, and Nintendo is an aggressive
> litigant. **Have qualified IP counsel review §4 + the provenance bundle before
> anything is pushed public.** Citations below are to real sources (§Sources) but
> are summarised by a non-lawyer; nothing here is a guarantee of non-infringement.

---

## 1. Scope & terminology

- **The "SPC700 BIOS" = the 64-byte IPL boot ROM** mapped at `$FFC0–$FFFF` in the
  SNES APU (S-SMP) address space, reset vector at `$FFFE/$FFFF`. A tiny bootloader:
  at reset it sets `SP=$EF`, zeroes `$00–$EF`, signals readiness on the CPU↔APU
  mailbox, runs an upload handshake that streams code into the 64 KB of APU RAM,
  then jumps to it. **That is the whole artifact** — 64 bytes.
- **Out of scope:** the S-DSP, BRR audio, and any game's audio engine (uploaded by
  cartridges at runtime; not the boot ROM, not Nintendo's to begin with).
- **Naming hygiene:** the deliverable is an *"independent SPC700 IPL-compatible boot
  ROM."* Use **no** Nintendo trademark. Trademark ≠ copyright; keep both clean.

---

## 2. Toolchain — WLA-DX (decided)

**Primary assembler: WLA-DX's `wla-spc700` + `wlalink`.** Rationale (this is the
reproducibility win, and it fits `~/SRC/CLAUDE.md` "everything must be reproducible"):

- **FOSS + Linux-native** (GPL; `apt`/Homebrew or a CMake build) — *anyone* can
  rebuild the bytes from source with stock tooling. A public clean-room artifact
  whose build needs a private 1994 DOS toolchain fails the "reproducible by the
  world" goal.
- **Independent of us.** A neutral, widely-used third-party assembler strengthens
  the "the bytes are a function of the source, not our hand" story.
- **Kills the old Phase-0 blocker** — no DOSBox/TASM/Borland needed.

**`spasm` (drdevtools) — verified to exist, deliberately NOT used.** The required
"first, verify an SPC700 assembler exists in drdevtools" check (2026-06-19) found
`tools/spasm`'s SPC700 backend (`opcode70.asm`/`cam700.asm`, SPC700-unique
`TCALL/XCN/CBNE/DBNZ/BBS`, built via `m7.bat` → `spasm700.exe`; we own it,
`spasm.cpp:311`). We're **not** using it: it's a 16-bit DOS binary (impractical for
public CI) and being our own tool weakens the independence story. It remains a
possible *extra* cross-check, but is **off the critical path**.

**Independent cross-check (Phase 5): `bass`** (byuu's table-driven assembler, also
SPC700-capable). Two unrelated assemblers (`wla-spc700` + `bass`) agreeing on the
64 bytes documents that the output is determined by the source, not the toolchain.

> Neither `wla-spc700` nor `bass` is installed on this host yet (`command -v` →
> absent as of 2026-06-19). Phase 0 installs + smoke-tests them.

---

## 3. Does it have to be byte-identical to Nintendo's? (the checksum question)

**Short answer: no hardware checksum forces it — but the constraints make a correct
independent implementation *converge* on the same bytes, and MAME separately
*requires* the exact bytes. The convergence is legally helpful, not harmful.**

### 3.1 There is NO integrity check on the IPL bytes (real hardware)

No SNES component (SPC700, S-SMP, S-DSP, or the 65816) computes a checksum/CRC over
the 64 IPL bytes. The S-SMP just executes them. Confirmed by faithful emulators:

- **snes9x** compiles the IPL in as `const uint8 iplrom[64]` and indexes it raw —
  no check ([snes9x `iplrom.cpp`]).
- **bsnes** / **ares** load `ipl.rom` from disk and take the reset vector straight
  from bytes 62/63 — **no CRC/SHA** ([bsnes `smp.cpp`], [ares `smp.cpp`]). If the
  hardware self-checked, accurate emulators would model it; none do.

So **boot requires behavioural/protocol equivalence, not byte-identity.**

### 3.2 What IS forced: behaviour + size (the protocol contract)

Within a **64-byte** budget, with a reset vector at `$FFFE/$FFFF`, the ROM must drive
the documented mailbox handshake on the **fixed** ports (`$2140–$2143` ↔ `$F4–$F7`)
using the exact magic values every commercial loader hard-codes: `$AA`/`$BB` ready,
`$CC` kickoff, the zero-first counter/index echo pacing, address via ports 2/3, and
the write-0-to-port-1 jump trigger ([nesdev "Booting the SPC700"], [SnesLab IPL],
[Wikibooks SPC700 loading]). Any 64-byte program reproducing that observable
behaviour boots; the *bytes* are not hardware-pinned — but the freedom inside 64
bytes is tiny, so a correct implementation is **effectively canonical**.

### 3.3 The ONLY hard byte-identity requirement: MAME's romset hash

MAME pins its IPL: `ROM_LOAD("spc700.rom", 0, 0x40, CRC(44bb3a40)
SHA1(97e352553e94242ae823547cd853eecda55c20f0))`, region `"sound_ipl"`
([mamedev `s_smp.cpp`]). A byte-different (even behaviourally perfect) IPL **fails
this audit** → not a drop-in. This is **distribution/romset management inside MAME,
not a hardware mechanism.** bsnes/ares load `ipl.rom` with no hash check; snes9x
compiles it in. **Consequence for our goal:** the "drop into MAME" deliverable
*does* need the exact bytes; "runs on real hardware / bsnes / ares" needs only
behavioural equivalence.

### 3.4 Convergence is EVIDENCE FOR us, not against (merger / *NEC v. Intel*)

Because the constraints force the result, a clean implementation will very likely be
byte-identical. That is the **merger-doctrine** posture in its strongest form: in
*NEC v. Intel* the clean-room engineer's independently-written microcode came out
"similar in many regards" to Intel's, and the court inferred the similarity was
*"dictated not by copying … but rather by functional constraints of the hardware,
the architecture, and the need for … compatibility"* (1989 WL 67434 (N.D. Cal.)).
Identity here is **proof the expression merged with function** — provided the wall
(§5) is real.

> **The flip side is practical, not legal:** a bit-identical file invites
> hash-matching DMCA takedowns (a claimant matches a CRC/SHA and files; GitHub
> doesn't adjudicate merger). The provenance bundle (§Phase 5) is the
> counter-notice ammunition. Plan for the takedown even though we believe we're right.

### 3.5 Not the cartridge checksum

The SNES *cartridge header* checksum (`$FFDE` + `$FFDC` complement) is a 16-bit
sum-of-bytes over the **game** ROM on the 65816 side, *"not needed by the SNES
hardware"* at boot ([nesdev ROM header]) — a different chip, address space, and
purpose from the SPC700 IPL. Unrelated; the `$FFC0–$FFFF` overlap is coincidental.

---

## 4. Legal strategy — why this can be public

Four independent doctrines stack; we rely on **all**, not any one. (Settled vs
unsettled flagged; see §Sources for citations.)

1. **Independent creation is a complete defense.** Copyright forbids *copying*, not
   arriving at the same result independently. Infringement-by-copying is proven
   circumstantially by **access + substantial similarity**; *independent creation
   rebuts it* (*Three Boys Music v. Bolton*, 212 F.3d 477 (9th Cir. 2000); *Feist*,
   499 U.S. 340). The clean room manufactures **provable** independent creation.
   *(SETTLED.)*

2. **Reverse-engineering to reach functional elements is fair use** — the legal
   ground for the "dirty team" (§5): *Sony v. Connectix*, 203 F.3d 596 (9th Cir.
   2000) — **reverse-engineering the PlayStation BIOS to build an emulator was fair
   use**; the shipped product contained none of Sony's code (the **most on-point**
   case for us). *Sega v. Accolade*, 977 F.2d 1510 (9th Cir. 1992) — disassembly to
   find "functional requirements for compatibility … aspects not protected by
   copyright" is fair use. *(SETTLED.)*

3. **Merger / scènes à faire / abstraction-filtration-comparison.** Elements
   dictated by external constraints, efficiency, and compatibility are filtered out
   before any infringement comparison (*Computer Associates v. Altai*, 982 F.2d 693
   (2d Cir. 1992)); "when there is essentially only one way to express an idea …
   copyright is no bar." A **64-byte**, interface-dictated boot ROM is at/below the
   floor of protectable expression (*Baker v. Selden*, 101 U.S. 99; *Lotus v.
   Borland*, 49 F.3d 807 (1st Cir. 1995), aff'd 4–4, 516 U.S. 233 — method of
   operation). *(SETTLED doctrine; merger of THIS ROM is a fact question — but the
   shorter + more interface-forced the code, the stronger it is.)*

4. **Interfaces are functional/reimplementable.** *Google v. Oracle*, 593 U.S. 1
   (2021) — reimplementing interface declarations to let skills transfer to a new
   platform is fair use. The CPU↔APU protocol is an interface. *(SETTLED as to the
   reuse; API copyrightability expressly reserved.)*

### 4.1 The cautionary tale: how a clean room FAILS — *Atari v. Nintendo*

*Atari Games v. Nintendo*, 975 F.2d 832 (Fed. Cir. 1992): "Reverse engineering
object code to discern the unprotectable ideas … is a fair use" — **but** Atari had
also obtained Nintendo's **registered 10NES source code from the Copyright Office by
fraud**, so "any copying … from the Copyright Office does not qualify as a fair
use." **A clean room is only as clean as its inputs:** tainted *access* to the
protected expression — even indirect — sinks it. This is the lens for §5's taint
analysis (and directly the reason an LLM trained on the IPL is risky).

### 4.2 Cases raised by the user — mapped

- **"IBM vs NEC, V20/V30"** → actually **NEC Corp. v. Intel Corp.**, 1989 WL 67434
  (N.D. Cal. 1989) — the **seminal** clean-room ruling (engineer Davidian wrote V20/
  V30 microcode "without access to any other microcode"; similarity held compelled
  by functional constraints). Adversary was **Intel**, not IBM. Backbone of §3.4/§5.
- **"EA vs Sega / Razorsoft"** → the law-making case is **Sega v. Accolade** (above).
  EA's Genesis reverse-engineering was resolved by a **business deal**, not a
  published opinion; **Razorsoft** — no reported copyright opinion located (do not
  cite as precedent without verification).
- **"Compaq vs IBM/Microsoft BIOS"** → Compaq's clean-room targeted **IBM's** BIOS
  copyright (not Microsoft). **Industry-practice** example — the wall was so solid it
  *deterred* litigation rather than producing a famous opinion.
- **"Phoenix / AMI BIOS"** → **Phoenix** is the canonical documented two-team clean
  room (a separate engineer "not exposed to IBM BIOS source code," from a different
  architecture). **AMI** — no specific clean-room lawsuit located; Phoenix is the one.

---

## 5. Roles, taint, and WHO may implement

A clean room is a **Chinese wall**: people who may study behaviour are walled off
from people who write code; they communicate only through a lawyer-reviewed
functional spec. The test for the implementer is **access** — did they have a
*"reasonable opportunity to view"* the protected work (*Three Boys*, 212 F.3d 477)?
*Potential* access counts if it's more than a "bare possibility."

| Role | May see | May NOT see | Output |
|---|---|---|---|
| **Spec author (A)** | Public docs of the CPU↔APU *interface/protocol*; black-box port behaviour; the SPC700 ISA. | Nintendo's ROM image / any disassembly of it. | Behavioural spec (§Phase 1), no Nintendo code. |
| **Clean implementer (B)** | Only A's lawyer-reviewed spec; the SPC700 ISA; WLA-DX docs. | Nintendo's IPL bytes/disassembly; **must attest non-exposure.** | SPC700 source → 64-byte image. |
| **Verification lead (C)** | Everything behavioural; runs MAME; *forensic-only* byte comparison after Phase 3 freeze. | (must not relay Nintendo bytes back to B). | Behavioural PASS/FAIL + provenance bundle. |

**A and B must be different people.** Now the three implementer candidates the user
asked about — answered against the access test (all AI points **UNSETTLED**: no court
has ruled on LLM-as-clean-implementer):

- **A different *instance* of Claude? → No.** The taint is in **training weights, not
  chat context**; a fresh instance has identical weights → identical access. The
  64-byte IPL is in training data — it is a *verbatim array* in open-source snes9x
  ([snes9x `iplrom.cpp`]) and copied across GitHub (so it's also "widely
  disseminated" → access under *Three Boys*). LLMs demonstrably memorise/regurgitate
  (Carlini [2012.07805], [2202.07646]; Nasr [2311.17035]; a reproduction study even
  singled out *"Claude's Opus"* for high baseline memorisation, [2412.06370]). That
  is *Atari*'s "unauthorized possession," not *Connectix*'s clean access. Resetting
  context clears only *this conversation's* leakage. **Claude is presumed tainted →
  Claude does tooling/spec-scaffolding/verification, NOT the boot-ROM bytes.**
- **A different *model* (Gemma/GPT/Qwen/DeepSeek/…)? → Generally no.** Same test on
  *that* corpus. The IPL is ubiquitous in code/web scrapes → any frontier model is
  presumptively tainted, and for proprietary models you **cannot audit** the training
  set to disprove it. Only a model whose corpus is *documented to exclude* the IPL
  qualifies — you must *prove the negative*. Footnote: purely AI-authored output may
  itself be **uncopyrightable** (U.S. Copyright Office 2023, 88 Fed. Reg. 16190;
  *Thaler v. Perlmutter*, aff'd D.C. Cir. 2025) — irrelevant to non-infringement,
  moot for forced functional code.
- **A human who's never used a SNES / seen its docs? → YES, gold standard.** Exactly
  *NEC v. Intel* (microcode "without access to any other microcode") and *Phoenix/
  Compaq* (a separate engineer "not exposed to IBM BIOS source code," from a
  different architecture). Refinements: (1) the wall excludes the **IPL's expression
  specifically** (bytes/disassembly), **not** general knowledge — B may/must learn
  the **SPC700 ISA** (unprotected functional facts; *Sega*) and WLA-DX; (2) "never
  used a SNES" maximises credibility but means you **teach B SPC700 asm** from the
  public ISA; (3) B works only from A's spec, signs the attestation, audit trail
  kept (*Altai*, *NEC*). **This is who to recruit.**

**Honest note on the original author.** You wrote `spasm`'s SPC700 backend and have
decades of SNES work — you've very likely seen the IPL disassembly, so you're an
excellent **spec author / verification lead / tooling owner** but a **poor clean
implementer**. If no untainted B is available, we fall back on merger (§3.4/§4.3) and
label the clean-room "best-effort" — weaker; flag to counsel.

---

## 6. The wall — allowed vs forbidden specification sources

- **Allowed:** documentation of *what the main CPU does* to upload to the APU (the
  mailbox ports, the `$AA/$BB/$CC` handshake, the counter pacing, the jump); the
  SPC700 ISA and timing; black-box observation of an APU's port behaviour at reset;
  WLA-DX docs. Interface facts / methods of operation are not protectable expression
  (*Google v. Oracle*; *Sega*).
- **Forbidden:** Nintendo's ROM image; any disassembly/annotated listing of it; any
  "reference implementation" that is really a transcription of the original.
- **Hard rules:** never look at Nintendo's bytes during spec/implementation; never
  commit them (or a disassembly) to any repo — including as a test fixture or in this
  plan; never feed a byte-level comparison back into the implementation (observing
  coincidence after a Phase-3 freeze is fine as forensic notes; *editing toward*
  Nintendo's bytes converts independent creation into copying — the *Atari* trap).
  Correctness is judged **behaviourally** (§Phase 4).

---

## 7. Phases

### Phase 0 — Stand up WLA-DX (+ bass)  *(now trivial)*

`apt install wla-dx` (or CMake build from `github.com/vhelin/wla-dx`); confirm
`wla-spc700` + `wlalink` run. Install `bass` for the Phase-5 cross-check. **Exit:**
`wla-spc700` assembles a trivial SPC700 `.s` (a `MOV`/`TCALL`/`DBNZ` smoke file) to
the expected opcodes (verification step 1).

### Phase 1 — Behavioural specification (Person A, untainted)

`spec/ipl-behaviour.md` in the new public repo: a numbered, testable spec derived
**only** from §6-allowed sources — reset entry/vector; readiness signalling; the
upload handshake (address setup, counter/echo pacing, multi-block, final jump);
zero-page/SP init; the 64-byte budget. **No byte listings.** Each clause gets an ID
so Phase-4 tests cite it. Lawyer review strips any protected expression.

### Phase 2 — Clean implementation (Person B, untainted)

From the Phase-1 spec **only**, B writes `src/ipl.s` for `wla-spc700` and signs
`ATTESTATION.md` (never studied the SNES IPL; worked solely from the spec).
Iterate against the **spec** and **behavioural** test failures — never Nintendo's
bytes.

### Phase 3 — Assemble

`wla-spc700 src/ipl.s` + `wlalink` → `build/ipl.bin`. Assert **exactly 64 bytes**;
reset vector at `$FFFE/$FFFF`. Record our SHA-256. (No comparison to Nintendo's hash
to drive anything — §6.)

### Phase 4 — Behavioural verification in MAME (Person C)

The real correctness oracle, via the drmon SNES Lua bridge (SPC700 register +
APU-RAM windows, [`2026-06-12-spc700-window.md`]):

1. Make MAME use **our** `ipl.bin` instead of its built-in `spc700.rom` (§8).
2. Boot a cart whose audio engine performs a real APU upload.
3. Via the bridge confirm: readiness handshake → bytes land in APU RAM at intended
   addresses → SPC700 PC jumps into the uploaded program → audio runs. Cross-check
   APU-RAM + SPC700 regs.
4. Regression set: several carts / a purpose-built APU exerciser; multi-block;
   counter-handshake edge cases.

PASS = real software boots and runs through *our* ROM. No byte-diffing.

### Phase 5 — Independent reproduction + provenance bundle

- **Second toolchain:** assemble the same `src/ipl.s` with `bass`; confirm identical
  bytes (output is a function of the source, not the assembler).
- **Provenance bundle (the counter-notice kit):** Phase-1 spec, B's attestation,
  dated build logs, the two-toolchain reproduction, this plan, and `PROVENANCE.md`
  narrating the wall + timeline — the §3.4 merger / §4 independent-creation evidence.

### Phase 6 — Public release  *(GATED on counsel + user go-ahead)*

Stand up the public repo (§9), license it (permissive code license + an explicit
"independent reimplementation, see PROVENANCE.md" statement). **Do not push until §4
review happens and the user explicitly approves** — publishing is irreversible and is
the whole risk surface.

---

## 8. MAME integration specifics  *(verify, don't assume)*

- Region `"sound_ipl"`, file `spc700.rom`, 64 bytes, `CRC(44bb3a40)
  SHA1(97e352553e94242ae823547cd853eecda55c20f0)` ([mamedev `s_smp.cpp`]); it lives
  in parent BIOS sets (e.g. `nss.zip`). **Action:** confirm the exact override path
  in the MAME build drdevtools drives (Phase-4 step 0).
- **If our bytes == Nintendo's** (likely, §3.2): the file satisfies MAME's hash → a
  literal drop-in.
- **If they differ:** MAME rejects the hash; we'd run a MAME built to accept an
  alternate IPL, or pursue an upstream "open IPL" option. Realistic note: MAME
  upstream generally ships only bit-exact dumps, so a *differing* reimplementation is
  mainly useful for our own builds + the public repo with a "how to use" note.

---

## 9. Deliverables — public repo layout

A **separate, single-purpose public repo** (clean provenance), e.g.
`snes-ipl-cleanroom`:

```
README.md             # what it is / is NOT (no Nintendo marks); how to use w/ MAME
LICENSE               # permissive code license
PROVENANCE.md         # the wall, roles, timeline — the legal narrative
ATTESTATION.md        # Person B's signed non-exposure statement
spec/ipl-behaviour.md # Phase-1 behavioural spec (interface-derived)
src/ipl.s             # SPC700 source (assembles under wla-spc700; cross-checks under bass)
build/ipl.bin         # assembled 64-byte image  ← the "public bytes"
build/ipl.sha256      # our hash
build.sh / Taskfile   # reproducible: source → bytes, BOTH toolchains (wla + bass)
tests/                # behavioural MAME harness + exerciser (NO Nintendo bytes)
```

Keep the messy bits (MAME bridge) in **drdevtools**; ship only the clean artifact +
provenance publicly. The build needs **only FOSS tools** — that is the point of §2.

---

## 10. Risks & decisions needed before execution

| # | Decision / risk | Status / recommendation |
|---|---|---|
| D1 | Assembler. | **RESOLVED → WLA-DX (`wla-spc700`)** primary; `bass` cross-check (§2). |
| D2 | **Who is the untainted Person B?** | **Recruit one** (gold standard, §5). Neither the LLM nor (likely) the original author qualifies. Else lean on merger, label "best-effort" — flag to counsel. |
| D3 | Public host / repo name / license. | New repo `snes-ipl-cleanroom`; permissive license. **User decides.** |
| D4 | Counsel review before push? | **Strongly yes** given the public goal. Non-negotiable in my recommendation. |
| R1 | Hash-matching DMCA takedown despite legal merit. | Pre-build the provenance bundle (§5/Phase 5) as counter-notice ammo. |
| R2 | MAME won't accept a differing ROM. | §8; acceptable for own-use + public repo; upstreaming is a stretch goal. |
| Q-open | EA v. Sega / Razorsoft specifics. | Not located as reported opinions; dig only if needed (Sega v. Accolade is the precedent). |

---

## 11. Verification  *(steps are the spec; fill raw output + PASS/FAIL during execution)*

> Per `~/SRC/CLAUDE.md`: keep these numbered steps verbatim; paste raw output under
> each; mark PASS/FAIL; write results back here.

1. **Assembler runs.** `wla-spc700` (+`wlalink`) assembles a trivial SPC700 `.s`
   smoke file to expected opcodes. *(Output: TBD)*
2. **Image shape.** `wla-spc700 src/ipl.s` → **exactly 64 bytes**; reset vector at
   `$FFFE/$FFFF`. *(Output: TBD)*
3. **Wall integrity.** `PROVENANCE.md` + `ATTESTATION.md` exist; Person A ≠ B; no
   Nintendo ROM/disassembly anywhere in the repo (grep/hash audit). *(Output: TBD)*
4. **Behavioural boot.** MAME using our `ipl.bin` (§8): a real cart's audio upload
   completes, SPC700 jumps into uploaded code, audio runs — confirmed via the drmon
   bridge (APU-RAM + SPC700 regs). *(Output: TBD)*
5. **Independent reproduction.** `bass` assembles the same source to **identical
   bytes**. *(Output: TBD)*
6. **MAME region facts.** Exact override path for `spc700.rom`/`sound_ipl` in the
   driven MAME build documented (no guesses). *(Output: TBD)*

---

## Sources

Hardware / boot (retrieved 2026-06-19):
- nesdev "Booting the SPC700": https://snes.nesdev.org/wiki/Booting_the_SPC700
- nesdev S-SMP: https://snes.nesdev.org/wiki/S-SMP · ROM header (cart checksum
  "not needed by the SNES hardware"): https://snes.nesdev.org/wiki/ROM_header
- SnesLab IPL ROM: https://sneslab.net/wiki/SPC700/IPL_ROM
- Wikibooks, loading SPC700 programs:
  https://en.wikibooks.org/wiki/Super_NES_Programming/Loading_SPC700_programs
- nocash fullsnes: https://problemkaputt.de/fullsnes.htm
- snes9x `iplrom.cpp` (IPL as verbatim 64-byte array; no check):
  https://raw.githubusercontent.com/snes9xgit/snes9x/master/apu/bapu/smp/iplrom.cpp
- bsnes `smp.cpp`: https://raw.githubusercontent.com/bsnes-emu/bsnes/master/bsnes/sfc/smp/smp.cpp
- ares `smp.cpp`: https://raw.githubusercontent.com/ares-emulator/ares/master/ares/sfc/smp/smp.cpp
- MAME `s_smp.cpp` (`spc700.rom` CRC 44bb3a40 / SHA1 97e3…):
  https://github.com/mamedev/mame/blob/master/src/devices/machine/s_smp.cpp

Law — clean room / reverse engineering (SETTLED unless noted):
- Sony Computer Entertainment v. Connectix, 203 F.3d 596 (9th Cir. 2000) — PlayStation BIOS RE = fair use.
- Sega Enterprises v. Accolade, 977 F.2d 1510 (9th Cir. 1992).
- Atari Games v. Nintendo, 975 F.2d 832 (Fed. Cir. 1992) — tainted access killed fair use.
- NEC Corp. v. Intel Corp., 1989 WL 67434 (N.D. Cal. 1989) — seminal clean-room microcode.
- Google LLC v. Oracle America, 593 U.S. 1 (2021) — interface reuse fair use (API copyrightability reserved).
- Computer Associates v. Altai, 982 F.2d 693 (2d Cir. 1992) — abstraction-filtration-comparison.
- Baker v. Selden, 101 U.S. 99 (1879); Lotus v. Borland, 49 F.3d 807 (1st Cir. 1995), aff'd 4–4, 516 U.S. 233 (1996) — *Lotus binding 1st Cir. only; no national precedent.*
- Access standard: Three Boys Music v. Bolton, 212 F.3d 477 (9th Cir. 2000); inverse-ratio rule abrogated, Skidmore v. Led Zeppelin, 952 F.3d 1051 (9th Cir. 2020). Originality: Feist, 499 U.S. 340 (1991).

Law / facts — AI (UNSETTLED / empirical):
- Memorization: Carlini et al. arXiv:2012.07805 (USENIX 2021), arXiv:2202.07646 (ICLR 2023); Nasr et al. arXiv:2311.17035 (2023); reproduction/counter-nuance arXiv:2412.06370 (2024, notes "Claude's Opus" baseline memorization).
- Litigation (pleading-stage, not merits): NYT v. OpenAI/Microsoft (MDL 1:25-md-03143); Doe v. GitHub (Copilot); Andersen v. Stability; Tremblay v. OpenAI; Kadrey v. Meta; **Bartz v. Anthropic** (N.D. Cal. 2025 — training fair use, pirated library NOT; output-filtering layer; settled $1.5B).
- Authorship: U.S. Copyright Office, 88 Fed. Reg. 16190 (Mar. 16, 2023); "Zarya of the Dawn" (2023); Thaler v. Perlmutter, 687 F. Supp. 3d 140 (D.D.C. 2023), aff'd No. 23-5233 (D.C. Cir. Mar. 18, 2025).

> **Citation honesty:** summarised by a non-lawyer from sources retrieved 2026-06-19.
> Some pin-cites (notably *Sega*, a few reporter pages) were high-confidence from
> retrieved text but not byte-verified against bound reporters; the *NYT v. OpenAI*
> opinion is image-based and corroborated via secondary sources. Counsel should
> verify before any public reliance.

---

## Appendix — what already exists to build on

- **Assembler (chosen):** WLA-DX `wla-spc700` (FOSS; install in Phase 0). Cross-check:
  `bass`.
- **Assembler (verified, not used):** `tools/spasm` SPC700 backend
  (`opcode70.asm`/`cam700.asm`, `m7.bat`). Ours; off the critical path (§2).
- **SPC700 visibility in MAME:** drmon's SPC700 register + `MTYPE_SPC` APU-RAM windows
  over the Lua bridge — [`docs/plans/2026-06-12-spc700-window.md`](2026-06-12-spc700-window.md).
- **MAME SNES bridge:** `task drmon-snes` / `mame_cpu_bridge.lua`.
