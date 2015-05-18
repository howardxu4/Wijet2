EXEC = w2
TSRC = wijet2.cpp
TARGET = wijet2.a
OBJS = State.o Control.o 
CC = g++
AR = ar cru
CP = cp -f
CFLAGS = -g -Wall -O -c
LFLAGS = -Wall -o
LIBS =  ../libs/mynetlib.a ../libs/logtrace.a -lpthread
MYLIB = ../libs/loadfile.a 
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
	
