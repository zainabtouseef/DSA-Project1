#ifndef CONFIG_LOADER_HPP
#define CONFIG_LOADER_HPP

#include <string>
#include <cstdint>

struct Config {
    // [filesystem]
    uint64_t total_size = 0;
    uint64_t header_size = 0;
    uint64_t block_size = 0;
    uint32_t max_files = 0;
    uint32_t max_filename_length = 0;

    // [security]
    uint32_t max_users = 0;
    std::string admin_username;
    std::string admin_password;
    bool require_auth = false;

    // [server]
    uint16_t port = 0;
    uint32_t max_connections = 0;
    uint32_t queue_timeout = 0;

    // Metadata
    std::string sha256_hash;
    uint64_t timestamp = 0;
};

class ConfigLoader {
public:
    static bool load(
        const std::string &path,
        Config &config,
        std::string &error_message
    );

private:
    static std::string compute_sha256(const std::string &content);
};

#endif
