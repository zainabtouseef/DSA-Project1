#ifndef FS_FORMATTER_HPP
#define FS_FORMATTER_HPP

#include "odf_types.hpp"
#include "config_loader.hpp"
#include <string>

class FSFormatter {
public:
    // Create a brand new .omni file
    static bool fs_format(const std::string &filename, const Config &config, std::string &error_message);
};

#endif
