#include "../include/dir_ops.hpp"

// Create a new directory
OFSErrorCodes DirOperations::dir_create(const std::string &path) {
    auto [parent, name] = resolver->locate_parent(path);
    if (!parent) return OFSErrorCodes::ERROR_INVALID_PATH;
    if (parent->children.find(name) != parent->children.end()) 
        return OFSErrorCodes::ERROR_FILE_EXISTS;

    FileEntry entry(name, EntryType::DIRECTORY, 0, 0755, "root", 0);

    // Use unique_ptr instead of raw pointer
    auto node = std::make_unique<DirNode>();
    node->entry = entry;

    parent->children[name] = std::move(node);  // Move into unordered_map
    return OFSErrorCodes::SUCCESS;
}

// Delete an existing directory
OFSErrorCodes DirOperations::dir_delete(const std::string &path) {
    auto [parent, name] = resolver->locate_parent(path);
    if (!parent) return OFSErrorCodes::ERROR_INVALID_PATH;

    auto it = parent->children.find(name);
    if (it == parent->children.end()) return OFSErrorCodes::ERROR_NOT_FOUND;

    // Check if directory is empty
    if (!it->second->children.empty() || !it->second->files.empty())
        return OFSErrorCodes::ERROR_DIRECTORY_NOT_EMPTY;

    // No need to delete manually; unique_ptr handles it
    parent->children.erase(it);
    return OFSErrorCodes::SUCCESS;
}

// Check if directory exists
bool DirOperations::dir_exists(const std::string &path) {
    return resolver->locate_dir(path) != nullptr;
}

// List contents of directory
std::vector<std::string> DirOperations::dir_list(const std::string &path) {
    std::vector<std::string> list;
    DirNode* node = resolver->locate_dir(path);
    if (!node) return list;

    for (auto &c : node->children) list.push_back(c.first + "/");
    for (auto &f : node->files) list.push_back(f.first);
    return list;
}
