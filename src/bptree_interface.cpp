#include <filesystem>
#include <memory>
#include <string>

#include <iostream>

#include "utec/disk/btree.h"
#include "utec/disk/pagemanager.h"

#include "piece_info.h"
#include "read_tags.h"

#include "bptree_interface.h"

using std::string;
namespace fs = std::filesystem;
using namespace utec::disk;

using std::cout, std::endl;

std::ostream &operator<<(std::ostream &out, Pair &p) {
    out << p.id;
    return out;
}

BPTreeInterface::BPTreeInterface(bool ovewrite)
    : pm(std::make_shared<pagemanager>("b+tree.index", ovewrite)),
      bt(btree<Pair, BTREE_ORDER>(pm)),
      record_manager("music.bin", ovewrite)
{
    curr_page_id = 1;
}

BPTreeInterface::~BPTreeInterface() { }

void BPTreeInterface::insert(string &insert_path) {
    set<PieceInfo> album_pieces;
    read_album_tags(fs::path(insert_path), album_pieces);

    for (PieceInfo p : album_pieces) {
        Pair new_node;

        p.get_id(new_node.id);
        new_node.page_id = curr_page_id;
        cout << curr_page_id << ": " << new_node.id << endl;
        
        record_manager.save(curr_page_id, p);
        bt.insert(new_node);
        
        curr_page_id++;
    }
}

void BPTreeInterface::search(PieceInfo &p) {
    Pair to_find;
    p.get_id(to_find.id);
    cout << to_find.id << endl;
    to_find.page_id = -1;    

    auto found = bt.find(to_find);
    
    if (found != bt.end()) {
        PieceInfo recovered;
        cout << (*found).page_id;
        record_manager.recover((*found).page_id, recovered);
        std::cout<< "Name "<< recovered.name << '\n';
    } else {

    }
}

void BPTreeInterface::print_tree() {
    bt.print_tree();
}