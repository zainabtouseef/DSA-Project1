#ifndef PERSISTENCE_MANAGER_HPP
#define PERSISTENCE_MANAGER_HPP

#include <string>
#include <vector>
#include <fstream>
#include "dir_tree.hpp"
#include "free_block_manager.hpp"
#include "user_manager.hpp"
#include "odf_types.hpp"

class PersistenceManager {
public:
    static bool fs_shutdown(const std::string &omni_path,
                            OMNIHeader &header,
                            const UserManager &user_manager,
                            const DirectoryTree &dir_tree,
                            const FreeBlockManager &fbm,
                            std::string &error_msg);

    static bool fs_load(const std::string &omni_path,
                        OMNIHeader &header,
                        UserManager &user_manager,
                        DirectoryTree &dir_tree,
                        FreeBlockManager &fbm,
                        std::string &error_msg);

private:
    // Save/load helpers
    static bool save_header(std::ofstream &ofs, const OMNIHeader &header, std::string &error_msg);
    static bool save_user_table(std::ofstream &ofs, const OMNIHeader &header,
                                const UserManager &user_manager, std::string &error_msg);
    static bool save_directory_tree(std::ofstream &ofs, const OMNIHeader &header,
                                    const DirectoryTree &dir_tree,
                                    uint64_t &out_directory_bytes_written,
                                    std::string &error_msg);
    static bool save_free_block_map(std::ofstream &ofs, const OMNIHeader &header,
                                    const FreeBlockManager &fbm,
                                    uint64_t &out_offset,
                                    std::string &error_msg);

    static bool load_user_table(std::ifstream &ifs, const OMNIHeader &header,
                                UserManager &user_manager, std::string &error_msg);
    static bool load_directory_tree(std::ifstream &ifs, const OMNIHeader &header,
                                    DirectoryTree &dir_tree,
                                    std::string &error_msg);
    static bool load_free_block_map(std::ifstream &ifs, const OMNIHeader &header,
                                    FreeBlockManager &fbm,
                                    std::string &error_msg);

    // Recursively collect DirNodes for saving
    static void collect_dirnodes_recursive(DirNode* node, const std::string &path,
                                           std::vector<std::pair<std::string, DirNode*>> &out);
};

#endif
