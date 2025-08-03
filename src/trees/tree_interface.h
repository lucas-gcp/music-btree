#ifndef TREE_INTERFACE_H
#define TREE_INTERFACE_H

// Strategy interface

#include <string>
#include <cstring>
#include <filesystem>
#include <fstream>

using namespace std;
namespace fs = filesystem;

// Struct that we'll store in the B-tree
#define COMPOSER_SIZE 64
#define PIECE_NAME_SIZE 64
#define CATALOG_SIZE 8

struct BTreeData {
    char composer[COMPOSER_SIZE];
    char piece_name[PIECE_NAME_SIZE];
    char catalog[CATALOG_SIZE];

    BTreeData() {
        memset(composer, 0, sizeof(composer));
        memset(piece_name, 0, sizeof(piece_name));
        memset(catalog, 0, sizeof(catalog));
    }

    BTreeData(const string &c, const string &p, const string &cat) {
        strncpy(composer, c.c_str(), sizeof(composer) - 1);
        composer[sizeof(composer) - 1] = '\0';
        strncpy(piece_name, p.c_str(), sizeof(piece_name) - 1);
        piece_name[sizeof(piece_name) - 1] = '\0';
        strncpy(catalog, cat.c_str(), sizeof(catalog) - 1);
        catalog[sizeof(catalog) - 1] = '\0';
    }

    // Comparison operator for set
    bool operator<(const BTreeData &other) const {
        int comp_result = strcmp(composer, other.composer);
        if (comp_result != 0)
            return comp_result < 0;

        comp_result = strcmp(piece_name, other.piece_name);
        if (comp_result != 0)
            return comp_result < 0;

        return strcmp(catalog, other.catalog) < 0;
    }
};

class TreeStrategy {
public:
    TreeStrategy(fs::path db_path) {
        this->db_path = db_path;
    }

    virtual ~TreeStrategy() = default;

    virtual void insert_tree(BTreeData data, string insert_path) = 0;

    virtual BTreeData search_tree(string composer, string piece_name, string catalog) = 0;

    void insert_recording(BTreeData data, string insert_path) {
        if (!fs::exists(db_path / data.composer))
            fs::create_directory(db_path / data.composer);
        
        fs::path piece_file_path = db_path / data.composer / (data.catalog + string(" ") + data.piece_name);
        ofstream piece_file(piece_file_path, ofstream::app);
        piece_file << fs::absolute(fs::path(insert_path)) << endl;
        piece_file.close();
    }

    void get_recordings(BTreeData data) {
        fs::path piece_file_path = db_path / data.composer / (data.catalog + string(" ") + data.piece_name);
        ifstream piece_file(piece_file_path, ofstream::app);
        
        if (piece_file.is_open())
            cout << piece_file.rdbuf();

        piece_file.close();
    }

private:
    fs::path db_path;
};

#endif // TREE_INTERFACE_H