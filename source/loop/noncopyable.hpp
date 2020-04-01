#pragma once

namespace mbgl { 

 

class noncopyable
{
public:
    noncopyable( noncopyable const& ) = delete;
    noncopyable& operator=(noncopyable const& ) = delete;

protected:
    constexpr noncopyable() = default;
    ~noncopyable() = default;
};
 
} // namespace mbgl
