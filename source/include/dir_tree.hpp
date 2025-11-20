#ifndef DIR_TREE_HPP
#define DIR_TREE_HPP

#include "../include/odf_types.hpp"
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <sstream>

struct DirNode {
    FileEntry entry;
    std::unordered_map<std::string, std::unique_ptr<DirNode>> children;
    std::unordered_map<std::string, FileEntry> files;

    DirNode() = default;
    DirNode(const FileEntry &e) : entry(e) {}
};

std::pair<DirNode*, std::string> locate_parent(DirNode* root, const std::string &path);
class DirectoryTree {
private:
    std::unique_ptr<DirNode> root;

public:
    DirectoryTree();
    DirNode* get_root() const;
    DirNode* add_directory(const std::string &path, const FileEntry &entry);
    bool add_file(const std::string &dir_path, const FileEntry &file_entry);
  // dir_tree.hpp (add near other declarations)



    DirNode* find_directory(const std::string &path);

    // Optional: helper to count files/directories
    size_t count_files();
    size_t count_directories();
};

// Utility to split path
std::vector<std::string> split_path(const std::string &path);

#endif
