#include "status_bar.h"
#include "colors.h"

namespace BigMoney {

StatusBar::StatusBar(int x, int y, int startx, int starty) 
    :Window(x, y, startx, starty){
        wbkgd(win_, GetColorPair(kWhiteBlue));
        Update();
}

void StatusBar::Paint() {
    wclear(win_);
    wprintw(win_, _TEXT("今日收益: 9999           2020-07-13 12:00:00"));
    wrefresh(win_);
}

bool StatusBar::MessageProc(const Msg &msg) {
    return Window::MessageProc(msg);
}

} // namespace BigMoney