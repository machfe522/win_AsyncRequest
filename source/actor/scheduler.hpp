#pragma once

#include <memory>

namespace mbgl {

class Mailbox;

class Scheduler 
{
public:
    virtual ~Scheduler() = default;
    
    // Used by a Mailbox when it has a message in its queue to request that it
    // be scheduled. Specifically, the scheduler is expected to asynchronously
    // call `receive() on the given mailbox, provided it still exists at that
    // time.
	virtual void schedule(std::weak_ptr<Mailbox>) = 0;

    // Set/Get the current Scheduler for this thread
    static Scheduler* GetCurrent();
    static void SetCurrent(Scheduler*);

    // Get the scheduler for asynchronous tasks. This method
    // will lazily initialize a shared worker pool when ran
    // from the first time.
    static std::shared_ptr<Scheduler> GetBackground();
};

} // namespace mbgl
