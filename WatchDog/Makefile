EXEC = wd
TSRC = wd.cpp
TARGET = WatchDog.a 
CHKHW = ckall
CHKHW_SRC = ckall.cpp
OBJS = WatchDog.o WdDHCP.o WdXorXin.o WdUpload.o CheckHW.o CheckIR.o
CC = g++
AR = ar cru
CP = cp -f
CFLAGS = -g -Wall -O -c
LFLAGS = -Wall -o 
LIBS =  -L/usr/X11R6/lib -lX11 -lXpm -lXext -lm -ljpeg -lpthread
MYLIB = ../libs/logtrace.a ../libs/XWrapper.a ../libs/loadfile.a ../libs/mynetlib.a
DIST = ../libs
INSTALL = ../install/usr/local/bin

%.o : %.cpp 
	$(CC) $(CFLAGS) $< -o $@

$(EXEC): $(TSRC) $(TARGET) $(CHKHW) 
	$(CC) $(LFLAGS) $(EXEC) $(TSRC)  $(TARGET) $(LIBS) $(MYLIB)

$(TARGET) : $(OBJS)
	$(AR) $(TARGET) $(OBJS)

$(CHKHW) : $(CHKHW_SRC) $(TARGET)
	$(CC) $(LFLAGS) $(CHKHW) $(CHKHW_SRC) $(TARGET) $(MYLIB)

install: $(EXEC)
	$(CP) $(EXEC) $(INSTALL)
	$(CP) $(CHKHW) $(INSTALL)

clean: 
	$(RM) $(EXEC) $(OBJS) $(TARGET) $(CHKHW)

