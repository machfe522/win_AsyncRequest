#pragma once

#include "noncopyable.hpp"

#include <memory>
#include <functional>

namespace mbgl {

class AsyncTask : private noncopyable {
public:
    AsyncTask(std::function<void()>&&);
    ~AsyncTask();

    void send();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};


} // namespace mbgl
