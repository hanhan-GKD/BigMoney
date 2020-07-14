#include "fund_board.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include "util.h"

using namespace rapidjson;
namespace BigMoney {

size_t FundBoard::WriteFunction(void *data, size_t size, size_t bytes, void *user_data) {
    size_t all_bytes = size * bytes;
    std::string *str = reinterpret_cast<std::string*>(user_data);
    str->append(static_cast<char*>(data), all_bytes);
    return all_bytes;
}

FundBoard::FundBoard(int x, int y, int startx, int starty)
    : Window(x, y, startx, starty) {

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl_ = curl_easy_init();
    win_ = newwin(100, 100, 100, 100);
    wborder(win_, '|', '|', '-', '-', '+', '+', '+', '+');
    wrefresh(win_);
}

FundBoard::~FundBoard() {
    if (curl_) {
        curl_easy_cleanup(curl_);
    }
    if (win_) {
        delwin(win_);
    }
    curl_global_cleanup();
}

void FundBoard::RequestData() {
    for (auto &fund : funds_) {
        std::string http_response;
        std::string url(FUND_DATA_URL);
        url += fund.fund_code;
        curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, FundBoard::WriteFunction);
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, static_cast<void*>(&http_response));
        auto curl_code = curl_easy_perform(curl_);
        if (curl_code == CURLE_OK) {
            if (http_response.empty()) {
                // empty resposne
                printf("Resposne is empty\n");
                continue;
            } else {
                Document doc;
                // remove garbage char 
                http_response[http_response.find_last_of("}")] = '\0';
                size_t begin_offset = http_response.find("{");
                if(doc.Parse(http_response.c_str() + begin_offset).HasParseError()) {
                    printf("Parse response error\n");
                    continue;
                }
                if (!doc.IsObject()) {
                    printf("Invalid reponse data, %s\n", http_response.c_str());
                }
                int fund_code = -1;
                JSON_GET(Int, "fundcode", fund_code, doc);
                if (fund_code != fund.fund_code) {
                    printf("Reponse fund code mismatch\n");
                }
                JSON_GET(Float, "gsz", fund.valuation, doc);
                JSON_GET(Float, "gszzl", fund.fluctuations, doc);
                JSON_GET(Float, "dwjz", fund.fund_worth, doc);
                JSON_GET(String, "gztime", fund.last_update_time, doc);

            }
        } else {
            printf("Network request error: %s", curl_easy_strerror(curl_code));
        }
    }
    // network request finished, send msg for update ui
    Msg msg;
    msg.msg_type = kFundRequestFinished;
    PostMessage(msg);
}

void FundBoard::Update() {
    wrefresh(win_);
}

bool FundBoard::MessageProc(Msg *msg) {
    bool processed = true;
    switch(msg->msg_type) {
        case kFundRequestFinished:
            Update();
            break;
        default:
            processed = false;
            break;
    }
    return processed;
}

} // namespace BigMoney