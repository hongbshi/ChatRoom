CLIBS = -L./../lib/ -lTcpProxy -lpthread

INCLUDE_DIRS = -I./../src/

SOURCES = $(wildcard ./*.cc)

INCLUDES = $(wildcard ./../src/*.h)

BIN_DIRS = ./../bin/

OBJECTS = $(patsubst %.cc, %.o, $(SOURCES))

TARGET = $(patsubst %.cc, %, $(SOURCES))

ALL : $(TARGET)

$(TARGET) : % : %.o $(INCLUDES)
	$(CC) $< -o $@ $(CLIBS)
	cp $@ $(BIN_DIRS)

$(OBJECTS) : %.o : %.cc $(INCLUDES)
	$(CC) -c $(CFLAGS) $< -o $@ $(INCLUDE_DIRS)

.PHONY : clean

clean:  
	rm -rf $(OBJECTS) $(TARGET) $(BIN_DIRS)/*Test
