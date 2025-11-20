#include "globals.hpp"

// Global definitions (exactly once)
UserManager* g_user_mgr = nullptr;
FreeBlockManager* g_fbm = nullptr;
DirectoryTree* g_dir_tree = nullptr;
std::string g_omni_file = "data/filesystem.omni";
std::atomic<bool> g_shutdown_flag{false};
OMNIHeader g_header;
