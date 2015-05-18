

#if (!defined(_GET_PROPERTY_CLASS_DEFINED))
#define _GET_PROPERTY_CLASS_DEFINED

#include "LdProp.h"
#include <sys/stat.h>

/**
 *      The getProperty class to get defined property value from file
 *
 *  @author     Howard Xu
 *      @version        2.5
 */

class getProperty {
public:
        enum {
                P_VERSION,
                P_ENV,
                P_OTC,
                P_WLAN,
		P_SCAN,
                P_REFRESH,
                P_MAX
        };
        getProperty(int n=P_ENV);
        ~getProperty();
        int init(int n);
        inline char* getStr(char* name) { return m_list.getValue(name); };
        inline int getInt(char* name) { return m_list.getIntValue(name); };
	inline LdProp* getList() { return &m_list; };
	int check(int n=P_MAX);
	void dump();
private:
        LdProp  m_list;
	char* 	m_filenames[P_MAX];
};

#endif // _GET_PROPERTY_CLASS_DEFINED
