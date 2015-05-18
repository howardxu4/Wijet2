EXEC = logtest
TSRC = logtest.cpp
TARGET = logtrace.a
OBJS = Logger.o LogTrace.o 
CC = g++
AR = ar cru
CP = cp -f
CFLAGS = -g -Wall -O -c
LFLAGS = -Wall -o
LIBS =  -lpthread
DIST = ../libs

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
	
