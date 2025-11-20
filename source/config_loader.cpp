#include "../include/config_loader.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <iomanip>
#include <openssl/sha.h>

static std::string trim_copy(std::string s) {
    auto ws = [](char c){ return c==' ' || c=='\t'; };
    while (!s.empty() && ws(s.front())) s.erase(s.begin());
    while (!s.empty() && ws(s.back())) s.pop_back();
    return s;
}

static std::string remove_inline_comment(const std::string &line) {
    size_t pos = line.find('#');
    return (pos == std::string::npos) ? line : line.substr(0, pos);
}

bool ConfigLoader::load(const std::string &path, Config &config, std::string &error_message)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        error_message = "Cannot open config file: " + path;
        return false;
    }

    // Read entire file into memory for SHA256
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    // Compute SHA256 hash
    config.sha256_hash = compute_sha256(content);

    // Get file timestamp
    struct stat st{};
    if (stat(path.c_str(), &st) == 0)
        config.timestamp = static_cast<uint64_t>(st.st_mtime);

    // Reset file pointer
    file.clear();
    file.seekg(0);

    std::string line;
    std::string current_section;

    while (std::getline(file, line)) {
        line = remove_inline_comment(line);
        line = trim_copy(line);
        if (line.empty()) continue;

        // Section header
        if (line.front() == '[' && line.back() == ']') {
            current_section = line.substr(1, line.size() - 2);
            continue;
        }

        // Key-value
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key = trim_copy(line.substr(0, eq));
        std::string value = trim_copy(line.substr(eq + 1));

        // Handle quoted values
        if (value.size() >= 2 && value.front() == '"' && value.back() == '"')
            value = value.substr(1, value.size() - 2);

        // --- PARSE FIELDS BASED ON SECTION ---
        if (current_section == "filesystem") {
            if (key == "total_size") config.total_size = std::stoull(value);
            else if (key == "header_size") config.header_size = std::stoull(value);
            else if (key == "block_size") config.block_size = std::stoull(value);
            else if (key == "max_files") config.max_files = std::stoul(value);
            else if (key == "max_filename_length") config.max_filename_length = std::stoul(value);
        }

        else if (current_section == "security") {
            if (key == "max_users") config.max_users = std::stoul(value);
            else if (key == "admin_username") config.admin_username = value;
            else if (key == "admin_password") config.admin_password = value;
            else if (key == "require_auth") 
                config.require_auth = (value == "true" || value == "1" || value == "yes");
        }

        else if (current_section == "server") {
            if (key == "port") config.port = static_cast<uint16_t>(std::stoul(value));
            else if (key == "max_connections") config.max_connections = std::stoul(value);
            else if (key == "queue_timeout") config.queue_timeout = std::stoul(value);
        }
    }

    // --- VALIDATION ---
    if (config.total_size == 0) { error_message = "total_size missing!"; return false; }
    if (config.block_size == 0) { error_message = "block_size missing!"; return false; }
    if (config.header_size == 0) { error_message = "header_size missing!"; return false; }
    if (config.max_users == 0) { error_message = "max_users missing!"; return false; }
    if (config.port == 0) { error_message = "server port missing!"; return false; }

    return true;
}

// SHA256 helper
std::string ConfigLoader::compute_sha256(const std::string &content)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(content.c_str()),
           content.size(), hash);

    std::stringstream ss;
    for (unsigned char c : hash)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)c;
    return ss.str();
}
