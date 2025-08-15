cmake_minimum_required(VERSION 3.31)
project(psyengine VERSION 0.2.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(GenerateExportHeader)

# Options
option(PSYENGINE_EXAMPLES "Compile Examples" OFF)
option(PSYENGINE_INSTALL "Install psyengine package" OFF)
option(PSYENGINE_WERROR "Treat warnings as errors" ON)
option(PSYENGINE_LTO "Enable link-time optimization (IPO)" OFF)
option(PSYENGINE_UNITY "Enable Unity builds for faster compilation" OFF)
option(PSYENGINE_WITH_IMAGE "Enable SDL_image support" ON)
option(PSYENGINE_WITH_MIXER "Enable SDL_mixer support" ON)
option(PSYENGINE_WITH_TTF "Enable SDL_ttf support" ON)
option(PSYENGINE_ADDRESS_SANITIZE "Enable Address Sanitizer for debug builds" ON)

if (MSVC)
    if (NOT CMAKE_GENERATOR STREQUAL "Ninja")
        add_definitions(/MP)    # parallelize each target, unless Ninja is the generator
    endif ()
endif ()

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

# Dependencies (SDL3 and optional add-ons)
find_package(SDL3 CONFIG REQUIRED)
if (PSYENGINE_WITH_IMAGE)
    find_package(SDL3_image CONFIG REQUIRED)
endif ()
if (PSYENGINE_WITH_MIXER)
    find_package(SDL3_mixer CONFIG REQUIRED)
endif ()
if (PSYENGINE_WITH_TTF)
    find_package(SDL3_ttf CONFIG REQUIRED)
endif ()


if (PSYENGINE_EXAMPLES)
    add_subdirectory(examples)
endif ()

add_library(${PROJECT_NAME})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
generate_export_header(${PROJECT_NAME}
        BASE_NAME psyengine
)

target_sources(${PROJECT_NAME}
        PRIVATE
        ${PSY_ENGINE_SOURCE_FILES}

        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/include
        FILES
        ${PSY_ENGINE_HEADER_FILES}

        FILE_SET generated_headers
        TYPE HEADERS
        BASE_DIRS $<TARGET_PROPERTY:${PROJECT_NAME},BINARY_DIR>
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_export.h
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

# Warnings and UTF-8
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 $<$<BOOL:${PSYENGINE_WERROR}>:/WX>)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic $<$<BOOL:${PSYENGINE_WERROR}>:-Werror>)
endif ()

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        SDL3::SDL3
        $<$<BOOL:${PSYENGINE_WITH_TTF}>:SDL3_ttf::SDL3_ttf>
        $<$<BOOL:${PSYENGINE_WITH_MIXER}>:SDL3_mixer::SDL3_mixer>
        $<$<BOOL:${PSYENGINE_WITH_IMAGE}>:SDL3_image::SDL3_image>
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        UNITY_BUILD $<IF:$<BOOL:${PSYENGINE_UNITY}>,ON,OFF>
        INTERPROCEDURAL_OPTIMIZATION $<IF:$<BOOL:${PSYENGINE_LTO}>,ON,OFF>
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN TRUE
)

get_target_property(_lib_type ${PROJECT_NAME} TYPE)
if (_lib_type STREQUAL "STATIC_LIBRARY")
    target_compile_definitions(${PROJECT_NAME} PUBLIC psyengine_STATIC_DEFINE)
endif ()



if (PSYENGINE_ADDRESS_SANITIZE)
    # MSVC AddressSanitizer
    if (MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Debug>:/fsanitize=address /Zi /sdl>
        )
        target_link_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Debug>:/fsanitize=address /INCREMENTAL:NO>
        )
    endif ()

    # clang-cl, Clang, or GCC: AddressSanitizer + UBSan
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Debug>:-fsanitize=address,undefined -fno-omit-frame-pointer -g>
        )
        target_link_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Debug>:-fsanitize=address,undefined>
        )
    endif ()
endif ()

# Lightweight Windows noise reduction (safe as PRIVATE)
if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
endif ()

# Add Compile Definitions
if (PSYENGINE_WITH_IMAGE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PSYENGINE_WITH_IMAGE)
endif ()
if (PSYENGINE_WITH_MIXER)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PSYENGINE_WITH_MIXER)
endif ()
if (PSYENGINE_WITH_TTF)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PSYENGINE_WITH_TTF)
endif ()


if (PSYENGINE_INSTALL)
    # Install the library and the public header file set
    install(TARGETS ${PROJECT_NAME}
            EXPORT ${PROJECT_NAME}-targets
            FILE_SET HEADERS
            FILE_SET geberated_headers
    )

    # Export targets for consumers (psyengineConfig.cmake-style usage)
    install(EXPORT ${PROJECT_NAME}-targets
            NAMESPACE ${PROJECT_NAME}::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )
endif ()
