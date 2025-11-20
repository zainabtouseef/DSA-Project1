#pragma once
#include <string>
#include <atomic>
#include "user_ops.hpp"
#include "dir_ops.hpp"
#include "file_ops.hpp"
#include "session_manager.hpp"
#include "odf_types.hpp"
#include "free_block_manager.hpp"
#include "dir_tree.hpp"
#include "user_manager.hpp"

// Global pointers (declared only)
extern UserManager* g_user_mgr;
extern FreeBlockManager* g_fbm;
extern DirectoryTree* g_dir_tree;
extern std::string g_omni_file;
extern std::atomic<bool> g_shutdown_flag;
extern OMNIHeader g_header;
extern UserOperations* g_user_ops;

