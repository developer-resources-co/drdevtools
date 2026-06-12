//=============================================================================
// sliomame.cpp: slaveio.hpp contract implementation for the MAME TCP bridge.
// Included by which.cpp under the MAMEBACKEND arm (before SNES/GENESIS arms).
//
// Architecture: drmon (this file, C++ client) ↔ TCP 127.0.0.1:41816 ↔
//               linux/mame_bridge.lua (MAME-side Lua server).
// Protocol: newline-terminated ASCII, strict request→response.
//
// MAME address override: DRMON_MAME_ADDR=host:port env variable.
// Read cache: 1 KB aligned blocks; invalidated on any mutating operation.
//=============================================================================

#if !defined(__linux__) && !defined(__GNUC__)
#error "sliomame.cpp is Linux/GCC only"
#endif

#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// All drmon project headers are already included by which.cpp → moninc.hpp
// before this file is text-included.  Do not re-include them here — most
// lack include guards and cause redefinition errors on second inclusion.

//=============================================================================
// Globals expected by board.cpp and the legacy slio TUs
//=============================================================================

boolean bConvertUserBreaks = boolean::FALSE;   // no BRK patching; native MAME breakpoints

unsigned int  slaveCtrlc   = 0;                // residual for board.cpp
unsigned int  slaveBankc   = 0;
unsigned int  slaveWormc   = 0;
unsigned char *slaveBufferCmdc = 0;

// SlaveRunning is exposed via the slaveio.hpp declaration
int SlaveRunning(void) { return slaveRunning; }

//=============================================================================
// Internal state
//=============================================================================

static int   g_fd         = -1;    // TCP socket fd; -1 = disconnected
static char  g_host[64]   = "127.0.0.1";
static int   g_port        = 41816;
static int   g_dead        = 1;    // 1 = no live target

// Reconnect throttle (2 s between attempts)
static struct timespec g_last_connect = {0, 0};

// Poll throttle (50 ms between ? polls)
static struct timespec g_last_poll    = {0, 0};

// Last known execution state from ? poll
static int g_slave_running = 0;

//=============================================================================
// Read cache: 8 × 1 KB aligned blocks
//=============================================================================

#define CACHE_BLOCKS       8
#define CACHE_BLOCK_SIZE   1024u
#define CACHE_BLOCK_MASK   (~(unsigned long)(CACHE_BLOCK_SIZE - 1u))

struct CacheEntry {
    unsigned long base;              // 1 KB-aligned address; ~0UL = invalid
    unsigned char data[CACHE_BLOCK_SIZE];
};
static CacheEntry s_cache[CACHE_BLOCKS];

static void cache_init(void) {
    for (int i = 0; i < CACHE_BLOCKS; i++)
        s_cache[i].base = ~0UL;
}

static void cache_invalidate(void) {
    cache_init();
}

static int cache_slot(unsigned long base) {
    return (int)((base >> 10) % CACHE_BLOCKS);
}

//=============================================================================
// Network helpers
//=============================================================================

static long ms_elapsed(const struct timespec &a, const struct timespec &b) {
    return (long)((b.tv_sec - a.tv_sec) * 1000L +
                  (b.tv_nsec - a.tv_nsec) / 1000000L);
}

static void close_conn(void) {
    if (g_fd >= 0) { close(g_fd); g_fd = -1; }
    g_dead = 1;
    cache_invalidate();
}

// Blocking send + recv with 1 s timeout.
// cmd must not include newline; reply is stripped of newline.
// Returns 0 on success, -1 on error.
static int mame_cmd(const char *cmd, char *reply, int reply_sz) {
    if (g_fd < 0) return -1;

    // Send command + newline (commands are short; the bridge caps payloads).
    char sbuf[256];
    int len = snprintf(sbuf, sizeof(sbuf), "%s\n", cmd);
    if (len <= 0 || len >= (int)sizeof(sbuf)) return -1;

    int n = (int)send(g_fd, sbuf, len, MSG_NOSIGNAL);
    if (n != len) { close_conn(); return -1; }

    // Receive reply line directly into the caller's buffer, bounded by reply_sz.
    // (Reading into a fixed local but bounding by reply_sz would overflow it for
    // the large hex replies of R/RP — block reads return up to 2048 chars.)
    struct timeval tv = {1, 0};
    int rpos = 0;
    for (;;) {
        fd_set fds; FD_ZERO(&fds); FD_SET(g_fd, &fds);
        int rc = select(g_fd + 1, &fds, 0, 0, &tv);
        if (rc <= 0) { close_conn(); return -1; }   // timeout or error
        char c;
        int got = (int)recv(g_fd, &c, 1, 0);
        if (got <= 0) { close_conn(); return -1; }
        if (c == '\n') break;
        if (reply && rpos < reply_sz - 1) reply[rpos] = c;   // store if room; else drop
        rpos++;                                              // keep consuming to end of line
    }
    if (reply) reply[(rpos < reply_sz - 1) ? rpos : (reply_sz - 1)] = 0;
    return 0;
}

