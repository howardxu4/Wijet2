EXEC = myNetWK
TSRC = myNetWK.cpp
TARGET = mynetlib.a
OBJS = SocketBase.o SocketTCP.o SocketUDP.o SocketSystem.o MultiCastUDP.o SocketICMP.o Sender.o cmdMsgs.o
CC = g++
AR = ar cru
CP = cp -f
CFLAGS = -g -Wall -O -c
LFLAGS = -Wall -o
LIBS = 
MYLIB =
DIST = ../libs

%.o : %.cpp
	$(CC) $(CFLAGS) $< -o $@  


## $(EXEC): $(TSRC) $(TARGET)
##	$(CC) $(LFLGS) $(EXEC) $(TSRC) $(TARGET) $(LIBS) $(MYLIB)


$(TARGET) : $(OBJS) 
	$(AR) $(TARGET) $(OBJS) 

clean:
	$(RM) $(EXEC) $(OBJS) $(TARGET)

install: $(TARGET)
	$(CP) $(TARGET) $(DIST)

