//=============================================================================
// sliogdb.cpp: slaveio.hpp contract implementation for MAME's GDB RSP stub.
// Included by which.cpp under the MAMEBACKEND + GENESIS arm.
//
// Architecture: drmon (this file) ↔ TCP 127.0.0.1:23946 ↔
//               MAME -debugger gdbstub (no Lua bridge needed for Genesis).
//
// GDB RSP register layout confirmed by spike (MAME 0.277, m68k gdbstub):
//   D0-D7 (4B BE) + A0-A7 (4B BE) + SR (2B BE) + PC (4B BE) = 70 bytes
//   Stop reply: T05 0e:<A6>; 0f:<A7>; 11:<PC>;
//
// MAME address override: DRMON_GDB_ADDR=host:port env variable (default 23946).
// Read cache: 8 × 1 KB aligned blocks; invalidated on any mutating operation.
//=============================================================================

#if !defined(__linux__) && !defined(__GNUC__)
#error "sliogdb.cpp is Linux/GCC only"
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

//=============================================================================
// GDB RSP g-packet layout (MAME m68k gdbstub)
//=============================================================================

// Byte offsets within the 70-byte register dump:
//   D0-D7  : bytes  0-31  (8 × 4B big-endian)
//   A0-A7  : bytes 32-63  (8 × 4B big-endian)
//   SR     : bytes 64-65  (2B big-endian) — 16-bit only!
//   PC     : bytes 66-69  (4B big-endian)
#define GREG_BYTES      70
#define GREG_HEXLEN    140

// drmon register indices (from reg.hpp GENESIS enum):
//   0-7: D0-D7, 8-14: A0-A6, 15: A7/SSP, 16: USP, 17: SR, 18: PC
#define GIDX_USP    16
#define GIDX_SR     17
#define GIDX_PC     18

//=============================================================================
// Globals expected by board.cpp and the legacy slio TUs
//=============================================================================

boolean bConvertUserBreaks = boolean::FALSE;

unsigned int  slaveCtrlc   = 0;
unsigned int  slaveBankc   = 0;
unsigned int  slaveWormc   = 0;
unsigned char *slaveBufferCmdc = 0;

int SlaveRunning(void) { return slaveRunning; }

//=============================================================================
// Internal state
//=============================================================================

static int   g_fd           = -1;
static char  g_host[64]     = "127.0.0.1";
static int   g_port         = 23946;
static int   g_dead         = 1;
static int   g_gdb_running  = 0;   // 1 when c/s sent, waiting for stop

static struct timespec g_last_connect = {0, 0};

// Receive ring buffer for RSP packet assembly
static char  g_rx[8192];
static int   g_rx_len       = 0;

//=============================================================================
// Read cache: 8 × 1 KB aligned blocks
//=============================================================================

#define CACHE_BLOCKS       8
#define CACHE_BLOCK_SIZE   1024u
#define CACHE_BLOCK_MASK   (~(unsigned long)(CACHE_BLOCK_SIZE - 1u))

struct GdbCacheEntry {
    unsigned long base;
    unsigned char data[CACHE_BLOCK_SIZE];
};
static GdbCacheEntry s_gcache[CACHE_BLOCKS];

static void gcache_init(void) {
    for (int i = 0; i < CACHE_BLOCKS; i++) s_gcache[i].base = ~0UL;
}
static void gcache_invalidate(void) { gcache_init(); }
static int gcache_slot(unsigned long base) {
    return (int)((base >> 10) % CACHE_BLOCKS);
}

//=============================================================================
// RSP helpers
//=============================================================================

static void gdb_close(void) {
    if (g_fd >= 0) { close(g_fd); g_fd = -1; }
    g_dead = 1;
    g_gdb_running = 0;
    gcache_invalidate();
}

static int rsp_csum(const char *data, int len) {
    unsigned int c = 0;
    for (int i = 0; i < len; i++) c += (unsigned char)data[i];
    return (int)(c & 0xFF);
}

// Send an RSP packet: $data#XX.  Returns 0 on success.
static int rsp_send(const char *data) {
    if (g_fd < 0) return -1;
    int dlen = (int)strlen(data);
    char pkt[4096 + 8];
    int plen = snprintf(pkt, sizeof(pkt), "$%s#%02x", data, rsp_csum(data, dlen));
    if (plen <= 0 || plen >= (int)sizeof(pkt)) return -1;
    if (send(g_fd, pkt, plen, MSG_NOSIGNAL) != plen) { gdb_close(); return -1; }
    // Drain ack byte(s)
    struct timeval tv = {1, 0};
    for (;;) {
        fd_set fds; FD_ZERO(&fds); FD_SET(g_fd, &fds);
        int rc = select(g_fd + 1, &fds, 0, 0, &tv);
        if (rc <= 0) return 0;  // timeout = no ack, not fatal
        char ack;
        if (recv(g_fd, &ack, 1, 0) <= 0) { gdb_close(); return -1; }
        if (ack == '+' || ack == '-') break;
        // Unexpected byte: buffer it for later packet assembly
        if (g_rx_len < (int)sizeof(g_rx) - 1) g_rx[g_rx_len++] = ack;
    }
    return 0;
}

