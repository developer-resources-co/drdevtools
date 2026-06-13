//=============================================================================
// symfmt.cpp — ca65 .dbg and WLA-DX .sym parsers (see symfmt.hpp).
//
// Reference formats captured from real toolchains:
//
//   ca65 (.dbg, line-based text; "keyword<ws>key=val,key=val,..."):
//     version major=2,minor=0
//     file  id=0,name="foo.s",...
//     seg   id=0,name="CODE",start=0x008000,...      ; seg start is HEX
//     span  id=0,seg=0,start=0,size=1                ; span start/size DECIMAL
//     line  id=7,file=0,line=4,span=0[+1+2...]       ; lines w/o span = no code
//     sym   id=2,name="reset",val=0x8000,type=lab    ; val HEX; type=lab labels
//   Line address = seg[span.seg].start + span.start (min over a line's spans).
//
//   WLA-DX (.sym, INI-like sections; "; ..." banner/comments):
//     [labels]            BB:AAAA name               ; addr = (BB<<16)|AAAA
//     [definitions]       VVVVVVVV name              ; constants (skipped)
//     [source files v2]   FFFF:GGGG crc filename     ; filekey -> name
//     [addr-to-line mapping v2]
//                         OFFS BB:AAAA CPU FFFF:GGGG:LINE ; line is HEX
//     (older WLA: "[source files]" / "[addr-to-line mapping]" with FFFF / 2-col)
//=============================================================================

#include "symfmt.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <map>

//-----------------------------------------------------------------------------
// Small text helpers (gnu++98-safe; file scope so local-as-template-arg rules
// in strict C++98 don't bite).
//-----------------------------------------------------------------------------
namespace {

// File-scope record types (used as std::map/std::vector value types).
struct SpanInfo { int seg; uint32_t start; };
struct LineRec  { int file; int ln; std::vector<int> spanIds; };
struct AtlRec   { uint32_t addr; std::string fileKey; int ln; };

bool readLine(FILE* f, std::string& out)
{
    out.clear();
    int c = fgetc(f);
    if (c == EOF) return false;
    while (c != EOF && c != '\n') {
        if (c != '\r') out += (char)c;        // tolerate CRLF
        c = fgetc(f);
    }
    return true;
}

std::string trim(const std::string& s)
{
    size_t b = 0, e = s.size();
    while (b < e && isspace((unsigned char)s[b])) ++b;
    while (e > b && isspace((unsigned char)s[e - 1])) --e;
    return s.substr(b, e - b);
}

std::string lower(const std::string& s)
{
    std::string r = s;
    for (size_t i = 0; i < r.size(); ++i) r[i] = (char)tolower((unsigned char)r[i]);
    return r;
}

// Hex if "0x"/"0X" prefixed, else decimal.
uint32_t parseNum(const std::string& s)
{
    const char* p = s.c_str();
    if (s.size() > 2 && p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))
        return (uint32_t)strtoul(p + 2, NULL, 16);
    return (uint32_t)strtoul(p, NULL, 10);
}

uint32_t parseHex(const std::string& s)
{
    return (uint32_t)strtoul(s.c_str(), NULL, 16);
}

void split(const std::string& s, char delim, std::vector<std::string>& out)
{
    out.clear();
    std::string cur;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == delim) { out.push_back(cur); cur.clear(); }
        else cur += s[i];
    }
    out.push_back(cur);
}

void tokenize(const std::string& s, std::vector<std::string>& out)
{
    out.clear();
    size_t i = 0, n = s.size();
    while (i < n) {
        while (i < n && isspace((unsigned char)s[i])) ++i;
        if (i >= n) break;
        size_t j = i;
        while (j < n && !isspace((unsigned char)s[j])) ++j;
        out.push_back(s.substr(i, j - i));
        i = j;
    }
}

