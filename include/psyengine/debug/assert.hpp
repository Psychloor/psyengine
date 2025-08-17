//
// Created by blomq on 2025-08-16.
//

#ifndef PSYENGINE_ASSERT_HPP
#define PSYENGINE_ASSERT_HPP

#include <source_location>
#include <string_view>

#include <SDL3/SDL_assert.h>



namespace psyengine::debug
{

     inline void Assert(const bool condition,
                                        [[maybe_unused]] const std::string_view message,
                                        [[maybe_unused]] const std::source_location& location =
                                            std::source_location::current())
    {
        SDL_assert_always(condition); // NOLINT(*) - SDL knows what they're doing for all platforms
    }

#ifndef NDEBUG
     inline void DebugAssert(const bool condition,
                                             const std::string_view message,
                                             const std::source_location location = std::source_location::current())
    {
        SDL_assert(condition);
    }
#else
    constexpr void DebugAssert(const bool, const std::string_view,
                                      const std::source_location& = std::source_location::current())
    {}
#endif

} // namespace psyengine::debug

#define PSY_ASSERT(condition, message) ::psyengine::debug::Assert((condition), (message))
#define PSY_DEBUG_ASSERT(condition, message) ::psyengine::debug::DebugAssert((condition), (message))

#endif //PSYENGINE_ASSERT_HPP
