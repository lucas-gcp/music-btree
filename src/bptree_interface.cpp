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


BPTreeInterface::BPTreeInterface()
    : pm(std::make_shared<pagemanager>("b+tree.index", false)),
      bt(btree<Pair, BTREE_ORDER>(pm)),
      record_manager("music.bin", false)
{
    curr_page_id = 0;
}

BPTreeInterface::~BPTreeInterface() { }

void BPTreeInterface::insert(string &insert_path) {
    set<PieceInfo> album_pieces;
    read_album_tags(fs::path(insert_path), album_pieces);

    cout << "oi" << endl;

    for (PieceInfo p : album_pieces) {
        Pair new_node;

        unsigned i;
        for (i = 0; i < p.composer.length() && i < ID_SIZE - 1; i++) {
            new_node.id[i] = p.composer[i];
        }
        for (; (i - p.composer.length()) < p.catalog.length() && i < ID_SIZE - 1; i++) {
            new_node.id[i] = p.catalog[i];
        }
        new_node.id[i] = '\0';

        bt.insert(new_node);
    }
}