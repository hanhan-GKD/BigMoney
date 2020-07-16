#include "fund_board.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/prettywriter.h>
#include "util.h"
#include "timer.h"

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
    // compute content width percent
    field_width_map_ = {
        std::make_pair("编号", x * 0.1),
        {"名称", x * 0.3},
        {"净值", x * 0.1},
        {"估值", x * 0.1},
        {"份额", x * 0.1},
        {"增长率", x * 0.1},
        {"预计收益", x * 0.1},
        {"更新时间", x * 0.1}
    };
    LoadFundFromFile();
    timer.Start(30000, std::bind(&FundBoard::GetFundData, this));
}
void FundBoard::LoadFundFromFile() {
    FILE *fp = fopen("fund.json", "rb");
    if (fp == nullptr) {
        return;
    }
    Document doc;
    std::array<char, 65535> read_buffer;
    FileReadStream is(fp, read_buffer.data(), read_buffer.size());

    // load fund json file fail
    if(doc.ParseStream(is).HasParseError() || !doc.IsArray()) {
        return;
    };
    for(auto fund_itr = doc.Begin(); fund_itr != doc.End(); fund_itr ++) {
        Fund fund;
        JSON_GET(String, "fund_code", fund.fund_code, fund_itr->GetObject());
        JSON_GET(String, "fund_name", fund.fund_name, fund_itr->GetObject());
        JSON_GET(String, "fund_last_update", fund.last_update_time, fund_itr->GetObject());
        JSON_GET(Double, "fund_share", fund.share, fund_itr->GetObject());
        funds_.push_back(fund);
    }
    GetFundData();
}

FundBoard::~FundBoard() {
    timer.Stop();
    if (curl_) {
        curl_easy_cleanup(curl_);
    }

    curl_global_cleanup();
}

void FundBoard::GetFundData() {
    for (auto &fund : funds_) {
        std::string http_response;
        std::string url = GenerateFundUrl(fund.fund_code);
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
                
                http_response[http_response.find_last_of(")")]= 0;
                size_t begin_offset = http_response.find("{");
                if(doc.Parse(http_response.c_str() + begin_offset).HasParseError()) {
                    printf("Parse response error\n");
                    continue;
                }
                if (!doc.IsObject()) {
                    printf("Invalid reponse data, %s\n", http_response.c_str());
                }
                std::string fund_code;
                JSON_GET(String, "fundcode", fund_code, doc);
                if (fund_code != fund.fund_code) {
                    printf("Reponse fund code mismatch\n");
                }
                std::string valuation;
                JSON_GET(String, "gsz", valuation, doc);
                fund.valuation = std::atof(valuation.c_str());

                std::string fluctuations;
                JSON_GET(String, "gszzl", fluctuations, doc);
                fund.fluctuations = std::atof(fluctuations.c_str());

                std::string dwjz;
                JSON_GET(String, "dwjz", dwjz, doc);
                fund.fund_worth = std::atof(dwjz.c_str());

                JSON_GET(String, "name", fund.fund_name, doc);
                JSON_GET(String, "gztime", fund.last_update_time, doc);

            }
        } else {
            printf("Network request error: %s", curl_easy_strerror(curl_code));
        }
    }
    // network request finished, send msg for update ui
    Update();
}

bool FundBoard::UpdateFund(const Fund& fund) {
    bool changed = false;
    if (fund.fund_code.empty()) {
        return changed;
    }
    fund_mutex_.lock();
    auto itr = funds_.begin();
    while(itr != funds_.end()) {
        if (itr->fund_code == fund.fund_code) {
            *itr = fund;
            changed = true;
            break;
        }
        itr ++;
    }
    // new fund 
    if (itr == funds_.end()) {
        changed = true;
        funds_.push_back(fund);
    }
    fund_mutex_.unlock();
    if (changed) {
        // write fund info to file
        Serialize();
        // update local fund info from network
        GetFundData();
    }
    return changed;
}

bool FundBoard::DeleteFund(const std::string &fund_code) {
    bool changed = false;
    fund_mutex_.lock();
    for (auto itr = funds_.begin(); itr != funds_.end(); itr ++) {
        if (itr->fund_code == fund_code) {
            funds_.erase(itr);
            changed = true;
            break;
        }
    }
    fund_mutex_.unlock();
    if (changed) {
        Update();
        Serialize();
    }
    return changed;
}

void FundBoard::Paint() {
    wclear(win_);
    int x_offset = 1, y_offset = 1;
    for (auto &field : field_width_map_) {
        mvwprintw(win_, 1, x_offset, field.first.c_str());
        x_offset += field.second;
    }
    std::lock_guard<std::mutex> lock(fund_mutex_);
    for(auto &fund: funds_) {
        x_offset = 1;
        y_offset ++;
        mvwprintw(win_, y_offset, x_offset, fund.fund_code.c_str());
        x_offset += field_width_map_[0].second;
        mvwprintw(win_, y_offset, x_offset, fund.fund_name.c_str());
        x_offset += field_width_map_[1].second;
        mvwprintw(win_, y_offset, x_offset, "%.2f", fund.fund_worth);
        x_offset += field_width_map_[2].second;
        mvwprintw(win_, y_offset, x_offset, "%.2f", fund.valuation);
        x_offset += field_width_map_[3].second;
        mvwprintw(win_, y_offset, x_offset, "%.2f", fund.share);
        x_offset += field_width_map_[4].second;
        mvwprintw(win_, y_offset, x_offset, "%.2f", fund.fluctuations);
        x_offset += field_width_map_[5].second;
        mvwprintw(win_, y_offset, x_offset, "%.2f", fund.share * (fund.valuation - fund.fund_worth));
        x_offset += field_width_map_[6].second;
        mvwprintw(win_, y_offset, x_offset, fund.last_update_time.c_str());
    }
    wrefresh(win_);
}
bool FundBoard::Serialize() {
    FILE *fp = fopen("fund.json", "wb");
    std::array<char, 65535> write_buffer;
    FileWriteStream ws(fp, write_buffer.data(), write_buffer.size());
    PrettyWriter<FileWriteStream> writer(ws);
    writer.StartArray();
    std::lock_guard<std::mutex> lock(fund_mutex_);
    for (auto &fund : funds_) {
        writer.StartObject();
        writer.Key("fund_code");
        writer.String(fund.fund_code.c_str());
        writer.Key("fund_name");
        writer.String(fund.fund_name.c_str());
        writer.Key("fund_last_update");
        writer.String(fund.last_update_time.c_str());
        writer.Key("fund_share");
        writer.Double(fund.share);
        writer.EndObject();
    }
    writer.EndArray();
    fclose(fp);
}

bool FundBoard::MessageProc(const Msg &msg) {
    switch(msg.msg_type) {
        case kUpdateFund: {
            Fund *fund = reinterpret_cast<Fund *>(msg.data);
            // update fund fail
            UpdateFund(*fund);
            delete fund;
            break;
        }
        case kDeleteFund: {
            std::string *fund_code = reinterpret_cast<std::string*>(msg.data);
            DeleteFund(*fund_code);
            delete fund_code;
        }
        default:
            break;
    }
    return Window::MessageProc(msg);
}

} // namespace BigMoney