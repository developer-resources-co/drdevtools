//=============================================================================
// symbols.cpp — SymbolTable: binary .sld + Sierra COFF loaders, no TUI deps.
//=============================================================================

#include "symbols.hpp"

#include <cstdio>
#include <cstring>
#include <cctype>
#include <algorithm>

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
    for (const auto& p : vec)
        if (p.first == line) return p.second;
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
