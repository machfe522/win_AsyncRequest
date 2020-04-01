#pragma once

#include <memory>
#include <mutex>
#include <queue>

namespace mbgl {

class Scheduler;
class Message;

//Mailbox 消息盒

class Mailbox : public std::enable_shared_from_this<Mailbox> {
public:
 
    Mailbox();    
    Mailbox(Scheduler&);

	//打开消息盒
    void open(Scheduler& scheduler_);

	//关闭消息盒
    void close();

    bool isOpen() const;

	//存储一个消息数据
    void push(std::unique_ptr<Message>);

	//处理消息
    void receive();

	//尝试处理一个消息
    static void maybeReceive(std::weak_ptr<Mailbox>);

private:
	Scheduler* scheduler = nullptr;

    std::recursive_mutex receivingMutex;

	//存储消息的互斥锁
    std::mutex pushingMutex;

	//消息是否关闭的状态
    bool closed { false };

	//消息队列的互斥锁
    std::mutex queueMutex;

	//消息队列
    std::queue<std::unique_ptr<Message>> queue;
};

} // namespace mbgl
