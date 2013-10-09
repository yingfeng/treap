all: test 

test: rice_coding.o pfor_coding.o s9_coding.o vbyte_coding.o s16_coding.o coding_factory.o rice_coding2.o  unpack.o
		ar -rcs ../lib/encoding.a *.o

#	g++ $^

rice_coding.o: rice_coding.cpp
	g++ -c -O2 rice_coding.cpp

pfor_coding.o: pfor_coding.cpp
	g++ -c -O2 pfor_coding.cpp

s9_coding.o: s9_coding.cpp
	g++ -c -O2 s9_coding.cpp

s16_coding.o: s16_coding.cpp
	g++ -c -O2 s16_coding.cpp

vbyte_coding.o: vbyte_coding.cpp
	g++ -c -O2 vbyte_coding.cpp

coding_factory.o: coding_factory.cpp
	g++ -c -O2 coding_factory.cpp

rice_coding2.o: rice_coding2.cpp
	g++ -c -O2 rice_coding2.cpp

unpack.o: unpack.cpp
	g++ -c -O2 unpack.cpp
clean:
	rm -f *.o inter/* coding/*  test/*

