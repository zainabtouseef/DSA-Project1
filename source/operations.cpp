#include "../include/operations.hpp"
#include "../include/user_ops.hpp"
#include "../include/dir_ops.hpp"
#include "../include/file_ops.hpp"
#include "../include/session_manager.hpp"
#include "../include/odf_types.hpp"
#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <iostream>

// extern globals (you must define these in your program startup)
extern UserOperations* g_user_ops;     // pointer to UserOperations instance
extern DirOperations* g_dir_ops;       // pointer to DirOperations instance
extern FileOperations* g_file_ops;     // pointer to FileOperations instance
extern SessionManager* g_session_mgr;  // pointer to SessionManager instance

// Helper: convert OFSErrorCodes to int and message
static int ofs_code_to_int(OFSErrorCodes c) {
    return static_cast<int32_t>(c);
}
static std::string ofs_code_to_message(OFSErrorCodes c) {
    switch(c) {
        case OFSErrorCodes::SUCCESS: return "Success";
        case OFSErrorCodes::ERROR_NOT_FOUND: return "Not found";
        case OFSErrorCodes::ERROR_PERMISSION_DENIED: return "Permission denied";
        case OFSErrorCodes::ERROR_IO_ERROR: return "I/O error";
        case OFSErrorCodes::ERROR_INVALID_PATH: return "Invalid path";
        case OFSErrorCodes::ERROR_FILE_EXISTS: return "File or directory already exists";
        case OFSErrorCodes::ERROR_NO_SPACE: return "No space left";
        case OFSErrorCodes::ERROR_INVALID_CONFIG: return "Invalid config";
        case OFSErrorCodes::ERROR_NOT_IMPLEMENTED: return "Not implemented";
        case OFSErrorCodes::ERROR_INVALID_SESSION: return "Invalid session";
        case OFSErrorCodes::ERROR_DIRECTORY_NOT_EMPTY: return "Directory not empty";
        case OFSErrorCodes::ERROR_INVALID_OPERATION: return "Invalid operation";
        default: return "Unknown error";
    }
}

