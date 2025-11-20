#ifndef FILE_INDEX_HPP
#define FILE_INDEX_HPP

#include "odf_types.hpp"
#include <unordered_map>
#include <string>

class FileIndex {
private:
    // key = full path, value = pointer to file entry in DirectoryTree
    std::unordered_map<std::string, FileEntry*> index;

public:
    void add(const std::string& path, FileEntry* fe);
    FileEntry* get(const std::string& path);
    void remove(const std::string& path);
};

#endif
