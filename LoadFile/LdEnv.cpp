
#include "LdEnv.h"
/**
 *      The LdEnv class load the environment file 
 *
 *  @author     Howard Xu
 *      @version        2.5
 */

#define WJE_BIN 	"/usr/local/bin"
#define WJE_USB 	"/mnt"
#define WJE_MAP 	"/mnt1"
#define WJE_IMAGE	"/banners/images"

#define ENV_BIN		"WJ2_BIN"
#define ENV_USB		"WJ2_USB"
#define ENV_MAP		"WJ2_MAP"
#define ENV_IMAGE	"WJ2_IMG"

/**
 *      The constructor
 *
 */
LdEnv::LdEnv()
{
}

/**
 *      The destructor
 *
 */
LdEnv::~LdEnv()
{
}

char* LdEnv::getPath() 
{
	char* p = m_gP.getStr(ENV_BIN);
	if (strlen(p) ==0) p = WJE_BIN;
	return p;
}

char* LdEnv::getUSB()
{
        char* p = m_gP.getStr(ENV_USB);
	if (strlen(p) ==0) p  = WJE_USB;
        return p;
}

char* LdEnv::getMapping()
{
        char* p = m_gP.getStr(ENV_MAP);
	if (strlen(p) ==0) p = WJE_MAP;
        return p;
}

char* LdEnv::getImage()
{
        char* p = m_gP.getStr(ENV_IMAGE);
	if (strlen(p) ==0) p = WJE_IMAGE;
        return p;
}

