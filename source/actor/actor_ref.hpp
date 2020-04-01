#pragma once

#include "mailbox.hpp"
#include "message.hpp"

#include <memory>
#include <type_traits>

namespace mbgl {

/*
*  ActorRef用来存储消息到消息盒中。
*/

template <class Object>
class ActorRef {
public:
    ActorRef(Object& object_, std::weak_ptr<Mailbox> weakMailbox_)
        : object(&object_), weakMailbox(std::move(weakMailbox_)) 
	{
    }

	//存储无返回值的消息函数
    template <typename Fn, class... Args>
    void invoke(Fn fn, Args&&... args) const 
	{
        if (auto mailbox = weakMailbox.lock()) 
		{
            mailbox->push(actor::makeMessage(*object, fn, std::forward<Args>(args)...));
        }
    }

	//存储带返回值的消息函数，并返回消息函数返回的future对象
    template <typename Fn, class... Args>
    auto ask(Fn fn, Args&&... args) const 
	{
        // 获取消息函数的返回值类型
        using ResultType = typename std::result_of<decltype(fn)(Object, Args...)>::type;

        std::promise<ResultType> promise;
        auto future = promise.get_future();

        if (auto mailbox = weakMailbox.lock()) 
		{
            mailbox->push(
                    actor::makeMessage(
                            std::move(promise), *object, fn, std::forward<Args>(args)...
                    )
            );
        } 
		else 
		{
            promise.set_exception(std::make_exception_ptr(std::runtime_error("Actor has gone away")));
        }

        return future;
    }

private:
    Object* object;
    std::weak_ptr<Mailbox> weakMailbox;
};

} // namespace mbgl
