#pragma once
#include <pthread.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include "request_queue.hpp"
#include "file_ops.hpp"
#include "config_loader.hpp"


extern RequestQueue requestQueue;
void server_init(const std::string &omni_file, int port);
void start_server(int port);
void* client_thread(void* arg);
void* worker_thread(void* arg);

void start_server(int port);
