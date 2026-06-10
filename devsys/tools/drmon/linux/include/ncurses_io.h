// Phase 1.5 ncurses front end — see linux/ncurses_io.cpp.
#ifndef DRMON_NCURSES_IO_H
#define DRMON_NCURSES_IO_H
#ifdef __cplusplus
extern "C" {
#endif
void           drmon_nc_init(void);
void           drmon_nc_shutdown(void);
void           drmon_nc_blit(const unsigned char *buf, int w, int h);
unsigned char *drmon_nc_shiftbyte(void);
int            drmon_nc_keyready(void);
int            drmon_nc_getbyte(void);
int            drmon_nc_bioskeybrd(int cmd);
#ifdef __cplusplus
}
#endif
#endif
