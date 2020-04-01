
#include "response.hpp"

namespace mbgl {

Response::Response(const Response& res) {
    *this = res;
}

Response& Response::operator=(const Response& res) {
    error = res.error ? std::make_unique<Error>(*res.error) : nullptr;
    noContent = res.noContent;
    notModified = res.notModified;
    mustRevalidate = res.mustRevalidate;
    data = res.data;

    return *this;
}

Response::Error::Error(Reason reason_, std::string message_)
    : reason(reason_), message(std::move(message_)) {
}

} // namespace mbgl
