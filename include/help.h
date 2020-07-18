#pragma once

#include "win.h"

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