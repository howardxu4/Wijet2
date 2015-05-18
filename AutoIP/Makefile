EXEC = aip
TSRC = apMain.cpp
TARGET = autoiplib.a
OBJS = AutoIpProcess.o AutoIpHandler.o AutoIpUtils.o WiJETService.o
CC = g++
AR = ar cru
CP = cp -f
CFLAGS = -g -Wall -O -c
LFLAGS = -Wall -o
LIBS = ../libs/mynetlib.a -L/usr/lib -liw  -lpthread
MYLIB = ../libs/logtrace.a ../libs/loadfile.a ../libs/wlanlib.a
DIST = ../libs
INSTALL = ../install/usr/local/bin

%.o : %.cpp
	$(CC) $(CFLAGS) $< -o $@


$(EXEC): $(TSRC) $(TARGET)
	$(CC) $(LFLAGS) $(EXEC) $(TSRC) $(TARGET) $(LIBS) $(MYLIB)

$(TARGET) : $(OBJS)
	$(AR) $(TARGET) $(OBJS)

clean:
	$(RM) $(EXEC) $(OBJS) $(TARGET)

install: $(EXEC)
	$(CP) $(EXEC) $(INSTALL)


