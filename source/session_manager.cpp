#include "../include/session_manager.hpp"
#include <sstream>
#include <iomanip>

std::string SessionManager::generate_session_id() {
    std::stringstream ss;
    static std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    static std::uniform_int_distribution<uint64_t> dist;
    for (int i = 0; i < 2; ++i) { // 2 × 16 hex digits = 64 chars
        ss << std::hex << std::setw(16) << std::setfill('0') << dist(rng);
    }
    return ss.str();
}

std::string SessionManager::create_session(const std::string &username) {
    UserInfo* user = user_manager->find_user(username);
    if (!user || !user->is_active) return "";

    std::string session_id = generate_session_id();
    uint64_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    SessionInfo session(session_id, *user, now);
    sessions[session_id] = session;
    return session_id;
}

bool SessionManager::validate_session(const std::string &session_id) {
    auto it = sessions.find(session_id);
    if (it == sessions.end()) return false;
    return it->second.user.is_active == 1;
}

bool SessionManager::destroy_session(const std::string &session_id) {
    return sessions.erase(session_id) > 0;
}

bool SessionManager::update_activity(const std::string &session_id) {
    auto it = sessions.find(session_id);
    if (it == sessions.end()) return false;
    it->second.last_activity = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    it->second.operations_count += 1;
    return true;
}

SessionInfo* SessionManager::get_session(const std::string &session_id) {
    auto it = sessions.find(session_id);
    if (it == sessions.end()) return nullptr;
    return &it->second;
}
