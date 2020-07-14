#pragma once

#include <string>
#include <stdint.h>

namespace BigMoney {
struct Fund {
    int32_t fund_code;
    float fund_worth; 
    float valuation;
    float fluctuations;
    std::string fund_name;
    std::string last_update_time;
};
} // namespace BigMoney