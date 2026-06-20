---
name: links-in-docs
description: "Proactively write links/diffs/references into the relevant markdown doc — don't wait to be asked"
metadata: 
  node_type: memory
  type: feedback
  originSessionId: fb4a9499-7812-44e9-9f76-f41888b56ec6
---

Whenever work produces something linkable — a commit, a plan, a diff, a related doc, a cross-reference — write those links into the relevant markdown document (plan, README, BUGS.md, etc.) as part of the work. Don't wait for the user to ask.

**Why:** Links in chat are throwaway; links in docs persist. The user corrected "put them in the document" and then "don't make me ask for them" — the trigger is the work itself, not an explicit ask.

**How to apply:** After any commit, plan write, doc edit, or investigation — scan for natural cross-reference opportunities (e.g. a new commit → link it from the plan's verification section; a new plan → link it from TODO.md and the relevant README; a bug fix → link the commit from BUGS.md). Write those links into the docs in the same turn.