// Fill receive buffer from socket, non-blocking if timeout_ms == 0.
static void rsp_fill(int timeout_ms) {
    if (g_fd < 0) return;
    struct timeval tv;
    tv.tv_sec  = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    fd_set fds; FD_ZERO(&fds); FD_SET(g_fd, &fds);
    int rc = select(g_fd + 1, &fds, 0, 0, &tv);
    if (rc <= 0) return;
    int space = (int)sizeof(g_rx) - g_rx_len - 1;
    if (space <= 0) return;
    int n = (int)recv(g_fd, g_rx + g_rx_len, space, 0);
    if (n > 0) g_rx_len += n;
    else if (n == 0 || (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK))
        gdb_close();
}

// Try to extract a complete $..#xx packet from g_rx[].
// Sends '+' ack and returns 1 (packet in out[]).  Returns 0 if incomplete.
static int rsp_extract(char *out, int out_sz) {
    // Skip leading ack bytes and noise before '$'
    int si = 0;
    while (si < g_rx_len && g_rx[si] != '$') si++;
    if (si > 0) { memmove(g_rx, g_rx + si, g_rx_len - si); g_rx_len -= si; }
    if (g_rx_len < 4) return 0;     // need at least $#XX

    int hash = -1;
    for (int i = 1; i < g_rx_len - 2; i++) {
        if (g_rx[i] == '#') { hash = i; break; }
    }
    if (hash < 0 || g_rx_len < hash + 3) return 0;

    int dlen = hash - 1;
    if (dlen >= out_sz) dlen = out_sz - 1;
    memcpy(out, g_rx + 1, dlen);
    out[dlen] = 0;

    // Consume from buffer
    int consumed = hash + 3;
    memmove(g_rx, g_rx + consumed, g_rx_len - consumed);
    g_rx_len -= consumed;

    // Ack
    send(g_fd, "+", 1, MSG_NOSIGNAL);
    return 1;
}

// Send an RSP packet and wait for reply.  Returns 0 on success.
static int rsp_cmd(const char *data, char *reply, int reply_sz, int timeout_ms) {
    if (rsp_send(data) < 0) return -1;
    int elapsed = 0;
    int step = 20;
    while (elapsed < timeout_ms) {
        rsp_fill(step);
        if (rsp_extract(reply, reply_sz)) return 0;
        elapsed += step;
    }
    return -1;
}

// Non-blocking check: did a stop reply arrive?  Returns 1 if yes.
static int rsp_try_recv_stop(char *out, int out_sz) {
    rsp_fill(0);
    return rsp_extract(out, out_sz);
}

// Send RSP packet with no wait for reply (for c / s — reply is deferred).
static void rsp_send_nowait(const char *data) {
    rsp_send(data);  // ack drain is part of rsp_send
}

//=============================================================================
// Network connect
//=============================================================================

static void parse_env_addr_gdb(void) {
    const char *env = getenv("DRMON_GDB_ADDR");
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

static long gdb_ms_elapsed(const struct timespec &a, const struct timespec &b) {
    return (long)((b.tv_sec - a.tv_sec) * 1000L +
                  (b.tv_nsec - a.tv_nsec) / 1000000L);
}

static int gdb_try_connect(void) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port   = htons((unsigned short)g_port);
    if (inet_pton(AF_INET, g_host, &sa.sin_addr) <= 0) { close(fd); return -1; }

    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    int rc = connect(fd, (struct sockaddr*)&sa, sizeof(sa));
    if (rc < 0 && errno != EINPROGRESS) { close(fd); return -1; }

    fd_set wfds; FD_ZERO(&wfds); FD_SET(fd, &wfds);
    struct timeval tv = {0, 200000};
    rc = select(fd + 1, 0, &wfds, 0, &tv);
    if (rc <= 0) { close(fd); return -1; }
    int err = 0; socklen_t elen = sizeof(err);
    getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &elen);
    if (err) { close(fd); return -1; }

    fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
    g_fd = fd;
    g_rx_len = 0;
    return 0;
}

