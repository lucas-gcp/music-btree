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
#include "piece_info.h"

#include "read_tags.h"

using std::string;
using std::set;
namespace fs = std::filesystem;

set<string> SUPPORTED_FILE_TYPES = {".mp3", ".flac", ".ogg"};

void read_tags(fs::path music_path, PieceInfo &info) {

    // std::cout << "* \"" << music_path.string() << "\" *" << std::endl;

    TagLib::FileRef f(music_path.c_str());

    if(!f.isNull() && f.tag()) {
        TagLib::PropertyMap tags = f.properties();

        if(!tags.isEmpty()) {
            // std::cout << "-- TAG (properties) --" << std::endl;
            for(auto j = tags.cbegin(); j != tags.cend(); ++j) {
                for(auto k = j->second.begin(); k != j->second.end(); ++k) {
                    if (info.composer.length() == 0 || info.name.length() == 0 || info.catalog.length() == 0)
                    if (j->first == "COMPOSER")
                        info.composer = *k;
                    else if (j->first == "WORK")
                        info.name = *k;
                    else if (j->first == "CATALOG")
                        info.catalog = *k;
                }
            }
            if (info.name.length() != 0)
                std::cout << info.composer << ": " << info.name << ", Op. " << info.catalog << std::endl; 
        }
    }
}

bool is_music_file(fs::path file_path) {
    return (SUPPORTED_FILE_TYPES.find(file_path.extension().string()) != SUPPORTED_FILE_TYPES.end());
}

void read_album_tags(fs::path album_path, set<PieceInfo> &album_pieces) {
    for (auto const &album_entry : fs::directory_iterator{album_path}) {
        if(!album_entry.is_directory() && is_music_file(album_entry.path())) {
            PieceInfo info;
            read_tags(album_entry.path(), info);
            album_pieces.insert(info);
        }
    }
}