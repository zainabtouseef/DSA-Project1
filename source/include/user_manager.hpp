#ifndef USER_MANAGER_HPP
#define USER_MANAGER_HPP

#include "odf_types.hpp"
#include <unordered_map>
#include <string>
#include <vector>

class UserManager {
private:
    std::unordered_map<std::string, UserInfo> users; 

public:
    UserManager() = default;

    void load_users(const std::vector<UserInfo>& user_table);
    std::vector<UserInfo> save_users() const;

    bool create_user(const std::string &username, const std::string &password_hash, UserRole role, uint64_t created_time);
    bool delete_user(const std::string &username);

    UserInfo* find_user(const std::string &username);
    bool verify_password(const std::string &username, const std::string &password_hash);
    void dump_users() const;

};

#endif
