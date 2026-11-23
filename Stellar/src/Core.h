#pragma once

#include <cstdio>
#include <memory>
#include <utility>

#ifdef ST_PLATFORM_WINDOWS
    #ifdef ST_BUILD_DLL
        #define ST_API __declspec(dllexport)
    #else
        #define ST_API __declspec(dllimport)
    #endif
#else
    #error Stellar currently only supports Windows!
#endif

#ifdef ST_DEBUG
    #define ST_ENABLE_ASSERTS
#endif

#ifdef ST_ENABLE_ASSERTS
    #define ST_ASSERT(x, msg) { if (!(x)) { std::fprintf(stderr, "Assertion failed: %s (%s:%d)\n", msg, __FILE__, __LINE__); __debugbreak(); } }
#else
    #define ST_ASSERT(x, msg)
#endif

namespace Stellar {

    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T, typename... Args>
    constexpr Scope<T> CreateScope(Args&&... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Ref = std::shared_ptr<T>;

    template<typename T, typename... Args>
    constexpr Ref<T> CreateRef(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

}
