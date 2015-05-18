EXEC = bn
TSRC = bn.cpp
TARGET = banner.a 
OBJS = Banner.o
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

$(EXEC): $(TSRC) $(TARGET) 
	$(CC) $(LFLAGS) $(EXEC) $(TSRC)  $(TARGET) $(LIBS) $(MYLIB)

$(TARGET) : $(OBJS)
	$(AR) $(TARGET) $(OBJS)

install: $(EXEC)
	$(CP) $(EXEC) $(INSTALL)

clean: 
	$(RM) $(EXEC) $(OBJS) $(TARGET)

