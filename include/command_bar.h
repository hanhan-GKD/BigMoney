#pragma once

#include <thread>
#include "port.h"
#include "msg.h"
#include "win.h"

namespace BigMoney {

class CommandBar : public Window{
public:
    CommandBar(int x, int y, int startx, int starty);
    void GetCommand();
    ~CommandBar();

private:
    void ParseCommand(const std::string &cmd);
private:
    std::thread *listen_thread_{nullptr};
    bool is_listen_{false};
};

} // namespace BigMoney