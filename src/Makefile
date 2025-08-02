CPP = g++ -std=c++17
INCLUDES = -I/opt/homebrew/Cellar/taglib/2.1.1/include -I/opt/homebrew/Cellar/taglib/2.1.1/include/taglib
LIB = -L/opt/homebrew/Cellar/taglib/2.1.1/lib -ltag

all: program

program: btree.o bptree.o read_tags.o main.o
	$(CPP) $(INCLUDES) btree.o bptree.o read_tags.o main.o -o program $(LIB)

read_tags.o: read_tags.cpp read_tags.h
	$(CPP) $(INCLUDES) -c read_tags.cpp

btree.o: ./trees/btree/btree.cpp ./trees/btree/btree.h
	$(CPP) $(INCLUDES) -c ./trees/btree/btree.cpp

bptree.o: ./trees/bptree/bptree.cpp ./trees/bptree/bptree.h
	$(CPP) $(INCLUDES) -c ./trees/bptree/bptree.cpp

main.o: main.cpp
	$(CPP) $(INCLUDES) -c main.cpp