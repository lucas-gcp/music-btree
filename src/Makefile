CPP = g++ -O3 -std=c++20
INCLUDES = -I../taglib/include/ -I../taglib/include/taglib -Itrees/bptree
LIB = -L../taglib/lib -ltag -lz

all: program

program: bptree read_tags.o main.o
	$(CPP) $(INCLUDES) property.o read_tags.o main.o -o program $(LIB)

read_tags.o: read_tags.cpp read_tags.h
	$(CPP) $(INCLUDES) -c read_tags.cpp

bptree: trees/bptree/bplustree.hpp property.o

property.o: trees/bptree/property.cpp
	$(CPP) $(INCLUDES) -c trees/bptree/property.cpp

main.o: main.cpp trees/bptree/bplustree.hpp
	$(CPP) $(INCLUDES) -c main.cpp