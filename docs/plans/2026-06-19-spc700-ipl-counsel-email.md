---
title: Draft cover email to IP counsel — SPC700 IPL clean-room question
date: 2026-06-19
companion: 2026-06-19-spc700-ipl-cleanroom.md  (§0.A briefing packet = the attachment)
status: DRAFT — fill placeholders, attach the plan, send
---

# Draft email to counsel

**How to use this.** Fill the `[bracketed]` placeholders, attach the plan
([`2026-06-19-spc700-ipl-cleanroom.md`](2026-06-19-spc700-ipl-cleanroom.md) — §0.A is
the self-contained briefing packet), and send. **Recipient:** the FSF licensing &
compliance lab (<licensing@fsf.org>) per your steer; if they treat this as outside a
pure GPL/licensing-FAQ scope, the natural alternates for an emulation /
reverse-engineering copyright question are the **Software Freedom Law Center**
(softwarefreedom.org), the **Software Freedom Conservancy**, or the **EFF** (which
litigates emulation/preservation copyright matters). Keep the email itself short; the
homework lives in the attached packet.

---

**Subject:** Copyright question — independently reimplementing a 64-byte SNES audio boot ROM for public release

**To:** [FSF Licensing & Compliance — licensing@fsf.org / your preferred contact]

Dear [Name],

I'm a software developer working on a free-software preservation project, and I'd
value your professional read on one copyright question **before** I invest further
effort or publish anything. I've done a lot of homework (a fully-sourced briefing is
attached), but I'm not a lawyer, and the whole project turns on a single threshold
question only counsel can answer.

**What I want to do.** Independently re-create the Super Nintendo's 64-byte "SPC700
IPL" — the tiny boot ROM in the SNES's audio CPU — and publish *both the source and
the assembled 64 bytes* in a public repository under a free license. The goal is that
emulators such as MAME could use a non-Nintendo IPL, and anyone could run the SNES
audio subsystem legally rather than only people who dump their own console. **Public
distribution is the point — and the risk.**

**The single threshold question:** *Is a 64-byte, interface-forced boot ROM
copyrightable at all?* Everything downstream follows from your answer:
- If merger / thin-copyright means it isn't protectable → I can likely just publish,
  and there is little more to do.
- If it is protectable → I would run a documented clean-room (Chinese-wall)
  reimplementation, and I need to know whether that gives a defensible basis to
  publish.

I'd much rather pay for an hour on this threshold question now than build the whole
thing on a wrong assumption.

**The wrinkle that makes this unusual.** The ROM is so constrained — 64 bytes, fixed
hardware I/O ports, and a fixed handshake every commercial game depends on — that a
*correct independent* reimplementation will almost certainly come out **byte-identical**
to Nintendo's. There is **no hardware checksum** forcing this; the function simply
compels the bytes (faithful emulators run the 64 bytes raw, with no integrity check).
The only place exact bytes are *required* is the MAME project's romset hash, which is a
distribution-management checksum, not a hardware mechanism. So I expect to produce bytes
identical to Nintendo's, *by independent creation*, and I need to understand whether —
and by what process — that is safe to publish.

**What already exists.** Several open-source projects already ship Nintendo's actual
bytes or a disassembly of them (e.g., the snes9x emulator embeds the verbatim 64 bytes;
an annotated disassembly is on community wikis). To my knowledge **no documented
clean-room reimplementation exists** — only copies/disassemblies, which carry Nintendo's
copyright rather than independent provenance. I don't want to merely re-publish a copy.

**My layperson's understanding of the landscape** (which I'm sure you'll correct):
independent creation is a complete defense (*Three Boys Music v. Bolton*); reverse-
engineering a console BIOS to build an emulator has been held fair use (*Sony v.
Connectix*; *Sega v. Accolade*); a clean room is only as good as its isolation (*Atari
v. Nintendo*, where tainted access sank the defense); highly-constrained functional code
can merge idea and expression (*CA v. Altai*; *Baker v. Selden*); and the seminal
clean-room microcode case treated independently-produced similarity as compelled by
functional constraints (*NEC v. Intel*).

**My specific questions:**
1. Is a 64-byte, interface-forced boot ROM protectable expression, or does merger /
   scènes à faire / thin-copyright remove protection?
2. If it is unprotectable, may I publish the bytes (from a clean re-derivation) plus
   source publicly without meaningful infringement exposure?
3. If it is protectable, does a documented Chinese-wall process (interface-derived
   spec → an implementer with no exposure to the original → a preserved provenance/audit
   trail) give a defensible independent-creation basis to publish — *even though the
   output will likely be byte-identical to Nintendo's*?
4. Does the result matching MAME's published IPL hash (i.e., being a drop-in) add any
   exposure?
5. For the clean room: what "no access" attestation standard would you want? (I assume
   an AI model trained on the original cannot serve as the "clean" implementer, and that
   a human who has never seen the IPL is required — is that right?)
6. Any trademark/labeling guidance to keep this clearly non-Nintendo?

**Logistics.** I understand this may go beyond a pure licensing-FAQ question and may
call for a formal engagement or a referral — please let me know the right path and any
fee. I'm happy to send the full technical-and-case briefing (a few pages, fully sourced)
and the project plan; I've attached them and kept this email to the essentials.

This is squarely a free-software / preservation effort, which is why I thought of
[the FSF / SFLC] first. Thank you for considering it.

Best regards,
[Your name]
[email / phone]

---

### References (for verification; full set in the attached briefing)

Technical:
- snes9x — Nintendo's verbatim 64-byte IPL embedded as a C array:
  https://raw.githubusercontent.com/snes9xgit/snes9x/master/apu/bapu/smp/iplrom.cpp
- MAME — IPL pinned by hash (`spc700.rom`, CRC 44bb3a40):
  https://github.com/mamedev/mame/blob/master/src/devices/machine/s_smp.cpp
- SNES audio boot protocol: https://snes.nesdev.org/wiki/Booting_the_SPC700 ·
  https://snes.nesdev.org/wiki/S-SMP

Cases (layperson's reading — for your correction):
- *Sony Computer Entertainment v. Connectix*, 203 F.3d 596 (9th Cir. 2000) —
  https://law.justia.com/cases/federal/appellate-courts/F3/203/596/
- *Sega Enterprises v. Accolade*, 977 F.2d 1510 (9th Cir. 1992) —
  https://law.justia.com/cases/federal/appellate-courts/F2/977/1510/
- *Atari Games v. Nintendo*, 975 F.2d 832 (Fed. Cir. 1992) —
  https://law.justia.com/cases/federal/appellate-courts/F2/975/832/
- *NEC Corp. v. Intel Corp.*, 1989 WL 67434 (N.D. Cal. 1989) — (Westlaw)
- *Google LLC v. Oracle America*, 593 U.S. 1 (2021) —
  https://www.law.cornell.edu/supremecourt/text/18-956
- *Computer Associates v. Altai*, 982 F.2d 693 (2d Cir. 1992) —
  https://law.justia.com/cases/federal/appellate-courts/F2/982/693/
- *Baker v. Selden*, 101 U.S. 99 (1879) —
  https://www.law.cornell.edu/supremecourt/text/101/99
- *Three Boys Music v. Bolton*, 212 F.3d 477 (9th Cir. 2000) —
  https://law.justia.com/cases/federal/appellate-courts/F3/212/477/
- AI authorship (re: Q5): U.S. Copyright Office guidance, 88 Fed. Reg. 16190 (2023) —
  https://www.federalregister.gov/documents/2023/03/16/2023-05321/copyright-registration-guidance-works-containing-material-generated-by-artificial-intelligence