static void gdb_maybe_reconnect(void) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    if (gdb_ms_elapsed(g_last_connect, now) < 2000) return;
    g_last_connect = now;

    if (gdb_try_connect() < 0) return;

    // Drain any initial stop notification (MAME sends T05 on connect under -debug)
    char buf[512];
    rsp_fill(300);
    rsp_extract(buf, sizeof(buf));

    // qSupported negotiation
    char reply[512];
    if (rsp_cmd("qSupported:xmlRegisters+;swbreak+", reply, sizeof(reply), 2000) < 0) {
        gdb_close(); return;
    }

    // Fetch target description — MAME 0.277 gdbstub requires this before g works.
    // Without it, g always returns E01 (register layout uninitialized on the MAME side).
    char xml_buf[4096];
    rsp_cmd("qXfer:features:read:target.xml:0,4000", xml_buf, sizeof(xml_buf), 2000);
    rsp_cmd("Hg0", reply, sizeof(reply), 2000);

    g_dead = 0;
    g_gdb_running = 0;
    gcache_invalidate();
}

//=============================================================================
// g-packet helpers
//=============================================================================

static unsigned long be32_from_hex(const char *h) {
    unsigned long v = 0;
    for (int i = 0; i < 8; i++) {
        int c = h[i];
        int d = (c>='0'&&c<='9') ? c-'0' : (c>='a'&&c<='f') ? c-'a'+10 :
                (c>='A'&&c<='F') ? c-'A'+10 : 0;
        v = (v << 4) | (unsigned long)d;
    }
    return v;
}

static unsigned long be16_from_hex(const char *h) {
    unsigned long v = 0;
    for (int i = 0; i < 4; i++) {
        int c = h[i];
        int d = (c>='0'&&c<='9') ? c-'0' : (c>='a'&&c<='f') ? c-'a'+10 :
                (c>='A'&&c<='F') ? c-'A'+10 : 0;
        v = (v << 4) | (unsigned long)d;
    }
    return v;
}

// Parse the 140-char g-packet hex string into drmon's regs_out[NUMREGS].
// drmon GENESIS order: D0-D7 (0-7), A0-A6 (8-14), A7 (15), USP (16), SR (17), PC (18)
// g-packet order:      D0-D7 (0-7), A0-A7 (8-15), SR (16, 2B), PC (17, 4B)
static void parse_g_packet(const char *hex, ULONG regs_out[]) {
    if ((int)strlen(hex) < GREG_HEXLEN) return;
    // D0-D7: hex[0..63], 8 hex chars each
    for (int i = 0; i < 8; i++)
        regs_out[i] = (ULONG)be32_from_hex(hex + i * 8);
    // A0-A7: hex[64..127], 8 hex chars each; drmon A0-A6 = idx 8-14, A7 = idx 15
    for (int i = 0; i < 8; i++)
        regs_out[8 + i] = (ULONG)be32_from_hex(hex + 64 + i * 8);
    // USP: not in g-packet
    regs_out[GIDX_USP] = 0;
    // SR: hex[128..131], 4 hex chars (16-bit)
    regs_out[GIDX_SR] = (ULONG)be16_from_hex(hex + 128);
    // PC: hex[132..139], 8 hex chars
    regs_out[GIDX_PC] = (ULONG)be32_from_hex(hex + 132);
}

// Build the 140-char G-packet from drmon's regs_in[].
static void build_g_packet(const ULONG regs_in[], char *out) {
    char *p = out;
    // D0-D7
    for (int i = 0; i < 8; i++) { sprintf(p, "%08lx", (unsigned long)regs_in[i]); p += 8; }
    // A0-A7
    for (int i = 0; i < 8; i++) { sprintf(p, "%08lx", (unsigned long)regs_in[8+i]); p += 8; }
    // SR (16-bit, zero-pad to 4 hex chars)
    sprintf(p, "%04lx", (unsigned long)(regs_in[GIDX_SR] & 0xFFFF)); p += 4;
    // PC
    sprintf(p, "%08lx", (unsigned long)regs_in[GIDX_PC]); p += 8;
    *p = 0;
}

//=============================================================================
// Read cache
//=============================================================================

static const unsigned char *gcache_fetch(unsigned long addr) {
    unsigned long base = addr & CACHE_BLOCK_MASK;
    int slot = gcache_slot(base);
    if (s_gcache[slot].base == base)
        return s_gcache[slot].data + (addr - base);

    // Fetch via RSP m packet
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "m%lx,%x", (unsigned long)base, CACHE_BLOCK_SIZE);
    char reply[CACHE_BLOCK_SIZE * 2 + 4];
    if (rsp_cmd(cmd, reply, sizeof(reply), 2000) < 0) return NULL;

    unsigned char *block = s_gcache[slot].data;
    s_gcache[slot].base = base;
    unsigned int off = 0;
    const char *rp = reply;
    while (off < CACHE_BLOCK_SIZE && *rp && *(rp+1)) {
        char h[3] = { *rp, *(rp+1), 0 };
        block[off++] = (unsigned char)strtoul(h, NULL, 16);
        rp += 2;
    }
    return block + (addr - base);
}

