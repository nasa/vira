# ViraFetchSTB.cmake checks for stb and falls back to FetchContent:
include(FetchContent)
include(MarkSystemLibrary)

# Prevent multiple inclusions
if(FETCHSTB_INCLUDED)
    return()
endif()
set(FETCHSTB_INCLUDED TRUE)

set(used_fetchcontent FALSE)

# Try vcpkg pattern first
find_package(Stb QUIET)
if(Stb_FOUND AND Stb_INCLUDE_DIR)
    # vcpkg provides include directory but no target, so create one
    add_library(stb::stb INTERFACE IMPORTED)
    target_include_directories(stb::stb INTERFACE ${Stb_INCLUDE_DIR})
    set(stb_FOUND TRUE)
endif()

if(NOT stb_FOUND)
    # Fall back to FetchContent
    FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG f1c79c02822848a9bed4315b12c8c8f3761e1296  # Current main as of Dec 2025
        GIT_SHALLOW ON
    )
    FetchContent_GetProperties(stb)
    if(NOT stb_POPULATED)
        FetchContent_MakeAvailable(stb)
    endif()
    
    # Create target for FetchContent version
    add_library(stb::stb INTERFACE IMPORTED)
    target_include_directories(stb::stb INTERFACE ${stb_SOURCE_DIR})
    set(used_fetchcontent TRUE)
endif()

# Verify target exists
if(NOT TARGET stb::stb)
    message(FATAL_ERROR "stb::stb target not found")
endif()

mark_as_system_library(stb::stb)

# Install headers if we used FetchContent
if(used_fetchcontent)
    if(NOT stb_SOURCE_DIR)
        message(FATAL_ERROR "FetchContent was used for stb but stb_SOURCE_DIR is not set - this indicates a FetchContent failure")
    endif()

    message(STATUS "STB Used FetchContent: ${stb_SOURCE_DIR}")

    install(DIRECTORY "${stb_SOURCE_DIR}/"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h"
    )
else()
    message(STATUS "STB - Used system/package manager")
endif()