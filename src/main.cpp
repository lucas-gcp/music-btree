#include <iostream>
#include <string>
#include <set>

#include "./trees/tree_interface.h"
// #include "./trees/bptree/bptree.h"
#include "./trees/btree/btree.h"
#include "./read_tags.h"

using namespace std;

int c = 0;

int main(int argc, char **argv) {
    bool overwrite = true;
    std::unique_ptr<TreeStrategy> treePtr = 
        std::make_unique<BTreeStrategy>();

    string op; 
    do {
        getline(cin, op);
        
        if (op == "scan") {
            string scan_path;
            cout << "Dir: " << flush;
            getline(cin, scan_path);
            
            // scan(scan_path);

        } else if (op == "insert") {
            string insert_path;
            cout << "Dir: " << flush;
            getline(cin, insert_path);

            set<BTreeData> to_insert;

            read_album_tags(insert_path, to_insert);

            for (const auto& data : to_insert) {
                cout << "Piece name: " << data.piece_name << endl;
                cout << "Composer: " << data.composer << endl;
                cout << "Catalog number: " << data.catalog << endl;
                treePtr->insert_tree(std::to_string(c++), data);
            }
        } else if (op == "search") {
            string key;
            getline(cin, key);
            // string composer, piece_name, catalog;
            // cout << "Composer: " << flush;
            // getline(cin, composer);

            // cout << "Piece name: " << flush;
            // getline(cin, piece_name);

            // cout << "Catalog number: " << flush;
            // getline(cin, catalog);

            // BTreeData to_search {composer, piece_name, catalog};

            cout << treePtr->search_tree(key).piece_name << endl;

            // treePtr->search_tree(to_search);
        } else if (op == "exit") {
            break;
        } else if (op == "help") {
            cout << "Commands:\n"
                 << "scan: scans (recursively) all files and subdirectories of dir.\n"
                 << "insert: insert all files of dir in the database.\n"
                 << "search: finds a piece of music using composer, piece name and catalog information.\n"
                 << "exit: stop execution.\n"
                 << "help: show information about all avaliable commands." << endl;
        } else {
            cout << "Valid operations: scan, insert, search, exit, help" << endl;
        }
    } while (op != "exit");

    return 0;
}
