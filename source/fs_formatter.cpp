#include "../include/fs_formatter.hpp"
#include "../include/omni_header_builder.hpp"
#include <fstream>
#include <vector>
#include <cstring>
#include <iostream>

bool FSFormatter::fs_format(const std::string &filename, const Config &config, std::string &error_message)
{
    std::ofstream file(filename, std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        error_message = "Failed to create file: " + filename;
        return false;
    }

    // 1️⃣ Build header
    OMNIHeader header = OMNIHeaderBuilder::build(config);

    // 2️⃣ Write header (512 bytes)
    file.write(reinterpret_cast<const char*>(&header), sizeof(header));
    if (!file) {
        error_message = "Failed to write header!";
        return false;
    }

    // 3️⃣ Write empty user table
    std::vector<UserInfo> user_table(config.max_users);
    file.seekp(header.user_table_offset, std::ios::beg);
    file.write(reinterpret_cast<const char*>(user_table.data()), sizeof(UserInfo) * config.max_users);
    if (!file) {
        error_message = "Failed to write user table!";
        return false;
    }

    // 4️⃣ Create root directory entry
    FileEntry root_dir("/", EntryType::DIRECTORY, 0, 0755, "root", 1);
    uint64_t root_offset = header.user_table_offset + sizeof(UserInfo) * config.max_users;
    file.seekp(root_offset, std::ios::beg);
    file.write(reinterpret_cast<const char*>(&root_dir), sizeof(root_dir));
    if (!file) {
        error_message = "Failed to write root directory!";
        return false;
    }

    // 5️⃣ Initialize free block map (simple vector for now)
    uint64_t blocks_count = config.total_size / config.block_size;
    std::vector<uint8_t> free_block_map(blocks_count, 0); // 0 = free
    // Optionally write this after directory tree (we skip for simplicity here)

    // 6️⃣ Fill remaining file with zeroes
    uint64_t written_bytes = sizeof(header) + sizeof(UserInfo) * config.max_users + sizeof(root_dir);
    if (written_bytes < config.total_size) {
        uint64_t remaining = config.total_size - written_bytes;
        const size_t buffer_size = 8192;
        char zero_buf[buffer_size] = {0};

        while (remaining > 0) {
            size_t to_write = (remaining > buffer_size) ? buffer_size : static_cast<size_t>(remaining);
            file.write(zero_buf, to_write);
            remaining -= to_write;
        }
    }

    file.close();
    return true;
}
