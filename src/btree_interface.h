#ifndef BTREE_INTERFACE_H
#define BTREE_INTERFACE_H

#include <string>
#include <filesystem>

#include "btree/btree.h"

#include "piece_info.h"

using std::string;
namespace fs = std::filesystem;

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

std::ostream &operator<<(std::ostream &out, Pair &p);

#define PAIR_SIZE (sizeof(char) * ID_SIZE + sizeof(long))

#define PAGE_SIZE   1024
#define BTREE_ORDER ((PAGE_SIZE - (5 * sizeof(long) + sizeof(Pair)) ) /  (sizeof(Pair) + sizeof(long)))

class BTreeInterface {
    public: 
        BTreeInterface(bool ovewrite);
        ~BTreeInterface();

        void scan(string &scan_path);
        void insert(string &insert_path);
        void search(PieceInfo &p);
        
        void print_tree();
        
    private:
        Apontador *arv;
        node *LISTA;

        long curr_page_id;
};

#endif