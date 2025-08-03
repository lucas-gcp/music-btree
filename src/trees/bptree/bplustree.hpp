//
// Created by juan diego on 9/7/23.
//

#ifndef B_PLUS_TREE_BPLUSTREE_HPP
#define B_PLUS_TREE_BPLUSTREE_HPP


#include <vector>
#include <functional>
#include <queue>
#include <string>
#include <cstring>
#include <iostream>

#include "data_page.hpp"
#include "index_page.hpp"

#include "../tree_interface.h"


template <
    typename FieldType,
    typename RecordType,
    typename Compare = std::greater<FieldType>,
    typename FieldMapping = std::function<FieldType(RecordType&)>
> class BPlusTree {

    friend struct Page<TYPES()>;
    friend struct DataPage<TYPES()>;
    friend struct IndexPage<TYPES()>;

private:

    std::fstream b_plus_index_file;
    std::fstream metadata_file;

    Property properties;
    Compare gt;
    FieldMapping get_search_field;

    auto create_index() -> void;

    auto locate_data_page(const FieldType &key) -> std::streampos;

    auto insert(std::streampos seek_page, PageType type, RecordType &record) -> InsertResult;

    auto remove(std::streampos seek_page, PageType type, const FieldType &key) -> RemoveResult<FieldType>;

public:

    explicit BPlusTree(Property property, FieldMapping search_field, Compare greater = Compare());

    auto insert(RecordType &record) -> void;

    auto remove(const FieldType &key) -> void;

    auto search(const FieldType &key) -> std::vector<RecordType>;

    auto above(const FieldType &lower_bound) -> std::vector<RecordType>;

    auto below(const FieldType &upper_bound) -> std::vector<RecordType>;

    auto between(const FieldType &lower_bound, const FieldType &upper_bound) -> std::vector<RecordType>;
};


#include "bplustree.tpp"

struct IndexData {
    char composer[COMPOSER_SIZE];
    char catalog[CATALOG_SIZE];

    IndexData() {
        std::memset(composer, 0, sizeof(composer));
        std::memset(catalog, 0, sizeof(catalog));
    }

    IndexData(const char *c, const char *p, const char *cat) {
        std::strncpy(composer, c, sizeof(composer) - 1);
        composer[sizeof(composer) - 1] = '\0';
        std::strncpy(catalog, cat, sizeof(catalog) - 1);
        catalog[sizeof(catalog) - 1] = '\0';
    }

    bool operator>(const IndexData& other) const {
        int comp_result = std::strcmp(composer, other.composer);
        if (comp_result != 0) return comp_result > 0;
        
        comp_result = std::strcmp(catalog, other.catalog);
        if (comp_result != 0) return comp_result > 0;
        
        return 0;
    }
};


class BPTreeStrategy : public TreeStrategy {
private:
    const Property props;

    const std::function<IndexData (BTreeData &)> index_by_data = [](BTreeData &piece) -> IndexData {
        IndexData data = {piece.composer, piece.piece_name, piece.catalog};
        return data;
    };

    BPlusTree<IndexData, BTreeData> btree;
public:
    
    BPTreeStrategy()
        : props(
        "./index/bptree/",
        "metadata_index_by_id_1",
        "index_by_id_1",
        get_expected_index_page_capacity<IndexData>(),
        get_expected_data_page_capacity<BTreeData>(),
        true
    ),
    btree(props, index_by_data)
    {}

    void insert_tree(BTreeData data) override {
        btree.insert(data);
    }


    BTreeData search_tree(std::string composer, std::string piece_name, std::string catalog) override {
        return btree.search(IndexData{composer.c_str(), piece_name.c_str(), catalog.c_str()})[0];
    }
};

#endif //B_PLUS_TREE_BPLUSTREE_HPP
