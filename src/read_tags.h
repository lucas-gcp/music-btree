#ifndef READ_TAGS_H
#define READ_TAGS_H

#include <filesystem>
#include <set>
#include "piece_info.h"

using std::set;
namespace fs = std::filesystem;

void read_tags(fs::path music_path, PieceInfo &info);

void read_album_tags(fs::path album_path, set<PieceInfo> &album_pieces);

#endif