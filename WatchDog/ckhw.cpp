#include "WatchDog.h"

#define DEBUG
#ifdef DEBUG
#define PRINT_OUT	printf
#else
#define PRINT_OUT	dummy
void dummy(char*f, ...) {}
#endif

int main(int n, char*a[])
{
	if (n > 1) {
	        if (checkIR() == EXIT_SUCCESS) {
                	PRINT_OUT("IR = OK\n");
			return 0;
		}
	}
	else {
        char SN[10];
//	return 0;		// pass first
        switch( checkHW(SN) ) {
        case 0:
                PRINT_OUT("SN = %s\n", SN);
                return 0;
                break;
        case 1:
                fprintf(stderr, "Error: Can't find OTC componet.\n") ;
                break;
        case 2:
                fprintf(stderr, "Error: Can't read DOM ID.\n");
		break;
        case 4:
                fprintf(stderr, "Error: Can't open FIFO.\n");
                break;
        default:
                break;
        }
	}
        fprintf(stderr, "Hardware check fails!\n");
        return -1;
}

