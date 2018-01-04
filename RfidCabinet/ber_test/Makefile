CC = gcc
XX = g++
CFLAGS = -Wall -O -I$(ORACLE_INCLUDES)  -L$(ORACLE_LIBS)
CTD = -I$(ORACLE_INCLUDES)  -L$(ORACLE_LIBS)
TARGET = rfid
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ 
%.o:%.cpp
	$(XX) $(CFLAGS) -c $< -o $@ 
SOURCES = $(wildcard *.c ./src/*.c)
OBJS = $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES)))
$(TARGET) : $(OBJS)
	$(CC) $(OBJS) $(CTD) -o $(TARGET) -lpthread  -lm -g -rdynamic 
	chmod a+x $(TARGET)
clean:
	rm -rf $(wildcard rfid *.log *.o ./src/*.o)
