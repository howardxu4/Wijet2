EXEC = fltest
TSRC = fltest.cpp
TARGET = loadfile.a
OBJS =  LdFlist.o LdProp.o getProperty.o LdEnv.o LdConf.o LdMagic.o
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

$(EXEC): $(TSRC) $(TARGET)
	$(CC) $(LFLAGS) $(EXEC) $(TSRC) $(TARGET) $(LIBS) $(MYLIB)

$(TARGET) : $(OBJS)
	$(AR) $(TARGET) $(OBJS) 

clean:
	$(RM) $(EXEC) $(OBJS) $(TARGET)

install: $(TARGET)
	$(CP) $(TARGET) $(DIST)
