#ifndef READ_TAGS_H
#define READ_TAGS_H

#include <filesystem>
#include <set>
#include "trees/tree_interface.h"

using std::set;
namespace fs = std::filesystem;

void read_tags(fs::path music_path, BTreeData &data);

void read_album_tags(fs::path album_path, set<BTreeData> &album_pieces);

void read_fake_album_tags(fs::path album_path, set<BTreeData> &album_pieces);

#endif