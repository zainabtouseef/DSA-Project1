#include "../include/omni_header_builder.hpp"
#include <cstring>

OMNIHeader OMNIHeaderBuilder::build(const Config &config)
{
    OMNIHeader header;

    // Set magic bytes
    std::memset(header.magic, 0, sizeof(header.magic));
    std::memcpy(header.magic, "OMNIFS01", 8);

    header.format_version = 0x00010000;
    header.total_size = config.total_size;
    header.header_size = 512;
    header.block_size = config.block_size;

    // Phase 1: student_id & submission_date are empty
    std::memset(header.student_id, 0, sizeof(header.student_id));
    std::memset(header.submission_date, 0, sizeof(header.submission_date));

    // config hash
    std::memset(header.config_hash, 0, sizeof(header.config_hash));
    std::strncpy(header.config_hash, config.sha256_hash.c_str(), sizeof(header.config_hash) - 1);

    // timestamp
    header.config_timestamp = config.timestamp;

    // offsets
    header.user_table_offset = 512;   // Immediately after header
    header.max_users = config.max_users;

    header.file_state_storage_offset = 0; // Reserved for Phase 2
    header.change_log_offset = 0;        // Reserved for Phase 2

    return header;
}
