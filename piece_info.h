#ifndef PIECE_INFO_H
#define PIECE_INFO_H

#include <string>

using std::string;

struct {
    string composer;
    string name;
    string catalog;
} typedef PieceInfo;

bool operator <(PieceInfo a, PieceInfo b);
bool operator >(PieceInfo a, PieceInfo b);
bool operator ==(PieceInfo a, PieceInfo b);

#endif