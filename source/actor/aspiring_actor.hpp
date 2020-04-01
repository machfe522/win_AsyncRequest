#pragma once

#include "mailbox.hpp"
#include "message.hpp"
#include "actor_ref.hpp"

#include <memory>
#include <future>
#include <type_traits>
#include <cassert>

namespace mbgl {

template <class Object>
class EstablishedActor;

template <class Object>
class Actor;


/*
*  AspiringActor只用来存储一个类对象关联的所有消息数据，不负责消息对象、消息数据的生成和管理。
*/
template <class Object>
class AspiringActor 
{
public:
    AspiringActor() : mailbox(std::make_shared<Mailbox>())
	{
        assert(!mailbox->isOpen());
    }
    
    AspiringActor(const AspiringActor&) = delete;
    
    ActorRef<std::decay_t<Object>> self() 
	{
        return ActorRef<std::decay_t<Object>>(object(), mailbox);
    }
    
private:
	Object& object()
	{
		return *reinterpret_cast<Object *>(&objectStorage);
	}

private:
	//消息盒，用来存储消息对象关联的消息函数以及对应参数。
    std::shared_ptr<Mailbox> mailbox;

	//消息对象存储位置。
    std::aligned_storage_t<sizeof(Object)> objectStorage;
    
	//友元类声明，为了在外部访问AspiringActor类的私有成员。
    friend class EstablishedActor<Object>;
    friend class Actor<Object>;
};

} // namespace mbgl
