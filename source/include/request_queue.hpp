// request_queue.hpp

#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include "json.hpp"
using json = nlohmann::json;

struct Request {
    json request;
    int client_socket;
};

// request_queue.hpp (continued)

class RequestQueue {
private:
    std::queue<Request> q;
    std::mutex mtx;
    std::condition_variable cv;

public:
    void push(const Request& r) {
        std::unique_lock<std::mutex> lock(mtx);
        q.push(r);
        cv.notify_one(); // wake consumer
    }

    Request pop() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]{ return !q.empty(); }); // wait for request
        Request r = q.front();
        q.pop();
        return r;
    }
};


