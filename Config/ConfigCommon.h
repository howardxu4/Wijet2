#if !defined(_WEB_CONFIG_COMMON_CLASS_DEFINED)
#define _WEB_CONFIG_COMMON_CLASS_DEFINED

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <fcntl.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "../LogTrace/LogTrace.h"
#include "../LoadFile/LdProp.h"
#include "../LoadFile/LdFlist.h"
#include "../LoadFile/LdConf.h"
#include "../LoadFile/LdMagic.h"
#include "../LoadFile/getProperty.h"
#include "../myNetLib/myNetWK.h"
#include "../AutoIP/PsmProtocol.h"
#include "cgic.h"
#include "ConfigConst.h"

class ConfigCommon
{
public:
	ConfigCommon();
	~ConfigCommon();

	// output
	void runScript(char *func);
	void putHeader(char* title);
	void putScript(char* script);
	void writeHeader(char* name, char* descript);
	void startArea();
	void endArea();
	void skipArea();
	void startPage(char* name, char* descript);
	void putSubtitle(char* subtitle);
	void skipRow();
	void dispRow(char* name, char* value, bool bold=false);
	void actionRow(char* name, char* action, bool right=false);
	void endPage();
	char* getSelect(char* name, int size, char* array[], int sel, char* opt=" ");
	void startTable(char* name, int size, char* headers[]);
	void endTable();
	void putFmtTag(char* fmt, ...);
	char* convertS(int n);
	inline void putTag(char* tag) { fprintf(cgiOut, tag); fprintf(cgiOut, "\n"); };
	inline char* getFValue(char* format, char* value, char* opt=" ") { sprintf( m_buffer, format, value, opt); return m_buffer; };
	inline char* getIValue(char* format, int value, int opt=0) { sprintf( m_buffer, format, value, opt); return m_buffer; };
 
private:
	char m_buffer[2000];	
	char m_conv[10];
};
#endif	//_WEB_CONFIG_COMMON_CLASS_DEFINED

