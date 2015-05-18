
////////////////////////////
// The Load environment variable class

#if (!defined(_LOAD_ENVIRONMENT_VIRABLES_DEFINED))
#define _LOAD_ENVIRONMENT_VIRABLES_DEFINED

#include "getProperty.h"

/**
 *	This class load the environment variables from file
 *	for sharing with all wijet2 components
 */

class LdEnv
{
public:
	LdEnv();
	~LdEnv();
	char* getPath();
	char* getUSB();
	char* getMapping();
	char* getImage();

private:
	getProperty m_gP;
};

#endif // _LOAD_ENVIRONMENT_VIRABLES_DEFINED
