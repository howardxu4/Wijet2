#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>

#ifdef DEBUG
#define DEBUG_OUT printf
#else
void DEBUG_OUT(char*, ...){};
#endif
int main(int n, char* a[])
{
	char line[200];
	char *pname = NULL;
	FILE *fp = stdin;
	int i = 1;
	int sig = 9;	// KILL
	while (i < n) {
		if (a[i][0] == '-') {
			if (a[i][1] == 'f') {
				i++;
				if (i < n) {
					fp = fopen(a[i], "r");
				        if (fp == NULL) {
                				fprintf(stderr, "Can't open file %s\n", a[1]);
                				return -1;
					}
        			}

			}
			else if (a[i][1] == 'k') {
				sig = 15;	// TERM
				if (a[i][2] != 0) {
					sig = atoi(&a[i][2]);
					if (sig < 1 || sig > 28) sig = 2;
				}
				i++;
			}
			else {
				fprintf(stderr,"usage: %s name [-k[#]|f file}]\n", a[0]);
				return -2;			
			}
		}
		else
			pname = a[i];
		i++;	 
	}

	i = 0;
	while(fgets(line, 200, fp)) {
		if (pname != NULL) {
			if (strstr(line, pname) != NULL) {
				n = atoi(line);
				if (n > 0) {
					kill (n, sig);
                                	DEBUG_OUT("[%d]  killed %s", ++i, line);
				}
			}		
		}
		else 
			DEBUG_OUT("[%d] %s", ++i, line);
	}
	fclose(fp);

	return(0);
}
