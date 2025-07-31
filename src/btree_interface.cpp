#include "btree.h"
#include <cstdlib>
#include <malloc.h>

#include <filesystem>
#include <set>
#include <iostream>

#include "read_tags.h"

using namespace std;

#include "btree_interface.h"

BTreeInterface::BTreeInterface(bool ovewrite) {
    arv = (Apontador*) malloc(sizeof(Apontador));
    node *LISTA = (node*) malloc(sizeof(node));

    Inicializa(arv);
    inicia(LISTA);

    if (file_exists(namefile)){
        recuperarReg(arv, LISTA);
    }
}

BTreeInterface::~BTreeInterface() {
    free(arv);
    free(LISTA);
}


void BTreeInterface::insert(string &insert_path) {
    set<PieceInfo> album_pieces;
    read_album_tags(fs::path(insert_path), album_pieces);

    for (PieceInfo p : album_pieces) {
        Pair new_node;

        p.get_id(new_node.id);
        new_node.page_id = curr_page_id;
        cout << curr_page_id << ": " << new_node.id << endl;
        
        
        
        curr_page_id++;
    }
}