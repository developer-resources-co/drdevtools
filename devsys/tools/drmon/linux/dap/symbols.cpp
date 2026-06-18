//=============================================================================
// symbols.cpp — SymbolTable: binary .sld + Sierra COFF loaders, no TUI deps.
//=============================================================================

#include "symbols.hpp"
#include "symfmt.hpp"   // shared ca65 .dbg / WLA-DX .sym parsers

#include <cstdio>
#include <cstring>
#include <cctype>
#include <climits>
#include <algorithm>

// BSD-licensed libdwarf; modern dwarf_init_path/dwarf_finish API (>=0.3).
// NB: Ubuntu 26.04's libdwarf-dev pkg-config advertises -I/usr/include/libdwarf-1
// but actually ships the headers in /usr/include/libdwarf/, so include via that
// prefix (resolved through the default /usr/include path).
#include <libdwarf/libdwarf.h>
#include <libdwarf/dwarf.h>

//-----------------------------------------------------------------------------
// Table maintenance
//-----------------------------------------------------------------------------

void SymbolTable::add(uint32_t addr, const std::string& name) {
    if (name.empty()) return;
    byAddr_[addr] = {addr, name};
    std::string lower = name;
    for (char& c : lower) c = (char)tolower((unsigned char)c);
    byName_[lower] = addr;
}

void SymbolTable::addSrc(const SrcLine& sl) {
    auto& vec = srcMap_[sl.file];
    vec.push_back({sl.line, sl.addr});
}

//-----------------------------------------------------------------------------
// Lookups
//-----------------------------------------------------------------------------

const Symbol* SymbolTable::findByAddr(uint32_t addr) const {
    if (byAddr_.empty()) return nullptr;
    auto it = byAddr_.upper_bound(addr);
    if (it == byAddr_.begin()) return nullptr;
    --it;
    if (addr - it->second.addr > 256) return nullptr;
    return &it->second;
}

const Symbol* SymbolTable::findByName(const std::string& name) const {
    std::string lower = name;
    for (char& c : lower) c = (char)tolower((unsigned char)c);
    auto it = byName_.find(lower);
    if (it == byName_.end()) return nullptr;
    auto it2 = byAddr_.find(it->second);
    if (it2 == byAddr_.end()) return nullptr;
    return &it2->second;
}

uint32_t SymbolTable::addrForSrc(const std::string& file, int line) const {
    auto it = srcMap_.find(file);
    if (it == srcMap_.end()) return 0;
    const auto& vec = it->second;
    // Exact line first.
    for (const auto& p : vec)
        if (p.first == line) return p.second;
    // Fallback: nearest *following* mapped line within a small window. Optimized
    // builds (-Os) drop line-table rows for blank/brace/comment lines, so a
    // breakpoint set on a gap should advance to the next real line. Pick the
    // smallest line >= target; among ties, the lowest address (that line's first
    // instruction).
    int bestLine = INT_MAX; uint32_t bestAddr = 0;
    for (const auto& p : vec) {
        if (p.first >= line &&
            (p.first < bestLine || (p.first == bestLine && p.second < bestAddr))) {
            bestLine = p.first; bestAddr = p.second;
        }
    }
    if (bestLine != INT_MAX && bestLine <= line + 10) return bestAddr;
    return 0;
}

std::vector<std::string> SymbolTable::sourceFiles() const {
    std::vector<std::string> out;
    out.reserve(srcMap_.size());
    for (const auto& kv : srcMap_) out.push_back(kv.first);
    return out;
}

//-----------------------------------------------------------------------------
// Binary .sld loader (Developer Resources format)
//
// Header: 256 bytes, first 27 = "Source level debugging file"
// Records at offset 0x100:
//   0x01: filename, read bytes until 0xFF (then skip second 0xFF)
//   0x02: 3-byte addr (lo,hi,bank little-endian) + 2-byte line (lo,hi)
//-----------------------------------------------------------------------------

