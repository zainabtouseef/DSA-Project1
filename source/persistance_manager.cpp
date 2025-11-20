#include "persistence_manager.hpp"
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdint>
#include <iostream>
#include "include/dir_tree.hpp"  


// ====================================================
// SHUTDOWN / SAVE FILESYSTEM
// ====================================================
bool PersistenceManager::fs_shutdown(
const std::string &omni_path,
OMNIHeader &header,
const UserManager &user_manager,
const DirectoryTree &dir_tree,
const FreeBlockManager &fbm,
std::string &error_msg)
{
std::ofstream ofs(omni_path, std::ios::binary | std::ios::in | std::ios::out);
if (!ofs.is_open()) {
ofs.open(omni_path, std::ios::binary | std::ios::out);
if (!ofs.is_open()) {
error_msg = "Failed to open .omni file for shutdown: " + omni_path;
return false;
}
}


if (!save_header(ofs, header, error_msg)) return false;
if (!save_user_table(ofs, header, user_manager, error_msg)) return false;

uint64_t dir_bytes = 0;
if (!save_directory_tree(ofs, header, dir_tree, dir_bytes, error_msg)) return false;

uint64_t fbm_offset = 0;
if (!save_free_block_map(ofs, header, fbm, fbm_offset, error_msg)) return false;

header.file_state_storage_offset = static_cast<uint32_t>(fbm_offset);
header.change_log_offset = 0;

ofs.seekp(0, std::ios::beg);
if (!ofs.good()) { error_msg = "Failed to seek header for rewrite"; ofs.close(); return false; }
ofs.write(reinterpret_cast<const char*>(&header), sizeof(header));
if (!ofs) { error_msg = "Failed to rewrite header"; ofs.close(); return false; }

ofs.close();
return true;


}

// ====================================================
// SAVE HELPERS
// ====================================================
bool PersistenceManager::save_header(std::ofstream &ofs, const OMNIHeader &header, std::string &error_msg) {
ofs.seekp(0, std::ios::beg);
ofs.write(reinterpret_cast<const char*>(&header), sizeof(header));
if (!ofs) { error_msg = "Failed to write header"; return false; }
return true;
}

bool PersistenceManager::save_user_table(std::ofstream &ofs, const OMNIHeader &header, const UserManager &user_manager, std::string &error_msg) {
std::vector<UserInfo> users = user_manager.save_users();
uint64_t offset = header.user_table_offset;
ofs.seekp(static_cast<std::streamoff>(offset), std::ios::beg);
if (!ofs.good()) { error_msg = "Failed to seek to user_table_offset"; return false; }


size_t write_count = users.size();
if (write_count > header.max_users) write_count = header.max_users;

ofs.write(reinterpret_cast<const char*>(users.data()), sizeof(UserInfo) * write_count);
if (!ofs) { error_msg = "Failed to write user entries"; return false; }

size_t remaining = header.max_users - write_count;
if (remaining > 0) {
    std::vector<UserInfo> empty(remaining);
    ofs.write(reinterpret_cast<const char*>(empty.data()), sizeof(UserInfo) * remaining);
    if (!ofs) { error_msg = "Failed to write remaining empty user slots"; return false; }
}
return true;


}

// ====================================================
// DIRECTORY TREE
// ====================================================
void PersistenceManager::collect_dirnodes_recursive(DirNode* node, const std::string &path, std::vector<std::pair<std::string, DirNode*>> &out) {
out.emplace_back(path, node);
for (auto &c : node->children) {
std::string child_path = path;
if (child_path != "/" && child_path.back() != '/') child_path += '/';
child_path += c.first;
collect_dirnodes_recursive(c.second.get(), child_path, out);
}
}

