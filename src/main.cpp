#include <iostream>
#include <string>

#include "btree_interface.h"

using namespace std;

int main() {
    BTreeInterface interface();
    
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

            // insert(insert_path);

        } else if (op == "search") {
            string composer, piece_name, catalog;
            cout << "Composer: " << flush;
            getline(cin, composer);

            cout << "Piece name: " << flush;
            getline(cin, piece_name);

            cout << "Catalog number: " << flush;
            getline(cin, catalog);

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
}