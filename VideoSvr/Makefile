EXEC = vds
TSRC = mdplayer.cpp
iTSRC = videosvr.cpp
TARGET = ctl
TFMAIN = ctrlbox.cpp
OBJS = ctrlbox.o
CC = g++
AR = ar cru
CP = cp -f
CFLAGS = -g -Wall -O -c
OPTIONS = `xine-config --cflags` `xine-config --libs`
LFLAGS = -Wall -o 
LIBS = -L/usr/X11R6/lib -lX11 -lXpm -lXext -lXm -lm -ljpeg -lpthread
MYLIB = ../libs/mynetlib.a ../libs/XWrapper.a ../libs/logtrace.a ../libs/myXine.a ../libs/loadfile.a
DIST = 
INSTALL = ../install/usr/local/bin

%.o : %.cpp
	$(CC) $(CFLAGS) $< -o $@


$(EXEC) : $(TSRC) $(TARGET) 
	$(CC) $(OPTIONS) $(LFLAGS) $(EXEC) $(TSRC) $(MYLIB) $(LIBS)

$(TARGET) : $(OBJS)
	$(CC) $(LFLAGS) $(TARGET) $(TFMAIN) $(MYLIB) $(LIBS)
	
clean:
	$(RM) $(EXEC) $(TARGET) $(OBJS) 

install: $(EXEC)
	$(CP) $(TARGET) $(INSTALL)
	$(CP) $(EXEC) $(INSTALL)
