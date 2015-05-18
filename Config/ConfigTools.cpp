#include "ConfigTools.h"
#include "../WatchDog/ShMemory.h"

ConfigTools::ConfigTools() {}
ConfigTools::~ConfigTools() {}

void ConfigTools::Changes(int from, int what)
{
	ShMemory shm;
	shm.setChanges(what);
	switch(from) {
	case ADMIN:
		shm.tellWD(ShMemory::M_REST);
	break;
	case ULOAD:
		shm.tellWD(ShMemory::M_LOG);
	break;
	case WRLES:
	break;
	case SECUR:
	break;
	case ADVNC:
		shm.tellWD(ShMemory::M_CONF);
	break;
	case DISPY:
	break;
	case DHCPD:
		shm.tellWD(ShMemory::M_APPL);
	break;
	case TESTD:
		shm.tellWD(ShMemory::M_DEBG);
	break;
	default:
	break;
	}
}

void ConfigTools::InitSB(char *SNumber, char* BTimes)
{
        ShMemory shm;
        int n = shm.getBTimes() * 5;
        sprintf(SNumber, "%s", shm.getSNumber());
        sprintf(BTimes, "%02d:%02d:%02d", n/3600, n/60%60, n%60);
}

