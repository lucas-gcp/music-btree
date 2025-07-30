#ifndef PIECE_INFO_H
#define PIECE_INFO_H

#include "tstring.h"

#ifndef ID_SIZE
#define ID_SIZE 40
#endif

struct {
    TagLib::String composer;
    TagLib::String name;
    TagLib::String catalog;

    void get_id(char id[ID_SIZE]) {
        unsigned i;
        for (i = 0; i < composer.length() && i < ID_SIZE - 1; i++) {
            id[i] = composer[i];
        }
        for (; (i - composer.length()) < catalog.length() && i < ID_SIZE - 1; i++) {
            id[i] = catalog[i - composer.length()];
        }
        id[i] = '\0';
    }
    
} typedef PieceInfo;

bool operator <(const PieceInfo &a, const PieceInfo &b);
bool operator <=(const PieceInfo &a, const PieceInfo &b);
bool operator >(const PieceInfo &a, const PieceInfo &b);
bool operator ==(const PieceInfo &a, const PieceInfo &b);

#endif