#include <string>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "../read_tags.h"

using namespace std;
namespace fs = filesystem;

void TreeStrategy::insert_dir(fs::path insert_path) {
    set<BTreeData> to_insert;

    read_album_tags(insert_path, to_insert);

    for (const auto &data : to_insert) {
        insert(data, insert_path);

        cout << "Piece name: " << data.piece_name << "\n"
                << "Composer: " << data.composer << "\n"
                << "Catalog number: " << data.catalog << "\n";
    }
    cout << flush;
}

void TreeStrategy::insert_fake_dir(fs::path insert_path) {
    set<BTreeData> to_insert;

    read_fake_album_tags(insert_path, to_insert);

    for (const auto &data : to_insert) {
        insert(data, insert_path);

        #if defined (VERBOSE)
        cout << "Piece name: " << data.piece_name << "\n"
                << "Composer: " << data.composer << "\n"
                << "Catalog number: " << data.catalog << "\n";
        #endif
    }
    cout << flush;
}

void TreeStrategy::insert_recording(BTreeData data, fs::path insert_path) {
    if (!fs::exists(db_path / data.composer))
        fs::create_directory(db_path / data.composer);
    
    fs::path piece_file_path = db_path / data.composer / (data.catalog + string(" ") + data.piece_name);
    ofstream piece_file(piece_file_path, ofstream::app);
    piece_file << fs::absolute(insert_path) << endl;
    piece_file.close();
}

void TreeStrategy::get_recordings(BTreeData data) {
    fs::path piece_file_path = db_path / data.composer / (data.catalog + string(" ") + data.piece_name);
    ifstream piece_file(piece_file_path, ofstream::app);
    
    if (piece_file.is_open())
        cout << piece_file.rdbuf();

    piece_file.close();
}