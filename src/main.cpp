#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <set>

#include "trees/tree_interface.h"
#include "trees/btree/btree.h"
#include "trees/bptree/bplustree.hpp"
#include "read_tags.h"

using namespace std;
namespace fs = filesystem;

int main(int argc, char **argv) {
    fs::path db_path("music_db");
    if (!fs::exists(db_path))
        fs::create_directory(db_path);

    unique_ptr<TreeStrategy> treePtr =
        make_unique<BTreeStrategy>(db_path);

    string op;
    do {
        getline(cin, op);

        if (op == "scan") {
            string scan_path;
            cout << "Dir: " << flush;
            getline(cin, scan_path);

            // scan(scan_path);
        }
        else if (op == "insert") {
            string insert_path;
            cout << "Dir: " << flush;
            getline(cin, insert_path);

            set<BTreeData> to_insert;

            read_album_tags(insert_path, to_insert);

            for (const auto &data : to_insert) {
                treePtr->insert_tree(data, insert_path);

                cout << "Piece name: " << data.piece_name << "\n"
                     << "Composer: " << data.composer << "\n"
                     << "Catalog number: " << data.catalog << "\n";
            }
            cout << flush;
        } else if (op == "search") {
            string composer, piece_name, catalog;

            cout << "Composer: " << flush;
            getline(cin, composer);

            cout << "Piece name: " << flush;
            getline(cin, piece_name);

            cout << "Catalog number: " << flush;
            getline(cin, catalog);

            BTreeData found = treePtr->search_tree(composer, piece_name, catalog);
            
            if (strcmp(found.catalog, "0") != 0) {
                cout << "Found: " << found.piece_name << endl;
                cout << "Recordings: " << endl;
                treePtr->get_recordings(found);
            } else {
                cout << "Piece not on database" << endl;
            }
        } else if (op == "exit") {
            break;
        } else if (op == "help") {
            cout << "Commands:\n"
                 << "scan: scans (recursively) all files and subdirectories of dir.\n"
                 << "insert: insert all files of dir in the database.\n"
                 << "search: finds a piece of music using composer, piece name and catalog information.\n"
                 << "exit: stop execution.\n"
                 << "help: show information about all avaliable commands." << endl;
        }
        else {
            cout << "Valid operations: scan, insert, search, exit, help" << endl;
        }
    } while (op != "exit");

    return 0;
}
