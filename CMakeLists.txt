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
option(PSYENGINE_INSTALL OFF)

# Sources (implementation files)
file(GLOB_RECURSE PSY_ENGINE_SOURCE_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${PROJECT_SOURCE_DIR}/src/*.cpp
)

# Headers (public API + templates)
file(GLOB_RECURSE PSY_ENGINE_HEADER_FILES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        ${PROJECT_SOURCE_DIR}/include/psyengine/*.hpp
        ${PROJECT_SOURCE_DIR}/include/psyengine/*.tpp
)

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_mixer CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)

if (PSYENGINE_EXAMPLES)
    add_subdirectory(examples)
endif ()

add_library(${PROJECT_NAME} STATIC)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_sources(${PROJECT_NAME}
        PRIVATE
        ${PSY_ENGINE_SOURCE_FILES}
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/include
        FILES
        ${PSY_ENGINE_HEADER_FILES}
)


target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
    # Enforce UTF-8 encoding on MSVC.
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif ()

# Public include path for consumers
target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        SDL3::SDL3
        SDL3_ttf::SDL3_ttf
        SDL3_mixer::SDL3_mixer
        SDL3_image::SDL3_image
)

if (PSYENGINE_INSTALL)
    # Install the library and the public header file set
    install(TARGETS ${PROJECT_NAME}
            EXPORT ${PROJECT_NAME}-targets
            FILE_SET HEADERS
    )

    # (Optional) export for find_package in consumers
    install(EXPORT ${PROJECT_NAME}-targets
            NAMESPACE ${PROJECT_NAME}::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )
endif ()