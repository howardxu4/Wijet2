EXEC = vs
TSRC = videosvr.cpp
TARGET = tf
TFMAIN = tfMain.cpp
OBJS = Transfer.o
CC = g++
AR = ar cru
CFLAGS = -g -Wall -O -c
LFLAGS = -Wall -o 
LIBS = ../mynetlib.a -lpthread
MYLIB =
DIST = 

%.o : %.cpp
	$(CC) $(CFLAGS) $< -o $@


$(EXEC) : $(TSRC) $(TARGET)
	$(CC) $(LFLAGS) $(EXEC) $(TSRC) $(OBJS) $(LIBS)

$(TARGET) : $(OBJS)
	$(CC) $(LFLAGS) $(TARGET) $(TFMAIN) $(OBJS) $(LIBS)
	
clean:
	$(RM) $(EXEC) $(TARGET) $(OBJS)

