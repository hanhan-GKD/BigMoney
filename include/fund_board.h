#pragma once

#include "fund.h"
#include <vector>
#include <utility>
#include <curl/curl.h>
#include "port.h"
#include "msg.h"
#include "win.h"
#include <mutex>
#include "timer.h"

namespace BigMoney {
class FundBoard : public Window {
public:
    FundBoard(int x, int y, int startx, int starty);
    ~FundBoard();
    void Paint() override;
    virtual bool MessageProc(const Msg &msg) override;
private:
    void GetFundData();
    void LoadFundFromFile();
    static size_t WriteFunction(void *data, size_t size, size_t bytes, void *user_data);
    bool UpdateFund(const Fund& fund);
    bool DeleteFund(const std::string &fund_code);
    bool Serialize();
    std::array<std::pair<std::string, int>, 8> field_width_map_;
private:
    std::vector<Fund> funds_;
    CURL *curl_{nullptr};
    std::mutex fund_mutex_;
    Timer timer;
};
} // namespace BigMoney