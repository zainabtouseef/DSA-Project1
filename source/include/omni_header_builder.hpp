#ifndef OMNI_HEADER_BUILDER_HPP
#define OMNI_HEADER_BUILDER_HPP

#include "odf_types.hpp"
#include "config_loader.hpp"
#include <string>

class OMNIHeaderBuilder {
public:
    static OMNIHeader build(const Config &config);
};

#endif