//=============================================================================
// slaveio.hpp contract
//=============================================================================

boolean InitSlaveIO(int /*portBase*/, unsigned short /*slaveBufferSeg*/) {
    parse_env_addr_gdb();
    gcache_init();
    memset(&g_last_connect, 0, sizeof(g_last_connect));
    gdb_maybe_reconnect();
    return boolean::TRUE;
}

void UnInitSlaveIO(void) { gdb_close(); }

void GoodByeSlave(void) {
    // GDB RSP has no explicit disconnect; just close the socket.
    gdb_close();
}

int SlaveGetVer(void) {
    return (g_fd >= 0) ? SLAVE_VER_NUMBER : 0;
}

boolean CheckSlaveAlive(void) {
    return (boolean)(g_dead != 0);
}

// HandleSlaveInput: ~60 fps main-loop poll
void HandleSlaveInput(void) {
    if (g_fd < 0) {
        gdb_maybe_reconnect();
        if (g_fd < 0) { PrintMessageStatus(STATUS_SLAVE_DEAD); return; }
    }

    if (!g_gdb_running) {
        PrintMessageStatus(STATUS_STOPPED);
        return;
    }

    // Non-blocking check for stop notification from MAME
    char stop[512];
    if (!rsp_try_recv_stop(stop, sizeof(stop))) {
        PrintMessageStatus(modeUpdate ? STATUS_RUNNING : STATUS_RUNNOUPDATE);
        return;
    }

    // Machine stopped (T05 or S05 reply)
    g_gdb_running = 0;
    slaveRunning  = 0;
    modeUpdate    = boolean::TRUE;
    gcache_invalidate();
    GetRegsFromSlave(regs);

    ulong pcl = GetReg(REG_INSTRUCTIONPOINTER);
    BreakPointHit(pcl);
    PrintMessageStatus(STATUS_STOPPED);
}

// --- Register I/O ------------------------------------------------------------

void GetRegsFromSlave(ULONG regs_out[]) {
    if (g_fd < 0) return;
    char reply[GREG_HEXLEN + 4];
    if (rsp_cmd("g", reply, sizeof(reply), 2000) < 0) return;
    parse_g_packet(reply, regs_out);
}

void PutRegsToSlave(ULONG regs_in[]) {
    if (g_fd < 0) return;
    char gpkt[GREG_HEXLEN + 4];
    build_g_packet(regs_in, gpkt);
    char cmd[GREG_HEXLEN + 4];
    snprintf(cmd, sizeof(cmd), "G%s", gpkt);
    char reply[16];
    rsp_cmd(cmd, reply, sizeof(reply), 2000);
    gcache_invalidate();
}

// --- Memory I/O --------------------------------------------------------------

void ReadSlaveData(unsigned long addr, char *data, unsigned int len) {
    if (g_fd < 0) { memset(data, 0, len); return; }
    for (unsigned int i = 0; i < len; i++) {
        const unsigned char *p = gcache_fetch(addr + i);
        data[i] = p ? (char)*p : 0;
    }
}

void WriteSlaveData(unsigned long addr, char *data, unsigned int len) {
    if (g_fd < 0) return;
    char cmd[8 + 1 + 4096*2 + 8];
    int pos = snprintf(cmd, sizeof(cmd), "M%lx,%x:", (unsigned long)addr, len);
    for (unsigned int i = 0; i < len && pos < (int)sizeof(cmd) - 2; i++)
        pos += snprintf(cmd + pos, sizeof(cmd) - pos, "%02x", (unsigned char)data[i]);
    char reply[16];
    rsp_cmd(cmd, reply, sizeof(reply), 2000);
    gcache_invalidate();
}

unsigned long MemRead(unsigned long addr, UBYTE size) {
    if (g_fd < 0) return 0;
    unsigned long val = 0;
    for (UBYTE i = 0; i < size; i++) {
        const unsigned char *p = gcache_fetch(addr + i);
        if (!p) return 0;
        val = (val << 8) | *p;   // big-endian (m68k)
    }
    return val;
}

