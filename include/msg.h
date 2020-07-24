#pragma once

#include <stdint.h>
#include <queue>
#include <memory>
#include <mutex>
#include <singleton.h>
#include <unordered_set>

namespace BigMoney {

enum MsgType {
  kUpdateFund,
  kDeleteFund,
  kUpdateIncome,
  kUpdateStatus,
  kPrePage,
  kNextPage,
  kPaint,
  kShowHelp,
  kHiddenPop,
  kReloadFile,
  kQuit,
};

struct Msg {
  MsgType msg_type;
  void *lparam;
  void *rparam;
};

class MsgQueue {
public:
  bool Enqueue(const Msg& msg);
  bool Dequeue(Msg *msg);
  bool Empty();
private:
  std::queue<Msg> msg_queue_;
  std::mutex mutex_;
};


class MsgReactor {
public:
  MsgReactor();
  virtual ~MsgReactor();
  virtual bool MessageProc(const Msg &msg) = 0;
};

class MsgManager : public sy::Singleton<MsgManager> {
public:
  inline bool Enqueue(const Msg& msg) {
    return msg_queue_.Enqueue(msg);
  }
  inline bool Dequeue(Msg *msg) {
    return msg_queue_.Dequeue(msg);
  }
  inline bool QueueEmpty() {
    return msg_queue_.Empty();
  }
  void AddReactor(MsgReactor *reactor);
  void RemoveReactor(MsgReactor *reactor);
  void StartMainLoop();
private:
  MsgQueue msg_queue_;
  std::unordered_set<MsgReactor*> reactors_;
  std::mutex reactors_mutex_;
};

bool GetMsg(Msg *msg);
bool PostMsg(const Msg &msg);
void StartMainLoop();
} // namespace BigMoney
