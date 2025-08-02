#ifndef BPTREE_H
#define BPTREE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include <cstring>
#include <type_traits>

#include "../tree_interface.h"


template<typename ValueType>
class PersistentBTree {
    // C++11/14 compatible static_assert
    static_assert(std::is_trivially_copyable<ValueType>::value, 
                  "ValueType must be trivially copyable for binary serialization");
    
private:
    static constexpr int MIN_DEGREE = 20;
    static constexpr int MAX_KEYS = 2 * MIN_DEGREE - 1;
    static constexpr int MAX_CHILDREN = 2 * MIN_DEGREE;
    static constexpr size_t NODE_SIZE = 4096;
    static constexpr size_t METADATA_SIZE = 32;
    
    struct Node {
        bool is_leaf;
        int num_keys;
        std::vector<std::string> keys;
        std::vector<ValueType> values;
        std::vector<long> children;
        long file_position;
        bool dirty;
        
        Node(bool leaf = true) : is_leaf(leaf), num_keys(0), 
                                file_position(-1), dirty(true) {
            keys.reserve(MAX_KEYS);
            values.reserve(MAX_KEYS);
            if (!leaf) {
                children.reserve(MAX_CHILDREN);
            }
        }
    };
    
    std::string filename;
    mutable std::fstream file;
    std::unique_ptr<Node> root;
    long root_position;
    long next_free_position;
    
public:
    explicit PersistentBTree(const std::string& db_filename) 
        : filename(db_filename), root_position(-1) {
        
        // Try to open existing file
        file.open(filename.c_str(), std::ios::in | std::ios::out | std::ios::binary);
        
        if (!file.is_open() || file.peek() == std::ifstream::traits_type::eof()) {
            // Create new file or file is empty
            file.close();
            file.open(filename.c_str(), std::ios::out | std::ios::binary);
            file.close();
            file.open(filename.c_str(), std::ios::in | std::ios::out | std::ios::binary);
            
            // Initialize with empty root
            next_free_position = METADATA_SIZE;
            root = std::unique_ptr<Node>(new Node(true));
            root->file_position = allocate_node_position();
            root_position = root->file_position;
            write_metadata();
            write_node(root.get());
        } else {
            // Load existing database
            read_metadata();
            if (root_position >= 0) {
                root = read_node(root_position);
            } else {
                // Corrupted metadata, create new root
                next_free_position = METADATA_SIZE;
                root = std::unique_ptr<Node>(new Node(true));
                root->file_position = allocate_node_position();
                root_position = root->file_position;
                write_metadata();
                write_node(root.get());
            }
        }
    }
    
    ~PersistentBTree() {
        try {
            if (file.is_open()) {
                if (root && root->dirty) {
                    write_node(root.get());
                }
                write_metadata();
                file.close();
            }
        } catch (...) {
            // Ignore exceptions in destructor
        }
    }
    
    bool insert(const std::string& key, const ValueType& value) {
        if (root->num_keys == MAX_KEYS) {
            // Root is full, need to split
            std::unique_ptr<Node> new_root(new Node(false));
            new_root->file_position = allocate_node_position();
            new_root->children.push_back(root->file_position);
            
            split_child(new_root.get(), 0, root.get());
            root = std::move(new_root);
            root_position = root->file_position;
            write_metadata();
        }
        
        return insert_non_full(root.get(), key, value);
    }
    
    // Changed to return by value with a success indicator
    bool search(const std::string& key, ValueType& result) {
        if (!root) return false;
        return search_node(root.get(), key, result);
    }
    
    // Convenience function that returns a copy
    ValueType search(const std::string& key) {
        ValueType result;
        if (search(key, result)) {
            return result;
        }
        return ValueType(); // Return default-constructed value if not found
    }
    
    void print_tree() {
        if (root) {
            print_node(root.get(), 0);
        } else {
            std::cout << "Tree is empty" << std::endl;
        }
    }
    
    // Iterator-style function to get all key-value pairs
    void get_all_pairs(std::vector<std::pair<std::string, ValueType> >& pairs) {
        pairs.clear();
        if (root) {
            collect_pairs(root.get(), pairs);
        }
    }

private:
    long allocate_node_position() {
        long pos = next_free_position;
        next_free_position += NODE_SIZE;
        return pos;
    }
    
    void deallocate_node_position(long position) {
        // Simple implementation - just mark as unused
    }
    
