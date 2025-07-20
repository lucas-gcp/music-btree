#include "btree/btree_set.h"

#include <string>
#include <vector>

#include <filesystem>

#include <iostream>
#include <fstream>

#include "read_tags.h"
#include "piece_info.h"

using std::string;
using std::cout;
using std::endl;

int main(int argc, char *argv[]) {
    btree::btree_set<PieceInfo> bt;

    PieceInfo piece;
    read_tags(argv[1], piece);

    bt.insert(piece);
    auto found = bt.find(piece);
    cout << "Achei: " << found->name << endl;
}