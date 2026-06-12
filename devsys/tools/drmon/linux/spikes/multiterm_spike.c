/* multiterm_spike.c  — THROWAWAY feasibility spike (not wired into the build).
 *
 * Proves the Option-A architecture for drmon "multiple monitors" BEFORE committing
 * to the ~400-600 LOC refactor — because the previous multi-window design rested on
 * an untested premise ("MAME accepts multiple connections") that turned out false.
 *
 * Three claims under test:
 *   #1  one process can run N independent ncurses SCREENs via newterm() on N PTYs
 *       and render to each independently (set_term()).
 *   #2  one process can multiplex input from N terminals (poll() the PTY fds) and
 *       route each key to the right SCREEN.
 *   #3  `xterm -S` can attach a real visible window to a pre-opened PTY, so one
 *       process drives N xterms (vs. exec-ing a whole new drmon per window).
 *
 * Build:  gcc -O2 -o /tmp/multiterm_spike multiterm_spike.c -lncursesw -lutil
 * Run:    ./multiterm_spike headless   # deterministic, no X — proves #1 and #2
 *         ./multiterm_spike xterm      # needs $DISPLAY + xterm — proves #3
 */
#define _GNU_SOURCE
#include <curses.h>
#include <pty.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <sys/wait.h>
#include <sys/types.h>

#define N 2

static int fails = 0;
#define CHECK(cond, ...) do {                                   \
    char _m[160]; snprintf(_m, sizeof _m, __VA_ARGS__);         \
    if (cond) fprintf(stderr, "  SPIKE PASS: %s\n", _m);        \
    else    { fprintf(stderr, "  SPIKE FAIL: %s\n", _m); fails++; } \
} while (0)

/* newterm() against a list of terminal types; first that succeeds wins. */
static SCREEN *newterm_try(FILE *out, FILE *in) {
    const char *types[] = { "xterm", "vt100", "ansi", NULL };
    for (int i = 0; types[i]; i++) {
        SCREEN *s = newterm((char *)types[i], out, in);
        if (s) return s;
    }
    return NULL;
}

/* Read whatever is currently available on fd (nonblocking) into buf. */
static int drain(int fd, char *buf, int max) {
    int total = 0, n;
    int fl = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, fl | O_NONBLOCK);
    while (total < max - 1 && (n = read(fd, buf + total, max - 1 - total)) > 0)
        total += n;
    buf[total > 0 ? total : 0] = 0;
    fcntl(fd, F_SETFL, fl);
    return total;
}

/* ---- headless: prove #1 (independent render) and #2 (input multiplexing) ---- */
static int headless(void) {
    fprintf(stderr, "[headless] N=%d PTYs, one process, newterm()+poll()\n", N);
    int  master[N], infd[N];
    SCREEN *scr[N];
    char rendered[N][8192];
    struct winsize ws = { 24, 80, 0, 0 };

    for (int i = 0; i < N; i++) {
        int slave;
        if (openpty(&master[i], &slave, NULL, NULL, &ws) < 0) { perror("openpty"); return 2; }
        FILE *out = fdopen(slave, "w");
        infd[i]   = dup(slave);              /* ncurses reads keystrokes from here   */
        FILE *in  = fdopen(infd[i], "r");
        scr[i] = newterm_try(out, in);
        CHECK(scr[i] != NULL, "newterm() #%d created an independent SCREEN", i);
        if (!scr[i]) return 2;
        set_term(scr[i]);
        cbreak(); noecho(); nodelay(stdscr, TRUE); keypad(stdscr, TRUE);
    }

    /* render distinct content to each, draining its master right away */
    for (int i = 0; i < N; i++) {
        set_term(scr[i]);
        mvprintw(0, 0, "SCREEN_%d_READY", i);
        refresh();
        drain(master[i], rendered[i], sizeof rendered[i]);
    }

    /* #1: each terminal saw ONLY its own render */
    for (int i = 0; i < N; i++) {
        char mine[32], other[32];
        snprintf(mine,  sizeof mine,  "SCREEN_%d_READY", i);
        snprintf(other, sizeof other, "SCREEN_%d_READY", (i + 1) % N);
        CHECK(strstr(rendered[i], mine)  != NULL, "SCREEN %d render reached its own terminal", i);
        CHECK(strstr(rendered[i], other) == NULL, "terminal %d free of the other SCREEN's render", i);
    }

    /* #2: inject a distinct key per terminal, poll(), route to the matching SCREEN */
    const char key[N] = { 'A', 'B' };
    for (int i = 0; i < N; i++)
        if (write(master[i], &key[i], 1) != 1) perror("write key");

    struct pollfd pfd[N];
    for (int i = 0; i < N; i++) { pfd[i].fd = infd[i]; pfd[i].events = POLLIN; pfd[i].revents = 0; }
    int ready = poll(pfd, N, 1500);
    CHECK(ready == N, "poll() saw input ready on all %d terminals (got %d)", N, ready);

    for (int i = 0; i < N; i++) {
        CHECK(pfd[i].revents & POLLIN, "terminal %d flagged readable by poll()", i);
        set_term(scr[i]);
        int c = getch();
        CHECK(c == key[i], "key from terminal %d routed to SCREEN %d (wanted '%c', got '%c')",
              i, i, key[i], c < 32 ? '?' : c);
    }

    for (int i = 0; i < N; i++) { set_term(scr[i]); endwin(); delscreen(scr[i]); }
    return fails ? 1 : 0;
}

