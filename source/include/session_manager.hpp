#ifndef SESSION_MANAGER_HPP
#define SESSION_MANAGER_HPP

#include "../include/odf_types.hpp"
#include "user_manager.hpp"
#include <unordered_map>
#include <string>
#include <random>
#include <chrono>

class SessionManager {
private:
    std::unordered_map<std::string, SessionInfo> sessions;
    UserManager* user_manager; // Reference to the user manager for user info

    std::string generate_session_id();

public:
    SessionManager(UserManager* um) : user_manager(um) {}

    std::string create_session(const std::string &username);
    bool validate_session(const std::string &session_id);
    bool destroy_session(const std::string &session_id);
    bool update_activity(const std::string &session_id);
    SessionInfo* get_session(const std::string &session_id);
};

#endif
