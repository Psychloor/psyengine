cmake_minimum_required(VERSION 3.31)
project(psyengine
        VERSION 0.2.0
        LANGUAGES CXX
        DESCRIPTION "A modern C++ game engine library"
)

# ============================================================================
# Standard CMake modules
# ============================================================================
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(GenerateExportHeader)
include(CheckIPOSupported)

# ============================================================================
# Build options
# ============================================================================
option(PSYENGINE_EXAMPLES "Build examples" OFF)
option(PSYENGINE_INSTALL "Install psyengine package" ON)
option(PSYENGINE_WERROR "Treat warnings as errors" ON)
option(PSYENGINE_LTO "Enable link-time optimization" OFF)
option(PSYENGINE_UNITY "Enable Unity builds for faster compilation" OFF)
option(PSYENGINE_WITH_IMAGE "Enable SDL_image support" ON)
option(PSYENGINE_WITH_MIXER "Enable SDL_mixer support" ON)
option(PSYENGINE_WITH_TTF "Enable SDL_ttf support" ON)
option(PSYENGINE_ADDRESS_SANITIZE "Enable Address Sanitizer for debug builds" ON)
option(PSYENGINE_BUILD_SHARED "Build as shared library" ON)

# ============================================================================
# Compiler-specific optimizations
# ============================================================================
if(MSVC AND NOT CMAKE_GENERATOR STREQUAL "Ninja")
    # Enable parallel compilation for MSVC (except when using Ninja)
    add_compile_options(/MP)
endif()

# ============================================================================
# Dependencies
# ============================================================================
find_package(SDL3 CONFIG REQUIRED)

# Optional SDL3 extensions
set(PSYENGINE_SDL_LIBRARIES SDL3::SDL3)

if(PSYENGINE_WITH_IMAGE)
    find_package(SDL3_image CONFIG REQUIRED)
    list(APPEND PSYENGINE_SDL_LIBRARIES SDL3_image::SDL3_image)
endif()

if(PSYENGINE_WITH_MIXER)
    find_package(SDL3_mixer CONFIG REQUIRED)
    list(APPEND PSYENGINE_SDL_LIBRARIES SDL3_mixer::SDL3_mixer)
endif()

if(PSYENGINE_WITH_TTF)
    find_package(SDL3_ttf CONFIG REQUIRED)
    list(APPEND PSYENGINE_SDL_LIBRARIES SDL3_ttf::SDL3_ttf)
endif()

