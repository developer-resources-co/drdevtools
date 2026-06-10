# SourceForge archive notice

The notice posted on the original [SourceForge project](https://sourceforge.net/projects/drdevtools/)
(2026-06-10) to redirect visitors to the new GitHub home after the
[CVS → git migration](plans/2026-06-10-migrate-cvs-to-github.md). Kept here so the
exact wording is reproducible.

## Where to post it

The project has **no Wiki and no News tool enabled** — only Activity, Bugs, and Code
(CVS). The notice goes on the **Admin → Metadata** page
([sourceforge.net/p/drdevtools/admin/overview](https://sourceforge.net/p/drdevtools/admin/overview),
requires SF login). Three controls there, in priority order:

1. **"Moved Project to:" radio** (under *Project Status*, right column) → set the URL below.
   This is SourceForge's native "project moved" mechanism: shows a banner, keeps code/files
   readable as an archive. Flips status from *Active* to *Moved* (accurate for an archive).
2. **Short Summary** — **hard 70-character limit** → use the 61-char line below.
3. **Full Description** *(optional)* → prepend the Markdown block. Markdown rendering in this
   field is not guaranteed, so treat it as secondary to 1 + 2.

### "Moved Project to:" URL

```text
https://github.com/developer-resources-co/drdevtools
```

### Short Summary (≤70 chars — this is 61)

```text
Moved to GitHub: github.com/developer-resources-co/drdevtools
```

### Full Description block (optional, Markdown)

```markdown
# ⚠ This project has moved to GitHub

**drdevtools is now maintained on GitHub:**
**[github.com/developer-resources-co/drdevtools](https://github.com/developer-resources-co/drdevtools)**

The complete CVS history was migrated to git on 2026-06-10. This SourceForge
project is retained only as a read-only archive — all current development happens
on GitHub. Please update your bookmarks and clone from the GitHub URL above.
```
