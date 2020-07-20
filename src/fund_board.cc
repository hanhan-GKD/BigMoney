#include "fund_board.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/prettywriter.h>
#include <array>
#include "util.h"
#include "timer.h"
#include "colors.h"
#include <memory>

using namespace rapidjson;
namespace BigMoney {

// compute content 
const std::array<std::pair<std::string, int>, 8> FundBoard::FIELD_WIDTH_MAP = {
        std::make_pair("编号", StringWidth("编号")),
        std::make_pair("名称", StringWidth("名称")),
        std::make_pair("净值", StringWidth("净值")),
        std::make_pair("估值", StringWidth("估值")),
        std::make_pair("份额", StringWidth("份额")),
        std::make_pair("增长率", StringWidth("增长率")),
        std::make_pair("预计收益", StringWidth("预计收益")),
        std::make_pair("更新时间", StringWidth("更新时间"))
 };

size_t FundBoard::WriteFunction(void *data, size_t size, size_t bytes, void *user_data) {
    size_t all_bytes = size * bytes;
    std::string *str = reinterpret_cast<std::string*>(user_data);
    if (str->max_size() > str->size() + all_bytes) {
        str->append(static_cast<char*>(data), all_bytes);
        return all_bytes;
    } else {
        return 0;
    }
}

FundBoard::FundBoard(int x, int y, int startx, int starty)
    : Window(x, y, startx, starty) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl_ = curl_easy_init();
    LoadFundFromFile();
    timer = new Timer(std::bind(&FundBoard::GetFundData, this));
    timer->Start(60000);
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
        UPDATE_STATUS("解析基金配置文件失败");
        return;
    };
    fund_mutex_.lock();
    funds_.clear();
    for(auto fund_itr = doc.Begin(); fund_itr != doc.End(); fund_itr ++) {
        Fund fund;
        JSON_GET(String, "fund_code", fund.fund_code, fund_itr->GetObject());
        JSON_GET(String, "fund_name", fund.fund_name, fund_itr->GetObject());
        JSON_GET(String, "fund_last_update", fund.last_update_time, fund_itr->GetObject());
        JSON_GET(Double, "fund_share", fund.share, fund_itr->GetObject());
        funds_.push_back(fund);
    }
    fund_mutex_.unlock();
    fclose(fp);
    GetFundData();
}

FundBoard::~FundBoard() {
    if (timer) {
        timer->Stop();
        delete timer;
    }
    if (curl_) {
        curl_easy_cleanup(curl_);
    }

    curl_global_cleanup();
}