json dispatch_operation(const json &req) {
    json res;
    std::string op = req.value("operation", "");
    std::string req_id = req.value("request_id", "");
    std::string session_id = req.value("session_id", "");

    // session pointer (NULL for no session)
    SessionInfo* sess = nullptr;
    if (!session_id.empty()) sess = g_session_mgr->get_session(session_id);

    // allow only user_login without a valid session
    if (op != "user_login" && sess == nullptr) {
        res["status"] = "error";
        res["operation"] = op;
        res["request_id"] = req_id;
        res["code"] = ofs_code_to_int(OFSErrorCodes::ERROR_INVALID_SESSION);
        res["error_message"] = ofs_code_to_message(OFSErrorCodes::ERROR_INVALID_SESSION);
        return res;
    }

    // ----------------------
    // USER OPERATIONS
    // ----------------------
    if (op == "user_login") {
    json payload = req.value("payload", json::object());
    std::string username = payload.value("username", "");
    std::string password = payload.value("password", "");

    std::cout << "[DEBUG dispatch_operation] login attempt, username='" 
              << username << "', password='" << password << "'\n";

    std::string new_session;
    OFSErrorCodes c = g_user_ops->user_login(username, password, new_session);
    

    if (c == OFSErrorCodes::SUCCESS) {
        res["status"] = "success";
        res["data"] = { {"session_id", new_session} };
        res["code"] = ofs_code_to_int(c);
    } else {
        res["status"] = "error";
        res["error_message"] = ofs_code_to_message(c);
        res["code"] = ofs_code_to_int(c);
    }
    res["operation"] = op; 
    res["request_id"] = req_id;
    return res;
}


    if (op == "user_logout") {
        // session_id must be provided
        OFSErrorCodes c = g_user_ops->user_logout(session_id);
        if (c == OFSErrorCodes::SUCCESS) {
            res["status"] = "success";
            res["code"] = ofs_code_to_int(c);
        } else {
            res["status"] = "error";
            res["error_message"] = ofs_code_to_message(c);
            res["code"] = ofs_code_to_int(c);
        }
        res["operation"] = op; res["request_id"] = req_id;
        return res;
    }

    if (op == "user_create") {
        std::string username = req.value("username", "");
        std::string password_hash = req.value("password_hash", "");
        UserRole role = static_cast<UserRole>(req.value("role", 0));
        OFSErrorCodes c = g_user_ops->user_create(session_id, username, password_hash, role);
        if (c == OFSErrorCodes::SUCCESS) res["status"]="success"; else { res["status"]="error"; res["error_message"]=ofs_code_to_message(c); }
        res["code"] = ofs_code_to_int(c);
        res["operation"]=op; res["request_id"]=req_id;
        return res;
    }

    if (op == "user_delete") {
        std::string username = req.value("username", "");
        OFSErrorCodes c = g_user_ops->user_delete(session_id, username);
        if (c == OFSErrorCodes::SUCCESS) res["status"]="success"; else { res["status"]="error"; res["error_message"]=ofs_code_to_message(c); }
        res["code"] = ofs_code_to_int(c);
        res["operation"]=op; res["request_id"]=req_id;
        return res;
    }

    if (op == "user_list") {
        std::vector<UserInfo> users;
        OFSErrorCodes c = g_user_ops->user_list(session_id, users);
        if (c == OFSErrorCodes::SUCCESS) {
            json uarr = json::array();
            for (const auto &u : users) {
                uarr.push_back({
                    {"username", std::string(u.username)},
                    {"role", static_cast<uint32_t>(u.role)},
                    {"created_time", u.created_time},
                    {"last_login", u.last_login},
                    {"is_active", u.is_active}
                });
            }
            res["status"]="success";
            res["data"] = { {"users", uarr} };
        } else {
            res["status"]="error";
            res["error_message"]=ofs_code_to_message(c);
        }
        res["code"]=ofs_code_to_int(c);
        res["operation"]=op; res["request_id"]=req_id;
        return res;
    }

    // ----------------------
    // DIRECTORY OPERATIONS
    // ----------------------
    if (op == "dir_create") {
        std::string path = req.value("path", "");
        OFSErrorCodes c = g_dir_ops->dir_create(path);
        if (c==OFSErrorCodes::SUCCESS) res["status"]="success"; else { res["status"]="error"; res["error_message"]=ofs_code_to_message(c); }
        res["code"]=ofs_code_to_int(c);
        res["operation"]=op; res["request_id"]=req_id;
        return res;
    }

    if (op == "dir_delete") {
        std::string path = req.value("path", "");
        OFSErrorCodes c = g_dir_ops->dir_delete(path);
        if (c==OFSErrorCodes::SUCCESS) res["status"]="success"; else { res["status"]="error"; res["error_message"]=ofs_code_to_message(c); }
        res["code"]=ofs_code_to_int(c);
        res["operation"]=op; res["request_id"]=req_id;
        return res;
    }

    if (op == "dir_exists") {
        std::string path = req.value("path", "");
        bool ex = g_dir_ops->dir_exists(path);
        res["status"] = "success";
        res["data"] = { {"exists", ex} };
        res["code"] = 0;
        res["operation"]=op; res["request_id"]=req_id;
        return res;
    }

    if (op == "dir_list") {
        std::string path = req.value("path", "");
        auto list = g_dir_ops->dir_list(path);
        res["status"]="success";
        res["data"] = { {"entries", list} };
        res["code"]=0;
        res["operation"]=op; res["request_id"]=req_id;
        return res;
    }

 // ----------------------
// FILE OPERATIONS
// ----------------------
if (op == "file_create") {
    std::string path = req.value("path", "");
    uint64_t size = req.value("size", 0ULL);
    OFSErrorCodes c = g_file_ops->file_create(path, size);
    if (c == OFSErrorCodes::SUCCESS) res["status"] = "success";
    else { res["status"] = "error"; res["error_message"] = ofs_code_to_message(c); }
    res["code"] = ofs_code_to_int(c); res["operation"] = op; res["request_id"] = req_id;
    return res;
}

if (op == "file_read") {
    std::string path = req.value("path", "");
    std::vector<char> buf;
    g_file_ops->file_read(path, buf); // now matches signature
    if (!buf.empty()) {
        res["status"] = "success";
        res["data"] = { {"content", std::string(buf.begin(), buf.end())} };
        res["code"] = 0;
    } else {
        res["status"] = "error";
        res["error_message"] = ofs_code_to_message(OFSErrorCodes::ERROR_NOT_FOUND);
        res["code"] = ofs_code_to_int(OFSErrorCodes::ERROR_NOT_FOUND);
    }
    res["operation"] = op; res["request_id"] = req_id;
    return res;
}

if (op == "file_edit") {
    std::string path = req.value("path", "");
    uint64_t index = req.value("index", 0ULL);
    std::string data_str = req.value("data", "");
    std::vector<char> data(data_str.begin(), data_str.end());
    g_file_ops->file_edit(path, data, index); // void
    res["status"] = "success";
    res["code"] = 0;
    res["operation"] = op; res["request_id"] = req_id;
    return res;
}

if (op == "file_truncate") {
    std::string path = req.value("path", "");
    size_t new_size = req.value("size", 0ULL);
    g_file_ops->file_truncate(path, new_size); // void
    res["status"] = "success";
    res["code"] = 0;
    res["operation"] = op; res["request_id"] = req_id;
    return res;
}

if (op == "file_delete") {
    std::string path = req.value("path", "");
    OFSErrorCodes c = g_file_ops->file_delete(path);
    if (c == OFSErrorCodes::SUCCESS) res["status"] = "success";
    else { res["status"] = "error"; res["error_message"] = ofs_code_to_message(c); }
    res["code"] = ofs_code_to_int(c); res["operation"] = op; res["request_id"] = req_id;
    return res;
}

if (op == "file_rename") {
    std::string oldp = req.value("old_path", "");
    std::string newp = req.value("new_path", "");
    g_file_ops->file_rename(oldp, newp); // void
    res["status"] = "success";
    res["code"] = 0;
    res["operation"] = op; res["request_id"] = req_id;
    return res;
}

if (op == "file_exists") {
    std::string path = req.value("path", "");
    bool ex = g_file_ops->file_exists(path);
    res["status"] = "success";
    res["data"] = { {"exists", ex} };
    res["code"] = 0; res["operation"] = op; res["request_id"] = req_id;
    return res;
}


    // ----------------------
    // METADATA / STATS
    // ----------------------
    if (op == "get_metadata") {
        std::string path = req.value("path", "");
        FileMetadata meta = g_file_ops->get_metadata(path); // assumed signature
        // If default-constructed path empty -> error
        if (strlen(meta.path) == 0) {
            res["status"]="error";
            res["error_message"]=ofs_code_to_message(OFSErrorCodes::ERROR_NOT_FOUND);
            res["code"]=ofs_code_to_int(OFSErrorCodes::ERROR_NOT_FOUND);
        } else {
            res["status"]="success";
            res["data"] = {
                {"path", std::string(meta.path)},
                {"size", meta.entry.size},
                {"blocks_used", meta.blocks_used},
                {"actual_size", meta.actual_size},
                {"owner", std::string(meta.entry.owner)}
            };
            res["code"]=0;
        }
        res["operation"]=op; res["request_id"]=req_id;
        return res;
    }

    if (op == "set_permissions") {
        std::string path = req.value("path", "");
        uint32_t perms = req.value("permissions", 0u);
        OFSErrorCodes c = g_file_ops->set_permissions(path, perms);
        if (c==OFSErrorCodes::SUCCESS) res["status"]="success"; else { res["status"]="error"; res["error_message"]=ofs_code_to_message(c); }
        res["code"]=ofs_code_to_int(c); res["operation"]=op; res["request_id"]=req_id;
        return res;
    }

    if (op == "get_stats") {
        FSStats stats = g_file_ops->get_stats();
        res["status"]="success";
        res["data"] = {
            {"total_size", stats.total_size},
            {"used_space", stats.used_space},
            {"free_space", stats.free_space},
            {"total_files", stats.total_files},
            {"total_directories", stats.total_directories},
            {"total_users", stats.total_users},
            {"active_sessions", stats.active_sessions},
            {"fragmentation", stats.fragmentation}
        };
        res["code"]=0; res["operation"]=op; res["request_id"]=req_id;
        return res;
    }

    // Unknown operation
    res["status"] = "error";
    res["operation"] = op;
    res["request_id"] = req_id;
    res["code"] = ofs_code_to_int(OFSErrorCodes::ERROR_INVALID_OPERATION);
    res["error_message"] = "Unknown operation: " + op;
    return res;
}
