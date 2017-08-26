LIB_DIR = ./../lib/  

SOURCES = $(wildcard ./*.cc)

INCLUDES = $(wildcard ./*.h)

INCLUDE_DIRS = -I./   

TARGET = libTcpProxy.a 

OBJECTS = $(patsubst %.cc, %.o, $(SOURCES))  

$(TARGET) : $(OBJECTS) $(INCLUDES) 
	$(AR) $(ARFLAG) $@ $^  
	cp $@ $(LIB_DIR)  

$(OBJECTS) : %.o : %.cc $(INCLUDES) 
	$(CC) -c $(CFLAGS) $< -o $@ $(INCLUDE_DIRS)  

.PHONY : clean

clean:  
	rm -rf $(TARGET) $(OBJECTS) $(LIB_DIR)/*.a
