#pragma once

#include "port.h"
#include "fund.h"
#include <vector>
#include <utility>
#include <curl/curl.h>
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
private:
    const static std::array<std::pair<std::string, int>, 9> FIELD_WIDTH_MAP;
    std::vector<Fund> funds_;
    CURL *curl_{nullptr};
    std::mutex fund_mutex_;
    Timer *timer{nullptr};
};
} // namespace BigMoney