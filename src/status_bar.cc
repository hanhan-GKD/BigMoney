#include "status_bar.h"
#include "colors.h"
#include "util.h"
#include <vector>

namespace BigMoney {

StatusBar::StatusBar(int x, int y, int startx, int starty) 
  :Window(x, y, startx, starty){
  wbkgd(win_, GetColorPair(kWhiteBlue));
  Update();
  timer_ = new Timer([this]{
    if (!update_) {
      std::lock_guard<std::mutex> lock(msg_mutex_);
      show_msg_.clear();
      Update();
    }
    update_ = false;
  });
  timer_->Start(3000);
}

StatusBar::~StatusBar() {
  if (timer_) {
    timer_->Stop();
  }
  delete timer_;
}
void StatusBar::Paint() {
  update_ = true;
  wclear(win_);
  std::vector<char> output_buf(x_ + 2);
  msg_mutex_.lock();
  wprintw(win_, _TEXT(show_msg_.c_str()));
  msg_mutex_.unlock();
  snprintf(output_buf.data(), output_buf.size(),
           " 估算收益: %.2f  估算总值: %.2f ",
           fund_income_.income, fund_income_.sum);
  int width = StringWidth(output_buf.data());
  mvwprintw(win_, 0, x_ - width, _TEXT(output_buf.data()));
  wrefresh(win_);
}

bool StatusBar::MessageProc(const Msg &msg) {
  bool processed = false;
  if (msg.msg_type == kUpdateIncome) {
    auto fund_income = static_cast<FundIncome*>(msg.lparam);
    fund_income_ = *fund_income;
    delete fund_income;
    Update();
    processed = true;
  } else if (msg.msg_type == kUpdateStatus){
    auto *str = reinterpret_cast<std::vector<char>*>(msg.lparam);
    msg_mutex_.lock();
    show_msg_ = str->data();
    msg_mutex_.unlock();
    delete str;
    Update();
    processed = true;
  } else {
    processed = Window::MessageProc(msg);
  }
  return processed;
}

} // namespace BigMoney
