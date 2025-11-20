#ifndef USER_OPS_HPP
#define USER_OPS_HPP

#include "../include/odf_types.hpp"
#include "user_manager.hpp"
#include "session_manager.hpp"
#include <vector>
#include <string>

class UserOperations {
private:
    UserManager* user_manager;
    SessionManager* session_manager;

public:
    UserOperations(UserManager* um, SessionManager* sm)
        : user_manager(um), session_manager(sm) {}

    OFSErrorCodes user_login(const std::string &username, const std::string &password, std::string &session_id);
    OFSErrorCodes user_logout(const std::string &session_id);

    OFSErrorCodes user_create(const std::string &session_id, const std::string &username,
                              const std::string &password_hash, UserRole role);
    OFSErrorCodes user_delete(const std::string &session_id, const std::string &username);

    OFSErrorCodes user_list(const std::string &session_id, std::vector<UserInfo> &out_users);
};

#endif
