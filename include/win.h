#pragma once
#include "port.h"
#include "msg.h"

namespace BigMoney {
class Window : public MsgReactor{
public:
    Window(int x, int y, int startx, int starty)
        : x_(x),
          y_(y),
          startx_(startx),
          starty_(starty) {
    }

protected:
    int x_;
    int y_;
    int startx_;
    int starty_;
};
} // namespace BigMoney