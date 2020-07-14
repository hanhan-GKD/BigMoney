#pragma once

#if (defined WIN32) || (defined MSVC)
#include <curses.h>
#else
#include <ncurses.h>
#endif 
