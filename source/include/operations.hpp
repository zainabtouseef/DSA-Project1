#pragma once
#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include "fs_core.hpp"
#include "user_manager.hpp"
#include "session_manager.hpp"


json dispatch_operation(const json &req);
