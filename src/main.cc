#include <stdio.h>
#include <ncurses.h>
#include "command_bar.h"
#include <locale.h>
#include "fund_board.h"
#include "msg.h"
#include "status_bar.h"

using namespace BigMoney;

int main() {
	setlocale(LC_ALL, "");
	int ch;

	initscr();			/* Start curses mode 		*/
	raw();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	curs_set(0);
	cbreak();
	FundBoard fundboard(200, 20, 0, 40);
	StatusBar sbar(200, 10, 0, 0);
	BigMoney::CommandBar bar(200, 20, 0, 20);
	bar.GetCommand();
	StartMainLoop();
	endwin();			/* End curses mode		  */

	return 0;
}