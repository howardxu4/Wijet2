#include <stdio.h>
#include <stdlib.h>

#include "Transfer.h"

void usage(char *a[])
{
	printf("Usage: %s {option} [{-i ip}| {-f file} |{-p port} |{-o outfile}\n", a[0]);
	printf("       {-b size} {-s step}]\n");
	printf(" option\n");
	printf("	2:   [-{i|p|o|b}] TCPClient receiver -> SCREEN\n"); 
	printf("	20:  [-{p|o|b}]	  TCPSVR receiver -> SCREEN\n");
	printf("	200: [-{i|p|o|b}] UDP Receiver -> SCREEN\n"); 
	printf("	4:   {-f[p|s|b]}    FILE -> TCPSVR sender -> TCPClient\n");
	printf("	40:  [-{f|i|p|s|b}] FILE -> TCPClient sender -> TCPSVR\n"); 
	printf("	400: [-{f|i|p|s|b}] FILE -> UDP sender -> UDP\n");
	printf("	10:  {-f[c|s|b]}       FILE to SCREEN\n"); 
	printf("	11:  {-o[b]}  SCREEN to FILE\n"); 
	printf("    *:   this display\n");
	printf(" e.g.\n");
	printf("   %s 2 -i 192.168.11.144 -p 9800 -- TCP Client receive message and output\n", a[0]);
	printf("   %s 20 -p 9800 -o outfile	-- TCP Server wait Client to receive message and output\n", a[0]);
	printf("   %s 200 -i 169.254.98.66 -p 30700 -- UDP receive message and output\n", a[0]);
	printf("   %s 4 -f test.mpg -p 9800 -- TCP Server wait client connect then send a file to it\n", a[0]);
	printf("   %s 40 -f test.txt -i 192.168.11.144 -p 9800 -- TCP Client send a file\n", a[0]);
	printf("   %s 400 -f test.txt -i 169.254.98.66 -p 30700 -- UDP send a file\n", a[0]);
	printf("   %s 10 -f test.txt  -- File to Screen\n", a[0]);
	printf("   %s 11 -o test.txt  -- Screen to File\n", a[0]);

	exit(0);
}

void reqfile(char *io)
{
	printf("Need an %s file\n", io); 
	exit(0);
}

// Main program

int main (int n, char *a[])
{
 
	Transfer myTransfer;
	char *ip = NULL;
	char *file = NULL;
	char *outfile = NULL;
	int port = -1;
	int option = 0;
	if (n < 2) usage(a);

	option = atoi(a[1]);
	SocketSystem mySockSys;

	if (n > 2) {
	int i = 2;
		do {
			if (a[i][0] == '-' && i+1 < n) {
				switch(a[i][1]) {
				case 'i':
				  ip = a[i+1];
				  break;
				case 'f':
				  file = a[i+1];
				  break;
				case 'p':
				  port = atoi(a[i+1]);
				  break;
				case 'o':
				  outfile = a[i+1];
				  break;
				case 'b':
					myTransfer.setBufferSize(atoi(a[i+1]));
				  break;
				case 's':
					myTransfer.setStep(atol(a[i+1]));
				  break;
				case 'z':
					myTransfer.setSleep(atol(a[i+1]));
				  break;
				default:
				  usage(a);
				}
				i +=2;
			} else usage(a);
		} while (i < n);
	}

	switch(option) {
		case 2:
		if (port < 0) port = 9800;
		myTransfer.TCPClientReceive(ip, port, outfile);	// TCP client receive 
		break;

		case 20:	
		if (port < 0) port = 9800;
		myTransfer.TCPServerReceive(port, outfile);	// TCP SVR receive (Server to Screen)
		break;

		case 200:
		if (port < 0) port = 30700;
		myTransfer.UDPReceiver(ip, port, outfile);	// UDP receive
		break;

		case 4:
		if (port < 0) port = 9800;
		if (file == NULL) reqfile("input");
		myTransfer.TCPServerSend(file, port);		// TCP SVR sender
		break;

		case 40:
		if (port < 0) port = 9800;
		myTransfer.TCPClientSend(file, ip, port);	// TCP client sender
		break;

		case 400:
		if (port < 0) port = 30700;
		myTransfer.UDPSender(file, ip, port);		// UDP sender
		break;

		case 10:
		if (file == NULL) reqfile("input");
		myTransfer.FileVsScreen(file);				// File to Screen
		break;
		case 11:
		if (outfile == NULL) reqfile("output");
		myTransfer.FileVsScreen(NULL, outfile);		// Screen to File
		break;

		default:
		usage(a);
		break;
	}
	return(0);
} 
