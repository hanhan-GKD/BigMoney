#pragma once
#include "win.h"

namespace BigMoney {

class StatusBar : Window{
public:
    StatusBar(int x, int y, int startx, int starty);
    void Paint() override;
    bool MessageProc(const Msg &msg) override;
private:
    std::string show_msg_;
};
} // namespace BigMoney