errorcode LoadFileToSlave(FILE *f, unsigned long address, unsigned long len) {
    if (g_fd < 0) return NOERR;
    const int CHUNK = 256;
    char buf[CHUNK];
    unsigned long remaining = len, base = address;
    while (remaining > 0) {
        int n = (int)(remaining < CHUNK ? remaining : CHUNK);
        if ((int)fread(buf, 1, n, f) != n) break;
        WriteSlaveData(base, buf, n);
        base += n; remaining -= n;
    }
    return NOERR;
}

errorcode SaveFileFromSlave(FILE *f, unsigned long address, unsigned long len) {
    if (g_fd < 0) return NOERR;
    const int CHUNK = 256;
    char buf[CHUNK];
    unsigned long remaining = len, base = address;
    while (remaining > 0) {
        int n = (int)(remaining < CHUNK ? remaining : CHUNK);
        ReadSlaveData(base, buf, n);
        if ((int)fwrite(buf, 1, n, f) != n) break;
        base += n; remaining -= n;
    }
    return NOERR;
}

boolean SlaveCopyMem(long startaddr, long destaddr, long len) {
    if (g_fd < 0) return boolean::FALSE;
    char buf[256];
    long remaining = len, src = startaddr, dst = destaddr;
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
        for (int i = 0; i < n; i++) buf[i] = pattern[i % patlen];
        WriteSlaveData((unsigned long)dst, buf, n);
        dst += n; remaining -= n;
    }
    return boolean::TRUE;
}

// --- Execution control -------------------------------------------------------

void SlaveRun(void) {
    if (g_fd < 0) return;
    rsp_send_nowait("c");
    slaveRunning  = 1;
    g_gdb_running = 1;
    gcache_invalidate();
}

void SlaveStop(void) {
    if (g_fd < 0) return;
    // Break-in: raw 0x03 byte (not wrapped in a packet)
    send(g_fd, "\x03", 1, MSG_NOSIGNAL);
    // Block for stop reply (up to 3 s)
    char stop[512];
    int got = 0;
    for (int ms = 0; ms < 3000 && !got; ms += 50) {
        rsp_fill(50);
        got = rsp_extract(stop, sizeof(stop));
    }
    g_gdb_running = 0;
    slaveRunning  = 0;
    modeUpdate    = boolean::TRUE;
    gcache_invalidate();
    GetRegsFromSlave(regs);
}

boolean SingleStep(void) {
    if (g_fd < 0) return boolean::FALSE;
    // s: single step — MAME executes exactly one instruction and returns T05
    rsp_send_nowait("s");
    char stop[512];
    int got = 0;
    for (int ms = 0; ms < 3000 && !got; ms += 50) {
        rsp_fill(50);
        got = rsp_extract(stop, sizeof(stop));
    }
    if (!got) return boolean::FALSE;
    slaveRunning  = 0;
    modeUpdate    = boolean::TRUE;
    gcache_invalidate();
    GetRegsFromSlave(regs);
    return boolean::TRUE;
}

boolean StepOverBreak(long /*addr*/, uword /*inst*/) {
    return SingleStep();
}

void SlaveReset(void) {}

void SlaveRestart(void) {
    // GDB RSP R0/vRun are not supported by MAME's gdbstub (confirmed by spike).
    // No-op; user must restart MAME manually.
}

// --- Breakpoints -------------------------------------------------------------

uword SlaveSetBkPt(ulong addr) {
    if (g_fd < 0) return 0;
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "Z0,%lx,2", (unsigned long)addr);
    char reply[16];
    rsp_cmd(cmd, reply, sizeof(reply), 2000);
    return 0;
}

boolean SlaveBkClr(long addr, unsigned int /*inst*/) {
    if (g_fd < 0) return boolean::FALSE;
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "z0,%lx,2", (unsigned long)addr);
    char reply[16];
    rsp_cmd(cmd, reply, sizeof(reply), 2000);
    return boolean::TRUE;
}

void SlaveSetWriteProtect(void)   {}
void SlaveClearWriteProtect(void) {}
void SlaveSetBRKOnWrite(void)     {}
void SlaveClearBRKOnWrite(void)   {}

// --- Platform-specific stubs -------------------------------------------------

void ReadSlaveVDP(unsigned long /*addr*/, char *data, unsigned int len) {
    memset(data, 0, len);
}
void WriteSlaveVDP(unsigned long /*addr*/, char */*data*/, unsigned int /*len*/) {}
void ReadSlaveCRAM(char *data)  { memset(data, 0, 128); }
void WriteSlaveCRAM(char */*data*/) {}
void ReadSlaveVSRAM(char *data) { memset(data, 0, 80); }
void WriteSlaveVSRAM(char */*data*/) {}

// --- Residual asm-export symbols --------------------------------------------

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
