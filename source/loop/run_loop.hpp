#pragma once

#include "scheduler.hpp"
#include "mailbox.hpp"
#include "noncopyable.hpp"
#include "work_task.hpp"
#include "work_request.hpp"

#include <functional>
#include <queue>
#include <mutex>
#include <thread>

namespace mbgl {

namespace {

#ifndef NDEBUG
	#define MBGL_STORE_THREAD(tid) const std::thread::id tid = std::this_thread::get_id();
	#define MBGL_VERIFY_THREAD(tid) assert(tid == std::this_thread::get_id());
#else
	#define MBGL_STORE_THREAD(tid)
	#define MBGL_VERIFY_THREAD(tid)
#endif

}

using LOOP_HANDLE = void *;

class RunLoop : public Scheduler,
	private noncopyable {

public:
    enum class Type : uint8_t {
        Default,
        New,
    };

    enum class Priority : bool {
        Default = false,
        High = true,
    };

    enum class Event : uint8_t {
        None      = 0,
        Read      = 1,
        Write     = 2,
        ReadWrite = Read | Write,
    };


    RunLoop(Type type = Type::Default);
    ~RunLoop() override;

    static RunLoop* Get();
    static LOOP_HANDLE getLoopHandle();

    void run();
    void runOnce();
    void stop();

    // So far only needed by the libcurl backend.
    void addWatch(int fd, Event, std::function<void(int, Event)>&& callback);
    void removeWatch(int fd);

    // Invoke fn(args...) on this RunLoop.
    template <class Fn, class... Args>
    void invoke(Priority priority, Fn&& fn, Args&&... args) {
        push(priority, makeWorkTask(std::forward<Fn>(fn), std::forward<Args>(args)...));
    }

    // Invoke fn(args...) on this RunLoop.
    template <class Fn, class... Args>
    void invoke(Fn&& fn, Args&&... args) {
        invoke(Priority::Default, std::forward<Fn>(fn), std::forward<Args>(args)...);
    }

    // Post the cancellable work fn(args...) to this RunLoop.
    template <class Fn, class... Args>
    std::unique_ptr<AsyncRequest>
    invokeCancellable(Fn&& fn, Args&&... args) {
        std::shared_ptr<WorkTask> task = WorkTask::make(std::forward<Fn>(fn), std::forward<Args>(args)...);
        push(Priority::Default, task);
        return std::make_unique<WorkRequest>(task);
    }
                    
    void schedule(std::weak_ptr<Mailbox> mailbox) override {
        invoke([mailbox] () {
            Mailbox::maybeReceive(mailbox);
        });
    }

    class Impl;

private:
    MBGL_STORE_THREAD(tid)

    using Queue = std::queue<std::shared_ptr<WorkTask>>;

    // Wakes up the RunLoop so that it starts processing items in the queue.
    void wake();

    // Adds a WorkTask to the queue, and wakes it up.
    void push(Priority priority, std::shared_ptr<WorkTask> task) {
        std::lock_guard<std::mutex> lock(mutex);
        if (priority == Priority::High) {
            highPriorityQueue.emplace(std::move(task));
        } else {
            defaultQueue.emplace(std::move(task));
        }
        wake();
    }

    void process() {
        std::shared_ptr<WorkTask> task;
        std::unique_lock<std::mutex> lock(mutex);
        while (true) {
            if (!highPriorityQueue.empty()) {
                task = std::move(highPriorityQueue.front());
                highPriorityQueue.pop();
            } else if (!defaultQueue.empty()) {
                task = std::move(defaultQueue.front());
                defaultQueue.pop();
            } else {
                break;
            }
            lock.unlock();
            (*task)();
            task.reset();
            lock.lock();
        }
    }

    Queue defaultQueue;
    Queue highPriorityQueue;
    std::mutex mutex;

    std::unique_ptr<Impl> impl;
};


} // namespace mbgl