    void write_metadata() {
        if (!file.is_open()) return;
        
        file.seekp(0);
        file.write(reinterpret_cast<const char*>(&root_position), sizeof(root_position));
        file.write(reinterpret_cast<const char*>(&next_free_position), sizeof(next_free_position));
        
        // Write type information for validation
        size_t value_type_size = sizeof(ValueType);
        file.write(reinterpret_cast<const char*>(&value_type_size), sizeof(value_type_size));
        
        // Write a simple type hash for basic validation
        size_t type_hash = typeid(ValueType).hash_code();
        file.write(reinterpret_cast<const char*>(&type_hash), sizeof(type_hash));
        
        file.flush();
    }
    
    void read_metadata() {
        if (!file.is_open()) return;
        
        file.seekg(0);
        if (!file.read(reinterpret_cast<char*>(&root_position), sizeof(root_position))) {
            root_position = -1;
            next_free_position = METADATA_SIZE;
            return;
        }
        
        if (!file.read(reinterpret_cast<char*>(&next_free_position), sizeof(next_free_position))) {
            next_free_position = METADATA_SIZE;
        }
        
        // Read and validate type information
        size_t stored_value_type_size;
        if (file.read(reinterpret_cast<char*>(&stored_value_type_size), sizeof(stored_value_type_size))) {
            if (stored_value_type_size != sizeof(ValueType)) {
                std::cerr << "Warning: Value type size mismatch. Expected " 
                         << sizeof(ValueType) << ", got " << stored_value_type_size << std::endl;
            }
            
            size_t stored_type_hash;
            if (file.read(reinterpret_cast<char*>(&stored_type_hash), sizeof(stored_type_hash))) {
                size_t current_type_hash = typeid(ValueType).hash_code();
                if (stored_type_hash != current_type_hash) {
                    std::cerr << "Warning: Value type hash mismatch. Data may be incompatible." << std::endl;
                }
            }
        }
        
        // Validate metadata
        if (root_position < METADATA_SIZE && root_position != -1) {
            root_position = -1;
            next_free_position = METADATA_SIZE;
        }
    }
    
    void serialize_node(const Node* node, char* buffer) {
        size_t offset = 0;
        
        // Write basic node info
        std::memcpy(buffer + offset, &node->is_leaf, sizeof(node->is_leaf));
        offset += sizeof(node->is_leaf);
        
        std::memcpy(buffer + offset, &node->num_keys, sizeof(node->num_keys));
        offset += sizeof(node->num_keys);
        
        // Write keys and values
        for (int i = 0; i < node->num_keys && i < MAX_KEYS; ++i) {
            // Write key
            uint32_t key_len = static_cast<uint32_t>(
                std::min(node->keys[i].length(), size_t(255))
            );
            
            if (offset + sizeof(key_len) + key_len + sizeof(ValueType) >= NODE_SIZE) break;
            
            std::memcpy(buffer + offset, &key_len, sizeof(key_len));
            offset += sizeof(key_len);
            
            std::memcpy(buffer + offset, node->keys[i].c_str(), key_len);
            offset += key_len;
            
            // Write value (binary copy of struct)
            std::memcpy(buffer + offset, &node->values[i], sizeof(ValueType));
            offset += sizeof(ValueType);
        }
        
        // Write children positions
        if (!node->is_leaf && offset + sizeof(uint32_t) + sizeof(long) * node->children.size() < NODE_SIZE) {
            uint32_t children_count = static_cast<uint32_t>(node->children.size());
            std::memcpy(buffer + offset, &children_count, sizeof(children_count));
            offset += sizeof(children_count);
            
            for (size_t i = 0; i < node->children.size() && offset + sizeof(long) < NODE_SIZE; ++i) {
                std::memcpy(buffer + offset, &node->children[i], sizeof(long));
                offset += sizeof(long);
            }
        }
    }
    