// Parse host:port from env DRMON_MAME_ADDR.
static void parse_env_addr(void) {
    const char *env = getenv("DRMON_MAME_ADDR");
    if (!env) return;
    const char *colon = strrchr(env, ':');
    if (colon) {
        int plen = (int)(colon - env);
        if (plen > 0 && plen < (int)sizeof(g_host) - 1) {
            strncpy(g_host, env, plen);
            g_host[plen] = 0;
        }
        g_port = atoi(colon + 1);
    } else {
        strncpy(g_host, env, sizeof(g_host) - 1);
    }
}

// Non-blocking connect attempt. Returns 0 on success.
static int try_connect(void) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port   = htons((unsigned short)g_port);
    if (inet_pton(AF_INET, g_host, &sa.sin_addr) <= 0)
        { close(fd); return -1; }

    // Non-blocking connect with 200 ms timeout
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    int rc = connect(fd, (struct sockaddr*)&sa, sizeof(sa));
    if (rc < 0 && errno != EINPROGRESS) { close(fd); return -1; }

    fd_set wfds; FD_ZERO(&wfds); FD_SET(fd, &wfds);
    struct timeval tv = {0, 200000};
    rc = select(fd + 1, 0, &wfds, 0, &tv);
    if (rc <= 0) { close(fd); return -1; }
    int err = 0; socklen_t len = sizeof(err);
    getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len);
    if (err) { close(fd); return -1; }

    // Restore blocking with 1 s receive timeout
    fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
    struct timeval rtv = {1, 0};
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &rtv, sizeof(rtv));

    g_fd = fd;
    return 0;
}

// Throttled reconnect: attempt at most once every 2 s.
static void maybe_reconnect(void) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    if (ms_elapsed(g_last_connect, now) < 2000) return;
    g_last_connect = now;

    if (try_connect() < 0) return;

    // Handshake
    char reply[128];
    if (mame_cmd("V", reply, sizeof(reply)) < 0) return;
    if (strncmp(reply, "ok drmon-bridge", 15) != 0) { close_conn(); return; }

    // Announce register layout
    char regs_cmd[256];
    int pos = snprintf(regs_cmd, sizeof(regs_cmd), "REGS ");
    for (int i = 0; i < NUMREGS; i++) {
        pos += snprintf(regs_cmd + pos, sizeof(regs_cmd) - pos,
                        "%s%s", regNameArray[i], i < NUMREGS-1 ? "," : "");
    }
    if (mame_cmd(regs_cmd, reply, sizeof(reply)) < 0) return;

    g_dead = 0;
    cache_invalidate();
}

//=============================================================================
// Read cache lookup (fetches from bridge on miss)
//=============================================================================

static const unsigned char *cache_fetch(unsigned long addr) {
    unsigned long base = addr & CACHE_BLOCK_MASK;
    int slot = cache_slot(base);
    if (s_cache[slot].base == base)
        return s_cache[slot].data + (addr - base);

    // Cache miss — fetch from bridge
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "R %lx %x", (unsigned long)base, CACHE_BLOCK_SIZE);
    char reply[CACHE_BLOCK_SIZE * 2 + 4];
    if (mame_cmd(cmd, reply, sizeof(reply)) < 0) return NULL;

    unsigned char *block = s_cache[slot].data;
    s_cache[slot].base = base;
    unsigned int off = 0;
    const char *p = reply;
    while (off < CACHE_BLOCK_SIZE && *p && *(p+1)) {
        char h[3] = { *p, *(p+1), 0 };
        block[off++] = (unsigned char)strtoul(h, NULL, 16);
        p += 2;
    }
    return block + (addr - base);
}

//=============================================================================
// slaveio.hpp contract implementation
//=============================================================================

