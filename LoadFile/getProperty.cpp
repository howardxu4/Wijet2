

#include "getProperty.h"

#define VERSION_CONF            "/etc/version"
#define OTCENV_CONF		"/etc/otc_env.conf"
#define OTC_CONFIG              "/etc/otc_defaults.conf"
#define WLAN_CONFIG             "/etc/wlan.conf"
#define SCAN_TXT		"/etc/scan.txt"
#define REFRESH_CONFIG          "/etc/refresh.conf"

/**
 *      The getProperty class to get defined property value from file
 *
 *  @author     Howard Xu
 *      @version        2.5
 */

/**
 *      The constructor
 *
 */

getProperty::getProperty(int n)
{
	m_filenames[P_VERSION] = VERSION_CONF;
	m_filenames[P_ENV] = OTCENV_CONF;
	m_filenames[P_OTC] = OTC_CONFIG;
	m_filenames[P_WLAN] = WLAN_CONFIG;
	m_filenames[P_SCAN] = SCAN_TXT;
	m_filenames[P_REFRESH] = REFRESH_CONFIG;
	init(n);
};

getProperty::~getProperty() 
{
}

int getProperty::init(int n)
{
	if (n >= P_MAX) n = P_VERSION;
	m_list.init(0, 0);
	if (n == P_WLAN || n == P_SCAN)
		m_list.noComment("SSID");
	else if (n == P_OTC) {
		m_list.noComment("USER");
		m_list.noComment("PASSWORD");
	}
	return m_list.loadFromFile(m_filenames[n]);	
}

int getProperty::check(int n)
{
	struct stat buf;
	if (n < P_MAX) {
		if (stat(m_filenames[n], &buf))
			return -(n+1);
	}
	else for (int i=0; i<P_MAX; i++) {
		if (stat(m_filenames[i], &buf))
			if (i != P_SCAN)	// ignore scan.txt
				return -(i+1);	
	}
	return 0;
}

void getProperty::dump()
{
	char *comment = NULL;
	for(int i=0; i<m_list.getCount(); i++) { 
		comment = m_list.getCMT(i);
		if (comment != NULL) 
			printf("Name ->  [%s]  Value -> [%s] Comment -> [%s]\n",
				m_list.getName(i), m_list.getValue(i), comment);
		else
			printf("Name ->  [%s]  Value -> [%s]\n",
				 m_list.getName(i), m_list.getValue(i));
	}
}
