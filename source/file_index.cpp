#include "file_index.hpp"

void FileIndex::add(const std::string& path, FileEntry* fe) {
    index[path] = fe;
}

FileEntry* FileIndex::get(const std::string& path) {
    auto it = index.find(path);
    return it == index.end() ? nullptr : it->second;
}

void FileIndex::remove(const std::string& path) {
    index.erase(path);
}
