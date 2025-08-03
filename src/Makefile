CPP = g++ -O3 -std=c++20
INCLUDES = -I../taglib/include/ -I../taglib/include/taglib -Itrees/bptree
LIB = -L../taglib/lib -ltag -lz

all: program

program: bptree read_tags.o tree_interface.o main.o
	$(CPP) $(INCLUDES) read_tags.o property.o tree_interface.o main.o -o program $(LIB)

read_tags.o: read_tags.cpp read_tags.h
	$(CPP) $(INCLUDES) -c read_tags.cpp

tree_interface.o: trees/tree_interface.cpp trees/tree_interface.h
	$(CPP) $(INCLUDES) -c trees/tree_interface.cpp

bptree: trees/bptree/bplustree.hpp property.o

property.o: trees/bptree/property.cpp
	$(CPP) $(INCLUDES) -c trees/bptree/property.cpp

main.o: main.cpp trees/bptree/bplustree.hpp
	$(CPP) $(INCLUDES) -c main.cpp