// Parse a ca65 record's "key=val,key=val,..." list. Double-quoted values may
// contain commas; quotes are stripped.
void parseAttrs(const std::string& s, std::map<std::string, std::string>& m)
{
    m.clear();
    size_t i = 0, n = s.size();
    while (i < n) {
        size_t keyStart = i;
        while (i < n && s[i] != '=') ++i;
        if (i >= n) break;
        std::string key = trim(s.substr(keyStart, i - keyStart));
        ++i;                                  // skip '='
        std::string val;
        if (i < n && s[i] == '"') {
            ++i;
            while (i < n && s[i] != '"') { val += s[i]; ++i; }
            if (i < n) ++i;                   // skip closing quote
            while (i < n && s[i] != ',') ++i; // skip to separator
        } else {
            while (i < n && s[i] != ',') { val += s[i]; ++i; }
        }
        if (i < n && s[i] == ',') ++i;
        m[key] = val;
    }
}

} // namespace

//-----------------------------------------------------------------------------
// ca65 .dbg
//-----------------------------------------------------------------------------
bool parseCa65Dbg(const char* path, SymData& out)
{
    FILE* f = fopen(path, "rb");
    if (!f) return false;

    std::map<int, std::string> files;     // file id -> name
    std::map<int, uint32_t>    segStart;  // seg id  -> absolute start
    std::map<int, SpanInfo>    spans;     // span id -> (seg, offset)
    std::vector<LineRec>       lineRecs;
    std::vector<SymLabel>      labels;

    bool sniffed = false;
    std::string line;
    while (readLine(f, line)) {
        std::string t = trim(line);
        if (t.empty()) continue;

        size_t ws = 0;
        while (ws < t.size() && !isspace((unsigned char)t[ws])) ++ws;
        std::string kw   = t.substr(0, ws);
        std::string rest = trim(t.substr(ws));

        if (!sniffed) {                       // first record must be `version`
            if (kw != "version") { fclose(f); return false; }
            sniffed = true;
            continue;
        }

        std::map<std::string, std::string> a;
        parseAttrs(rest, a);

        if (kw == "file") {
            files[(int)parseNum(a["id"])] = a["name"];
        } else if (kw == "seg") {
            segStart[(int)parseNum(a["id"])] = parseNum(a["start"]);
        } else if (kw == "span") {
            SpanInfo si;
            si.seg   = (int)parseNum(a["seg"]);
            si.start = parseNum(a["start"]);
            spans[(int)parseNum(a["id"])] = si;
        } else if (kw == "line") {
            std::map<std::string, std::string>::iterator sp = a.find("span");
            if (sp == a.end()) continue;      // no span => no code address
            LineRec lr;
            lr.file = (int)parseNum(a["file"]);
            lr.ln   = (int)parseNum(a["line"]);
            std::vector<std::string> ids;
            split(sp->second, '+', ids);
            for (size_t k = 0; k < ids.size(); ++k)
                lr.spanIds.push_back((int)parseNum(ids[k]));
            lineRecs.push_back(lr);
        } else if (kw == "sym") {
            std::map<std::string, std::string>::iterator ty = a.find("type");
            if (ty != a.end() && ty->second != "lab") continue;  // labels only
            std::map<std::string, std::string>::iterator v = a.find("val");
            if (v == a.end()) continue;       // imports etc. have no value
            SymLabel sl;
            sl.addr = parseNum(v->second);
            sl.name = a["name"];
            if (!sl.name.empty()) labels.push_back(sl);
        }
    }
    fclose(f);

    if (!sniffed) return false;

    // Resolve each source line to the lowest address among its spans.
    for (size_t i = 0; i < lineRecs.size(); ++i) {
        const LineRec& lr = lineRecs[i];
        bool have = false;
        uint32_t best = 0;
        for (size_t k = 0; k < lr.spanIds.size(); ++k) {
            std::map<int, SpanInfo>::iterator s = spans.find(lr.spanIds[k]);
            if (s == spans.end()) continue;
            std::map<int, uint32_t>::iterator g = segStart.find(s->second.seg);
            if (g == segStart.end()) continue;
            uint32_t addr = g->second + s->second.start;
            if (!have || addr < best) { best = addr; have = true; }
        }
        if (!have) continue;
        SymLine sl;
        sl.addr = best;
        sl.line = lr.ln;
        std::map<int, std::string>::iterator fit = files.find(lr.file);
        sl.file = (fit != files.end()) ? fit->second : std::string();
        out.lines.push_back(sl);
    }
    out.labels = labels;
    return true;
}

