#include "../include/file_ops.hpp"
#include <algorithm>



OFSErrorCodes FileOperations::file_create(const std::string &path, uint64_t size) {
    auto [parent, name] = locate_parent(root, path);
    if (!parent) return OFSErrorCodes::ERROR_INVALID_PATH;
    if (parent->files.find(name) != parent->files.end()) return OFSErrorCodes::ERROR_FILE_EXISTS;

    int blk = block_manager->allocate_block();
    if (blk == -1) return OFSErrorCodes::ERROR_NO_SPACE;

    FileEntry entry(name, EntryType::FILE, size, 0644, "root", blk);
    parent->files[name] = entry;
    (*inode_table)[entry.inode] = entry;

    return OFSErrorCodes::SUCCESS;
}

OFSErrorCodes FileOperations::file_delete(const std::string &path) {
    auto [parent, name] = locate_parent(root, path);
    if (!parent) return OFSErrorCodes::ERROR_INVALID_PATH;

    auto it = parent->files.find(name);
    if (it == parent->files.end()) return OFSErrorCodes::ERROR_NOT_FOUND;

    block_manager->free_block(it->second.inode);
    inode_table->erase(it->second.inode);
    parent->files.erase(it);

    return OFSErrorCodes::SUCCESS;
}

bool FileOperations::file_exists(const std::string &path) {
    auto [parent, name] = locate_parent(root, path);
    if (!parent) return false;
    return parent->files.find(name) != parent->files.end();
}

FileMetadata FileOperations::get_metadata(const std::string &path) {
    auto [parent, name] = locate_parent(root, path);
    if (!parent) return FileMetadata();
    auto it = parent->files.find(name);
    if (it == parent->files.end()) return FileMetadata();
    return FileMetadata(path, it->second);
}

OFSErrorCodes FileOperations::set_permissions(const std::string &path, uint32_t perms) {
    auto [parent, name] = locate_parent(root, path);
    if (!parent) return OFSErrorCodes::ERROR_INVALID_PATH;
    auto it = parent->files.find(name);
    if (it == parent->files.end()) return OFSErrorCodes::ERROR_NOT_FOUND;
    it->second.permissions = perms;
    return OFSErrorCodes::SUCCESS;
}

FSStats FileOperations::get_stats() {
    FSStats stats;
    std::function<void(DirNode*)> dfs = [&](DirNode* node) {
        stats.total_directories++;
        stats.total_files += node->files.size();
        for (auto &c : node->children) dfs(c.second.get());
    };
    dfs(root);

    stats.used_space = block_manager->used_blocks() * block_manager->block_size();
    stats.free_space = block_manager->free_blocks() * block_manager->block_size();
    return stats;
}

void FileOperations::free_dir_tree(DirNode* node) {
    for (auto &c : node->children) free_dir_tree(c.second.get());
    delete node;
}

// -------------------- New Methods --------------------

void FileOperations::file_edit(const std::string &path, const std::vector<char> &data, size_t offset) {
    auto [parent, name] = locate_parent(root, path);
    if (!parent) return;
    auto it = parent->files.find(name);
    if (it == parent->files.end()) return;

    FileEntry &entry = it->second;
    if (entry.content.size() < offset + data.size()) entry.content.resize(offset + data.size());
    std::copy(data.begin(), data.end(), entry.content.begin() + offset);
}

void FileOperations::file_read(const std::string &path, std::vector<char> &out) {
    auto [parent, name] = locate_parent(root, path);
    if (!parent) return;
    auto it = parent->files.find(name);
    if (it == parent->files.end()) return;
    out = it->second.content;
}

void FileOperations::file_truncate(const std::string &path, size_t new_size) {
    auto [parent, name] = locate_parent(root, path);
    if (!parent) return;
    auto it = parent->files.find(name);
    if (it == parent->files.end()) return;
    it->second.content.resize(new_size);
}

void FileOperations::file_rename(const std::string &old_path, const std::string &new_path) {
    // Locate parent directories and entry names
    auto [parent_old, name_old] = locate_parent(root, old_path);
    auto [parent_new, name_new] = locate_parent(root, new_path);
    if (!parent_old || !parent_new) return;

    // Find the file entry in the old parent
    auto it = parent_old->files.find(name_old);
    if (it == parent_old->files.end()) return;

    // Copy the entry
    FileEntry entry = it->second;

    // Safely update the name (char array)
    std::strncpy(entry.name, name_new.c_str(), sizeof(entry.name) - 1);
    entry.name[sizeof(entry.name) - 1] = '\0';  // Ensure null-termination

    // Insert into the new parent and erase from the old parent
    parent_new->files[name_new] = entry;
    parent_old->files.erase(it);
}
