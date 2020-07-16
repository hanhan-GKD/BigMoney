#pragma once

#include "util.h"

#ifdef _WIN32
#include <curses.h>
#include <windows.h>
#define _TEXT(str) UTF8ToANSI((str)).c_str()
#else
#include <ncurses.h>
#define _TEXT(str) str
#endif 
