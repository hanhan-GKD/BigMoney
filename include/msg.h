#pragma once
#include <stdint.h>
#include <queue>
#include <memory>
#include <mutex>

namespace BigMoney {

enum MsgType {
    kUpdateFund,
    kDeleteFund,
    kPaint,
    kQuit,
};

struct Msg {
    MsgType msg_type;
    void *data;
    uint16_t data_size;
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
    bool PostMessage(const Msg &msg);
    bool GetMessage(Msg *msg);
};

void StartMainLoop();
} // namespace BigMoney
