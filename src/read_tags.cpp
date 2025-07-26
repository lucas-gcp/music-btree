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

    std::cout << "******************** \"" << music_path.string() << "\" ********************" << std::endl;

    TagLib::FileRef f(music_path.c_str());

    if(!f.isNull() && f.tag()) {

        TagLib::Tag *tag = f.tag();

        std::cout << "-- TAG (basic) --" << std::endl;
        std::cout << "title   - \"" << tag->title()   << "\"" << std::endl;
        std::cout << "artist  - \"" << tag->artist()  << "\"" << std::endl;
        std::cout << "album   - \"" << tag->album()   << "\"" << std::endl;
        std::cout << "year    - \"" << tag->year()    << "\"" << std::endl;
        std::cout << "comment - \"" << tag->comment() << "\"" << std::endl;
        std::cout << "track   - \"" << tag->track()   << "\"" << std::endl;
        std::cout << "genre   - \"" << tag->genre()   << "\"" << std::endl;

        TagLib::PropertyMap tags = f.properties();

        if(!tags.isEmpty()) {
            std::cout << "-- TAG (properties) --" << std::endl;
            for(auto j = tags.cbegin(); j != tags.cend(); ++j) {
                for(auto k = j->second.begin(); k != j->second.end(); ++k) {
                    if (j->first == "COMPOSER")
                        info.composer = *k;
                    else if (j->first == "WORK")
                        info.name = *k;
                    else if (j->first == "CATALOG")
                        info.catalog = *k;
                }
            }
            std::cout << info.composer << ": " << info.name << ", Op. " << info.catalog << std::endl; 
        }
    }
}

bool is_music_file(fs::path file_path) {
    return (SUPPORTED_FILE_TYPES.find(file_path.extension().string()) != SUPPORTED_FILE_TYPES.end());
}

void read_album_tags(fs::path album_path, set<PieceInfo> &album_pieces) {
    for (auto const &album_entry : fs::directory_iterator{album_path}) {
        if(is_music_file(album_entry.path())) {
            PieceInfo info;
            read_tags(album_entry.path(), info);
            album_pieces.insert(info);
        }
    }
}