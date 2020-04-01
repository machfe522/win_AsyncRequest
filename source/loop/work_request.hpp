#pragma once

#include "async_request.hpp"

#include <memory>

namespace mbgl {

class WorkTask;

//异步工作请求类定义
class WorkRequest : public AsyncRequest {
public:
    using Task = std::shared_ptr<WorkTask>;

    WorkRequest(Task);
    ~WorkRequest() override;

private:
    std::shared_ptr<WorkTask> task;
};

} // namespace mbgl
