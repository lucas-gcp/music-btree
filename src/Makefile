CPP = g++ -std=c++17
include = -ID:\pkg\include -ID:\pkg\include\taglib
lib = -LD:\pkg\lib -ltag

all: program

program: piece_info.o read_tags.o main.o
	$(CPP) $(include) piece_info.o read_tags.o btree_interface.o main.o -o program $(lib)

piece_info.o: piece_info.cpp piece_info.h
	$(CPP) $(include) -c piece_info.cpp

read_tags.o: read_tags.cpp read_tags.h
	$(CPP) $(include) -c read_tags.cpp

btree_interface.o: btree_interface.cpp btree_interface.h
	$(CPP) $(include) -c btree_interface.cpp

main.o: main.cpp
	$(CPP) $(include) -c main.cpp