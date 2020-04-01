#pragma once

#include "noncopyable.hpp"

namespace mbgl {

class AsyncRequest : private noncopyable {
public:
    virtual ~AsyncRequest() = default;
};

} // namespace mbgl