bool SymbolTable::loadSld(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) return false;

    char hdr[27] = {};
    if (fread(hdr, 1, 27, f) != 27 ||
        memcmp(hdr, "Source level debugging file", 27) != 0) {
        fclose(f);
        return false;
    }
    if (fseek(f, 0x100, SEEK_SET) != 0) {
        fclose(f);
        return false;
    }

    std::string curFile;
    int c;
    while ((c = fgetc(f)) != EOF) {
        if (c == 1) {
            curFile.clear();
            int ch;
            while ((ch = fgetc(f)) != EOF && ch != 0xFF && ch != 0)
                curFile += (char)ch;
            if (ch == 0xFF) fgetc(f);  // consume second 0xFF
        } else if (c == 2) {
            uint8_t b[5];
            if (fread(b, 1, 5, f) != 5) break;
            uint32_t addr = (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16);
            int line = (int)(b[3] | ((unsigned)b[4] << 8));
            if (line == 0xFFFF) continue;
            if (!curFile.empty())
                addSrc({addr, curFile, line});
        } else {
            break;  // unknown record type
        }
    }

    fclose(f);
    return true;
}

//-----------------------------------------------------------------------------
// Sierra COFF loader (magic 0x0150, big-endian / 68k byte order)
//
// FILHDR (20 bytes): f_magic(2) f_nscns(2) f_timdat(4) f_symptr(4)
//                    f_nsyms(4) f_opthdr(2) f_flags(2)
// SYMENT (18 bytes): name_or_offset(8) n_value(4) n_scnum(2) n_type(2)
//                    n_sclass(1) n_numaux(1)
// String table at symptr + nsyms*18: len(4,BE) then null-terminated strings
//-----------------------------------------------------------------------------

static inline uint16_t rd16be(const uint8_t* b) {
    return (uint16_t)((b[0] << 8) | b[1]);
}
static inline uint32_t rd32be(const uint8_t* b) {
    return ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) |
           ((uint32_t)b[2] << 8)  | b[3];
}

bool SymbolTable::loadCoff(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) return false;

    // Read file header
    uint8_t fh[20];
    if (fread(fh, 1, 20, f) != 20) { fclose(f); return false; }
    if (rd16be(fh) != 0x0150) { fclose(f); return false; }

    int32_t  symptr  = (int32_t)rd32be(fh + 8);
    int32_t  nsyms   = (int32_t)rd32be(fh + 12);
    uint16_t opthdr  = rd16be(fh + 16);

    if (symptr <= 0 || nsyms <= 0) { fclose(f); return false; }

    // Skip optional header if present
    if (opthdr > 0) {
        // already read 20 bytes; optional header starts after sections
        // (section parsing not needed for symbol loading — skip)
    }

    // Load string table
    long strtab_off = symptr + (long)18 * nsyms;
    std::vector<char> strtab;
    if (fseek(f, strtab_off, SEEK_SET) == 0) {
        uint8_t slen_b[4];
        if (fread(slen_b, 1, 4, f) == 4) {
            uint32_t slen = rd32be(slen_b);
            if (slen > 4) {
                strtab.resize(slen - 4);
                fread(strtab.data(), 1, strtab.size(), f);
            }
        }
    }

    // Iterate symbol table
    fseek(f, symptr, SEEK_SET);
    uint8_t raw[18];
    for (int32_t i = 0; i < nsyms; ) {
        if (fread(raw, 1, 18, f) != 18) break;
        ++i;

        uint32_t n_value  = rd32be(raw + 8);
        int16_t  n_scnum  = (int16_t)rd16be(raw + 12);
        uint8_t  n_sclass = raw[16];
        uint8_t  n_numaux = raw[17];

        // Determine symbol name
        char namebuf[9] = {};
        const char* symname = nullptr;
        bool in_strtab = (raw[0] == 0 && raw[1] == 0 && raw[2] == 0 && raw[3] == 0);
        if (in_strtab) {
            uint32_t n_off = rd32be(raw + 4);
            if (n_off >= 4 && !strtab.empty()) {
                size_t idx = n_off - 4;
                if (idx < strtab.size())
                    symname = strtab.data() + idx;
            }
        } else {
            memcpy(namebuf, raw, 8);
            symname = namebuf;
        }

        // Skip aux entries (each 18 bytes)
        for (uint8_t j = 0; j < n_numaux; ++j) {
            fread(raw, 1, 18, f);
            ++i;
        }

        // Skip structural/metadata symbols
        switch (n_sclass) {
        case 100: // C_BLOCK
        case 101: // C_FCN
        case 102: // C_EOS
        case 103: // C_FILE
        case  13: // C_TPDEF
        case 105: // C_ALIAS
            continue;
        case   2: // C_EXT — skip undefined
            if (n_scnum == 0) continue;
            break;
        case   3: // C_STAT — skip section names (".")
            if (symname && symname[0] == '.') continue;
            break;
        default:
            break;
        }

        if (!symname || symname[0] == '\0') continue;
        add(n_value, symname);
    }

    fclose(f);
    return true;
}

