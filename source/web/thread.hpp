#pragma once

#include "actor.hpp"
#include "mailbox.hpp"
#include "scheduler.hpp"
#include "run_loop.hpp"

#include <memory> 
#include <cassert>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <utility>

namespace mbgl {


 
template<class Object>
class Thread {

public:
    template <class... Args>
    Thread(const std::string& name, Args&&... args) {

        std::promise<void> running_;
        running = running_.get_future();

        auto capturedArgs = std::make_tuple(std::forward<Args>(args)...);

        thread = std::thread([
            this,
            name,
            capturedArgs = std::move(capturedArgs),
            runningPromise = std::move(running_)
        ] () mutable {

			RunLoop loop_(RunLoop::Type::New);
			loop = &loop_;

            EstablishedActor<Object> establishedActor(loop_, object, std::move(capturedArgs));

            runningPromise.set_value();
            
            loop->run();
            
            (void) establishedActor;
            
			loop = nullptr;
        });
    }

    ~Thread() {
        if (paused) {
            resume();
        }

        std::promise<void> stoppable;
        
        running.wait();

    

        loop->invoke([&] {
            stoppable.set_value();
        });

        stoppable.get_future().get();

        loop->stop();
        thread.join();
    }

    ActorRef<std::decay_t<Object>> actor() {
        return object.self();
    }

    void pause() {
        MBGL_VERIFY_THREAD(tid);

        assert(!paused);

        paused = std::make_unique<std::promise<void>>();
        resumed = std::make_unique<std::promise<void>>();

        auto pausing = paused->get_future();

        running.wait();

        loop->invoke(RunLoop::Priority::High, [this] {
            auto resuming = resumed->get_future();
            paused->set_value();
            resuming.get();
        });

        pausing.get();
    }

    // Resumes the `Object` thread previously paused by `pause()`.
    void resume() {
        MBGL_VERIFY_THREAD(tid);

        assert(paused);

        resumed->set_value();

        resumed.reset();
        paused.reset();
    }

private:
    MBGL_STORE_THREAD(tid);

    AspiringActor<Object> object;

    std::thread thread;

    std::future<void> running;
    
    std::unique_ptr<std::promise<void>> paused;
    std::unique_ptr<std::promise<void>> resumed;

	RunLoop*  loop = nullptr;
};


} // namespace mbgl