bool PersistenceManager::save_directory_tree(std::ofstream &ofs, const OMNIHeader &header, const DirectoryTree &dir_tree, uint64_t &out_directory_bytes_written, std::string &error_msg) {
uint64_t offset = header.user_table_offset + static_cast<uint64_t>(header.max_users) * sizeof(UserInfo);
ofs.seekp(static_cast<std::streamoff>(offset), std::ios::beg);
if (!ofs.good()) { error_msg = "Failed to seek directory offset"; return false; }


std::vector<std::pair<std::string, DirNode*>> nodes;
collect_dirnodes_recursive(dir_tree.get_root(), "/", nodes);

uint32_t node_count = static_cast<uint32_t>(nodes.size());
ofs.write(reinterpret_cast<const char*>(&node_count), sizeof(node_count));
if (!ofs) { error_msg = "Failed to write directory node count"; return false; }

for (auto &p : nodes) {
    const std::string &path = p.first;
    DirNode* node = p.second;

    uint32_t path_len = static_cast<uint32_t>(path.size());
    ofs.write(reinterpret_cast<const char*>(&path_len), sizeof(path_len));
    ofs.write(path.data(), path_len);

    ofs.write(reinterpret_cast<const char*>(&node->entry), sizeof(node->entry));

    uint32_t file_count = static_cast<uint32_t>(node->files.size());
    ofs.write(reinterpret_cast<const char*>(&file_count), sizeof(file_count));
    if (!ofs) { error_msg = "Failed writing directory files metadata"; return false; }

    for (auto &fe_pair : node->files) {
        const FileEntry &fe = fe_pair.second;
        ofs.write(reinterpret_cast<const char*>(&fe), sizeof(fe));
        if (!ofs) { error_msg = "Failed to write FileEntry"; return false; }
    }
}

std::streamoff pos = ofs.tellp();
if (pos < 0) { error_msg = "Failed to get directory tree end"; return false; }
out_directory_bytes_written = static_cast<uint64_t>(pos) - offset;
return true;


}

// ====================================================
// FREE BLOCK MAP
// ====================================================
bool PersistenceManager::save_free_block_map(std::ofstream &ofs, const OMNIHeader &header, const FreeBlockManager &fbm, uint64_t &out_offset, std::string &error_msg) {
ofs.seekp(0, std::ios::end);
std::streamoff pos = ofs.tellp();
if (pos < 0) { error_msg = "Failed to seek end for free block map"; return false; }
uint64_t write_offset = static_cast<uint64_t>(pos);


std::vector<bool> bits = fbm.to_vector_bool();
uint64_t total_bits = bits.size();
uint64_t num_bytes = (total_bits + 7) / 8;

ofs.write(reinterpret_cast<const char*>(&total_bits), sizeof(total_bits));
uint64_t blk_size = fbm.block_size();
ofs.write(reinterpret_cast<const char*>(&blk_size), sizeof(blk_size));

std::vector<uint8_t> bytes(static_cast<size_t>(num_bytes), 0);
for (uint64_t i = 0; i < total_bits; ++i) {
    if (bits[i]) bytes[i / 8] |= (1 << (i % 8));
}
if (num_bytes > 0) ofs.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(num_bytes));
if (!ofs) { error_msg = "Failed to write free block map"; return false; }

out_offset = write_offset;
return true;


}

// ====================================================
// LOAD FILESYSTEM
// ====================================================
bool PersistenceManager::fs_load(
const std::string &omni_path,
OMNIHeader &header,
UserManager &user_manager,
DirectoryTree &dir_tree,
FreeBlockManager &fbm,
std::string &error_msg)
{
std::ifstream ifs(omni_path, std::ios::binary);
if (!ifs.is_open()) { error_msg = "File not found"; return false; }


ifs.read(reinterpret_cast<char*>(&header), sizeof(header));
if (!ifs) { error_msg = "Failed to read header"; return false; }

if (!load_user_table(ifs, header, user_manager, error_msg)) return false;
if (!load_directory_tree(ifs, header, dir_tree, error_msg)) return false;
if (!load_free_block_map(ifs, header, fbm, error_msg)) return false;

ifs.close();
return true;


}

// ====================================================
// LOAD HELPERS
// ====================================================
bool PersistenceManager::load_user_table(std::ifstream &ifs, const OMNIHeader &header, UserManager &user_manager, std::string &error_msg) {
ifs.seekg(static_cast<std::streamoff>(header.user_table_offset), std::ios::beg);
if (!ifs.good()) { error_msg = "Failed to seek to user_table_offset"; return false; }


std::vector<UserInfo> users(header.max_users);
ifs.read(reinterpret_cast<char*>(users.data()), sizeof(UserInfo) * header.max_users);
if (!ifs) { error_msg = "Failed to read user table"; return false; }

user_manager.load_users(users);
return true;


}

