LIB_DIR = ./../lib/  

SOURCES = $(wildcard ./*.cc)

INCLUDE_DIRS = -I./   

TARGET = libTcpProxy.a 

OBJECTS = $(patsubst %.cc, %.o, $(SOURCES))  

$(TARGET) : $(OBJECTS)  
	$(AR) $(ARFLAG) $@ $^  
	cp $@ $(LIB_DIR)  

$(OBJECTS) : %.o : %.cc   
	$(CC) -c $(CFLAGS) $< -o $@ $(INCLUDE_DIRS)  

.PHONY : clean

clean:  
	rm -rf $(TARGET) $(OBJECTS) $(LIB_DIR)/*.a