// --- Init / teardown ---------------------------------------------------------

boolean InitSlaveIO(int /*portBase*/, unsigned short /*slaveBufferSeg*/) {
    parse_env_addr();
    cache_init();
    memset(&g_last_connect, 0, sizeof(g_last_connect));
    memset(&g_last_poll,    0, sizeof(g_last_poll));
    // Attempt initial connection; failure is non-fatal (run disconnected).
    maybe_reconnect();
    return boolean::TRUE;
}

void UnInitSlaveIO(void) {
    close_conn();
}

void GoodByeSlave(void) {
    if (g_fd >= 0) {
        char reply[16];
        mame_cmd("BYE", reply, sizeof(reply));
    }
    close_conn();
}

// --- Version -----------------------------------------------------------------

int SlaveGetVer(void) {
    if (g_fd < 0) return 0;
    char reply[128];
    if (mame_cmd("V", reply, sizeof(reply)) < 0) return 0;
    // reply: "ok drmon-bridge 1 <shortname>"
    return SLAVE_VER_NUMBER;   // pretend correct version when connected
}

// --- Liveness ----------------------------------------------------------------

boolean CheckSlaveAlive(void) {
    return (boolean)(g_dead != 0);  // TRUE = dead (matches slaveio.hpp semantics)
}

// --- HandleSlaveInput: called ~60fps from the main loop ---------------------

void HandleSlaveInput(void) {
    if (g_fd < 0) {
        maybe_reconnect();
        if (g_fd < 0) { PrintMessageStatus(STATUS_SLAVE_DEAD); return; }
    }

    // Throttle: poll at most every 50 ms
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    if (ms_elapsed(g_last_poll, now) < 50) {
        if (!slaveRunning)
            PrintMessageStatus(STATUS_STOPPED);
        else
            PrintMessageStatus(modeUpdate ? STATUS_RUNNING : STATUS_RUNNOUPDATE);
        return;
    }
    g_last_poll = now;

    // Send ? poll
    char reply[64];
    if (mame_cmd("?", reply, sizeof(reply)) < 0) {
        g_dead = 1;
        PrintMessageStatus(STATUS_SLAVE_DEAD);
        return;
    }

    if (strncmp(reply, "running", 7) == 0) {
        slaveRunning = 1;
        PrintMessageStatus(modeUpdate ? STATUS_RUNNING : STATUS_RUNNOUPDATE);
        return;
    }

    // "stopped <pc> <reason>"
    if (strncmp(reply, "stopped", 7) == 0) {
        unsigned long pc_val = 0;
        char reason[32] = "unknown";
        sscanf(reply, "stopped %lx %31s", &pc_val, reason);

        int was_running = slaveRunning;
        slaveRunning = 0;
        modeUpdate = boolean::TRUE;

        if (was_running) {
            // Running → stopped transition: fetch registers, classify bp
            cache_invalidate();
            GetRegsFromSlave(regs);
            if (strcmp(reason, "bp") == 0) {
                ulong pcl = GetReg(REG_INSTRUCTIONPOINTER);
                if (!BreakPointHit(pcl)) {
                    // MAME-internal bp drmon doesn't know about; just stay stopped
                }
            }
        }
        PrintMessageStatus(STATUS_STOPPED);
        return;
    }

    // Unexpected reply — treat as disconnect
    close_conn();
    g_dead = 1;
    PrintMessageStatus(STATUS_SLAVE_DEAD);
}

// --- Register I/O ------------------------------------------------------------

void GetRegsFromSlave(ULONG regs_out[]) {
    if (g_fd < 0) return;
    char reply[512];
    if (mame_cmd("G", reply, sizeof(reply)) < 0) return;

    // Parse space-separated hex values in regNameArray order
    char *p = reply;
    for (int i = 0; i < NUMREGS; i++) {
        while (*p == ' ') p++;
        if (!*p) break;
        char *end;
        unsigned long v = strtoul(p, &end, 16);
        regs_out[i] = (ULONG)(v & regMaskArray[i]);
        p = end;
    }

#ifdef SNES
    // Ensure PCL reflects PB:PC composite (bridge returns GENPC for PCL slot)
    LoadPCL();
#endif
}

