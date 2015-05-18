#include "../LoadFile/getProperty.h"

void usage(char* app)
{
	fprintf( stderr, "Usage: %s {name} {File#}\n", app);
        fprintf( stderr, "      name -- property name\n");
        fprintf( stderr, "      File# --  0: /etc/version\n");
        fprintf( stderr, "                1: /etc/otc_env.conf\n");
        fprintf( stderr, "                2: /etc/otc_defaults.conf\n");
        fprintf( stderr, "                3: /etc/wlan.conf\n");
	fprintf( stderr, "                4: /etc/scan.txt\n");
        fprintf( stderr, "                5: /etc/refresh.conf\n");
	exit(1);
}
int main (int argc, char **argv)
{
	if (argc == 1) {
		getProperty gP;
		int n = gP.check();
		if (n!=0) {
			fprintf(stderr, "Please check missing file %d\n", n);
			return -1;
		}
		fprintf(stderr, "Check files OK\n");
	}
	else {
		if (argc < 3) 	usage(argv[0]);
		int n = atoi(argv[2]);
		if (n < 0 || n > 4) usage(argv[0]);
		getProperty gP(n);
		printf("%s", gP.getStr(argv[1]));
	}
	return 0;
}
