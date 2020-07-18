#include "earn.h"
#include "port.h"

namespace BigMoney {

Earn::Earn() {
    setlocale(LC_ALL, "");
	initscr();			
	raw();				
	keypad(stdscr, TRUE);		
	curs_set(0);
	cbreak();
    int max_row = 0, max_col = 0;
    getmaxyx(stdscr, max_col, max_row);

	fund_board_ = new FundBoard(max_row, max_col - 2, 0, 0);
    int help_win_width = max_row * 0.6;
    int help_win_height = (max_col -2) * 0.6;
    int help_win_left = (max_row - help_win_width) / 2;
    int help_win_top = (max_col - help_win_height - 2) / 2;
    help_windows_ = new HelpWindow(help_win_width, 
        help_win_height, help_win_left, help_win_top);
	status_bar_ = new StatusBar(max_row, 1, 0, max_col - 2);
	command_bar_ = new CommandBar(max_row, 1, 0, max_col - 1);
    command_bar_->GetCommand();
}

Earn::~Earn() {
    endwin();
    if (fund_board_) {
        delete fund_board_;
    }
    if (help_windows_) {
        delete help_windows_;
    }
    if (status_bar_) {
        delete status_bar_;
    }
    if (command_bar_) {
        delete command_bar_;
    }
}

} // namespace BigMoney