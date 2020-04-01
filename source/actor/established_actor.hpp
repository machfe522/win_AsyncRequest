#pragma once

#include "aspiring_actor.hpp"
#include "mailbox.hpp"
#include "message.hpp"
#include "actor_ref.hpp"

#include <memory>
#include <future>
#include <type_traits>
#include <cassert>

namespace mbgl 
{

/*
*  EstablishedActor用来构建和管理消息对象，以及消息盒的打开和关闭。
*/

template <class Object>
class EstablishedActor 
{
public:
	template <typename U = Object, class... Args, typename std::enable_if<
        std::is_constructible<U, Args...>::value ||
		std::is_constructible<U, ActorRef<U>, Args...>::value>::type * = nullptr>
    EstablishedActor(Scheduler& scheduler, AspiringActor<Object>& parent_, Args&& ... args)
    :   parent(parent_)
	{
		//构建消息对象， 并存储到AspiringActor<Object>类中
        emplaceObject(std::forward<Args>(args)...);
		//打开消息盒
        parent.mailbox->open(scheduler);
    }
   
    template <class ArgsTuple, std::size_t ArgCount = std::tuple_size<std::decay_t<ArgsTuple>>::value>
    EstablishedActor(Scheduler& scheduler, AspiringActor<Object>& parent_, ArgsTuple&& args)
    :   parent(parent_) 
	{
		//构建消息对象， 并存储到AspiringActor<Object>类中
        emplaceObject(std::forward<ArgsTuple>(args), std::make_index_sequence<ArgCount>{});
		
		//打开消息盒
        parent.mailbox->open(scheduler);
    }
    
    EstablishedActor(const EstablishedActor&) = delete;

    ~EstablishedActor() 
	{
		//关闭消息盒，并清理AspiringActor类存储的消息对象和消息数据
        parent.mailbox->close();
        parent.object().~Object();
    }
    
private:

	template <typename U = Object, class... Args, typename std::enable_if<std::is_constructible<U, ActorRef<U>, Args...>::value>::type * = nullptr>
    void emplaceObject(Args&&... args_) 
	{
        new (&parent.objectStorage) Object(parent.self(), std::forward<Args>(args_)...);
    }

    // Enabled for plain Objects
    template <typename U = Object, class... Args, typename std::enable_if<std::is_constructible<U, Args...>::value>::type * = nullptr>
    void emplaceObject(Args&&... args_) 
	{
        new (&parent.objectStorage) Object(std::forward<Args>(args_)...);
    }    
 
	template <class ArgsTuple, std::size_t... I>
    void emplaceObject(ArgsTuple&& args, std::index_sequence<I...>)
	{
        emplaceObject(std::move(std::get<I>(std::forward<ArgsTuple>(args)))...);
        (void) args; 
    }
	
	//消息对象和消息数据的存储位置。
    AspiringActor<Object>& parent;
};

} // namespace mbgl
