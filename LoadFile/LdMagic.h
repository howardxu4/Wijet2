
////////////////////////////
// The Load Magic file  class

#if (!defined(_LOAD_MAGIC_IAMGEFILE_DEFINED))
#define _LOAD_MAGIC_IAMGEFILE_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

//	MAGICNUM	 WIJET-TWOE
#define MAGICNUM	(94538-8963)
#define UPGRADE		"upgrade"

#define TEMP_DIR	"/tmp"
#define PATCH_DESCRIPT	"patch.txt"

class LdMagic
{
public:
	enum {  U_DATA,
        	U_CONF,
        	U_SHELL,
        	U_WIJET,
        	U_APP,
        	U_SYS
	};
	
	LdMagic();
	virtual ~LdMagic();
	inline int getType() { return m_type; };
	inline int getSize() { return m_size; };
	inline int getCksum() { return m_cksum; };
	bool setTarFile(char *fname);
	int checkMagic();
	void runScript();
	void setUpgrade(char* name);
	char* getUpgrade(bool f=true);
	int loadFromFile(char* filename);
	int getFileSize(char *fname);
	int getCheckSum(char *fname);
	int saveToFile(char*filename, int type=U_DATA, char* upgrade=NULL);

private:
	int m_magic;
	int m_size;
	int m_type;
	int m_cksum;
	char* m_upgrade;
	char* m_tarfile;
};

#endif	// _LOAD_MAGIC_IAMGEFILE_DEFINED
