#pragma once

#include "async_request.hpp"

#include <functional>
#include <memory>


namespace mbgl {

using Callback = std::function<void(void*)>;


class HTTPFileSource {
public:
    HTTPFileSource();
    ~HTTPFileSource();

    std::unique_ptr<AsyncRequest> request(const std::string&,  Callback);

    class Impl;

private:
    std::unique_ptr<Impl> impl;
};

} // namespace mbgl
