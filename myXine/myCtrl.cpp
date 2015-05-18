#include "myXine.h"

int main(int n, char *a[])
{
	int port = XINE_SVR_PORT;
	if (n < 2) {
		printf("Usage: %s {cmd}\n", a[0]);
		printf("   cmd:   stop|exit|speed_{0|1|2|4|8|16}\n");
	}
	else {
		Sender mySender(port);
		mySender.Send(a[1], strlen(a[1]));
		if (strstr(a[1], "query")) {
			char buffer[20];
			strcpy(buffer, "no response");
			mySender.Echo(buffer, 20);
			printf("%s\n", buffer);
		}
	}
	return 0;
}
