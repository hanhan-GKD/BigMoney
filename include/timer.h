#pragma once

#include <thread>
#include <functional>
#include <atomic>
#include <unordered_set>
#include "msg.h"
#include <chrono>
#include <singleton.h>


namespace BigMoney {

class TimerManager;

class Timer {
  friend class TimerManager;
  typedef std::function<void(void)> TimeoutCallback;
public:
  Timer(const TimeoutCallback &callback);
  ~Timer();
  void Start(uint32_t milliseconds);
  void Stop();
private:
  std::atomic<bool> running_{false};
  TimeoutCallback callback_;
  std::chrono::time_point<std::chrono::system_clock> start_time_;
  int timeout_{0};
};

class TimerManager : 
    public sy::Singleton<TimerManager>, public MsgReactor {
public:
  void AddTimer(Timer *timer);
  void RemoveTimer(Timer *timer);
  bool MessageProc(const Msg &msg) override;
  void StartWork();
  ~TimerManager();
private:
  std::unordered_set<Timer*> timers_;
  std::mutex timers_mutex_;
  std::atomic<bool> running_{false};
  std::thread work_thread_;
};

void StartTimerLoop();
} // namespace BigMoney
