#pragma once

#include "aspiring_actor.hpp"
#include "established_actor.hpp"
#include "mailbox.hpp"
#include "message.hpp"
#include "actor_ref.hpp"
 

#include <memory>
#include <future>
#include <type_traits>
#include <cassert>

namespace mbgl {
 
template <class Object>
class Actor 
{
public:
    template <class... Args>
    Actor(Scheduler& scheduler, Args&&... args)
        : target(scheduler, parent, std::forward<Args>(args)...) 
	{
	}

    template <class... Args>
    Actor(std::shared_ptr<Scheduler> scheduler, Args&&... args)
        : retainer(std::move(scheduler)), target(retainer, parent, std::forward<Args>(args)...) 
	{
	}

    Actor(const Actor&) = delete;

    ActorRef<std::decay_t<Object>> self()
	{
        return parent.self();
    }

private:
    std::shared_ptr<Scheduler> retainer;
    AspiringActor<Object> parent;
    EstablishedActor<Object> target;
};

} // namespace mbgl
