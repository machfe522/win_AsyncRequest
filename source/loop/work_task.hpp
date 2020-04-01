#pragma once

#include "noncopyable.hpp"

#include <memory>
#include <mutex>
#include <atomic>

namespace mbgl {

 
//工作任务抽象类定义
class WorkTask : private noncopyable {
public:
    virtual ~WorkTask() = default;

    virtual void operator()() = 0;
    virtual void cancel() = 0;
};

//WorkTaskImpl存在，是为了将定义和实现分离，便于数据管理。
template <class F, class P>
class WorkTaskImpl : public WorkTask {
public:
	WorkTaskImpl(F f, P p, std::shared_ptr<std::atomic<bool>> canceled_)
		: canceled(std::move(canceled_)),
		func(std::move(f)),
		params(std::move(p)) {
	}

	void operator()() override {

		std::lock_guard<std::recursive_mutex> lock(mutex);
		if (!*canceled) {
			invoke(std::make_index_sequence<std::tuple_size<P>::value>{});
		}
	}

	void cancel() override {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		*canceled = true;
	}

private:
	template <std::size_t... I>
	void invoke(std::index_sequence<I...>) {
		func(std::move(std::get<I>(std::forward<P>(params)))...);
	}

	std::recursive_mutex mutex;
	std::shared_ptr<std::atomic<bool>> canceled;

	F func;
	P params;
};


//生成工作任务消息
template <class Fn, class... Args>
std::shared_ptr<WorkTask> makeWorkTask(Fn&& fn, Args&&... args) {
	auto flag = std::make_shared<std::atomic<bool>>();
	*flag = false;

	auto tuple = std::make_tuple(std::forward<Args>(args)...);
	return std::make_shared<WorkTaskImpl<std::decay_t<Fn>, decltype(tuple)>>(
		std::forward<Fn>(fn),
		std::move(tuple),
		flag);
}

} // namespace mbgl