void PutRegsToSlave(ULONG regs_in[]) {
    if (g_fd < 0) return;
    char cmd[512];
    int pos = snprintf(cmd, sizeof(cmd), "P");
    for (int i = 0; i < NUMREGS; i++)
        pos += snprintf(cmd + pos, sizeof(cmd) - pos, " %lx", (unsigned long)regs_in[i]);
    char reply[16];
    mame_cmd(cmd, reply, sizeof(reply));
    cache_invalidate();
}

// --- Memory I/O --------------------------------------------------------------

void ReadSlaveData(unsigned long addr, char *data, unsigned int len) {
    if (g_fd < 0) { memset(data, 0, len); return; }
    for (unsigned int i = 0; i < len; i++) {
        const unsigned char *p = cache_fetch(addr + i);
        data[i] = p ? (char)*p : 0;
    }
}

void WriteSlaveData(unsigned long addr, char *data, unsigned int len) {
    if (g_fd < 0) return;
    // Build W command with hex payload
    char cmd[8 + 1 + 4096*2 + 1];   // "W <addr> <hexbytes>"
    int pos = snprintf(cmd, sizeof(cmd), "W %lx ", (unsigned long)addr);
    for (unsigned int i = 0; i < len && pos < (int)sizeof(cmd) - 2; i++)
        pos += snprintf(cmd + pos, sizeof(cmd) - pos, "%02x",
                        (unsigned char)data[i]);
    char reply[16];
    mame_cmd(cmd, reply, sizeof(reply));
    cache_invalidate();

    // ROM-write warning: MAME silently drops writes to ROM-mapped addresses.
    // For interactive datum-sized writes, read the first byte back; if it didn't
    // take, the target is ROM/unmapped — surface a message instead of failing
    // silently.  Skipped for bulk writes/loads (len > 4) to avoid an extra
    // round-trip and message spam.  (Write-only I/O regs may read back differently
    // and trip a benign false warning — acceptable; data writes target RAM/ROM.)
    if (len > 0 && len <= 4) {
        const unsigned char *p = cache_fetch(addr);
        if (p && *p != (unsigned char)data[0]) {
            char warn[64];
            snprintf(warn, sizeof(warn),
                     "ROM write @ %06lX ignored (MAME ROM is read-only)",
                     (unsigned long)addr);
            PrintWarning(warn);
        }
    }
}

unsigned long MemRead(unsigned long addr, UBYTE size) {
    if (g_fd < 0) return 0;
    // Use cache for small reads
    unsigned long val = 0;
    for (UBYTE i = 0; i < size; i++) {
        const unsigned char *p = cache_fetch(addr + i);
        if (!p) return 0;
#ifdef GENESIS
        val = (val << 8) | *p;   // big-endian
#else
        val |= ((unsigned long)*p) << (i * 8);  // little-endian
#endif
    }
    return val;
}

// WriteSlaveDatum is defined in command.cpp; it calls WriteSlaveData() above
// after applying WORDSWAP byte-ordering.  Do not redefine here.

// Load file from disk to slave RAM
errorcode LoadFileToSlave(FILE *f, unsigned long address, unsigned long len) {
    if (g_fd < 0) return NOERR;
    const int CHUNK = 256;
    char buf[CHUNK];
    unsigned long remaining = len;
    unsigned long base = address;
    while (remaining > 0) {
        int n = (int)(remaining < CHUNK ? remaining : CHUNK);
        if ((int)fread(buf, 1, n, f) != n) break;
        WriteSlaveData(base, buf, n);
        base += n; remaining -= n;
    }
    return NOERR;
}

// Save slave RAM to disk
errorcode SaveFileFromSlave(FILE *f, unsigned long address, unsigned long len) {
    if (g_fd < 0) return NOERR;
    const int CHUNK = 256;
    char buf[CHUNK];
    unsigned long remaining = len;
    unsigned long base = address;
    while (remaining > 0) {
        int n = (int)(remaining < CHUNK ? remaining : CHUNK);
        ReadSlaveData(base, buf, n);
        if ((int)fwrite(buf, 1, n, f) != n) break;
        base += n; remaining -= n;
    }
    return NOERR;
}

// Copy / fill — implemented via bridge read/write
boolean SlaveCopyMem(long startaddr, long destaddr, long len) {
    if (g_fd < 0) return boolean::FALSE;
    char buf[256];
    long remaining = len;
    long src = startaddr, dst = destaddr;
    while (remaining > 0) {
        int n = (int)(remaining < 256 ? remaining : 256);
        ReadSlaveData((unsigned long)src, buf, n);
        WriteSlaveData((unsigned long)dst, buf, n);
        src += n; dst += n; remaining -= n;
    }
    return boolean::TRUE;
}

