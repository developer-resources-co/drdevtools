---
title: SPC700 IPL boot ROM — clean-room reimplementation for legal MAME use + public release
date: 2026-06-19
status: draft / GATE — IP counsel first (§0) before any Phase 1–6 work
toolchain: WLA-DX (wla-spc700) — primary; bass — independent cross-check
revised: 2026-06-19 (research-grounded; assembler spasm → WLA-DX; re-sequenced IP-counsel-first, §0)
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

## 0. START HERE — consult IP counsel BEFORE any build/spec work

The hardest part of this project is legal, not technical, so the **first action is to
talk to an IP lawyer** — *before* recruiting a clean-room implementer or writing a
line of spec. Recommended avenue: **the FSF's counsel** — the Free Software
Foundation's licensing & compliance lab (<licensing@fsf.org>) and/or the
[Software Freedom Law Center](https://www.softwarefreedom.org/) (founded by Eben
Moglen, the FSF's longtime general counsel). That is exactly the right kind of lawyer
for a free-software copyright / reverse-engineering question.

**Why counsel first (the re-sequencing).** The whole project reduces to one unsettled
question — *is a 64-byte, interface-forced boot ROM copyrightable at all?* — and the
answer decides whether the clean room is even needed:
- Counsel: *"merger clearly applies, ship it"* → done cheaply; **skip Phases 1–6**;
  optionally publish a bytes-only repo resting on the written opinion.
- Counsel: *"maybe / yes, it's protectable"* → the clean room is justified **and
  genuinely additive — nobody has done one (§0.A)** — proceed to Phase 1 with the
  isolation bar counsel sets.

So we spend a ~1-hour question before any project-sized commitment. **Until counsel
answers, Phases 1–2 are ON HOLD** (recruiting Person B is premature). Phase 0
(tooling) was safe regardless and is ✅ done.

### 0.A — Counsel briefing packet  *(self-contained; hand this to the lawyer)*

**Who we are / the goal.** We want to publish — source **and** the assembled 64-byte
image — an *independently created* SPC700 IPL boot ROM for the Super Nintendo's audio
subsystem, so that (a) it can replace the copyrighted Nintendo IPL in the
[MAME](https://github.com/mamedev/mame) emulator (letting anyone run the SNES driver
without dumping their own console) and (b) the bytes + source can live in a **public**
repository. "Public" is the whole point.

**The artifact.** A 64-byte boot ROM at `$FFC0–$FFFF` in the SNES audio CPU (Sony
SPC700). At reset it runs a tiny handshake over four mailbox ports that lets the main
CPU upload the game's audio code, then jumps to it (mechanism:
[nesdev "Booting the SPC700"](https://snes.nesdev.org/wiki/Booting_the_SPC700),
[SnesLab IPL ROM](https://sneslab.net/wiki/SPC700/IPL_ROM)). 64 bytes is the entire
program.

**Technical finding #1 — the bytes are *functionally forced*, not free.** Within the
64-byte budget, fixed mailbox-port addresses, and a fixed handshake every commercial
game depends on, a correct implementation is effectively canonical — essentially one
sensible way to write it. There is **no checksum** on the IPL in hardware; faithful
emulators consume the bytes raw with no integrity check
([snes9x compiles them in as a verbatim array](https://raw.githubusercontent.com/snes9xgit/snes9x/master/apu/bapu/smp/iplrom.cpp);
[bsnes loads `ipl.rom` with no hash check](https://raw.githubusercontent.com/bsnes-emu/bsnes/master/bsnes/sfc/smp/smp.cpp)).
**Consequence:** an independent reimplementation will very likely come out
*byte-identical* to Nintendo's — not by copying, but because the function compels it.

**Technical finding #2 — the only hard byte-match requirement is MAME's romset hash.**
MAME pins its IPL to a specific checksum
([mamedev `s_smp.cpp`: `ROM_LOAD("spc700.rom", … CRC(44bb3a40) SHA1(97e352…))`](https://github.com/mamedev/mame/blob/master/src/devices/machine/s_smp.cpp)),
so a byte-different (even behaviourally perfect) IPL is rejected by MAME's audit —
distribution/romset management, not a hardware mechanism. (The unrelated SNES
cartridge-header checksum is *"not needed by the SNES hardware"* —
[nesdev ROM header](https://snes.nesdev.org/wiki/ROM_header).)

**Legal landscape as we (non-lawyers) understand it.**
- *Independent creation is a complete defense; copying is proven by access +
  substantial similarity* —
  [Three Boys Music v. Bolton, 212 F.3d 477 (9th Cir. 2000)](https://law.justia.com/cases/federal/appellate-courts/F3/212/477/);
  originality needs only independent creation + minimal creativity
  ([Feist, 499 U.S. 340 (1991)](https://www.law.cornell.edu/supremecourt/text/499/340)).
- *Reverse-engineering a console BIOS to build an emulator can be fair use* — squarely
  on point: [Sony v. Connectix, 203 F.3d 596 (9th Cir. 2000)](https://law.justia.com/cases/federal/appellate-courts/F3/203/596/)
  (PlayStation BIOS); [Sega v. Accolade, 977 F.2d 1510 (9th Cir. 1992)](https://law.justia.com/cases/federal/appellate-courts/F2/977/1510/).
- *But a clean room is only as clean as its inputs* — fair use FAILED where the
  reimplementer had tainted access to the original:
  [Atari Games v. Nintendo, 975 F.2d 832 (Fed. Cir. 1992)](https://law.justia.com/cases/federal/appellate-courts/F2/975/832/).
- *Merger / constrained functional code:* where there's essentially one way to express
  a function, expression merges with idea and isn't protectable; elements dictated by
  compatibility/efficiency are filtered out —
  [Computer Associates v. Altai, 982 F.2d 693 (2d Cir. 1992)](https://law.justia.com/cases/federal/appellate-courts/F2/982/693/),
  [Baker v. Selden, 101 U.S. 99 (1879)](https://www.law.cornell.edu/supremecourt/text/101/99).
  The seminal clean-room case treated independently-produced *similar* microcode as
  compelled by "functional constraints … and the need for … compatibility" (**NEC v.
  Intel**, 1989 WL 67434 (N.D. Cal.) — Westlaw-only). Interfaces are
  functional/reimplementable:
  [Google v. Oracle, 593 U.S. 1 (2021)](https://www.law.cornell.edu/supremecourt/text/18-956).

**What already exists — COPIES, not clean rooms.** Everything public is a copy or
disassembly of Nintendo's original, carrying **Nintendo's copyright, not independent
provenance**: snes9x's verbatim array (above); spcasm's byte-annotated
`include/bootrom.s`; and eKid's commented disassembly
([wiki.superfamicom.org/spc700-reference](https://wiki.superfamicom.org/spc700-reference)).
A 2026-06-19 web search for a clean-room / public-domain reimplementation found **only
docs + disassemblies — no documented clean-room IPL.** Trap: **"the bytes are public"
≠ "the bytes are free to redistribute"** (Nintendo's full game ROMs are public too).
So the *only* thing a provenance-bearing project adds over the status quo is the
provenance itself.

**If a clean room is advised — the AI-implementer sub-question.** An AI model can't be
the "clean" implementer if it was trained on the IPL: LLMs demonstrably memorize
training data ([Carlini et al. 2021](https://arxiv.org/abs/2012.07805),
[Nasr et al. 2023](https://arxiv.org/abs/2311.17035)) and the IPL is in training
corpora (it's a verbatim array in open-source snes9x). A **human who has never seen
the IPL** is the gold-standard implementer (the classic NEC v. Intel / Phoenix-BIOS
setup; the U.S. Copyright Office also treats purely AI-authored output as
[uncopyrightable](https://www.federalregister.gov/documents/2023/03/16/2023-05321/copyright-registration-guidance-works-containing-material-generated-by-artificial-intelligence)).

### 0.B — The precise questions for counsel

1. **Copyrightability:** Is a 64-byte, interface-forced boot ROM protectable
   expression, or does merger / scènes à faire / thin-copyright remove protection?
2. **Publish-as-is?** If unprotected, may we publish the bytes (from a clean
   re-derivation) + source publicly without infringement exposure?
3. **Clean-room sufficiency:** If protectable, does a documented Chinese-wall process
   (interface-derived spec → untainted human implementer → provenance bundle) give a
   defensible independent-creation basis to publish — *even though the bytes will
   likely be byte-identical* to Nintendo's?
4. **MAME distribution:** Any added exposure from the result matching MAME's pinned
   IPL hash (i.e., being a drop-in)?
5. **Implementer standard:** Confirm the "no access" attestation bar; confirm a human
   with no IPL exposure (not an AI trained on it) is required.
6. **Trademark/labeling:** Confirm naming that avoids Nintendo marks.

**Outcomes that change the plan:** "merger applies, ship it" → skip Phases 1–6,
publish bytes-only + the opinion · "protectable, clean room OK" → proceed Phase 1 at
counsel's isolation bar · "too risky even with a clean room" → do not publish; stop at
private/own-use.

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

**Independent cross-check (Phase 5): a second SPC700 assembler.** Two unrelated
assemblers agreeing on the 64 bytes documents that the output is determined by the
source, not the toolchain.

> **Phase-0 finding (2026-06-19) — bass uses a non-canonical dialect.** byuu's
> `bass` (ARM9 fork, v18) is SPC700-capable, but its `spc700.arch` table uses
> **6502-style mnemonics** (`clc`/`sec`/`rts`/`lda #`/`ldx #`/`mul`), **not** the
> canonical Sony SPC700 mnemonics (`clrc`/`setc`/`ret`/`mov a,#`/`mul ya`) that
> `wla-spc700` uses. Consequence: a bass cross-check **cannot reuse the wla source**
> — it needs a hand-translated parallel source and compares only at the byte level
> (still valid evidence, just not "one source, two tools"). **Recommendation:** for
> the strongest same-source cross-check, prefer **`spcasm`** (a modern Rust SPC700
> assembler, [codeberg.org/filmroellchen/spcasm] — moved from GitHub) which uses
> **canonical Sony mnemonics** — see the evaluation below; keep bass as a byte-level
> fallback.

> **Phase-0 spcasm evaluation (2026-06-19).**
> - ✅ **Dialect fit:** spcasm uses canonical Sony SPC700 mnemonics (`mov x,#…`,
>   `mul ya`, `mov.b A,DSPDATA`) — verified from its own example sources — so it is a
>   **true one-source-two-tools cross-check** with `wla-spc700` (the instruction body
>   can be shared; only the tool-specific directive wrapper differs). This is the
>   intended Phase-5 cross-check tool.
> - ⚠️ **Taint hazard:** spcasm **bundles the Nintendo IPL disassembly** at
>   `include/bootrom.s` (byte-annotated, 2001 B). It must be kept **away from Person
>   A/B** and used **only by Person C** for the Phase-5 cross-check. `wla-spc700`
>   (what B uses) ships **no** bundled IPL — it is clean. (Yet another tool shipping
>   the IPL reinforces §5: the expression is widely disseminated.)
> - 🔧 **Build:** needs `cargo build` of a freshly-cloned third-party repo (runs
>   build scripts) + a pinned Rust nightly (`nightly-2026-06-18`). The agent's build
>   was **correctly gated** by the untrusted-code guardrail — **live byte
>   cross-check is pending explicit build authorization** (the dialect/taint findings
>   above stand regardless, from source inspection).

> **Phase-0 status — DONE (2026-06-19):** `wla-spc700` v10.6 (apt, foundry pkg) and
> `bass` v18 (built from source) both smoke-tested and **agree byte-for-byte** on
> the same logical program (`00 cd 12 e8 34 60 cf 6f`). `spcasm` evaluated (canonical
> dialect ✅, bundles IPL ⚠️); its live byte check awaits build authorization. See
> §11 step 1.

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

### 3.5 Not the cartridge checksum — and the overlap is *convergence*, not coincidence

The SNES *cartridge header* checksum (`$FFDE` + `$FFDC` complement) is a 16-bit
sum-of-bytes over the **game** ROM on the 65816 side, *"not needed by the SNES
hardware"* at boot ([nesdev ROM header]) — a different chip, address space, and
purpose from the SPC700 IPL, and it **does not validate the IPL** (the point that
matters here). *Functionally* the two are independent.

But the shared `$FFC0–$FFFF` address is **not coincidental** — it is the same
forced-convergence as the rest of this section, one level up in the *architecture*.
Both the main CPU (Ricoh 5A22, a 65816 core) and the audio CPU (SPC700) follow the
**6502 convention of placing CPU vectors at the very top of the 16-bit address space**
(6502 `$FFFA–$FFFF`; the 65816 extends it to `$FFE0–$FFFF`; the SPC700 reset vector
sits at `$FFFE/$FFFF`). So *both* chips carve their special region out of the **top 64
bytes** — `$FFC0 = $10000 − $40` — the 65816 placing the cart header just below its
forced vectors, the SPC700 making those top 64 bytes its overlay-able IPL ROM. Same
root constraint (vectors-at-top) + the same design house (Sony/Ricoh) applying the
same 6502 conventions (themselves constraint-driven) → the same magic boundary in two
otherwise-unrelated address spaces. **Convergent, not coincidental — a fractal of the
merger thesis (§3.4):** when the constraints are shared, the outputs collide even
across independent designs. (This is exactly why the *bytes* converge too.)

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
  **Concrete hazard (found 2026-06-19):** the `spcasm` assembler **bundles** such a
  disassembly at `include/bootrom.s`. If spcasm is installed for the Phase-5
  cross-check (Person C only), **quarantine/delete that file** from any environment
  Person A or B can see.
- **Hard rules:** never look at Nintendo's bytes during spec/implementation; never
  commit them (or a disassembly) to any repo — including as a test fixture or in this
  plan; never feed a byte-level comparison back into the implementation (observing
  coincidence after a Phase-3 freeze is fine as forensic notes; *editing toward*
  Nintendo's bytes converts independent creation into copying — the *Atari* trap).
  Correctness is judged **behaviourally** (§Phase 4).

---

## 7. Phases

> **Gate — IP counsel (§0) is the FIRST action and BLOCKS Phases 1–6.** Hand counsel
> the §0.A briefing packet; act on §0.B. Phase 0 (tooling) was independent of the
> legal question and is already done; everything from Phase 1 on proceeds only if
> counsel green-lights the clean room (else skip to a bytes-only publish, or stop).

### Phase 1 (gate) — Consult IP counsel  ⛔ DO FIRST

Send the **§0.A briefing packet** to the FSF's licensing/compliance counsel
(<licensing@fsf.org>) / [SFLC](https://www.softwarefreedom.org/); get a written read on
the §0.B questions. **This blocks Phases 2–6** (formerly numbered 1–5 below; the spec
and implementation only start on a green light).

### Phase 0 — Stand up WLA-DX (+ bass)  ✅ DONE (2026-06-19)

Reproducible recipe used:
- **wla-spc700 v10.6** — `apt install wla-dx` (foundry pkg `10.6-1foundry1`;
  `/usr/bin/wla-spc700` + `/usr/bin/wlalink`). Source-build alt: CMake from
  `github.com/vhelin/wla-dx` (commit `024f391`) → `build/binaries/`.
- **bass v18** — NOT apt-installable; built from source (ARM9/bass, commit
  `c3962ec`) with a one-line modern-GCC patch (`#include <stdexcept>` in
  `nall/arithmetic/natural.hpp`). Arch loaded via `arch spc700`, with
  `spc700.arch`+`defaults.arch` staged in `<bass-bin-dir>/architectures/`.
- **Flow:** `wla-spc700 -o x.o x.s` → `wlalink x.link x.bin` (raw 64-byte ROM via a
  `[objects]`/`x.o` link file). bass: `bass x.asm` with `output "x.bin", create`.

**Result:** both assemble a 6-instruction SPC700 smoke program to the **identical**
bytes `00 cd 12 e8 34 60 cf 6f` (`nop / mov x,#$12 / mov a,#$34 / clrc / mul ya /
ret`). Verification step 1 = **PASS**.

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
| D2 | **Who is the untainted Person B?** | **Recruit one** (gold standard, §5) — **but only after §0 counsel green-lights the clean room** (premature otherwise). Neither the LLM nor (likely) the original author qualifies. |
| D3 | Public host / repo name / license. | New repo `snes-ipl-cleanroom`; permissive license. **User decides.** |
| D4 | Counsel review — **RE-SEQUENCED → §0: now the FIRST action, not a pre-push gate.** | Send the §0.A briefing packet to FSF/SFLC counsel; act on §0.B. Blocks Phases 1–6. The whole project turns on this answer. |
| R1 | Hash-matching DMCA takedown despite legal merit. | Pre-build the provenance bundle (§5/Phase 5) as counter-notice ammo. |
| R2 | MAME won't accept a differing ROM. | §8; acceptable for own-use + public repo; upstreaming is a stretch goal. |
| Q-open | EA v. Sega / Razorsoft specifics. | Not located as reported opinions; dig only if needed (Sega v. Accolade is the precedent). |

---

## 11. Verification  *(steps are the spec; fill raw output + PASS/FAIL during execution)*

> Per `~/SRC/CLAUDE.md`: keep these numbered steps verbatim; paste raw output under
> each; mark PASS/FAIL; write results back here.

1. **Assembler runs.** `wla-spc700` (+`wlalink`) assembles a trivial SPC700 `.s`
   smoke file to expected opcodes.

   ```
   $ wla-spc700 -o smoke.o smoke.s && wlalink smoke.link smoke.bin
   $ wc -c < smoke.bin                # 64
   $ xxd smoke.bin | head -1
   00000000: 00cd 12e8 3460 cf6f 0000 0000 0000 0000  ....4`.o........
   # nop=00  mov x,#$12=CD 12  mov a,#$34=E8 34  clrc=60  mul ya=CF  ret=6F
   got: 00cd12e83460cf6f   exp: 00cd12e83460cf6f   -> MATCH
   ```
   **PASS** (wla-spc700 v10.6, 2026-06-19).
2. **Image shape.** `wla-spc700 src/ipl.s` → **exactly 64 bytes**; reset vector at
   `$FFFE/$FFFF`. *(Output: TBD)*
3. **Wall integrity.** `PROVENANCE.md` + `ATTESTATION.md` exist; Person A ≠ B; no
   Nintendo ROM/disassembly anywhere in the repo (grep/hash audit). *(Output: TBD)*
4. **Behavioural boot.** MAME using our `ipl.bin` (§8): a real cart's audio upload
   completes, SPC700 jumps into uploaded code, audio runs — confirmed via the drmon
   bridge (APU-RAM + SPC700 regs). *(Output: TBD)*
5. **Independent reproduction.** A second SPC700 assembler reproduces **identical
   bytes**. *Mechanism validated in Phase 0:* bass v18 assembled the byte-equivalent
   program (6502-dialect: `nop/ldx #$12/lda #$34/clc/mul/rts`) to the same
   `00 cd 12 e8 34 60 cf 6f` as wla-spc700 → both agree. *Caveat:* bass needs a
   hand-translated source (non-canonical dialect, §2). **`spcasm` evaluated
   (2026-06-19): canonical Sony dialect ✅ → the chosen one-source-two-tools
   cross-check, but it bundles the IPL (Person-C-only, §6); its live byte check is
   pending build authorization (untrusted `cargo build` + pinned nightly).**
   *(Re-run on the real `ipl.s` at Phase 5: TBD)*
6. **MAME region facts.** Exact override path for `spc700.rom`/`sound_ipl` in the
   driven MAME build documented (no guesses). *(Output: TBD)*

---

## Sources

Hardware / boot (retrieved 2026-06-19):
- nesdev "Booting the SPC700": https://snes.nesdev.org/wiki/Booting_the_SPC700
- nesdev S-SMP: https://snes.nesdev.org/wiki/S-SMP · ROM header (cart checksum
  "not needed by the SNES hardware"): https://snes.nesdev.org/wiki/ROM_header
- SnesLab IPL ROM: https://sneslab.net/wiki/SPC700/IPL_ROM
- eKid's commented SPC700 IPL disassembly (Super Famicom Dev Wiki): https://wiki.superfamicom.org/spc700-reference
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
