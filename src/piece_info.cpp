#include "piece_info.h"

bool operator <(PieceInfo a, PieceInfo b) {
    return a.composer + a.catalog + a.name < b.composer + b.catalog + b.name;
}

bool operator >(PieceInfo a, PieceInfo b) {
    return !(a.composer + a.catalog + a.name < b.composer + b.catalog + b.name);
}

bool operator ==(PieceInfo a, PieceInfo b) {
    return a.composer + a.catalog + a.name == b.composer + b.catalog + b.name;
}

bool operator <(const PieceInfo &a, const PieceInfo &b) {
    return a.composer + a.catalog + a.name < b.composer + b.catalog + b.name;
}

bool operator <=(const PieceInfo &a, const PieceInfo &b) {
    return (a.composer + a.catalog + a.name < b.composer + b.catalog + b.name) ||
           (a.composer + a.catalog + a.name == b.composer + b.catalog + b.name);
}

bool operator >(const PieceInfo &a, const PieceInfo &b) {
    return !(a.composer + a.catalog + a.name < b.composer + b.catalog + b.name);
}

bool operator ==(const PieceInfo &a, const PieceInfo &b) {
    return a.composer + a.catalog + a.name == b.composer + b.catalog + b.name;
}