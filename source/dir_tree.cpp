#include "../include/dir_tree.hpp"
#include <functional>   // Needed for std::function
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>    // optional, for counting used blocks if needed

// Constructor
DirectoryTree::DirectoryTree() {
    FileEntry root_entry("/", EntryType::DIRECTORY, 0, 0755, "root", 1);
    root = std::make_unique<DirNode>(root_entry);
}

// Get root directory
DirNode* DirectoryTree::get_root() const { return root.get(); }

// Find directory by path
DirNode* DirectoryTree::find_directory(const std::string &path) {
    if (path == "/") return root.get();
    DirNode* current = root.get();
    std::stringstream ss(path);
    std::string segment;
    while (std::getline(ss, segment, '/')) {
        if (segment.empty()) continue;
        auto it = current->children.find(segment);
        if (it == current->children.end()) return nullptr;
        current = it->second.get(); // Use .get() on unique_ptr
    }
    return current;
}

// Add directory under a parent path
DirNode* DirectoryTree::add_directory(const std::string &path, const FileEntry &entry) {
    DirNode* parent = find_directory(path);
    if (!parent) return nullptr;
    auto new_dir = std::make_unique<DirNode>(entry);
    DirNode* ptr = new_dir.get();
    parent->children[entry.name] = std::move(new_dir);
    return ptr;
}

// Add file under a directory
bool DirectoryTree::add_file(const std::string &dir_path, const FileEntry &file_entry) {
    DirNode* dir = find_directory(dir_path);
    if (!dir) return false;
    dir->files[file_entry.name] = file_entry;
    return true;
}

// Count files recursively
size_t DirectoryTree::count_files() {
    size_t counter = 0;
    std::function<void(DirNode*)> dfs = [&](DirNode* node) {
        counter += node->files.size();
        for (auto &c : node->children) dfs(c.second.get()); // Use .get()
    };
    dfs(root.get());
    return counter;
}

// Count directories recursively
size_t DirectoryTree::count_directories() {
    size_t counter = 0;
    std::function<void(DirNode*)> dfs = [&](DirNode* node) {
        ++counter;
        for (auto &c : node->children) dfs(c.second.get()); // Use .get()
    };
    dfs(root.get());
    return counter;
}

// -------------------- Utilities --------------------
std::vector<std::string> split_path(const std::string &path) {
    std::stringstream ss(path);
    std::string part;
    std::vector<std::string> result;
    while (std::getline(ss, part, '/')) {
        if (!part.empty()) result.push_back(part);
    }
    return result;
}

std::pair<DirNode*, std::string> locate_parent(DirNode* root, const std::string &path) {
    if (!root) return {nullptr, ""};
    auto parts = split_path(path);
    if (parts.empty()) return {nullptr, ""};

    std::string name = parts.back();
    parts.pop_back();

    DirNode* current = root;
    for (const auto &folder : parts) {
        auto it = current->children.find(folder);
        if (it == current->children.end()) return {nullptr, ""};
        current = it->second.get(); // Use .get() on unique_ptr
    }
    return {current, name};
}
