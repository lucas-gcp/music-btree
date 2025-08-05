#include <filesystem>
#include <fstream>
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
    
    ifstream composers_file("compositores.txt");
    int compositions_num = 18;

    // BENCHMARK BTREE
    fs::path B_db_path("B_benchmark_db");
    if (!fs::exists(B_db_path))
        fs::create_directory(B_db_path);

    unique_ptr<TreeStrategy> treePtr =
        make_unique<BTreeStrategy>(B_db_path);

    double b_min_insertion_time;
    double b_max_insertion_time;

    auto btree_insert_beg = chrono::high_resolution_clock::now();
    
    auto btree_album_insert_beg = chrono::high_resolution_clock::now();
    treePtr->insert_fake_dir(scan_path);
    auto btree_album_insert_end = chrono::high_resolution_clock::now();

    b_min_insertion_time = chrono::duration<double, milli>(btree_album_insert_end - btree_album_insert_beg).count();
    b_max_insertion_time = chrono::duration<double, milli>(btree_album_insert_end - btree_album_insert_beg).count();

    double current_duration;
    for (auto const &dir_entry : fs::recursive_directory_iterator{scan_path}) {
        if (dir_entry.is_directory()) {
            auto btree_album_insert_beg = chrono::high_resolution_clock::now();
            treePtr->insert_fake_dir(dir_entry.path());
            auto btree_album_insert_end = chrono::high_resolution_clock::now();

            current_duration = chrono::duration<double, milli>(btree_album_insert_end - btree_album_insert_beg).count();
            if (current_duration > b_max_insertion_time)
                b_max_insertion_time = current_duration;

            if (current_duration < b_min_insertion_time)
                b_min_insertion_time = current_duration;
        }
    }
    auto btree_insert_end = chrono::high_resolution_clock::now();


    auto btree_search_beg = chrono::high_resolution_clock::now();

    double b_min_search_time = __DBL_MAX__;
    double b_max_search_time = 0;

    string current_composer;
    while (getline(composers_file, current_composer)) {
        if (*prev(current_composer.end()) == '\n')
            current_composer = string(current_composer.begin(), prev(current_composer.end()));

        for (int i = 1; i <= compositions_num; i++) {
            auto btree_piece_search_beg = chrono::high_resolution_clock::now();

            treePtr->search(current_composer, "", to_string(i));

            auto btree_piece_search_end = chrono::high_resolution_clock::now();

            current_duration = chrono::duration<double, milli>(btree_piece_search_end - btree_piece_search_beg).count();
            if (current_duration > b_max_search_time)
                b_max_search_time = current_duration;
            
            if (current_duration < b_min_search_time)
                b_min_search_time = current_duration;
        }

    }
    auto btree_search_end = chrono::high_resolution_clock::now();


    // BENCHMARK BPTREE
    fs::path BP_db_path("BP_benchmark_db");
    if (!fs::exists(BP_db_path))
        fs::create_directory(BP_db_path);

    treePtr = make_unique<BPTreeStrategy>(BP_db_path);

    int album_count = 1;
    double bp_min_insertion_time;
    double bp_max_insertion_time;

    auto bptree_insert_beg = chrono::high_resolution_clock::now();
    
    auto bptree_album_insert_beg = chrono::high_resolution_clock::now();
    treePtr->insert_fake_dir(scan_path);
    auto bptree_album_insert_end = chrono::high_resolution_clock::now();

    bp_min_insertion_time = chrono::duration<double, milli>(bptree_album_insert_end - bptree_album_insert_beg).count();
    bp_max_insertion_time = chrono::duration<double, milli>(bptree_album_insert_end - bptree_album_insert_beg).count();


    for (auto const &dir_entry : fs::recursive_directory_iterator{scan_path}) {
        if (dir_entry.is_directory()) {
            auto bptree_album_insert_beg = chrono::high_resolution_clock::now();
            treePtr->insert_fake_dir(dir_entry.path());
            auto bptree_album_insert_end = chrono::high_resolution_clock::now();

            current_duration = chrono::duration<double, milli>(bptree_album_insert_end - bptree_album_insert_beg).count();
            if (current_duration > bp_max_insertion_time)
                bp_max_insertion_time = current_duration;

            if (current_duration < bp_min_insertion_time)
                bp_min_insertion_time = current_duration;

            album_count++;
        }
    }
    auto bptree_insert_end = chrono::high_resolution_clock::now();


    auto bptree_search_beg = chrono::high_resolution_clock::now();

    double bp_min_search_time = __DBL_MAX__;
    double bp_max_search_time = 0;

    int piece_count = 0;

    composers_file.clear();
    composers_file.seekg(0);
    while (getline(composers_file, current_composer)) {
        if (*prev(current_composer.end()) == '\n')
            current_composer = string(current_composer.begin(), prev(current_composer.end()));

        for (int i = 1; i <= compositions_num; i++) {
            auto bptree_piece_search_beg = chrono::high_resolution_clock::now();
        
            treePtr->search(current_composer, "", to_string(i));
        
            piece_count++;
            
            auto bptree_piece_search_end = chrono::high_resolution_clock::now();

            current_duration = chrono::duration<double, milli>(bptree_piece_search_end - bptree_piece_search_beg).count();
            if (current_duration > bp_max_search_time)
                bp_max_search_time = current_duration;
            
            if (current_duration < bp_min_search_time)
                bp_min_search_time = current_duration;
        }
    }
    auto bptree_search_end = chrono::high_resolution_clock::now();

    std::cout << "Album count: " << album_count << ", Piece count: " << piece_count << endl;
    std::cout << "B-Tree insertion time: " << fixed << setprecision(3) << chrono::duration<double, milli>(btree_insert_end - btree_insert_beg).count() << " ms\n"
         << "Min: " << fixed << setprecision(3) << b_min_insertion_time << " ms\n"
         << "Max: " << fixed << setprecision(3) << b_max_insertion_time << " ms\n"
         << "Mean: " << fixed << setprecision(3) << chrono::duration<double, milli>(btree_insert_end - btree_insert_beg).count()/album_count << endl;
        
    std::cout << "B-Tree search time: " << fixed << setprecision(3) << chrono::duration<double, milli>(btree_search_end - btree_search_beg).count() << " ms\n"
         << "Min: " << fixed << setprecision(3) << b_min_search_time << " ms\n"
         << "Max: " << fixed << setprecision(3) << b_max_search_time << " ms\n"
         << "Mean: " << fixed << setprecision(3) << chrono::duration<double, milli>(btree_search_end - btree_search_beg).count()/piece_count << endl;

    std::cout << "B+-Tree insertion: " << fixed << setprecision(3) << chrono::duration<double, milli>(bptree_insert_end - bptree_insert_beg).count() << " ms\n"
         << "Min: " << fixed << setprecision(3) << bp_min_insertion_time << " ms\n"
         << "Max: " << fixed << setprecision(3) << bp_max_insertion_time << " ms\n"
         << "Mean: " << fixed << setprecision(3) << chrono::duration<double, milli>(bptree_insert_end - bptree_insert_beg).count()/album_count << endl;

    std::cout << "B+-Tree search time: " << fixed << setprecision(3) << chrono::duration<double, milli>(bptree_search_end - bptree_search_beg).count() << " ms\n"
         << "Min: " << fixed << setprecision(3) << bp_min_search_time << " ms\n"
         << "Max: " << fixed << setprecision(3) << bp_max_search_time << " ms\n"
         << "Mean: " << fixed << setprecision(3) << chrono::duration<double, milli>(bptree_search_end - bptree_search_beg).count()/piece_count << endl;

    // album_count; piece_count; b_insert_total; b_insert_min; b_insert_max; b_insert_mean; b_search_total; b_search_min; b_search_max; b_search_mean;
    // album_count; piece_count; bt_insert_total; bt_insert_min; bt_insert_max; bt_insert_mean; bt_search_total; bt_search_min; bt_search_max; bt_search_mean;
    if (!fs::exists("benchmarks"))
        fs::create_directory("benchmarks");
    
    ofstream csv_out("benchmarks/benchmarks_b.csv", std::ofstream::app);

    csv_out << album_count << ";" << piece_count << ";" << chrono::duration<double, milli>(btree_insert_end - btree_insert_beg).count() << ";"
            << b_min_insertion_time << ";" << b_max_insertion_time << ";" << chrono::duration<double, milli>(btree_insert_end - btree_insert_beg).count()/album_count << ";"
            << chrono::duration<double, milli>(btree_search_end - btree_search_beg).count() << ";" << b_min_search_time << ";" << b_max_search_time << ";"
            << chrono::duration<double, milli>(btree_search_end - btree_search_beg).count()/piece_count << endl;

    csv_out.close();

    csv_out.open("benchmarks/benchmarks_bt.csv", std::ofstream::app);

    csv_out << album_count << ";" << piece_count << ";" << chrono::duration<double, milli>(bptree_insert_end - bptree_insert_beg).count() << ";"
            << bp_min_insertion_time << ";" << bp_max_insertion_time << ";" << chrono::duration<double, milli>(bptree_insert_end - bptree_insert_beg).count()/album_count << ";"
            << chrono::duration<double, milli>(bptree_search_end - bptree_search_beg).count() << ";" << bp_min_search_time << ";" << bp_max_search_time << ";"
            << chrono::duration<double, milli>(bptree_search_end - bptree_search_beg).count()/piece_count << endl;
            
    return 0;
}
