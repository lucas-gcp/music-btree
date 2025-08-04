#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <set>
#include <chrono>

#include "trees/tree_interface.h"
#include "trees/btree/btree.h"
#include "trees/bptree/bplustree.hpp"

using namespace std;
namespace fs = filesystem;


int main(int argc, char **argv) {
    if (argc != 2) {
        cout << "args: benchmark <music>/<folder>" << endl;
        exit(1);
    }

    fs::path scan_path(argv[1]);
    if (!fs::is_directory(scan_path)) {
        cout << "input is not directory" << endl;
        exit(2);
    }
    
    // BENCHMARK BTREE
    fs::path B_db_path("B_benchmark_db");
    if (!fs::exists(B_db_path))
        fs::create_directory(B_db_path);

    unique_ptr<TreeStrategy> treePtr =
        make_unique<BTreeStrategy>(B_db_path);
  
        
    auto btree_insert_beg = chrono::high_resolution_clock::now();

    treePtr->insert_fake_dir(scan_path);

    for (auto const &dir_entry : fs::recursive_directory_iterator{scan_path}) {
        if (dir_entry.is_directory()) {
            treePtr->insert_fake_dir(dir_entry.path());
        }
    }
    auto btree_insert_end = chrono::high_resolution_clock::now();


    // BENCHMARK BPTREE
    fs::path BP_db_path("BP_benchmark_db");
    if (!fs::exists(BP_db_path))
        fs::create_directory(BP_db_path);

    treePtr = make_unique<BPTreeStrategy>(BP_db_path);

    auto bptree_insert_beg = chrono::high_resolution_clock::now();

    treePtr->insert_fake_dir(scan_path);

    for (auto const &dir_entry : fs::recursive_directory_iterator{scan_path}) {
        if (dir_entry.is_directory()) {
            treePtr->insert_fake_dir(dir_entry.path());
        }
    }
    auto bptree_insert_end = chrono::high_resolution_clock::now();

    cout << "B-Tree insertion: " << fixed << setprecision(3) << chrono::duration<double, milli>(btree_insert_end - btree_insert_beg).count() << " ms" << endl;
    cout << "B+-Tree insertion: " << fixed << setprecision(3) << chrono::duration<double, milli>(bptree_insert_end - bptree_insert_beg).count() << " ms" << endl;
    return 0;
}
