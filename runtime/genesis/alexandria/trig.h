
#ifndef LIBRARY_TRIG_H
#define LIBRARY_TRIG_H

extern const short SineTable[];

#define sin( n )                ( (long)SineTable[ n ] )
#define cos( n )                ( (long)SineTable[ (n+64) & 0xFF ] )


#endif

