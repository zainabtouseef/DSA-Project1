#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <functional>
#include "dir_tree.hpp"
#include "free_block_manager.hpp"
#include "odf_types.hpp" // <-- includes OFSErrorCodes, FSStats, FileEntry

class FileOperations {
private:
    DirNode* root;
    FreeBlockManager* block_manager;
    std::unordered_map<uint32_t, FileEntry>* inode_table;

public:
    FileOperations(DirNode* root_, FreeBlockManager* fbm, std::unordered_map<uint32_t, FileEntry>* table)
        : root(root_), block_manager(fbm), inode_table(table) {}

    OFSErrorCodes file_create(const std::string &path, uint64_t size);
    OFSErrorCodes file_delete(const std::string &path);
    bool file_exists(const std::string &path);

    FileMetadata get_metadata(const std::string &path);
    OFSErrorCodes set_permissions(const std::string &path, uint32_t perms);
    FSStats get_stats();

    void free_dir_tree(DirNode* node);

    // New methods
    void file_edit(const std::string &path, const std::vector<char> &data, size_t offset);
    void file_read(const std::string &path, std::vector<char> &out);
    void file_truncate(const std::string &path, size_t new_size);
    void file_rename(const std::string &old_path, const std::string &new_path);
};
