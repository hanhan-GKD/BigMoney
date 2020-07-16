#include "msg.h"
#include <unordered_set>
#include <thread>
#include "port.h"



namespace BigMoney {
// global msg queue
static MsgQueue msg_queue;

static std::unordered_set<MsgReactor*> msg_reactors;
static std::mutex msg_reactors_mutex;

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
    *msg = msg_queue_.front();
    msg_queue_.pop();
    return true;
}
bool MsgQueue::Empty() {
    std::lock_guard<std::mutex> lock(mutex_);
    return msg_queue_.empty();
}

MsgReactor::MsgReactor() {
    std::lock_guard<std::mutex> lock(msg_reactors_mutex);
    msg_reactors.insert(this);
}

MsgReactor::~MsgReactor() {
    std::lock_guard<std::mutex> lock(msg_reactors_mutex);
    msg_reactors.erase(this);
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

void StartMainLoop() {
    for(;;) {
        if(msg_queue.Empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        Msg msg;
        if (msg_queue.Dequeue(&msg)) {
            for (auto reactor : msg_reactors) {
                if (reactor->MessageProc(msg)) {
                    break;
                }
            }
            if (msg.msg_type == kQuit) {
                break;
            }
        }
        
    }
    
}
} // namespace BigMoney
