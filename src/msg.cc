#include "msg.h"

namespace BigMoney {
// global msg queue
static MsgQueue msg_queue;

bool MsgQueue::Enqueue(const Msg &msg) {
    std::lock_guard<std::mutex> lock(mutex_);
    msg_queue_.push(msg);
    return true;
}
bool MsgQueue::Dequeue(Msg *msg) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (msg_queue_.empty()) {
        return false;
    }
    *msg = msg_queue_.back();
    msg_queue_.pop();
    return true;
}
bool MsgQueue::Empty() {
    std::lock_guard<std::mutex> lock(mutex_);
    return msg_queue_.empty();
}

bool MsgReactor::PostMessage(const Msg &msg) {
    return msg_queue.Enqueue(msg);
}
bool MsgReactor::GetMessage(Msg *msg) {
    if (msg_queue.Empty()) {
        return false;
    }
    return msg_queue.Dequeue(msg);
}
} // namespace BigMoney
