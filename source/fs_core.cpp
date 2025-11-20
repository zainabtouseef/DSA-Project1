#include "../include/persistence_manager.hpp"
#include "../include/config_loader.hpp"
#include "../include/user_manager.hpp"
#include "../include/dir_tree.hpp"
#include "../include/free_block_manager.hpp"
#include <iostream>

bool shutdown_filesystem(const std::string &omni_path,
                         OMNIHeader &header,
                         UserManager &user_manager,
                         DirectoryTree &dir_tree,
                         FreeBlockManager &fbm)
{
    std::string err;
    if (!PersistenceManager::fs_shutdown(omni_path, header, user_manager, dir_tree, fbm, err)) {
        std::cerr << "fs_shutdown failed: " << err << "\n";
        return false;
    }
    std::cout << "Filesystem saved successfully to " << omni_path << "\n";
    return true;
}