    std::unique_ptr<Node> deserialize_node(const char* buffer, long position) {
        std::unique_ptr<Node> node(new Node());
        node->file_position = position;
        node->dirty = false;
        
        size_t offset = 0;
        
        // Read basic node info
        if (offset + sizeof(node->is_leaf) > NODE_SIZE) return std::unique_ptr<Node>();
        std::memcpy(&node->is_leaf, buffer + offset, sizeof(node->is_leaf));
        offset += sizeof(node->is_leaf);
        
        if (offset + sizeof(node->num_keys) > NODE_SIZE) return std::unique_ptr<Node>();
        std::memcpy(&node->num_keys, buffer + offset, sizeof(node->num_keys));
        offset += sizeof(node->num_keys);
        
        // Validate num_keys
        if (node->num_keys < 0 || node->num_keys > MAX_KEYS) {
            node->num_keys = 0;
            return node;
        }
        
        // Read keys and values
        for (int i = 0; i < node->num_keys; ++i) {
            // Read key
            if (offset + sizeof(uint32_t) > NODE_SIZE) break;
            
            uint32_t key_len;
            std::memcpy(&key_len, buffer + offset, sizeof(key_len));
            offset += sizeof(key_len);
            
            if (key_len > 255 || offset + key_len > NODE_SIZE) break;
            
            std::string key(buffer + offset, key_len);
            node->keys.push_back(key);
            offset += key_len;
            
            // Read value (binary copy of struct)
            if (offset + sizeof(ValueType) > NODE_SIZE) break;
            
            ValueType value;
            std::memcpy(&value, buffer + offset, sizeof(ValueType));
            node->values.push_back(value);
            offset += sizeof(ValueType);
        }
        
        // Read children positions
        if (!node->is_leaf && offset + sizeof(uint32_t) < NODE_SIZE) {
            uint32_t children_count;
            std::memcpy(&children_count, buffer + offset, sizeof(children_count));
            offset += sizeof(children_count);
            
            if (children_count <= MAX_CHILDREN) {
                for (uint32_t i = 0; i < children_count && offset + sizeof(long) <= NODE_SIZE; ++i) {
                    long child_pos;
                    std::memcpy(&child_pos, buffer + offset, sizeof(long));
                    offset += sizeof(long);
                    node->children.push_back(child_pos);
                }
            }
        }
        
        return node;
    }
    
    void write_node(Node* node) {
        if (!node || !node->dirty || !file.is_open()) return;
        
        char buffer[NODE_SIZE];
        std::memset(buffer, 0, NODE_SIZE);
        serialize_node(node, buffer);
        
        file.seekp(node->file_position);
        file.write(buffer, NODE_SIZE);
        file.flush();
        
        node->dirty = false;
    }
    
    std::unique_ptr<Node> read_node(long position) {
        if (!file.is_open() || position < METADATA_SIZE) return std::unique_ptr<Node>();
        
        char buffer[NODE_SIZE];
        file.seekg(position);
        if (!file.read(buffer, NODE_SIZE)) {
            return std::unique_ptr<Node>();
        }
        
        return deserialize_node(buffer, position);
    }
    
    bool insert_non_full(Node* node, const std::string& key, const ValueType& value) {
        int i = node->num_keys - 1;
        
        if (node->is_leaf) {
            // Find position and check for duplicate
            while (i >= 0 && key < node->keys[i]) {
                i--;
            }
            
            if (i >= 0 && node->keys[i] == key) {
                // Update existing key
                node->values[i] = value;
                node->dirty = true;
                write_node(node);
                return true;
            }
            
            // Insert new key-value pair
            node->keys.insert(node->keys.begin() + i + 1, key);
            node->values.insert(node->values.begin() + i + 1, value);
            node->num_keys++;
            node->dirty = true;
            write_node(node);
            return true;
        } else {
            // Find child to insert into
            while (i >= 0 && key < node->keys[i]) {
                i--;
            }
            i++;
            
            if (i > 0 && i <= node->num_keys && node->keys[i-1] == key) {
                // Update existing key
                node->values[i-1] = value;
                node->dirty = true;
                write_node(node);
                return true;
            }
            
            if (i >= static_cast<int>(node->children.size())) return false;
            
            std::unique_ptr<Node> child = read_node(node->children[i]);
            if (!child) return false;
            
            // Split child if full
            if (child->num_keys == MAX_KEYS) {
                split_child(node, i, child.get());
                if (key > node->keys[i]) {
                    i++;
                    if (i >= static_cast<int>(node->children.size())) return false;
                    child = read_node(node->children[i]);
                    if (!child) return false;
                }
            }
            
            return insert_non_full(child.get(), key, value);
        }
    }
    