//-----------------------------------------------------------------------------
// Modern homebrew importers (ca65 .dbg, WLA-DX .sym) via the shared symfmt
// parsers. Each feeds labels into the address/name index and source lines into
// the file:line map, then content-sniffs via the parser's return value so the
// loadSld() || loadCoff() || ... dispatch chain falls through cleanly.
//-----------------------------------------------------------------------------

void SymbolTable::importSymData(const SymData& d) {
    for (size_t i = 0; i < d.labels.size(); ++i)
        add(d.labels[i].addr, d.labels[i].name);
    for (size_t i = 0; i < d.lines.size(); ++i)
        addSrc({d.lines[i].addr, d.lines[i].file, d.lines[i].line});
}

bool SymbolTable::loadCa65Dbg(const char* path) {
    SymData d;
    if (!parseCa65Dbg(path, d)) return false;
    importSymData(d);
    return true;
}

bool SymbolTable::loadWlaSym(const char* path) {
    SymData d;
    if (!parseWlaSym(path, d)) return false;
    importSymData(d);
    return true;
}

//-----------------------------------------------------------------------------
// ELF/DWARF loader (llvm-mos `-g` output) via libdwarf.
//
// Two passes per compile unit:
//   1. .debug_line  → srcMap_   (file:line → address) for source breakpoints
//   2. DIE tree     → byAddr_/byName_ for DW_TAG_subprogram entries (DW_AT_low_pc)
// Variable DW_AT_location decoding (DW_OP_regx/DW_OP_fbreg → value) is deliberately
// NOT done here — it is not needed for the source-line-breakpoint gate, and is a
// documented future follow-on.
//
// SNES note: llvm-mos ELFs are 32-bit (CodePointerSize=4); a 24-bit banked SNES
// address sits in the low bits of the 32-bit value, which is exactly what byAddr_/
// srcMap_ (uint32_t) and MAME's bridge expect — no conversion needed.
//-----------------------------------------------------------------------------

static std::string elf_basename(const char* p) {
    if (!p) return std::string();
    const char* slash = strrchr(p, '/');
    return slash ? std::string(slash + 1) : std::string(p);
}

// Recursively walk a DIE subtree, recording subprogram entry points.
void SymbolTable::walkDwarfDies(void* dbg_v, void* die_v) {
    Dwarf_Debug dbg = (Dwarf_Debug)dbg_v;
    Dwarf_Die   die = (Dwarf_Die)die_v;
    Dwarf_Error err = nullptr;

    Dwarf_Half tag = 0;
    if (dwarf_tag(die, &tag, &err) == DW_DLV_OK && tag == DW_TAG_subprogram) {
        Dwarf_Addr low = 0;
        if (dwarf_lowpc(die, &low, &err) == DW_DLV_OK) {
            char* name = nullptr;
            if (dwarf_diename(die, &name, &err) == DW_DLV_OK && name) {
                add((uint32_t)low, name);
                dwarf_dealloc(dbg, name, DW_DLA_STRING);
            }
        }
    }

    // Recurse into children, then iterate their siblings.
    Dwarf_Die child = nullptr;
    if (dwarf_child(die, &child, &err) == DW_DLV_OK) {
        Dwarf_Die cur = child;
        for (;;) {
            walkDwarfDies(dbg, cur);
            Dwarf_Die sib = nullptr;
            int r = dwarf_siblingof_b(dbg, cur, /*is_info=*/true, &sib, &err);
            dwarf_dealloc_die(cur);
            if (r != DW_DLV_OK) break;
            cur = sib;
        }
    }
}

