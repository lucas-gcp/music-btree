#include <iostream>
#include <iomanip>
#include <string>
#include <set>
#include <filesystem>

#include "tpropertymap.h"
#include "tstringlist.h"
#include "tvariant.h"
#include "fileref.h"
#include "tag.h"
#include "./trees/tree_interface.h"

#include "read_tags.h"

using std::string;
using std::set;
namespace fs = std::filesystem;

set<string> SUPPORTED_FILE_TYPES = {".mp3", ".flac", ".ogg"};

void read_tags(fs::path music_path, BTreeData &data) {
    TagLib::FileRef f(music_path.c_str());

    if(!f.isNull() && f.tag()) {
        TagLib::PropertyMap tags = f.properties();

        if(!tags.isEmpty()) {
            for(auto j = tags.cbegin(); j != tags.cend(); ++j) {
                for(auto k = j->second.begin(); k != j->second.end(); ++k) {
                    if (strlen(data.composer) == 0 || strlen(data.piece_name) == 0 || strlen(data.catalog) == 0) {
                        if (j->first == "COMPOSER")
                            strcpy(data.composer, k->toCString());
                        else if (j->first == "WORK")
                            strcpy(data.piece_name, k->toCString());
                        else if (j->first == "CATALOG")
                            strcpy(data.catalog, k->toCString());
                    }
                }
            }
        }
    }
}

bool is_music_file(fs::path file_path) {
    return (SUPPORTED_FILE_TYPES.find(file_path.extension().string()) != SUPPORTED_FILE_TYPES.end());
}

void read_album_tags(fs::path album_path, set<BTreeData> &album_pieces) {
    for (auto const &album_entry : fs::directory_iterator{album_path}) {
        if(!album_entry.is_directory() && is_music_file(album_entry.path())) {
            BTreeData data;
            read_tags(album_entry.path(), data);
            album_pieces.insert(data);
        }
    }
}