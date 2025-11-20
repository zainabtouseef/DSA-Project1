#include "../include/user_ops.hpp"
#include <string>
#include <iostream>

OFSErrorCodes UserOperations::user_login(const std::string &username, const std::string &password, std::string &session_id) {
    std::cout << "[DEBUG user_login] Attempting login for username='" << username << "'\n";

    UserInfo* user = user_manager->find_user(username);
    if (!user) {
        std::cout << "[DEBUG user_login] User not found: " << username << "\n";
        return OFSErrorCodes::ERROR_NOT_FOUND;
    }

    std::cout << "[DEBUG user_login] User found: " << user->username
              << ", is_active=" << static_cast<int>(user->is_active) << "\n";

    if (!user->is_active) {
        std::cout << "[DEBUG user_login] User is inactive: " << username << "\n";
        return OFSErrorCodes::ERROR_INVALID_OPERATION;
    }

    if (!user_manager->verify_password(username, password)) {
        std::cout << "[DEBUG user_login] Password mismatch for user: " << username
                  << ", stored_password='" << user->password_hash
                  << "' incoming_password='" << password << "'\n";
        return OFSErrorCodes::ERROR_PERMISSION_DENIED;
    }

    session_id = session_manager->create_session(username);
    if (session_id.empty()) {
        std::cout << "[DEBUG user_login] Failed to create session for user: " << username << "\n";
        return OFSErrorCodes::ERROR_INVALID_OPERATION;
    }

    std::cout << "[DEBUG user_login] Login successful, session_id=" << session_id << "\n";
    return OFSErrorCodes::SUCCESS;
}


OFSErrorCodes UserOperations::user_logout(const std::string &session_id) {
    if (!session_manager->destroy_session(session_id))
        return OFSErrorCodes::ERROR_INVALID_SESSION;
    return OFSErrorCodes::SUCCESS;
}

OFSErrorCodes UserOperations::user_create(const std::string &session_id, const std::string &username,
                                          const std::string &password_hash, UserRole role) {
    SessionInfo* sess = session_manager->get_session(session_id);
    if (!sess) return OFSErrorCodes::ERROR_INVALID_SESSION;
    if (sess->user.role != UserRole::ADMIN) return OFSErrorCodes::ERROR_PERMISSION_DENIED;

    if (!user_manager->create_user(username, password_hash, role, std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())))
        return OFSErrorCodes::ERROR_INVALID_OPERATION;

    session_manager->update_activity(session_id);
    return OFSErrorCodes::SUCCESS;
}

OFSErrorCodes UserOperations::user_delete(const std::string &session_id, const std::string &username) {
    SessionInfo* sess = session_manager->get_session(session_id);
    if (!sess) return OFSErrorCodes::ERROR_INVALID_SESSION;
    if (sess->user.role != UserRole::ADMIN) return OFSErrorCodes::ERROR_PERMISSION_DENIED;

    if (!user_manager->delete_user(username))
        return OFSErrorCodes::ERROR_NOT_FOUND;

    session_manager->update_activity(session_id);
    return OFSErrorCodes::SUCCESS;
}

OFSErrorCodes UserOperations::user_list(const std::string &session_id, std::vector<UserInfo> &out_users) {
    SessionInfo* sess = session_manager->get_session(session_id);
    if (!sess) return OFSErrorCodes::ERROR_INVALID_SESSION;
    if (sess->user.role != UserRole::ADMIN) return OFSErrorCodes::ERROR_PERMISSION_DENIED;

    out_users = user_manager->save_users();
    session_manager->update_activity(session_id);
    return OFSErrorCodes::SUCCESS;
}