boolean SlaveFillMem(long startaddr, long len, long patlen, char *pattern) {
    if (g_fd < 0) return boolean::FALSE;
    char buf[256];
    long remaining = len, dst = startaddr;
    while (remaining > 0) {
        int n = (int)(remaining < 256 ? remaining : 256);
        for (int i = 0; i < n; i++)
            buf[i] = pattern[i % patlen];
        WriteSlaveData((unsigned long)dst, buf, n);
        dst += n; remaining -= n;
    }
    return boolean::TRUE;
}

// --- Execution control -------------------------------------------------------

void SlaveRun(void) {
    if (g_fd < 0) return;
    char reply[16];
    mame_cmd("C", reply, sizeof(reply));
    slaveRunning = 1;
    cache_invalidate();
}

void SlaveStop(void) {
    if (g_fd < 0) return;
    char reply[64];
    mame_cmd("H", reply, sizeof(reply));
    // reply: "stopped <pc> halt" — parse PC
    if (strncmp(reply, "stopped", 7) == 0) {
        unsigned long pc_val = 0;
        sscanf(reply, "stopped %lx", &pc_val);
        slaveRunning = 0;
        modeUpdate = boolean::TRUE;
        cache_invalidate();
        GetRegsFromSlave(regs);
    }
}

boolean SingleStep(void) {
    if (g_fd < 0) return boolean::FALSE;

    ulong PC = GetReg(REG_INSTRUCTIONPOINTER);

    unsigned char xb[4] = {0, 0, 0, 0};
    ReadSlaveData(PC, (char *)xb, 4);
    char txt[40] = {0};
    int ilen = Disassem(PC, (char *)xb, (char *)txt, 0);

    ulong next = PC + (ulong)ilen;
    ulong alt  = 0;

    switch (xb[0]) {
    // Conditional branches: set bps at both fall-through and taken target.
    case 0x10: case 0x30: case 0x50: case 0x70:
    case 0x90: case 0xB0: case 0xD0: case 0xF0:
        alt = next + (ulong)(long)(signed char)xb[1];
        break;
    // BRA (always taken, 8-bit signed offset)
    case 0x80:
        next = next + (ulong)(long)(signed char)xb[1];
        break;
    // BRL (always taken, 16-bit signed offset)
    case 0x82:
        next = next + (ulong)(long)(short)(xb[1] | ((unsigned)xb[2] << 8));
        break;
    // JSR/JMP absolute (same bank)
    case 0x20: case 0x4C:
        next = (PC & ~0xFFFFUL) | xb[1] | ((ulong)xb[2] << 8);
        break;
    // JMP (a) — read target from memory
    case 0x6C: {
        ulong a = xb[1] | ((ulong)xb[2] << 8) | (PC & 0x00FF0000UL);
        unsigned char t[2] = {0};
        ReadSlaveData(a, (char *)t, 2);
        next = (PC & ~0xFFFFUL) | t[0] | ((ulong)t[1] << 8);
        break; }
    // JMP/JSR (a,x) — read target from memory + X
    case 0x7C: case 0xFC: {
        ulong a = xb[1] | ((ulong)xb[2] << 8) | (PC & 0x00FF0000UL);
        unsigned char t[2] = {0};
        ReadSlaveData(a + GetReg(REG_X), (char *)t, 2);
        next = (PC & ~0xFFFFUL) | t[0] | ((ulong)t[1] << 8);
        break; }
    // RTS — read return address from stack
    case 0x60: {
        ulong SP = GetReg(REG_STACKPOINTER);
        unsigned char t[2] = {0};
        ReadSlaveData(SP + 1, (char *)t, 2);
        next = (PC & ~0xFFFFUL) | (((ulong)(t[0] | ((ulong)t[1] << 8)) + 1) & 0xFFFFUL);
        break; }
    // JSL al / JML al — 24-bit absolute
    case 0x22: case 0x5C:
        next = xb[1] | ((ulong)xb[2] << 8) | ((ulong)xb[3] << 16);
        break;
    // RTL — read 24-bit return address from stack
    case 0x6B: {
        ulong SP = GetReg(REG_STACKPOINTER);
        unsigned char t[3] = {0};
        ReadSlaveData(SP + 1, (char *)t, 3);
        next = ((t[0] | ((ulong)t[1] << 8) | ((ulong)t[2] << 16)) + 1) & 0xFFFFFFUL;
        break; }
    // JML (a) — read 24-bit target from memory
    case 0xDC: {
        ulong a = xb[1] | ((ulong)xb[2] << 8);
        unsigned char t[3] = {0};
        ReadSlaveData(a, (char *)t, 3);
        next = t[0] | ((ulong)t[1] << 8) | ((ulong)t[2] << 16);
        break; }
    // RTI — read return PC from stack (skip flags byte)
    case 0x40: {
        ulong SP = GetReg(REG_STACKPOINTER);
        unsigned char t[2] = {0};
        ReadSlaveData(SP + 2, (char *)t, 2);
        next = (PC & ~0xFFFFUL) | (t[0] | ((ulong)t[1] << 8));
        break; }
    default: break;
    }

    char cmd[48];
    if (alt != 0)
        snprintf(cmd, sizeof(cmd), "S %lx %lx", (unsigned long)next, (unsigned long)alt);
    else
        snprintf(cmd, sizeof(cmd), "S %lx", (unsigned long)next);

    char reply[64];
    if (mame_cmd(cmd, reply, sizeof(reply)) < 0) return boolean::FALSE;
    slaveRunning = 0;
    modeUpdate = boolean::TRUE;
    cache_invalidate();
    GetRegsFromSlave(regs);
    return boolean::TRUE;
}

