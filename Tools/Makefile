EXEC = getprop
TSRC = getprop.cpp
TARGET = sendmsg
TFMAIN = sendmsg.cpp
TOOL = dome1st
TLSRC = dome1st.cpp
OBJS = sendmsg.o
CC = g++
AR = ar cru
CP = cp -f
CFLAGS = -g -Wall -O -c
LFLAGS = -Wall -o 
LIBS = ../libs/mynetlib.a
MYLIB = ../libs/loadfile.a
DIST =  
INSTALL = ../install/usr/local/bin

%.o : %.cpp
	$(CC) $(CFLAGS) $< -o $@

$(EXEC) : $(TSRC) $(TARGET) $(TOOL)
	$(CC) $(LFLAGS) $(EXEC) $(TSRC) $(LIBS) $(MYLIB)

$(TARGET) : $(OBJS)
	$(CC) $(LFLAGS) $(TARGET) $(TFMAIN) $(LIBS)

$(TOOL) : $(TLSRC)
	$(CC) $(LFLAGS) $(TOOL) $(TLSRC) $(MYLIB) $(LIBS)

clean:
	$(RM) $(EXEC) $(TARGET) $(TOOL) $(OBJS)

install: $(EXEC)
	$(CP) $(TARGET) $(INSTALL)
	$(CP) $(EXEC) $(INSTALL)
	$(CP) $(TOOL) $(INSTALL)
