

/**
 *      The program checking hardware
 *
 *      @author     Thomas Lo
 *      @version        2.5
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>
#include <linux/ioport.h>
#include <fcntl.h>

//#define HDC    0x170    /*   drive 1    */
#define HDC    0x1f0   	/*   drive 0    */
#define MASTER 0x0
#define SLAVE  0x10

/* The generator polynomial used for this version of the package is */
/* x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x^1+x^0 */
/* ===================== Begin the code ======================  */

#define POLYNOMIAL 0x04c11db7L
static unsigned short crc16_Table[256];

//#define APPLICATION
//#define USEPIPE
//#define DEBUG

#ifdef DEBUG
#define DEBUG_OUT printf
#else
void DEBUG_OUT(char* f, ...) {}
#endif

/* generate the table of CRC remainders for all possible combination */

void gen_crc16_table()
{
	register int i, j; register unsigned short crc_accum;
   	for ( i = 0; i < 256; i++ )
   	{ 
		crc_accum = ( (unsigned short) i << 8 );
        	for ( j = 0; j < 8; j++ )
			if ( crc_accum & 0x8000 )
                        	crc_accum =( crc_accum << 1 ) ^ 0x1021;
           		else
                        	crc_accum =( crc_accum << 1 );
        	crc16_Table[i] = crc_accum; 
   	}
}

/* update the CRC on the data block one byte at a time */

unsigned short update_crc16(unsigned short crc_accum, char *data_blk_ptr,
int data_blk_size)
{
    	register int i, j;
    	for ( j = 0; j < data_blk_size; j++ )
    	{
        	DEBUG_OUT("%x-",*data_blk_ptr);
        	i = ( (int) ( crc_accum >> 8) ^ *data_blk_ptr-- ) & 0xff;
		crc_accum = ( crc_accum << 8 ) ^ crc16_Table[i]; 
    	}
        DEBUG_OUT("\n");
    	return crc_accum; 
}

unsigned char HEXJ(unsigned char x1, unsigned char x2)
{
    	unsigned char z;
        x1=x1-0x30;
        if ( x1 > 9)  x1=x1-7;
        x2=x2-0x30;
        if ( x2 > 9 )  x2=x2-7;
        z = (x2 << 4) + x1 ;
	DEBUG_OUT("\n x1,x2,z --%x,%x,%x \n ",x1,x2,z);
        return(z) ;
}

unsigned char HEXL( unsigned char x2)
{
   	unsigned char x3 ;
        x3 = x2 & 0x0f;
        if ( x3 > 0x9 ) x3 = x3 + 0x37 ;
        else    x3 = x3 + 0x30;
        return(x3);
}

unsigned char HEXH( unsigned char x2)
{
   	unsigned char x3 ;
        x3 = (x2 >> 4) & 0x0f;
        if ( x3 > 0x9 ) x3 = x3 + 0x37 ;
        else    x3 = x3 + 0x30;
        return(x3);
}

unsigned int domrd(unsigned char* buff)
{
   	unsigned int x,y,z;
//      ioperm(HDC, 8,1);   	/* From HDC to HDC+7 */
	iopl(3) ;
	usleep(10000);
        while ( inb(HDC+7) & 0x80 ) usleep(10000);
        outb_p(0xec, HDC+7);
	while ( inb(HDC+7) & 0x80 ) usleep(10000);
        outb_p(0x0, HDC+6);
	while ( inb(HDC+7) & 0x80 ) usleep(10000);
        outb_p(0x0,HDC+5 );
	while ( inb(HDC+7) & 0x80 ) usleep(10000);
        outb_p(0x0,HDC+4) ;
	while ( inb(HDC+7) & 0x80 ) usleep(10000);
        outb_p(0x0,HDC+3 );
	while ( inb(HDC+7) & 0x80 ) usleep(10000);
        outb_p(0x0,HDC+2);
	while ( inb(HDC+7) & 0x80 ) usleep(10000);
        outb_p(0x0,HDC+1);

        y = inb(HDC+7);
	x = y & 0xD0;

        while ( x !=0x50 )
        {
DEBUG_OUT("WHILE x != 0x50 %x %x\n", x , y);

		usleep(10000);
		y=inb(HDC+7) ;
           	x= y & 0xD0 ; 
        }
	DEBUG_OUT("\nStatus is %x \n ",y );  
        for ( x=0 ; x < 512; x=x+2)
        {
		z=inw_p(HDC);
            	buff[x]=z & 0xff;
            	buff[x+1] = (z>>8 ) & 0xff;
        }
        z=( buff[1]<< 8) + buff[0] ;
//	DEBUG_OUT(" ID is %x%x  z=%x  \n",buff[1],buff[0], z) ;  
	iopl(0);
//	ioperm(HDC, 8,0);
        return(z);
}

#define CHECK_REAL

/* last one should be 0x58 */

int checkHW(char *SN)
{
	unsigned char x1, x2,z1,z2 ;
	unsigned short y, start_crc ;
	unsigned char *pt;
	unsigned int ID ;
	int res = 0;
	unsigned char buff[512] ;

#ifndef CHECK_REAL        
	sprintf(SN, "A00001");
	return 0;
#endif
//DEBUG_OUT("Start Check Hardware\n");

	y=0;
        do
        {
		ID = domrd(buff);
            	y=y+1 ;
            	if ( y > 10 )
              		return(2);
        } while ( ID != 0x44A) ;
//DEBUG_OUT("Generate CRC\n");
	gen_crc16_table();
        start_crc=0x0 ;

        pt = buff +0x27 ;
        start_crc = update_crc16(start_crc, (char *) pt, 16) ;
        z1 = (unsigned char) HEXJ(buff[20],buff[21]);
        z2 = (unsigned char) HEXJ(buff[22],buff[23]);   
        x1 = (unsigned char) start_crc ;
	x2 = (unsigned char)( start_crc >> 8 ) ;

	DEBUG_OUT ("\n DOM crc is -- %x %x  \n", x1 , x2);
	DEBUG_OUT("\n crc is -- %x %x  \n", x1 , x2);    

        if (( z1==x1) && (z2==x2))
        {
		pt = buff +0x18;
               	buff[0x1e]=0x0 ;
               	DEBUG_OUT("OTC S/N = %s  \n",pt);
#ifdef USEPIPE
		if (access("OTCSN", F_OK) ==-1)
		{
			res= mkfifo("OTCSN", 0777);
		  	if (res !=0 ) 
		    		return (4);
	        }
		res = open("OTCSN",O_WRONLY) ;
                write(res, pt, 6);
		close(res);	    
#endif
		for(res = 0; res< 6; res++)
			SN[res] =  pt[res];      
		SN[res] = 0;
		return(0);
        }
	return(1);
}

#ifdef APPLICATION
int main()
{
	char SN[10];
	switch( checkHW(SN) ) {
	case 0:
		DEBUG_OUT("SN = %s\n", SN);
		return 0;
		break;
	case 1:
		fprintf(stderr, " Can't find OTC componet.\n") ;
		break;	
	case 2:
		fprintf(stderr, " Can't read DOM ID.\n");
		break;	
	case 4:
		fprintf(stderr, " Can't open FIFO.\n");
		break;
	default:
		break;
	}
	fprintf(stderr, "Hardware check fails!\n");
	return -1;
}
UT("WHILE x != 0x58 %x %x\n", x , y);

#endif
