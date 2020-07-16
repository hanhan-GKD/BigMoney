#pragma once

#include "util.h"
#if (defined _WIN32)
#include <curses.h>
#include <windows.h>
#define _TEXT(str) UTF8ToANSI((str)).c_str()
#else
#include <ncurses.h>
#define _TEXT(str) 
#endif 
