#ifndef BTREE_INTERFACE_H
#define BTREE_INTERFACE_H

#include <string>
#include <filesystem>

#include "utec/disk/btree.h"
#include "utec/disk/pagemanager.h"

using std::string;
namespace fs = std::filesystem;
using namespace utec::disk;

struct Pair {
    char id[40];
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

#define PAGE_SIZE   1024
#define BTREE_ORDER ((PAGE_SIZE - (5 * sizeof(long) + sizeof(Pair)) ) /  (sizeof(Pair) + sizeof(long)))

class BTreeInterface {
    public: 
        BTreeInterface();
        ~BTreeInterface();

        void scan(string &scan_path);
        void insert(string &insert_path);
        
    private:
        std::shared_ptr<pagemanager> pm;
        btree<Pair, BTREE_ORDER> bt;
        pagemanager record_manager;
};

#endif