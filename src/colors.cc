#include "colors.h"
#include <map>

namespace BigMoney {

void Init() {
    static bool is_init = false;
    if (!is_init) {
        start_color();
        init_pair(kGreenBlack, COLOR_GREEN, COLOR_BLACK) ;
        init_pair(kRedBlack, COLOR_RED, COLOR_BLACK); 
        init_pair(kWhiteBlue, COLOR_WHITE, COLOR_BLUE);
        is_init = true;
    }
}

int GetColorPair(Color color) {
    Init();
    return COLOR_PAIR(color);
}

} // namespace BigMoney