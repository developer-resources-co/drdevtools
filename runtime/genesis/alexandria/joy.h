
#ifndef _JOY_H_
#define _JOY_H_


#define JOYMASK_up      	1
#define JOYMASK_down    	2
#define JOYMASK_left    	4
#define JOYMASK_right   	8
#define JOYMASK_b             16
#define JOYMASK_c             32
#define JOYMASK_a             64
#define JOYMASK_start   	128

unsigned char ReadJoy1(void);
unsigned char ReadJoy2(void);
void ButtonRelease(void);
ButtonDispatch(void(*p[])(void),void *a2val,int nJoyValue);

#endif
