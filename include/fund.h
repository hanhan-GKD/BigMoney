#pragma once

#include <string>
#include <stdint.h>

namespace BigMoney {
struct Fund {
    std::string fund_code;
    float fund_worth{0.0f}; 
    float valuation{0.0f};
    float fluctuations{0.0f};
    float share{0.0f};
    std::string fund_name;
    std::string last_update_time;
};
} // namespace BigMoney