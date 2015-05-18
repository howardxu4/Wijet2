EXEC = startUp
TSRC = startUp.cpp
TARGET = wlanlib.a
OBJS = wlan_advanced.o wlan_common.o myWlan.o wlan_wpa.o
CC = g++
AR = ar cru
CP = cp -f
CFLAGS = -g -Wall -O -c
LFLAGS = -Wall -o 
LIBS =  -L/usr/lib -liw  -lpthread
MYLIB = ../libs/logtrace.a ../libs/loadfile.a ../libs/mynetlib.a 
DIST = ../libs
INSTALL = ../install/usr/local/bin

%.o : %.cpp 
	$(CC) $(CFLAGS) $< -o $@

$(EXEC): $(TSRC) $(TARGET) 
	$(CC) $(LFLAGS) $(EXEC) $(TSRC)  $(TARGET) $(LIBS) $(MYLIB)

$(TARGET) : $(OBJS)
	 $(AR) $(TARGET) $(OBJS)

install: $(EXEC)
	$(CP) $(TARGET) $(DIST)
	$(CP) $(EXEC) $(INSTALL)
clean: 
	$(RM) $(EXEC) $(OBJS) $(TARGET)

