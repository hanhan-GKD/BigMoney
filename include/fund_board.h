#pragma once

#include "fund.h"
#include <vector>
#include <curl/curl.h>
#include "port.h"
#include "msg.h"
#include "win.h"

#define FUND_DATA_URL "https://fundmobapi.eastmoney.com/FundMApi/FundVarietieValuationDetail.ashx?deviceid=wap&plat=Wap&product=EFund&version=2.0.0&FCODE="


namespace BigMoney {
class FundBoard : public Window {
public:
    FundBoard(int x, int y, int startx, int starty);
    ~FundBoard();
    void Update();
    virtual bool MessageProc(Msg *msg) override;
private:
    void RequestData();
    static size_t WriteFunction(void *data, size_t size, size_t bytes, void *user_data);
private:
    std::vector<Fund> funds_;
    WINDOW *win_{nullptr};
    CURL *curl_{nullptr};
};
} // namespace BigMoney