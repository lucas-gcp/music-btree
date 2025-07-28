#include "utec/disk/btree.h"
#include "utec/disk/pagemanager.h"

#include <string>
#include <set>

#include <filesystem>
#include <memory>

#include <boost/locale.hpp>
#include <iostream>
#include <fstream>

#include "read_tags.h"
#include "piece_info.h"

using namespace std;
namespace fs = std::filesystem;

using namespace utec::disk;

struct Pair {
    long id;
    long page_id;

    bool operator<(const Pair &p) const {
        if (this->id < p.id) return true;
        else return false;
    }

    bool operator<=(const Pair &p) const {
        if (this->id <= p.id) return true;
        else return false;
    }

    bool operator==(const Pair &p) const {
        if (this->id == p.id) return true;
        else return false;
    }
};

#define PAGE_SIZE  1024
#define BTREE_ORDER   ((PAGE_SIZE - (5 * sizeof(long) + sizeof(Pair)) ) /  (sizeof(Pair) + sizeof(long)))


int main(int argc, char *argv[]) {

    fs::path db_path = "db";
    if (!fs::exists(db_path))
        fs::create_directory(db_path);
    
    std::shared_ptr<pagemanager> pm = std::make_shared<pagemanager>("b+tree.index", true);
    btree<Pair, BTREE_ORDER> bt(pm);
    
    pagemanager record_manager ("music.bin", true);

    fs::path library_dir(argv[1]);

    // for (auto const &dir_entry : fs::recursive_directory_iterator{library_dir}) {
    //     set<PieceInfo> album_pieces;
    //     if (dir_entry.is_directory()) {
    //         read_album_tags(dir_entry.path(), album_pieces);
            
    //         for (PieceInfo p : album_pieces) {
    //             if (!fs::exists(db_path / p.composer.toCString(true)))
    //                 fs::create_directory(db_path / p.composer.toCString(true));
                
    //             fs::path piece_file_path = db_path / p.composer.toCString(true) / (p.catalog.toCString() + string(" ") + p.name.toCString(true));
    //             ofstream piece_file(piece_file_path, ofstream::app);
    //             piece_file << fs::absolute(dir_entry.path()) << endl;
                
    //             bt.insert(p);
    //         }
    //     }
    // }

    PieceInfo test;
    test.composer = "Ludwig van Beethoven";
    test.name = "Piano Sonata No. 15 in D major";
    test.catalog = "28";

    long page_id;

    page_id = 1;
    record_manager.save(page_id, test); //principal
    bt.insert(Pair{10, page_id});  //index file


    auto found = bt.find(Pair{10, -1});
    auto pair = *found;
    PieceInfo s;
    record_manager.recover(pair.page_id, s);
    std::cout<< "Name "<< s.name <<  '\n';
    return 0;
}