#include "status_bar.h"

namespace BigMoney {

StatusBar::StatusBar(int x, int y, int startx, int starty) 
    :Window(x, y, startx, starty){
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLUE);
        Update();
}

void StatusBar::Paint() {
    wclear(win_);
    wattron(win_, COLOR_PAIR(1));
    wprintw(win_, "今日收益: 9999           2020-07-13 12:00:00");
    wattroff(win_, COLOR_PAIR(1));
    wrefresh(win_);
}

bool StatusBar::MessageProc(const Msg &msg) {
    return Window::MessageProc(msg);
}

} // namespace BigMoney