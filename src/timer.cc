#include "timer.h"
#include <mutex>
#include <unordered_set>
#include <ctime>

using namespace std::chrono;

namespace BigMoney {

Timer::Timer(const TimeoutCallback &callback) 
  : callback_(callback){
  TimerManager::instance().AddTimer(this);
}
void Timer::Start(uint32_t milliseconds) {
  running_ = true;
  timeout_ = milliseconds;
  start_time_ = system_clock::now();
}
void Timer::Stop() {
  running_ = false;
}

Timer::~Timer() {
  TimerManager::instance().RemoveTimer(this);
}

void TimerManager::AddTimer(Timer *timer) {
  std::lock_guard<std::mutex> lock(timers_mutex_);
  timers_.insert(timer);
}
void TimerManager::RemoveTimer(Timer *timer) {
  std::lock_guard<std::mutex> lock(timers_mutex_);
  timers_.erase(timer);
}

void TimerManager::StartWork() {
  running_ = true;
  work_thread_ = std::thread([this] {
    while (running_) {
      timers_mutex_.lock();
      auto now = system_clock::now();
      for (auto timer : timers_) {
        if (timer->running_) {
          auto elapsed = duration_cast<milliseconds>(now - timer->start_time_).count();
          if (elapsed > timer->timeout_) {
            timer->callback_();
            timer->start_time_ = now;
          }
        }
      }
      timers_mutex_.unlock();
      std::this_thread::sleep_for(milliseconds(200));
    }
  });
}

TimerManager::~TimerManager() {
  if (work_thread_.joinable()) {
    work_thread_.join();
  }
}

bool TimerManager::MessageProc(const Msg &msg) {
  if (msg.msg_type == kQuit) {
    running_ = false;
  }
  return false;
}

void StartTimerLoop() {
  TimerManager::instance().StartWork();
}

} // namespace BigMoney
