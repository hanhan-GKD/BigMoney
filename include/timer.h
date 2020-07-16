#pragma once

#include <thread>
#include <chrono>
#include <functional>
#include <atomic>


namespace BigMoney {

class Timer {
typedef std::function<void(void)> TimeoutCallback;
public:
    void Start(int milliseconds, const TimeoutCallback &callback) {
        running_ = true;
        wait_thread_ = std::thread([=]{
            while(running_) {
                std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
                callback();
            }
        });
    }
    void Stop() {
        if (running_) {
            running_ = false;
            if (wait_thread_.joinable()) {
                wait_thread_.join();
            }
        }
    }
private:
    std::thread wait_thread_;
    std::atomic<bool> running_{false};
};
} // namespace BigMoney
