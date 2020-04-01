#include "scheduler.hpp"
#include "thread_local.hpp"


namespace mbgl {


static auto& current() {
    static ThreadLocal<Scheduler> scheduler;
    return scheduler;
};

void Scheduler::SetCurrent(Scheduler* scheduler) {
    current().set(scheduler);
}

Scheduler* Scheduler::GetCurrent() {
    return current().get();
}
 

} //namespace mbgl