boolean StepOverBreak(long /*addr*/, uword /*inst*/) {
    // Native MAME breakpoints; no BRK patching needed.
    return SingleStep();
}

void SlaveReset(void) {
    // No-op: hardware-level reset not directly exposed via the bridge.
}

void SlaveRestart(void) {
    if (g_fd < 0) return;
    char reply[16];
    mame_cmd("RESET", reply, sizeof(reply));
    cache_invalidate();
}

// --- Breakpoints -------------------------------------------------------------

uword SlaveSetBkPt(ulong addr) {
    if (g_fd < 0) return 0;
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "B+ %lx", (unsigned long)addr);
    char reply[16];
    mame_cmd(cmd, reply, sizeof(reply));
    // Return 0: drmon stores the bp in its own list; bridge tracks by addr
    return 0;
}

boolean SlaveBkClr(long addr, unsigned int /*inst*/) {
    if (g_fd < 0) return boolean::FALSE;
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "B- %lx", (unsigned long)addr);
    char reply[16];
    mame_cmd(cmd, reply, sizeof(reply));
    return boolean::TRUE;
}

// Write-protect / break-on-write map to a MAME write watchpoint over the ROM
// window (bridge commands WP+/WP-/BW+/BW-).  MAME watchpoints *break* on write —
// they cannot silently *block* one — so both manifest as a halt on write.
static void wp_cmd(const char *c) {
    if (g_fd < 0) return;
    char reply[16];
    mame_cmd(c, reply, sizeof(reply));
}
void SlaveSetWriteProtect(void)   { wp_cmd("WP+"); }
void SlaveClearWriteProtect(void) { wp_cmd("WP-"); }
void SlaveSetBRKOnWrite(void)     { wp_cmd("BW+"); }
void SlaveClearBRKOnWrite(void)   { wp_cmd("BW-"); }

// --- Platform-specific stubs (PPU/VDP/etc.) ----------------------------------

#ifdef SNES
// PPU VRAM read for the MTYPE_PPU memory window.  Sources bytes from MAME's
// snes_ppu m_vram save-state item via the bridge's `RP` command (non-intrusive —
// no PPU port latch side effects).  addr is masked to 0xffff (VRAM) by memory.cpp.
// Not cached: VRAM updates continuously while the CPU runs, so a per-call read is
// both simpler and correct (the user inspects PPU windows while halted).
void ReadSlavePPU(unsigned long addr, char *data, unsigned int len) {
    if (g_fd < 0) { memset(data, 0, len); return; }
    unsigned int done = 0;
    while (done < len) {
        unsigned int chunk = len - done;
        if (chunk > CACHE_BLOCK_SIZE) chunk = CACHE_BLOCK_SIZE;
        char cmd[64];
        snprintf(cmd, sizeof(cmd), "RP %lx %x", (unsigned long)(addr + done), chunk);
        char reply[CACHE_BLOCK_SIZE * 2 + 4];
        if (mame_cmd(cmd, reply, sizeof(reply)) < 0) {
            memset(data + done, 0, len - done);
            return;
        }
        unsigned int off = 0;
        const char *p = reply;
        while (off < chunk && *p && *(p + 1)) {
            char h[3] = { *p, *(p + 1), 0 };
            data[done + off] = (char)strtoul(h, NULL, 16);
            p += 2;
            off++;
        }
        for (; off < chunk; off++) data[done + off] = 0;   // short reply → zero-fill
        done += chunk;
    }
}
#endif

