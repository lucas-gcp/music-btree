#ifndef TREE_INTERFACE_H
#define TREE_INTERFACE_H

// Strategy interface

#include <string>
#include <cstring>

// Example struct that we'll store in the B-tree
struct BTreeData {
    char composer[64];
    char piece_name[64];
    char catalog[16];
    float mean_duration;
    int num_recordings;

    BTreeData() {
        std::memset(composer, 0, sizeof(composer));
        std::memset(piece_name, 0, sizeof(piece_name));
        std::memset(catalog, 0, sizeof(catalog));
        mean_duration = 0.0f;
        num_recordings = 0;
    }

    BTreeData(const std::string& c, const std::string& p, const std::string& cat, float duration, int recordings) {
        std::strncpy(composer, c.c_str(), sizeof(composer) - 1);
        composer[sizeof(composer) - 1] = '\0';
        std::strncpy(piece_name, p.c_str(), sizeof(piece_name) - 1);
        piece_name[sizeof(piece_name) - 1] = '\0';
        std::strncpy(catalog, cat.c_str(), sizeof(catalog) - 1);
        catalog[sizeof(catalog) - 1] = '\0';
    }

    // Comparison operator for std::set
    bool operator<(const BTreeData& other) const {
        int comp_result = std::strcmp(composer, other.composer);
        if (comp_result != 0) return comp_result < 0;
        
        comp_result = std::strcmp(piece_name, other.piece_name);
        if (comp_result != 0) return comp_result < 0;

        if (mean_duration != other.mean_duration) return mean_duration < other.mean_duration;
        return num_recordings < other.num_recordings;
        
        return std::strcmp(catalog, other.catalog) < 0;
    }
};


class TreeStrategy {
public:
    virtual ~TreeStrategy() = default;
    virtual void insert_tree(std::string key, BTreeData data) = 0;
    virtual BTreeData search_tree(std::string key) = 0;
};

#endif // TREE_INTERFACE_H