/* ---- xterm: prove #3 (xterm -S attaches a real window to a pre-opened PTY) ---- */
static int xterm_mode(void) {
    if (!getenv("DISPLAY")) {
        fprintf(stderr, "[xterm] SPIKE SKIP: no $DISPLAY — run on a desktop with xterm.\n");
        return 3;
    }
    fprintf(stderr, "[xterm] N=%d xterms from ONE process via `xterm -S<fd>` on DISPLAY=%s\n",
            N, getenv("DISPLAY"));
    SCREEN *scr[N];
    int slavefd[N];

    for (int i = 0; i < N; i++) {
        int master, slave;
        struct winsize ws = { 12, 40, 0, 0 };
        if (openpty(&master, &slave, NULL, NULL, &ws) < 0) { perror("openpty"); return 2; }
        fcntl(master, F_SETFD, 0);                 /* survive exec into xterm */

        pid_t pid = fork();
        if (pid == 0) {                            /* child -> xterm, gets the MASTER */
            close(slave);
            char arg[64], title[32], geom[32];
            snprintf(arg,   sizeof arg,   "-Sxx/%d", master);
            snprintf(title, sizeof title, "spike-term-%d", i);
            snprintf(geom,  sizeof geom,  "40x12+%d+80", 60 + i * 380);
            execlp("xterm", "xterm", arg, "-title", title, "-geometry", geom, (char *)NULL);
            perror("execlp xterm"); _exit(127);
        }
        close(master);                             /* parent uses the SLAVE */
        slavefd[i] = slave;

        /* CPR handshake: only a live attached terminal answers ESC[6n with ESC[r;cR */
        struct termios tio, raw;
        tcgetattr(slave, &tio); raw = tio; cfmakeraw(&raw); tcsetattr(slave, TCSANOW, &raw);
        struct pollfd p = { slave, POLLIN, 0 };
        char rbuf[128] = {0}; int got = 0, ok = 0;
        for (int t = 0; t < 60 && !ok; t++) {      /* up to ~6s for xterm to come up */
            if (t % 8 == 0) { (void)write(slave, "\033[6n", 4); }   /* (re)issue query */
            if (poll(&p, 1, 100) > 0 && (p.revents & POLLIN)) {
                int n = read(slave, rbuf + got, (int)sizeof rbuf - 1 - got);
                if (n > 0) { got += n; rbuf[got] = 0;
                    if (strchr(rbuf, 'R')) ok = 1; }   /* CPR reply ESC[r;cR (xterm prefixes a short report) */
            }
        }
        CHECK(ok, "xterm #%d attached to PTY (answered ESC[6n cursor-position-report)", i);
        tcsetattr(slave, TCSANOW, &tio);
    }

    /* render through ncurses so the windows show content for a screenshot */
    for (int i = 0; i < N; i++) {
        FILE *out = fdopen(slavefd[i], "w");
        FILE *in  = fdopen(dup(slavefd[i]), "r");
        scr[i] = newterm_try(out, in);
        CHECK(scr[i] != NULL, "ncurses SCREEN #%d bound to xterm's PTY", i);
        if (scr[i]) {
            set_term(scr[i]);
            cbreak(); noecho();
            mvprintw(2, 2, "XTERM SCREEN %d", i);
            mvprintw(4, 2, "one drmon process,");
            mvprintw(5, 2, "two real windows.");
            refresh();
        }
    }

    fprintf(stderr, "[xterm] windows up; holding 6s for screenshot...\n");
    sleep(6);
    for (int i = 0; i < N; i++) if (scr[i]) { set_term(scr[i]); endwin(); delscreen(scr[i]); }
    return fails ? 1 : 0;
}

int main(int argc, char **argv) {
    const char *mode = argc > 1 ? argv[1] : "headless";
    int rc;
    if      (!strcmp(mode, "headless")) rc = headless();
    else if (!strcmp(mode, "xterm"))    rc = xterm_mode();
    else { fprintf(stderr, "usage: %s headless|xterm\n", argv[0]); return 64; }

    if (rc == 3) { fprintf(stderr, "RESULT: SKIPPED (%s)\n", mode); return 0; }
    fprintf(stderr, "RESULT: %s (%s) — %d failure(s)\n",
            fails ? "FAIL" : "PASS", mode, fails);
    return rc;
}