    void split_child(Node* parent, int index, Node* full_child) {
        std::unique_ptr<Node> new_child(new Node(full_child->is_leaf));
        new_child->file_position = allocate_node_position();
        new_child->num_keys = MIN_DEGREE - 1;
        
        // Copy second half of keys and values
        for (int j = 0; j < MIN_DEGREE - 1; j++) {
            new_child->keys.push_back(full_child->keys[j + MIN_DEGREE]);
            new_child->values.push_back(full_child->values[j + MIN_DEGREE]);
        }
        
        // Copy second half of children if not leaf
        if (!full_child->is_leaf) {
            for (int j = 0; j < MIN_DEGREE; j++) {
                new_child->children.push_back(full_child->children[j + MIN_DEGREE]);
            }
            full_child->children.resize(MIN_DEGREE);
        }
        
        // Reduce full_child size
        std::string median_key = full_child->keys[MIN_DEGREE - 1];
        ValueType median_value = full_child->values[MIN_DEGREE - 1];
        
        full_child->keys.resize(MIN_DEGREE - 1);
        full_child->values.resize(MIN_DEGREE - 1);
        full_child->num_keys = MIN_DEGREE - 1;
        
        // Insert new child into parent
        parent->children.insert(parent->children.begin() + index + 1, new_child->file_position);
        
        // Move median key up to parent
        parent->keys.insert(parent->keys.begin() + index, median_key);
        parent->values.insert(parent->values.begin() + index, median_value);
        parent->num_keys++;
        
        // Mark nodes as dirty and write them
        full_child->dirty = true;
        new_child->dirty = true;
        parent->dirty = true;
        
        write_node(full_child);
        write_node(new_child.get());
        write_node(parent);
    }
    
    // Fixed search function that returns by value copy
    bool search_node(Node* node, const std::string& key, ValueType& result) {
        if (!node) return false;
        
        int i = 0;
        while (i < node->num_keys && key > node->keys[i]) {
            i++;
        }
        
        if (i < node->num_keys && key == node->keys[i]) {
            result = node->values[i]; // Copy the value
            return true;
        }
        
        if (node->is_leaf) {
            return false; // Key not found
        }
        
        if (i >= static_cast<int>(node->children.size())) return false;
        
        std::unique_ptr<Node> child = read_node(node->children[i]);
        if (!child) return false;
        
        return search_node(child.get(), key, result);
    }
    
    void print_node(Node* node, int level) {
        if (!node) {
            std::cout << "Level " << level << ": NULL node" << std::endl;
            return;
        }
        
        std::cout << "Level " << level << ": ";
        for (int i = 0; i < node->num_keys; i++) {
            std::cout << "[" << node->keys[i] << ":" << node->values[i] << "] ";
        }
        std::cout << std::endl;
        
        if (!node->is_leaf) {
            for (size_t i = 0; i < node->children.size() && i <= static_cast<size_t>(node->num_keys); i++) {
                std::unique_ptr<Node> child = read_node(node->children[i]);
                if (child) {
                    print_node(child.get(), level + 1);
                } else {
                    std::cout << "Level " << (level + 1) << ": Failed to read child" << std::endl;
                }
            }
        }
    }
    
    void collect_pairs(Node* node, std::vector<std::pair<std::string, ValueType> >& pairs) {
        if (!node) return;
        
        if (node->is_leaf) {
            for (int i = 0; i < node->num_keys; i++) {
                pairs.push_back(std::make_pair(node->keys[i], node->values[i]));
            }
        } else {
            for (int i = 0; i < node->num_keys; i++) {
                // Collect from left child
                if (i < static_cast<int>(node->children.size())) {
                    std::unique_ptr<Node> child = read_node(node->children[i]);
                    if (child) {
                        collect_pairs(child.get(), pairs);
                    }
                }
                
                // Add current key-value pair
                pairs.push_back(std::make_pair(node->keys[i], node->values[i]));
            }
            
            // Collect from rightmost child
            if (node->num_keys < static_cast<int>(node->children.size())) {
                std::unique_ptr<Node> child = read_node(node->children[node->num_keys]);
                if (child) {
                    collect_pairs(child.get(), pairs);
                }
            }
        }
    }
};

class BTreeStrategy : public TreeStrategy {
private:
    PersistentBTree<BTreeData> btree;
public:
    BTreeStrategy():btree("person_btree.db") {}

    void insert_tree(std::string key, BTreeData data) override {
        this->btree.insert(key, data);
    }

    BTreeData search_tree(std::string key) override {
        return this->btree.search(key);
    }
};

#endif // BPTREE_H