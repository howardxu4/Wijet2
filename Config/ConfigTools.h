#if !defined(_WEB_CONFIG_TOOLS_CLASS_DEFINED)
#define _WEB_CONFIG_TOOLS_CLASS_DEFINED

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <fcntl.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "../LogTrace/LogTrace.h"
#include "../myNetLib/myNetWK.h"

class ConfigTools
{
public:
	ConfigTools();
	~ConfigTools();

	void Changes(int from, int what);
	void InitSB(char *SNumber, char* BTimes);

	enum {
		ADMIN,
		ULOAD,
		WRLES,
		SECUR,
		ADVNC,
		DISPY,
		DHCPD,
		TESTD
	};
private:
};

#endif	// _WEB_CONFIG_TOOLS_CLASS_DEFINED
