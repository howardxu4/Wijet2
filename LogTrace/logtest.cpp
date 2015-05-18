// logtest.cpp : Defines the entry point for the console application.
//

#include "stdio.h"
#include "LogTrace.h"

void func(int n)
{
	LogTrace* myLog = new LogTrace("FuncA");
	printf("inside function A\n");
	myLog->log(myLog->USER, "Xi.., I am in a %s %d", "Function", n);
	delete myLog;
}

void function(int n)
{
	LogTrace myLog ("FuncB");
	printf("inside function B\n");
	myLog.log(myLog.DEVLP, "Hi.., I am in a %s %d", "Function", n);
}

int main(int argc, char* argv[])
{
	LogTrace myLog;
	myLog.setLog(LogTrace::INFO+LogTrace::DEBUG+myLog.USER+myLog.DEVLP,
		"mytestlog");
	if (myLog.checkLevel(myLog.INFO))
		myLog.log(LogTrace::INFO, "hello");
	func(1);
	printf("Hello World!\n");
	function(2);
	if (myLog.checkLevel(LogTrace::DEBUG))
		myLog.log(LogTrace::DEBUG, "world %s", "Ha...");
	return 0;
}

