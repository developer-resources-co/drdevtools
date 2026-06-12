// Phase 1.5 ncurses front end — see linux/ncurses_io.cpp.
#ifndef DRMON_NCURSES_IO_H
#define DRMON_NCURSES_IO_H
#ifdef __cplusplus
extern "C" {
#endif
void           drmon_nc_init(void);          // primary terminal (handle 0)
void           drmon_nc_shutdown(void);
int            drmon_nc_open(int out_fd, int in_fd);  // additional terminal on a PTY; handle or -1
void           drmon_nc_select(int handle);  // make `handle` the current terminal
int            drmon_nc_infd(int handle);    // input fd of `handle` (for poll), or -1
void           drmon_nc_close(int handle);   // tear down a non-primary terminal
void           drmon_nc_blit(const unsigned char *buf, int w, int h,
                             int caretX, int caretY, int caretMode);
unsigned char *drmon_nc_shiftbyte(void);
int            drmon_nc_keyready(void);
int            drmon_nc_getbyte(void);
int            drmon_nc_bioskeybrd(int cmd);
int            drmon_nc_havemouse(void);
int            drmon_nc_getmouse(short *fx, short *fy);
void           drmon_nc_size(int *cols, int *rows);
int            drmon_nc_resized(void);
void           drmon_nc_resync(void);
#ifdef __cplusplus
}
#endif
#endif
