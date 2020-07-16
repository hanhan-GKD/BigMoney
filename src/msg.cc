#include "msg.h"
#include <thread>
#include "port.h"
#include "timer.h"



namespace BigMoney {

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
    MsgManager::instance()->AddReactor(this);
}

MsgReactor::~MsgReactor() {
    MsgManager::instance()->RemoveReactor(this);
}

void MsgManager::AddReactor(MsgReactor *reactor) {
    std::lock_guard<std::mutex> lock(reactors_mutex_);
    reactors_.insert(reactor);
}

void MsgManager::RemoveReactor(MsgReactor *reactor) {
    std::lock_guard<std::mutex> lock(reactors_mutex_);
    reactors_.erase(reactor);
}

bool GetMsg(Msg *msg) {
    if (MsgManager::instance()->QueueEmpty()) {
        return false;
    }
    return MsgManager::instance()->Dequeue(msg);
}

bool PostMsg(const Msg &msg) {
    return MsgManager::instance()->Enqueue(msg);
}

void MsgManager::StartMainLoop() {
    for(;;) {
        if(QueueEmpty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        Msg msg;
        if (Dequeue(&msg)) {
            for (auto reactor : reactors_) {
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

void StartMainLoop() {
    StartTimerLoop();
    MsgManager::instance()->StartMainLoop();
}
} // namespace BigMoney
