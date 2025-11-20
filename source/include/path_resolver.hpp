#ifndef PATH_RESOLVER_HPP
#define PATH_RESOLVER_HPP

#include "../include/odf_types.hpp"
#include "dir_tree.hpp"
#include <vector>
#include <string>

class PathResolver {
private:
    DirNode* root;

public:
    PathResolver(DirNode* root_node) : root(root_node) {}

    // Validate path format
    bool validate_path(const std::string &path);

    // Split path into components
    std::vector<std::string> split_path(const std::string &path);

    // Locate directory node for a given path
    DirNode* locate_dir(const std::string &path);

    // Locate parent dir and filename
    std::pair<DirNode*, std::string> locate_parent(const std::string &path);
};

#endif