bool SymbolTable::loadElf(const char* path) {
    // Cheap magic check so the loadSld||...||loadElf dispatch chain falls through
    // for non-ELF inputs without invoking libdwarf at all.
    {
        FILE* f = fopen(path, "rb");
        if (!f) return false;
        unsigned char m[4] = {0};
        size_t n = fread(m, 1, 4, f);
        fclose(f);
        if (n != 4 || memcmp(m, "\x7f""ELF", 4) != 0) return false;
    }

    Dwarf_Debug dbg = nullptr;
    Dwarf_Error err = nullptr;
    if (dwarf_init_path(path, nullptr, 0, DW_GROUPNUMBER_ANY,
                        nullptr, nullptr, &dbg, &err) != DW_DLV_OK) {
        if (err) dwarf_dealloc_error(dbg, err);
        return false;  // ELF without usable DWARF — let the caller treat as no symbols
    }

    bool anyCU = false;
    for (;;) {
        Dwarf_Unsigned cu_len = 0, abbrev_off = 0, next_cu = 0, typeoff = 0;
        Dwarf_Half version = 0, addr_size = 0, offset_size = 0, ext_size = 0, htype = 0;
        Dwarf_Sig8 sig; memset(&sig, 0, sizeof(sig));
        int r = dwarf_next_cu_header_d(dbg, /*is_info=*/true, &cu_len, &version,
                                       &abbrev_off, &addr_size, &offset_size, &ext_size,
                                       &sig, &typeoff, &next_cu, &htype, &err);
        if (r == DW_DLV_NO_ENTRY) break;
        if (r != DW_DLV_OK) break;

        Dwarf_Die cu_die = nullptr;
        if (dwarf_siblingof_b(dbg, nullptr, /*is_info=*/true, &cu_die, &err) != DW_DLV_OK)
            continue;
        anyCU = true;

        // Pass 1 — line table → srcMap_.
        Dwarf_Unsigned lineversion = 0;
        Dwarf_Small    table_count = 0;
        Dwarf_Line_Context linectx = nullptr;
        if (dwarf_srclines_b(cu_die, &lineversion, &table_count, &linectx, &err) == DW_DLV_OK) {
            Dwarf_Line* lines = nullptr;
            Dwarf_Signed nlines = 0;
            if (dwarf_srclines_from_linecontext(linectx, &lines, &nlines, &err) == DW_DLV_OK) {
                for (Dwarf_Signed i = 0; i < nlines; ++i) {
                    Dwarf_Bool endseq = 0;
                    if (dwarf_lineendsequence(lines[i], &endseq, &err) == DW_DLV_OK && endseq)
                        continue;
                    Dwarf_Addr addr = 0;
                    Dwarf_Unsigned lno = 0;
                    char* src = nullptr;
                    if (dwarf_lineaddr(lines[i], &addr, &err) != DW_DLV_OK) continue;
                    if (dwarf_lineno(lines[i], &lno, &err) != DW_DLV_OK)   continue;
                    if (lno == 0) continue;  // line-0 markers (e.g. loclist boundaries)
                    if (dwarf_linesrc(lines[i], &src, &err) == DW_DLV_OK && src) {
                        // Index under both the basename and the full path so a DAP
                        // client matches whether it sends "a16local.c" or an abs path.
                        std::string full = src;
                        std::string base = elf_basename(src);
                        addSrc({(uint32_t)addr, base, (int)lno});
                        if (full != base) addSrc({(uint32_t)addr, full, (int)lno});
                        dwarf_dealloc(dbg, src, DW_DLA_STRING);
                    }
                }
            }
            dwarf_srclines_dealloc_b(linectx);
        }

        // Pass 2 — subprogram entry points → byAddr_/byName_.
        walkDwarfDies(dbg, cu_die);
        dwarf_dealloc_die(cu_die);
    }

    dwarf_finish(dbg);
    return anyCU;
}
