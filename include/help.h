#pragma once

#include "win.h"

#define GITHUB "GitHub: https://github.com/hanhan-GKD/BigMoney"
#define BIG_MONEY_VERSION "0.14" 

namespace BigMoney {

class HelpWindow : public Window {
public:
  HelpWindow(int x, int y, int startx, int starty);
  bool MessageProc(const Msg& msg) override;
  void Paint() override;
private:
  bool show_{ false };
};

} // namespace BigMoney