#ifdef SPC700
// SPC700 (SNES audio co-CPU) access via the bridge's RA/GA/PA commands.

// APU RAM read for the MTYPE_SPC memory window — mirror of ReadSlavePPU (RA, not RP).
// Reads the :soundcpu program space (64K); addr masked to 0xffff by memory.cpp.  Live, uncached.
void ReadSlaveApuRam(unsigned long addr, char *data, unsigned int len) {
    if (g_fd < 0) { memset(data, 0, len); return; }
    unsigned int done = 0;
    while (done < len) {
        unsigned int chunk = len - done;
        if (chunk > CACHE_BLOCK_SIZE) chunk = CACHE_BLOCK_SIZE;
        char cmd[64];
        snprintf(cmd, sizeof(cmd), "RA %lx %x", (unsigned long)(addr + done), chunk);
        char reply[CACHE_BLOCK_SIZE * 2 + 4];
        if (mame_cmd(cmd, reply, sizeof(reply)) < 0) {
            memset(data + done, 0, len - done);
            return;
        }
        unsigned int off = 0;
        const char *p = reply;
        while (off < chunk && *p && *(p + 1)) {
            char h[3] = { *p, *(p + 1), 0 };
            data[done + off] = (char)strtoul(h, NULL, 16);
            p += 2;
            off++;
        }
        for (; off < chunk; off++) data[done + off] = 0;
        done += chunk;
    }
}

// SPC700 registers, fixed order PC A X Y SP PSW (6 slots) via GA/PA.
void GetSpc700Regs(ULONG regs_out[]) {
    if (g_fd < 0) return;
    char reply[128];
    if (mame_cmd("GA", reply, sizeof(reply)) < 0) return;
    char *p = reply;
    for (int i = 0; i < 6; i++) {
        while (*p == ' ') p++;
        if (!*p) break;
        char *end;
        regs_out[i] = (ULONG)strtoul(p, &end, 16);
        p = end;
    }
}

void PutSpc700Regs(ULONG regs_in[]) {
    if (g_fd < 0) return;
    char cmd[128];
    int pos = snprintf(cmd, sizeof(cmd), "PA");
    for (int i = 0; i < 6; i++)
        pos += snprintf(cmd + pos, sizeof(cmd) - pos, " %lx", (unsigned long)regs_in[i]);
    char reply[16];
    mame_cmd(cmd, reply, sizeof(reply));
}
#endif

#ifdef GENESIS
void ReadSlaveVDP(unsigned long /*addr*/, char *data, unsigned int len) {
    memset(data, 0, len);
}
void WriteSlaveVDP(unsigned long /*addr*/, char */*data*/, unsigned int /*len*/) {}
void ReadSlaveCRAM(char *data)  { memset(data, 0, 128); }
void WriteSlaveCRAM(char */*data*/) {}
void ReadSlaveVSRAM(char *data) { memset(data, 0, 80); }
void WriteSlaveVSRAM(char */*data*/) {}
#endif

// --- Residual asm-export symbols (still needed by board.cpp) -----------------

extern "C" {

unsigned int SwapWord(unsigned int a) {
    return (unsigned int)(((a & 0xFF) << 8) | ((a >> 8) & 0xFF));
}

unsigned long SwapLong(unsigned long a) {
    return ((a >> 24) & 0xFFUL) | ((a >> 8) & 0xFF00UL)
         | ((a << 8) & 0xFF0000UL) | ((a << 24) & 0xFF000000UL);
}

unsigned long SwapLongWords(unsigned long a) { return a; }

int  GetAttention(void)   { return g_dead; }
void GetSlaveBus(void)    {}
void ReturnSlaveBus(void) {}
void SendCmd(char)        {}

} // extern "C"

//=============================================================================
