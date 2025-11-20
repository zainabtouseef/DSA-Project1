#ifndef DIR_OPS_HPP
#define DIR_OPS_HPP

#include "../include/odf_types.hpp"
#include "dir_tree.hpp"
#include "path_resolver.hpp"

class DirOperations {
private:
    DirNode* root;
    PathResolver* resolver;

public:
    DirOperations(DirNode* root_node) : root(root_node) {
        resolver = new PathResolver(root);
    }
    ~DirOperations() { delete resolver; }

    OFSErrorCodes dir_create(const std::string &path);
    OFSErrorCodes dir_delete(const std::string &path);
    bool dir_exists(const std::string &path);
    std::vector<std::string> dir_list(const std::string &path);
};

#endif
