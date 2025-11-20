#include <iostream>
#include <memory>
#include <unordered_map>
#include "include/operations.hpp"
#include "include/user_ops.hpp"
#include "include/dir_ops.hpp"
#include "include/file_ops.hpp"
#include "include/session_manager.hpp"
#include "include/config_loader.hpp"
#include "server.hpp"
#include "include/globals.hpp"
// Global objects
UserOperations* g_user_ops = nullptr;
DirOperations* g_dir_ops = nullptr;
FileOperations* g_file_ops = nullptr;
SessionManager* g_session_mgr = nullptr;
DirNode* g_root_dir = nullptr;
std::unordered_map<uint32_t, FileEntry>* g_inode_table = nullptr;

int main(int argc, char* argv[]) {
    // -----------------------
    // Step 1: Load configuration
    // -----------------------
    Config cfg;
    std::string err_msg;
    std::string config_path = "config/default.uconf";

    if (!ConfigLoader::load(config_path, cfg, err_msg)) {
        std::cerr << "[ERROR] Failed to load config file (" << config_path << "): " << err_msg << "\n";
        return 1;
    }

    std::cout << "[INFO] Configuration loaded successfully:\n";
    std::cout << "Filesystem - total_size: " << cfg.total_size
              << ", block_size: " << cfg.block_size
              << ", header_size: " << cfg.header_size
              << ", max_files: " << cfg.max_files
              << ", max_filename_length: " << cfg.max_filename_length << "\n";
    std::cout << "Security - max_users: " << cfg.max_users
              << ", admin_username: " << cfg.admin_username
              << ", require_auth: " << cfg.require_auth << "\n";
    std::cout << "Server - port: " << cfg.port
              << ", max_connections: " << cfg.max_connections
              << ", queue_timeout: " << cfg.queue_timeout << "\n";

    // -----------------------
    // Step 2: Initialize core components
    // -----------------------
    g_root_dir = new DirNode();
    g_fbm = new FreeBlockManager();
    g_inode_table = new std::unordered_map<uint32_t, FileEntry>();
    g_user_mgr = new UserManager();
    g_session_mgr = new SessionManager(g_user_mgr);
    g_user_ops = new UserOperations(g_user_mgr, g_session_mgr);
    g_dir_ops = new DirOperations(g_root_dir);
    g_file_ops = new FileOperations(g_root_dir, g_fbm, g_inode_table);

    std::cout << "[INFO] Core components initialized successfully.\n";

    // -----------------------
    // Step 3: Start server (with persistence)
    // -----------------------
    std::string omni_file = "data/filesystem.omni";  // persistent FS file
    server_init(omni_file, cfg.port);  // this handles load or admin creation internally

    // -----------------------
    // Step 4: Cleanup (normally not reached)
    // -----------------------
    delete g_user_ops;
    delete g_dir_ops;
    delete g_file_ops;
    delete g_session_mgr;
    delete g_user_mgr;
    delete g_fbm;
    delete g_root_dir;
    delete g_inode_table;

    return 0;
}
