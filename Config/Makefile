EXEC = cgictest
TSRC = cgictest.cpp
WINFO = wlan_info.cgi
WINFO_SRC = wlan_info.cpp
WADMIN = wlan_admin.cgi
WADMIN_SRC = wlan_admin.cpp
WNTWK = wlan_network.cgi
WNTWK_SRC = wlan_network.cpp
WCONF = wlan_conf.cgi
WCONF_SRC = wlan_conf.cpp
WSECU = wlan_secu.cgi
WSECU_SRC = wlan_secu.cpp
WADVC = wlan_advc.cgi
WADVC_SRC = wlan_advc.cpp
WDISP = wlan_disp.cgi
WDISP_SRC = wlan_disp.cpp
WDHCP = wlan_dhcp.cgi
WDHCP_SRC = wlan_dhcp.cpp
WHELP = wlan_help.cgi
WHELP_SRC = wlan_help.cpp
WDEBUG = wlan_debg.cgi
WDEBUG_SRC = wlan_debg.cpp
WOPTN = wlan_optn.cgi
WOPTN_SRC = wlan_optn.cpp

WLANCGI = wlan_*.cgi
TARGET = cgic.a 
OBJS = cgic.o ConfigCommon.o ConfigTools.o
CC = g++
AR = ar cru
CP = cp -f
CFLAGS = -g -Wall -O -c
LFLAGS = -Wall -o 
LIBS =  -L/usr/lib -lm -ljpeg -lpthread 
MYLIB = ../libs/loadfile.a ../libs/mynetlib.a 
DIST = ../libs
INSTALL = ../install/www

%.o : %.cpp 
	$(CC) $(CFLAGS) $< -o $@

all:	$(EXEC)

$(WINFO): $(WINFO_SRC) $(TARGET) 
	$(CC) $(LFLAGS) $(WINFO) $(WINFO_SRC)  $(TARGET) $(LIBS) $(MYLIB)

$(WADMIN): $(WADMIN_SRC) $(TARGET) 
	$(CC) $(LFLAGS) $(WADMIN) $(WADMIN_SRC)  $(TARGET) $(LIBS) $(MYLIB)

$(WNTWK): $(WNTWK_SRC) $(TARGET)
	$(CC) $(LFLAGS) $(WNTWK) $(WNTWK_SRC)  $(TARGET) $(LIBS) $(MYLIB)

$(WCONF): $(WCONF_SRC) $(TARGET) 
	$(CC) $(LFLAGS) $(WCONF) $(WCONF_SRC)  $(TARGET) $(LIBS) $(MYLIB)

$(WSECU): $(WSECU_SRC) $(TARGET)
	$(CC) $(LFLAGS) $(WSECU) $(WSECU_SRC)  $(TARGET) $(LIBS) $(MYLIB)

$(WADVC): $(WADVC_SRC) $(TARGET) 
	$(CC) $(LFLAGS) $(WADVC) $(WADVC_SRC)  $(TARGET) $(LIBS) $(MYLIB)

$(WDISP): $(WDISP_SRC) $(TARGET) 
	$(CC) $(LFLAGS) $(WDISP) $(WDISP_SRC)  $(TARGET) $(LIBS) $(MYLIB)

$(WDHCP): $(WDHCP_SRC) $(TARGET)
	$(CC) $(LFLAGS) $(WDHCP) $(WDHCP_SRC)  $(TARGET) $(LIBS) $(MYLIB)

$(WHELP): $(WHELP_SRC) $(TARGET) 
	$(CC) $(LFLAGS) $(WHELP) $(WHELP_SRC)  $(TARGET) $(LIBS) $(MYLIB)

$(WDEBUG): $(WDEBUG_SRC) $(TARGET)
	$(CC) $(LFLAGS) $(WDEBUG) $(WDEBUG_SRC)  $(TARGET) $(LIBS) $(MYLIB)

$(WOPTN): $(WOPTN_SRC) $(TARGET)
	$(CC) $(LFLAGS) $(WOPTN) $(WOPTN_SRC)  $(TARGET) $(LIBS) $(MYLIB)

$(EXEC): $(TSRC) $(WINFO) $(WADMIN) $(WNTWK) $(WCONF) $(WSECU) $(WADVC) $(WDISP) $(WDHCP) $(WHELP) $(WDEBUG) $(WOPTN) $(TARGET) 
	$(CC) $(LFLAGS) $(EXEC) $(TSRC)  $(TARGET) $(LIBS) $(MYLIB)

$(TARGET) : $(OBJS)
	$(AR) $(TARGET) $(OBJS)

install: $(EXEC)
	$(CP) $(WLANCGI) $(INSTALL)

clean: 
	$(RM) $(EXEC) $(OBJS) $(WLANCGI) $(TARGET)

