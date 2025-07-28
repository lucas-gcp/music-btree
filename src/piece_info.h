#ifndef PIECE_INFO_H
#define PIECE_INFO_H

#include "tstring.h"

struct {
    TagLib::String composer;
    TagLib::String name;
    TagLib::String catalog;
} typedef PieceInfo;

bool operator <(const PieceInfo &a, const PieceInfo &b);
bool operator <=(const PieceInfo &a, const PieceInfo &b);
bool operator >(const PieceInfo &a, const PieceInfo &b);
bool operator ==(const PieceInfo &a, const PieceInfo &b);

#endif