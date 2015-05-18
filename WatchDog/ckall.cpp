
#include "../LoadFile/getProperty.h"

#define DEBUG
#ifdef DEBUG
#define PRINT_OUT       printf
#else
#define PRINT_OUT       dummy
void dummy(char*f, ...) {}
#endif

char *checkFile[] = {
	"/www/download/wlan_optn.cgi",
	"/banners/defaults/banner.jpg",
	"/root/.components/wj2.tar.gz",
	"/usr/local/bin",
	NULL
};

int main (int argc, char **argv)
{
	getProperty gP;
	int n = gP.check();		// Check application config
	for(int i=0; n == 0 && checkFile[i] != NULL; i++ ) {
		struct stat buf;
		n = stat(checkFile[i], &buf);
		if ( n != 0) PRINT_OUT("%s return %d\n", checkFile[i], n);
	}
	return n;
}
