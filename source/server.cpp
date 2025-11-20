#include "server.hpp"
#include "operations.hpp"
#include "persistence_manager.hpp"
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <atomic>
#include <iostream>
#include "include/globals.hpp"
RequestQueue requestQueue;  



  


// ===================== SAVE ALL =====================
void save_all() {
    std::string err;
    if (!PersistenceManager::fs_shutdown(g_omni_file, g_header, *g_user_mgr, *g_dir_tree, *g_fbm, err)) {
        std::cerr << "[ERROR] Failed to persist FS: " << err << "\n";
    } else {
        std::cout << "[INFO] Filesystem persisted successfully.\n";
    }
}

// ===================== SIGNAL HANDLER =====================
void signal_handler(int signum) {
    std::cout << "\n[INFO] Signal received, shutting down...\n";
    g_shutdown_flag = true;
    save_all();
    exit(0);
}
  
  


// ===================== WORKER THREAD =====================
void* worker_thread(void*) {
    while (!g_shutdown_flag) {
        Request req = requestQueue.pop();  // blocks until a request is available

        json response;
        try {
            response = dispatch_operation(req.request);
        } catch (const std::exception &e) {
            response = {{"status", "error"}, {"message", e.what()}, {"code", -500}};
        }

        response["operation"]  = req.request.value("operation", "");
        response["request_id"] = req.request.value("request_id", "");
        std::string out = response.dump() + "\n";
        send(req.client_socket, out.c_str(), out.size(), 0);
    }
    return nullptr;
}



// ===================== CLIENT THREAD =====================
void* client_thread(void* arg) {
    int client_socket = *(int*)arg;
    delete (int*)arg;

    char buffer[4096];
    std::string data_buffer;

    while (!g_shutdown_flag) {
        int bytes = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes <= 0) {
            close(client_socket);
            return nullptr;
        }

        data_buffer.append(buffer, bytes);
        size_t pos;
        while ((pos = data_buffer.find('\n')) != std::string::npos) {
            std::string message = data_buffer.substr(0, pos);
            data_buffer.erase(0, pos + 1);
            if (message.empty()) continue;

            try {
                json request = json::parse(message);
                Request r{request, client_socket};
                requestQueue.push(r);
            } catch (...) {
                std::cerr << "[ERROR] Invalid JSON: " << message << "\n";
            }
        }
    }

    close(client_socket);
    return nullptr;
}

// ===================== START SERVER =====================
void start_server(int port) {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket failed"); exit(1); }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind failed"); exit(1); }
    if (listen(server_fd, 50) < 0) { perror("listen failed"); exit(1); }

    std::cout << "[INFO] Server running on port " << port << "\n";

    pthread_t worker;
    pthread_create(&worker, nullptr, worker_thread, nullptr);

    while (!g_shutdown_flag) {
        int client_socket = accept(server_fd, nullptr, nullptr);
        if (client_socket < 0) { perror("accept failed"); continue; }

        int* sock_ptr = new int(client_socket);
        pthread_t t;
        pthread_create(&t, nullptr, client_thread, sock_ptr);
        pthread_detach(t);
    }

    save_all();
    close(server_fd);
}

// ===================== SERVER INIT =====================
void server_init(const std::string &omni_file, int port) {
    g_omni_file = omni_file;
    g_user_mgr = new UserManager();
    g_dir_tree = new DirectoryTree();
    g_fbm = new FreeBlockManager();

    std::string err;
    if (!PersistenceManager::fs_load(g_omni_file, g_header, *g_user_mgr, *g_dir_tree, *g_fbm, err)) {
        std::cerr << "[INFO] No existing FS or failed to load: " << err << "\n";
        uint64_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        g_user_mgr->create_user("admin", "admin123", UserRole::ADMIN, now);
  
        std::cout << "[INFO] Admin user created.\n";
  std::cout << "[DEBUG] user check: " << g_user_mgr->find_user("admin") << "\n";
  std::cout << "[DEBUG] dumping all users:\n";
g_user_mgr->dump_users();  // just call it; it prints internally


    } else {
        std::cout << "[INFO] Loaded existing FS from " << g_omni_file << "\n";
    }

    start_server(port);
}