void FundBoard::GetFundData() {
    std::lock_guard<std::mutex> lock(fund_mutex_);
    for (auto &fund : funds_) {
        UPDATE_STATUS("请求基金数据: %s", fund.fund_code.c_str());
        auto resp_buf = new std::string();
        auto http_response = std::unique_ptr<std::string>(resp_buf);
        std::string url = GenerateFundUrl(fund.fund_code);
        curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_, 
            CURLOPT_WRITEFUNCTION, FundBoard::WriteFunction);
        curl_easy_setopt(curl_, 
            CURLOPT_WRITEDATA, 
            static_cast<void*>(http_response.get()));
        auto curl_code = curl_easy_perform(curl_);
        if (curl_code == CURLE_OK) {
            if (http_response->empty()) {
                // empty resposne
                UPDATE_STATUS("请求失败, 基金: %s", fund.fund_code.c_str());
                continue;
            } else {
                Document doc;
                // remove garbage char 
                size_t begin_offset = http_response->find("{");
                size_t end_offset = http_response->find_last_of(")");
                if (begin_offset == http_response->npos || 
					end_offset == http_response->npos) {
                    UPDATE_STATUS("基金数据格式错误, 基金: %s", fund.fund_code.c_str());
                    continue;
                }
                http_response->at(end_offset)= 0;
                if(doc.Parse(http_response->c_str() + begin_offset).HasParseError()) {
                    UPDATE_STATUS("解析数据失败, 基金: %s", fund.fund_code.c_str());
                    continue;
                }
                if (!doc.IsObject()) {
                    UPDATE_STATUS("数据格式无效, 基金: %s", fund.fund_code.c_str());
                }
                std::string fund_code;
                JSON_GET(String, "fundcode", fund_code, doc);
                if (fund_code != fund.fund_code) {
                    UPDATE_STATUS("基金编码不匹配\n");
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
                if (fund.share > 0) {
                    fund.income = fund.share * (fund.valuation - fund.fund_worth);
                }

                JSON_GET(String, "name", fund.fund_name, doc);
                JSON_GET(String, "gztime", fund.last_update_time, doc);

            }
        } else {
            UPDATE_STATUS("网络请求出现错误");
        }
    }
    // network request finished, send msg for update ui
    Update();
    float income = 0.0f;
    float sum = 0.0f;
    for (auto &fund : funds_) {
        income += fund.income;
        sum += fund.share * fund.valuation;
    }
    Msg msg;
    msg.msg_type = kUpdateIncome;
    msg.lparam = reinterpret_cast<void *>((*reinterpret_cast<int32_t *>(&income)));
    msg.rparam = reinterpret_cast<void *>((*reinterpret_cast<int32_t *>(&sum)));
    PostMsg(msg);
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

bool FundBoard::DeleteFund(const std::string& fund_code) {
    bool changed = false;
    fund_mutex_.lock();
    if (fund_code == "all") {
        funds_.clear();
		changed = true;
    } else {
		for (auto itr = funds_.begin(); itr != funds_.end(); itr++) {
            if (itr->fund_code == fund_code) {
                funds_.erase(itr);
                changed = true;
                break;
            }
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
    int x_offset = 0, y_offset = 0;
    auto field_width_map = FIELD_WIDTH_MAP;
    std::lock_guard<std::mutex> lock(fund_mutex_);
    for(auto &fund: funds_) {
        int width = StringWidth(fund.fund_code);
        field_width_map[0].second = std::max(field_width_map[0].second, width);
        width = StringWidth(fund.fund_name);
        field_width_map[1].second = std::max(field_width_map[1].second, width);
        width = FloatWidth(fund.fund_worth);
        field_width_map[2].second = std::max(field_width_map[2].second, width);
        width = FloatWidth(fund.valuation);
        field_width_map[3].second = std::max(field_width_map[3].second, width);
        width = FloatWidth(fund.share);
        field_width_map[4].second = std::max(field_width_map[4].second, width);
        width = FloatWidth(fund.fluctuations);
        field_width_map[5].second = std::max(field_width_map[5].second, width);
        width = FloatWidth(fund.share * (fund.valuation - fund.fund_worth) * 1.0);
        field_width_map[6].second = std::max(field_width_map[6].second, width);
        width = StringWidth(fund.last_update_time);
        field_width_map[7].second = std::max(field_width_map[7].second, width);
    }
    for (auto &field : field_width_map) {
        mvwprintw(win_, 0, x_offset, _TEXT(field.first.c_str()));
        x_offset += field.second + 2;
    }
    mvwhline(win_, ++y_offset, 0, '-', x_);
    // compute float value format
    std::array<std::string, 5> format_table;
    std::array<char, 30> format_buffer;
    for (size_t i = 2, j = 0; i < 7; i ++, j ++) {
        memset(format_buffer.data(), 0, format_buffer.size());
        snprintf(format_buffer.data(), format_buffer.size(), "%%%d.2f", field_width_map[i].second);
        format_table[j] = std::string(format_buffer.data());
    }
    for(auto &fund: funds_) {
        x_offset = 0;
        y_offset ++;
        mvwprintw(win_, y_offset, x_offset, fund.fund_code.c_str());
        x_offset += field_width_map[0].second + 2;
        mvwprintw(win_, y_offset, x_offset, _TEXT(fund.fund_name.c_str()));
        x_offset += field_width_map[1].second + 2;
        mvwprintw(win_, y_offset, x_offset, format_table[0].c_str(), fund.fund_worth);
        x_offset += field_width_map[2].second + 2;
        mvwprintw(win_, y_offset, x_offset, format_table[1].c_str(), fund.valuation);
        x_offset += field_width_map[3].second + 2;
        mvwprintw(win_, y_offset, x_offset, format_table[2].c_str(), fund.share);
        x_offset += field_width_map[4].second + 2;
        if (fund.fluctuations > 0) {
            wattron(win_, GetColorPair(kRedBlack));
        } else if (fund.fluctuations < 0){
            wattron(win_, GetColorPair(kGreenBlack));
        }
        mvwprintw(win_, y_offset, x_offset, format_table[3].c_str(), fund.fluctuations);
        x_offset += field_width_map[5].second + 2;
        mvwprintw(win_, y_offset, x_offset, format_table[4].c_str(), fund.share * (fund.valuation - fund.fund_worth));
        x_offset += field_width_map[6].second + 2;
        if (fund.fluctuations > 0) {
            wattroff(win_, GetColorPair(kRedBlack));
        } else if(fund.fluctuations < 0){
            wattroff(win_, GetColorPair(kGreenBlack));
        }

        mvwprintw(win_, y_offset, x_offset, fund.last_update_time.c_str());
    }
    wrefresh(win_);
}
bool FundBoard::Serialize() {
    FILE *fp = fopen("fund.json", "wb");
    std::array<char, 65535> write_buffer;
    FileWriteStream ws(fp, write_buffer.data(), write_buffer.size());
    PrettyWriter<FileWriteStream> writer(ws);
    std::lock_guard<std::mutex> lock(fund_mutex_);
    writer.StartArray();
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
    UPDATE_STATUS("保存配置文件完成");
    return true;
}

bool FundBoard::MessageProc(const Msg &msg) {
    bool processed = false;
    switch(msg.msg_type) {
        case kUpdateFund: {
            Fund *fund = reinterpret_cast<Fund *>(msg.lparam);
            // update fund fail
            UPDATE_STATUS("添加基金: %s", fund->fund_code.c_str());
            UpdateFund(*fund);
            delete fund;
            processed = true;
            break;
        }
        case kDeleteFund: {
            std::string *fund_code = reinterpret_cast<std::string*>(msg.lparam);
            UPDATE_STATUS("删除基金: %s", fund_code->c_str());
            DeleteFund(*fund_code);
            delete fund_code;
            processed = true;
            break;
        }
        case kReloadFile: {
            UPDATE_STATUS("重新加载配置文件");
            LoadFundFromFile();
            processed = true;
            break;
        }
        default: {
            processed = Window::MessageProc(msg);
            break;
        }
    }
    return processed;
}

} // namespace BigMoney