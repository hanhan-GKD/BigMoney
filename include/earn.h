#pragma once

#include "fund_board.h"
#include "status_bar.h"
#include "command_bar.h"
#include "help.h"

namespace BigMoney {

class Earn {
public:
    Earn();
    ~Earn();
private:
    FundBoard *fund_board_{nullptr};
    HelpWindow* help_windows_{nullptr};
    StatusBar *status_bar_{nullptr};
    CommandBar *command_bar_{nullptr};
};

} // namespace BigMoney
