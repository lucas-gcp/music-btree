#ifndef BTREE_INTERFACE_H
#define BTREE_INTERFACE_H

#include <string>
#include <filesystem>

#include "utec/disk/btree.h"
#include "utec/disk/pagemanager.h"

#include "piece_info.h"

using std::string;
namespace fs = std::filesystem;
using namespace utec::disk;

#ifndef ID_SIZE
#define ID_SIZE 40
#endif

struct Pair {
    char id[ID_SIZE]; 
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

#define PAIR_SIZE (sizeof(char) * ID_SIZE + sizeof(long))

#define PAGE_SIZE   1024
#define BTREE_ORDER ((PAGE_SIZE - (5 * sizeof(long) + sizeof(Pair)) ) /  (sizeof(Pair) + sizeof(long)))

class BPTreeInterface {
    public: 
        BPTreeInterface();
        ~BPTreeInterface();

        void scan(string &scan_path);
        void insert(string &insert_path);
        void search(PieceInfo &p);
        
    private:
        std::shared_ptr<pagemanager> pm;
        btree<Pair, BTREE_ORDER> bt;
        pagemanager record_manager;

        long curr_page_id;
};

#endif