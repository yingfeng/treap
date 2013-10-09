CPP=g++

CPPFLAGS=-std=c++0x -c -O3 -Wall  -Wunused-variable #-Wno-sign-compare -Wunused-variable
CPPFLAGS2=-std=c++0x  -O3 -Wall -Wunused-variable   #-Wno-sign-compare -Wunused-variable

LIBCDS=./libcds
OBJECTS_DIR=./objects/

LIB=./lib/all_lib.a
LIB_ENCODING=./lib/encoding.a

INCLUDES_LIBCDS=-I$(LIBCDS)/includes/
INCLUDES_TREES=-I./trees/*.h
INCLUDES_ENCODING=./encoding/*.h

SRC_LIBCDS=./libdivsufsort-lite/divsufsort.c ./libcds/src/static/sequence/*.cpp ./libcds/src/static/bitsequence/*.cpp ./libcds/src/static/mapper/*.cpp ./libcds/src/static/coders/*.cpp ./libcds/src/static/permutation/*.cpp ./libcds/src/utils/*.cpp ./libcds/src/static/textindex/*.cpp ./libcds/src/static/suffixtree/*.cpp 
INCLUDES=-I. -I./trees/ -I$(LIBCDS)/includes/ -I./libdivsufsort-lite/ -I./rmq/ -I$(INCLUDES_ENCODING)
 
SRC_TREES= ./trees/*.cpp
SRC_RMQ=./rmq/rmq.cpp


program:
	$(CPP) $(CPPFLAGS2) $(INCLUDES) ./src/InvertedTest.cpp -o main lib/all_lib.a lib/encoding.a

all: 
	rm -rf ./libcds/includes/*.h
	cp ./libcds/src/static/sequence/*.h ./libcds/includes/
	cp ./libcds/src/static/bitsequence/*.h ./libcds/includes/
	cp ./libcds/src/static/mapper/*.h ./libcds/includes/	
	cp ./libcds/src/static/coders/*.h ./libcds/includes/
	cp ./libcds/src/static/permutation/*.h ./libcds/includes/
	cp ./libcds/src/utils/*.h ./libcds/includes/
	cp ./libcds/src/static/suffixtree/*.h ./libcds/includes/
	cp ./libcds/src/static/textindex/*.h ./libcds/includes/
	$(CPP) $(CPPFLAGS) $(INCLUDES)  $(SRC_LIBCDS)  $(SRC_TREES) $(SRC_RMQ)  
	rm -rf lib/all_lib.a
	ar -rcs lib/all_lib.a *.o
	rm -rf *.o
clean:
	rm -rf objects/*.o lib/*.a test  

