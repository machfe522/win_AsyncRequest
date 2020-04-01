#pragma once

#include "noncopyable.hpp"

#include <memory>
#include <functional>

namespace mbgl {
 
class Timer : private noncopyable {
public:
    Timer();
    ~Timer();

    void start(uint64_t timeout, uint64_t repeat, std::function<void()>&&);
    void stop();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

 
} // namespace mbgl
