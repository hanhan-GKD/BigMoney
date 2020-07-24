#pragma once
#include "port.h"


namespace BigMoney {

enum Color {
  kRedBlack = 1,
  kGreenBlack = 2,
  kWhiteBlue = 3
};

int GetColorPair(Color color);

} // namespace BigMoney
