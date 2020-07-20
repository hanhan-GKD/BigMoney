#include "help.h"
#include "colors.h"
#include <array>

namespace BigMoney {

static const std::array<const char*, 8> HELP_INFO = {
								"使用方法:",
								"\t添加/修改基金: \t\tupdate <基金编号> <持有份额>",
								"\t删除基金: \t\tdelete <基金编号>",
								"\t删除全部: \t\tdelete all",
								"\t重新加载: \t\treload",
								"\t关闭: \t\t\tq",
								"\t退出程序: \t\tquit",
								"\t帮助: \t\t\thelp"
};
static const char* GITHUB = "GitHub:\thttps://github.com/hanhan-GKD/BigMoney";

HelpWindow::HelpWindow(int x, int y, int startx, int starty)
	:Window(x, y, startx, starty){
}
bool HelpWindow::MessageProc(const Msg& msg) {
	bool processed = false;
	if (msg.msg_type == kHiddenPop) {
		show_ = false;
		UpdateNow();
	}
	else if (msg.msg_type == kShowHelp) {
		show_ = true;
		Update();
		processed = true;
	} else {
		processed = Window::MessageProc(msg);
	}
	return processed;
}

void HelpWindow::Paint() {
	wclear(win_);
	if (show_) {
		wborder(win_, '|', '|', '-', '-', '+', '+', '+', '+');
		int y_offset = 1;
		for (auto item : HELP_INFO) {
			mvwprintw(win_, y_offset, 1, _TEXT(item));
			y_offset++;
		}
		mvwprintw(win_, y_ - 2, 1, _TEXT(GITHUB));
	}
	wrefresh(win_);
}
} // namespace BigMoney