bool PersistenceManager::load_directory_tree(std::ifstream &ifs, const OMNIHeader &header, DirectoryTree &dir_tree, std::string &error_msg) {
    uint64_t offset = header.user_table_offset + static_cast<uint64_t>(header.max_users) * sizeof(UserInfo);
    ifs.seekg(static_cast<std::streamoff>(offset), std::ios::beg);
    if (!ifs.good()) { error_msg = "Failed to seek directory offset"; return false; }

    uint32_t node_count = 0;
    ifs.read(reinterpret_cast<char*>(&node_count), sizeof(node_count));
    if (!ifs) { error_msg = "Failed to read directory node count"; return false; }

    for (uint32_t i = 0; i < node_count; ++i) {
        uint32_t path_len = 0;
        ifs.read(reinterpret_cast<char*>(&path_len), sizeof(path_len));
        if (!ifs) { error_msg = "Failed to read path length"; return false; }

        std::string path(path_len, '\0');
        ifs.read(path.data(), path_len);
        if (!ifs) { error_msg = "Failed to read path"; return false; }

        FileEntry entry;
        ifs.read(reinterpret_cast<char*>(&entry), sizeof(entry));
        if (!ifs) { error_msg = "Failed to read FileEntry"; return false; }

        uint32_t file_count = 0;
        ifs.read(reinterpret_cast<char*>(&file_count), sizeof(file_count));
        if (!ifs) { error_msg = "Failed to read file count"; return false; }

        std::vector<FileEntry> files(file_count);
        for (uint32_t j = 0; j < file_count; ++j) {
            ifs.read(reinterpret_cast<char*>(&files[j]), sizeof(FileEntry));
            if (!ifs) { error_msg = "Failed to read FileEntry"; return false; }
        }

        // -------------------------------
        // Use locate_parent utility
        auto [parent, name] = locate_parent(dir_tree.get_root(), path);
        if (!parent) { error_msg = "Failed to locate parent for path: " + path; return false; }

        strncpy(entry.name, name.c_str(), sizeof(entry.name));
entry.name[sizeof(entry.name)-1] = '\0';  // Ensure null-termination

        auto new_node = std::make_unique<DirNode>(entry);
DirNode* node = new_node.get();
parent->children[name] = std::move(new_node);


        for (auto &fe : files) node->files[fe.name] = fe;
    }

    return true;
}

// ====================================================
// FREE BLOCK MAP LOADER
// ====================================================
bool PersistenceManager::load_free_block_map(std::ifstream &ifs, const OMNIHeader &header, FreeBlockManager &fbm, std::string &error_msg) {
    ifs.seekg(static_cast<std::streamoff>(header.file_state_storage_offset), std::ios::beg);
    if (!ifs.good()) { error_msg = "Failed to seek to free block map offset"; return false; }

    uint64_t total_bits = 0;
    ifs.read(reinterpret_cast<char*>(&total_bits), sizeof(total_bits));
    if (!ifs) { error_msg = "Failed to read total bits"; return false; }

    uint64_t blk_size = 0;
    ifs.read(reinterpret_cast<char*>(&blk_size), sizeof(blk_size));
    if (!ifs) { error_msg = "Failed to read block size"; return false; }

    uint64_t num_bytes = (total_bits + 7) / 8;
    std::vector<uint8_t> bytes(static_cast<size_t>(num_bytes), 0);
    if (num_bytes > 0) ifs.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(num_bytes));
    if (!ifs) { error_msg = "Failed to read free block map bytes"; return false; }

    std::vector<bool> bits(static_cast<size_t>(total_bits), false);
    for (uint64_t i = 0; i < total_bits; ++i) {
        bits[i] = (bytes[i / 8] & (1 << (i % 8))) != 0;
    }

    fbm.load_from_vector_bool(bits, static_cast<uint64_t>(blk_size));
    return true;
}
