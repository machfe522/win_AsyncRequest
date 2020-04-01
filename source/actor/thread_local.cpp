#include "thread_local.hpp"


#include <cassert>
#include <cstdlib>

#include <windows.h>

namespace mbgl {
namespace impl {

 
ThreadLocalBase::ThreadLocalBase() {
    static_assert(sizeof(storage) >= sizeof(DWORD), "storage is too small");
    static_assert(alignof(decltype(storage)) % alignof(DWORD) == 0, "storage is incorrectly aligned");
	new (&reinterpret_cast<DWORD&>(storage))DWORD(TlsAlloc());
	if (reinterpret_cast<DWORD&>(storage) == TLS_OUT_OF_INDEXES) {
       abort();
    }
}

ThreadLocalBase::~ThreadLocalBase() {

    assert(!get());

    if (!TlsFree(reinterpret_cast<DWORD&>(storage))) {
       abort();
    }
}

void* ThreadLocalBase::get() {
	return  TlsGetValue(reinterpret_cast<DWORD&>(storage));
}

void ThreadLocalBase::set(void* ptr) {	 
    if (!TlsSetValue(reinterpret_cast<DWORD&>(storage), ptr)) {
        abort();
    }
}

} // namespace impl
} // namespace mbgl
