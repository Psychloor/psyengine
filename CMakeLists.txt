cmake_minimum_required(VERSION 3.31)
project(psyengine VERSION 0.1.0 LANGUAGES CXX)

# Force modern MSVC runtime policy in all subdirs that use old cmake_minimum_required
cmake_policy(SET CMP0091 NEW)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if (MSVC)
    if (NOT CMAKE_GENERATOR STREQUAL "Ninja")
        add_definitions(/MP)    # parallelize each target, unless Ninja is the generator
    endif ()
endif ()

option(PSYENGINE_EXAMPLES "Compile Examples" OFF)

file(
        GLOB_RECURSE PSY_ENGINE_SOURCE_FILES
        CONFIGURE_DEPENDS  # Automatically reconfigure if source files are added/removed.
        ${PROJECT_SOURCE_DIR}/src/*.cpp
        ${PROJECT_SOURCE_DIR}/include/psyengine/*.hpp
        ${PROJECT_SOURCE_DIR}/include/psyengine/*.tpp
)

add_subdirectory(external)

if (PSYENGINE_EXAMPLES)
    add_subdirectory(examples)
endif ()

add_library(${PROJECT_NAME} STATIC ${PSY_ENGINE_SOURCE_FILES})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
    # Enforce UTF-8 encoding on MSVC.
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Public includes for consumers; build interface points to your include/ folder
target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        SDL3_ttf::SDL3_ttf
        SDL3_mixer::SDL3_mixer
        SDL3_image::SDL3_image
        SDL3::SDL3
)