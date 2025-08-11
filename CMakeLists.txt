cmake_minimum_required(VERSION 3.31)
project(psyengine VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if (MSVC)
    cmake_policy(SET CMP0091 NEW) # MultithreadedDll and ASM things errors without

    if (NOT CMAKE_GENERATOR STREQUAL "Ninja")
        add_definitions(/MP)    # parallelize each target, unless Ninja is the generator
    endif ()
endif ()

add_subdirectory(external)

set(engine_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/psyengine/sdl_raii.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/psyengine/sdl_game.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/psyengine/base_state.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/psyengine/state_manager.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/psyengine/input_handler.hpp
)

set(engine_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sdl_game.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/state_manager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/input_handler.cpp
)

add_library(psyengine STATIC ${engine_sources} ${engine_headers})
add_library(psyengine::psyengine ALIAS psyengine)

target_compile_features(psyengine PUBLIC cxx_std_23)
set_target_properties(psyengine PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

# Public includes for consumers; build interface points to your include/ folder
target_include_directories(psyengine
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(psyengine
        PUBLIC
        SDL3_ttf::SDL3_ttf
        SDL3_mixer::SDL3_mixer
        SDL3_image::SDL3_image
        SDL3::SDL3
)