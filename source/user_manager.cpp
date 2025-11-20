#include "user_manager.hpp"
#include <iostream>

void UserManager::load_users(const std::vector<UserInfo>& user_table) {
    users.clear();
    for (auto& u : user_table)
        users[u.username] = u;
}

std::vector<UserInfo> UserManager::save_users() const {
    std::vector<UserInfo> out;
    for (auto& kv : users)
        out.push_back(kv.second);
    return out;
}

bool UserManager::create_user(const std::string& username,
                              const std::string& pwd,
                              UserRole role,
                              uint64_t created_time) 
{
    if (users.count(username)) return false;

    // ❌ REMOVE HASHING — store password directly
    users[username] = UserInfo(username, pwd, role, created_time);

    std::cout << "[DEBUG create_user] stored user '" << username
              << "' password_plain = '" << pwd << "'\n";

    return true;
}

bool UserManager::delete_user(const std::string& username) {
    return users.erase(username) > 0;
}

UserInfo* UserManager::find_user(const std::string &username) {
    for (auto &kv : users) { // kv.first = key, kv.second = UserInfo
        std::cout << "[DEBUG find_user] key='" << kv.first 
                  << "' username='" << kv.second.username << "'\n";
        if (kv.first == username) // match map key directly
            return &kv.second;
    }
    return nullptr;
}




bool UserManager::verify_password(const std::string& username, const std::string& incoming_pwd) {
    auto u = find_user(username);
    if (!u) {
        std::cout << "[DEBUG verify_password] user not found: " << username << "\n";
        return false;
    }

    std::cout << "[DEBUG verify_password] stored='" << u->password_hash 
              << "' incoming='" << incoming_pwd << "'\n";

    // ✔ Direct comparison — no hashing
    return u->password_hash == incoming_pwd;
}
  
void UserManager::dump_users() const {
    std::cout << "[DEBUG dump_users] total users: " << users.size() << "\n";
    for (const auto &kv : users) {
        const UserInfo &u = kv.second;
        std::cout << "  key='" << kv.first 
                  << "' username='" << u.username 
                  << "' password='" << u.password_hash 
                  << "' role=" << static_cast<int>(u.role)
                  << " created_time=" << u.created_time
                  << " last_login=" << u.last_login
                  << " is_active=" << u.is_active
                  << "\n";
    }
}