//-----------------------------------------------------------------------------
// WLA-DX .sym
//-----------------------------------------------------------------------------
bool parseWlaSym(const char* path, SymData& out)
{
    FILE* f = fopen(path, "rb");
    if (!f) return false;

    std::map<std::string, std::string> fileNames;  // filekey -> filename
    std::vector<SymLabel> labels;
    std::vector<AtlRec>   atl;                      // resolve filenames at end

    bool isWla = false;
    std::string section;
    std::string line;
    while (readLine(f, line)) {
        std::string t = trim(line);
        if (t.empty() || t[0] == ';') continue;     // blank / banner / comment

        if (t[0] == '[') {                          // section header
            section = lower(t);
            if (section == "[labels]"      || section == "[symbols]"       ||
                section == "[definitions]" || section == "[sections]"      ||
                section == "[breakpoints]" || section == "[information]"   ||
                section == "[rom checksum]"                                ||
                section.find("[source files")  == 0 ||
                section.find("[addr-to-line") == 0)
                isWla = true;
            continue;
        }

        if (section == "[labels]" || section == "[symbols]") {
            std::vector<std::string> tok;
            tokenize(t, tok);
            if (tok.size() >= 2) {
                std::vector<std::string> ba;
                split(tok[0], ':', ba);
                if (ba.size() == 2) {
                    SymLabel sl;
                    sl.addr = (parseHex(ba[0]) << 16) | parseHex(ba[1]);
                    sl.name = tok[1];
                    labels.push_back(sl);
                }
            }
        } else if (section.find("[source files") == 0) {
            std::vector<std::string> tok;
            tokenize(t, tok);
            if (tok.size() >= 2)                    // key = first, name = last
                fileNames[tok[0]] = tok[tok.size() - 1];
        } else if (section.find("[addr-to-line") == 0) {
            std::vector<std::string> tok;
            tokenize(t, tok);
            if (tok.size() == 4) {                  // v2: OFFS BB:AAAA CPU F:G:LINE
                std::vector<std::string> ba, fl;
                split(tok[1], ':', ba);
                split(tok[3], ':', fl);
                if (ba.size() == 2 && fl.size() >= 2) {
                    AtlRec r;
                    r.addr = (parseHex(ba[0]) << 16) | parseHex(tok[2]);
                    r.ln   = (int)parseHex(fl[fl.size() - 1]);
                    std::string key = fl[0];        // file key = all but last field
                    for (size_t i = 1; i + 1 < fl.size(); ++i) key += ":" + fl[i];
                    r.fileKey = key;
                    atl.push_back(r);
                }
            } else if (tok.size() == 2) {           // old: BB:AAAA FFFF:LINE
                std::vector<std::string> ba, fl;
                split(tok[0], ':', ba);
                split(tok[1], ':', fl);
                if (ba.size() == 2 && fl.size() == 2) {
                    AtlRec r;
                    r.addr    = (parseHex(ba[0]) << 16) | parseHex(ba[1]);
                    r.ln      = (int)parseHex(fl[1]);
                    r.fileKey = fl[0];
                    atl.push_back(r);
                }
            }
        }
    }
    fclose(f);

    if (!isWla) return false;

    out.labels = labels;
    for (size_t i = 0; i < atl.size(); ++i) {
        SymLine sl;
        sl.addr = atl[i].addr;
        sl.line = atl[i].ln;
        std::map<std::string, std::string>::iterator it = fileNames.find(atl[i].fileKey);
        sl.file = (it != fileNames.end()) ? it->second : atl[i].fileKey;
        out.lines.push_back(sl);
    }
    return true;
}
