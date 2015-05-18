#ifndef _WJTYPES_H
#define _WJTYPES_H

#ifdef LONG64
typedef unsigned long 	CARD64;
typedef unsigned int 	CARD32;
#else
typedef unsigned long 	CARD32;
#endif

typedef unsigned short 	CARD16;
typedef unsigned char  	CARD8;

typedef CARD32          BITS32;
typedef CARD16          BITS16;

/*
 * This is bad style, but the central include file <os2.h> declares them
 * as well
 */
#ifndef BYTE
#define BYTE            CARD8
#endif
#define BOOL            CARD8

#define Bool 		int
#define Status 		int
#define True 		1
#define False 		0

/*
 * Data structure used by color operations
 */

#ifdef SOMEONEUSE
typedef struct {
        unsigned long pixel;
        unsigned short red, green, blue;
        char flags;  /* do_red, do_green, do_blue */
        char pad;
} XColor;

/* Flags used in StoreNamedColor, StoreColors */

#define DoRed           (1<<0)
#define DoGreen         (1<<1)
#define DoBlue          (1<<2)
#endif

#endif
