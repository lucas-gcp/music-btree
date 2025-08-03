#ifndef TREE_INTERFACE_H
#define TREE_INTERFACE_H

// Strategy interface

#include <string>
#include <cstring>

// Struct that we'll store in the B-tree
#define COMPOSER_SIZE 64
#define PIECE_NAME_SIZE 64
#define CATALOG_SIZE 8

struct BTreeData {
    char composer[COMPOSER_SIZE];
    char piece_name[PIECE_NAME_SIZE];
    char catalog[CATALOG_SIZE];

    BTreeData() {
        std::memset(composer, 0, sizeof(composer));
        std::memset(piece_name, 0, sizeof(piece_name));
        std::memset(catalog, 0, sizeof(catalog));
    }

    BTreeData(const std::string &c, const std::string &p, const std::string &cat) {
        std::strncpy(composer, c.c_str(), sizeof(composer) - 1);
        composer[sizeof(composer) - 1] = '\0';
        std::strncpy(piece_name, p.c_str(), sizeof(piece_name) - 1);
        piece_name[sizeof(piece_name) - 1] = '\0';
        std::strncpy(catalog, cat.c_str(), sizeof(catalog) - 1);
        catalog[sizeof(catalog) - 1] = '\0';
    }

    // Comparison operator for std::set
    bool operator<(const BTreeData &other) const {
        int comp_result = std::strcmp(composer, other.composer);
        if (comp_result != 0)
            return comp_result < 0;

        comp_result = std::strcmp(piece_name, other.piece_name);
        if (comp_result != 0)
            return comp_result < 0;

        return std::strcmp(catalog, other.catalog) < 0;
    }
};

class TreeStrategy {
public:
    virtual ~TreeStrategy() = default;
    virtual void insert_tree(BTreeData data) = 0;
    virtual BTreeData search_tree(std::string composer, std::string piece_name, std::string catalog) = 0;
};

#endif // TREE_INTERFACE_H