# ============================================================================
# Source files
# ============================================================================
file(GLOB_RECURSE PSYENGINE_SOURCES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        src/*.cpp
)

file(GLOB_RECURSE PSYENGINE_HEADERS
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        include/psyengine/*.hpp
        include/psyengine/*.tpp
)

# ============================================================================
# Create library target
# ============================================================================
if(PSYENGINE_BUILD_SHARED)
    add_library(${PROJECT_NAME} SHARED)
else()
    add_library(${PROJECT_NAME} STATIC)
endif()

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Generate export header
generate_export_header(${PROJECT_NAME}
        BASE_NAME ${PROJECT_NAME}
        EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_export.h
)

# ============================================================================
# Target configuration
# ============================================================================
target_sources(${PROJECT_NAME}
        PRIVATE ${PSYENGINE_SOURCES}
        PUBLIC FILE_SET HEADERS
        BASE_DIRS include
        FILES ${PSYENGINE_HEADERS}
        PUBLIC FILE_SET generated_headers
        TYPE HEADERS
        BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_export.h
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${PSYENGINE_SDL_LIBRARIES})

# ============================================================================
# Compiler warnings and flags
# ============================================================================
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
            /W4
            /utf-8  # Force UTF-8 encoding
            $<$<BOOL:${PSYENGINE_WERROR}>:/WX>
    )
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            WIN32_LEAN_AND_MEAN
            NOMINMAX
            _CRT_SECURE_NO_WARNINGS
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall -Wextra -Wpedantic
            $<$<BOOL:${PSYENGINE_WERROR}>:-Werror>
    )
endif()

# ============================================================================
# Feature-based compile definitions
# ============================================================================
if(PSYENGINE_WITH_IMAGE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PSYENGINE_WITH_IMAGE)
endif()

if(PSYENGINE_WITH_MIXER)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PSYENGINE_WITH_MIXER)
endif()

if(PSYENGINE_WITH_TTF)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PSYENGINE_WITH_TTF)
endif()

# Static library definition
get_target_property(LIB_TYPE ${PROJECT_NAME} TYPE)
if(LIB_TYPE STREQUAL "STATIC_LIBRARY")
    target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}_STATIC_DEFINE)
endif()

# ============================================================================
# Target properties
# ============================================================================
set_target_properties(${PROJECT_NAME} PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        POSITION_INDEPENDENT_CODE ON
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
        UNITY_BUILD ${PSYENGINE_UNITY}
)

# Link-time optimization
if(PSYENGINE_LTO)
    check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
    if(ipo_supported)
        set_target_properties(${PROJECT_NAME} PROPERTIES
                INTERPROCEDURAL_OPTIMIZATION ON
        )
    else()
        message(WARNING "LTO requested but not supported: ${ipo_error}")
    endif()
endif()

# ============================================================================
# Address Sanitizer (Debug builds only)
# ============================================================================
if(PSYENGINE_ADDRESS_SANITIZE)
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Debug>:/fsanitize=address /Zi>
        )
        target_link_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Debug>:/fsanitize=address /INCREMENTAL:NO>
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Debug>:-fsanitize=address,undefined -fno-omit-frame-pointer -g>
        )
        target_link_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Debug>:-fsanitize=address,undefined>
        )
    endif()
endif()

# ============================================================================
# Examples subdirectory
# ============================================================================
if(PSYENGINE_EXAMPLES)
    add_subdirectory(examples)
endif()

# ============================================================================
# Installation
# ============================================================================
if(PSYENGINE_INSTALL)
    # Install targets
    install(TARGETS ${PROJECT_NAME}
            EXPORT ${PROJECT_NAME}-targets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            FILE_SET generated_headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    # Export targets for find_package()
    install(EXPORT ${PROJECT_NAME}-targets
            FILE ${PROJECT_NAME}Targets.cmake
            NAMESPACE ${PROJECT_NAME}::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )

    # Create config file
    configure_package_config_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
            INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )

    # Create version file
    write_basic_package_version_file(
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
            VERSION ${PROJECT_VERSION}
            COMPATIBILITY SameMajorVersion
    )

    # Install config files
    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )

    # Install README, LICENSE, etc. if they exist
    foreach(doc_file README.md LICENSE LICENSE.txt CHANGELOG.md)
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${doc_file})
            install(FILES ${doc_file} DESTINATION ${CMAKE_INSTALL_DOCDIR})
        endif()
    endforeach()
endif()

# ============================================================================
# Summary
# ============================================================================
message(STATUS "")
message(STATUS "psyengine v${PROJECT_VERSION} Configuration Summary:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Library type: ${LIB_TYPE}")
message(STATUS "  Install: ${PSYENGINE_INSTALL}")
message(STATUS "  Examples: ${PSYENGINE_EXAMPLES}")
message(STATUS "  Warnings as errors: ${PSYENGINE_WERROR}")
message(STATUS "  Unity builds: ${PSYENGINE_UNITY}")
message(STATUS "  LTO: ${PSYENGINE_LTO}")
message(STATUS "  Address Sanitizer: ${PSYENGINE_ADDRESS_SANITIZE}")
message(STATUS "  SDL extensions:")
message(STATUS "    Image: ${PSYENGINE_WITH_IMAGE}")
message(STATUS "    Mixer: ${PSYENGINE_WITH_MIXER}")
message(STATUS "    TTF: ${PSYENGINE_WITH_TTF}")
message(STATUS "")