EXEC = xwinfo
TSRC = xwinfo.cpp
TARGET =XWrapper.a
OBJS =  LoadJpeg.o XwinObject.o 
CC = g++
AR = ar cru
CP = cp -f
CFLAGS = -g -Wall -O -c
LFLAGS = -Wall -o
LIBS =  -L/usr/X11R6/lib -lX11 -lXpm -lXext -lm -ljpeg
DIST = ../libs
INSTALL = ../install/usr/local/bin

%.o : %.cpp 
	$(CC) $(CFLAGS) $< -o $@

$(EXEC): $(TSRC) $(TARGET)
	$(CC) $(LFLAGS) $(EXEC) $(TSRC) $(TARGET) $(LIBS)

$(TARGET) : $(OBJS)
	$(AR) $(TARGET) $(OBJS) 

clean:
	$(RM) $(EXEC) $(OBJS) $(TARGET)

install: $(TARGET)
	$(CP) $(TARGET) $(DIST)
	$(CP) $(EXEC) $(INSTALL)
