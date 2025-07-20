/* Copyright (C) 2003 Scott Wheeler <wheeler@kde.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdio>

#include "tpropertymap.h"
#include "tstringlist.h"
#include "tvariant.h"
#include "fileref.h"
#include "tag.h"
#include "piece_info.h"

#include "read_tags.h"

using std::string;

void read_tags(char *file_name, PieceInfo &info) {

    std::cout << "******************** \"" << file_name << "\" ********************" << std::endl;

    TagLib::FileRef f(file_name);

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
                        info.composer = k->toCString();
                    else if (j->first == "WORK")
                        info.name = k->toCString();
                    else if (j->first == "CAT")
                        info.catalog = k->toCString();
                }
            }
            std::cout << info.composer << ": " << info.name << ", Op. " << info.catalog << std::endl; 
        }
    }
}