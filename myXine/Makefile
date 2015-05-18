EXEC = mx
TSRC = myMain.cpp 
iEXEC = ms
iTSRC = myCtrl.cpp
TARGET =myXine.a
OBJS = myXine.o
CC = g++
AR = ar cru
CP = cp -f
CFLAGS = -g -Wall -O -c
LFLAGS = -Wall -o
OPTIONS = `xine-config --cflags` `xine-config --libs`
INCLUDES = -I/usr/X11R6/include 
LIBS =  -L/usr/X11R6/lib -lX11 -lm -ljpeg
MYLIB = ../libs/mynetlib.a ../libs/XWrapper.a ../libs/logtrace.a
DIST = ../libs
INSTALL = ../install/usr/local/bin

%.o : %.cpp 
	$(CC) $(CFLAGS) $< -o $@

$(EXEC): $(TSRC) $(TARGET)
	$(CC) $(LFLAGS) $(EXEC) $(TSRC) $(TARGET) $(OPTIONS) $(INCLUDES) $(LIBS) $(MYLIB)

$(TARGET) : $(OBJS)
	$(AR) $(TARGET) $(OBJS)

clean:
	$(RM) $(EXEC) $(OBJS) $(TARGET)

install: $(EXEC) 
	$(CP) $(TARGET) $(DIST)

