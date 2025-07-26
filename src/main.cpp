#include "btree/btree_set.h"

#include <windows.h>

#include <string>
#include <set>

#include <filesystem>
#include <boost/locale.hpp>
#include <iostream>
#include <fstream>

#include "read_tags.h"
#include "piece_info.h"

using namespace std;
namespace fs = std::filesystem;


int main(int argc, char *argv[]) {

    fs::path db_path = "db";
    if (!fs::exists(db_path))
        fs::create_directory(db_path);

    btree::btree_set<PieceInfo> bt;

    set<PieceInfo> album_pieces;
    fs::path album_path(argv[1]);
    read_album_tags(album_path, album_pieces);

    for (PieceInfo p : album_pieces) {
        if (!fs::exists(db_path / p.composer.toCString(true)))
            fs::create_directory(db_path / p.composer.toCString(true));
        
        fs::path piece_file_path = db_path / p.composer.toCString(true) / (p.catalog.toCString() + string(" ") + p.name.toCString(true));
        ofstream piece_file(piece_file_path, ofstream::app);
        piece_file << fs::absolute(album_path) << endl;

        bt.insert(p);
    }

    auto found = bt.find(*(album_pieces.begin()));
    cout << "Achei: " << found->name << endl;
    return 0;
}