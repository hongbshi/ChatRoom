CC = g++
AR = ar  
ARFLAG = -rcs  
CFLAGS = -std=c++11

export CC CFLAGS AR ARFLAG

SUBDIRS = ./src ./test

.PHONY : TARGET

TARGET:
	mkdir -p ./lib
	mkdir -p ./bin
	for subdir in $(SUBDIRS);\
	do $(MAKE) -C $$subdir;\
	done

.PHONY : clean

clean:  
	for subdir in $(SUBDIRS);\
	do $(MAKE) -C $$subdir clean;\
	done
	rm -r -f ./lib
	rm -r -f ./bin

