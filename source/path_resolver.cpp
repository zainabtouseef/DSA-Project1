#include "../include/path_resolver.hpp"
#include <sstream>

bool PathResolver::validate_path(const std::string &path) {
    return !path.empty() && path.front() == '/';
}

std::vector<std::string> PathResolver::split_path(const std::string &path) {
    std::vector<std::string> components;
    std::stringstream ss(path);
    std::string item;
    while (std::getline(ss, item, '/')) {
        if (!item.empty()) components.push_back(item);
    }
    return components;
}

DirNode* PathResolver::locate_dir(const std::string &path) {
    if (!validate_path(path)) return nullptr;
    DirNode* node = root;
    for (auto &comp : split_path(path)) {
        auto it = node->children.find(comp);
        if (it == node->children.end()) return nullptr;
        node = it->second.get(); // .get() for unique_ptr
    }
    return node;
}

std::pair<DirNode*, std::string> PathResolver::locate_parent(const std::string &path) {
    auto comps = split_path(path);
    if (comps.empty()) return {nullptr, ""};
    std::string filename = comps.back();
    comps.pop_back();

    DirNode* parent = root;
    for (auto &c : comps) {
        auto it = parent->children.find(c);
        if (it == parent->children.end()) return {nullptr, ""};
        parent = it->second.get(); // .get() for unique_ptr
    }
    return {parent, filename};
}
