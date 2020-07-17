#pragma once
#include "win.h"
#include "timer.h"
#include <atomic>
#include <mutex>

namespace BigMoney {

class StatusBar : Window{
public:
    StatusBar(int x, int y, int startx, int starty);
    ~StatusBar();
    void Paint() override;
    bool MessageProc(const Msg &msg) override;
private:
    std::string show_msg_;
    float income_{0.0f};
    float sum_{0.0f};
    Timer *timer_{nullptr};
    std::atomic<bool> update_{false};
    std::mutex msg_mutex_;
};
} // namespace BigMoney