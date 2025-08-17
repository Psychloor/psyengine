target_sources(psyengine
        PUBLIC FILE_SET HEADERS TYPE HEADERS
        FILES

        psyengine.hpp

        debug/assert.hpp

        input/input_manager.hpp
        input/input_manager.ipp

        math/math_utils.hpp
        math/vector2.hpp
        math/vector2.ipp
        math/vector.hpp

        platform/sdl_runtime.hpp
        platform/sdl_raii.hpp

        resources/texture_manager.hpp

        state/base_state.hpp
        state/state_manager.hpp

        time/clock.hpp
        time/time.hpp

        utils/random_utils.hpp
)