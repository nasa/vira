cmake_minimum_required(VERSION 3.16)

#########################################
### Define Options and Initial Values ###
#########################################
option(VIRA_BUILD_TOOLS "Build command-line tools" ON)
option(VIRA_BUILD_EXAMPLES "Build Vira examples" OFF)
option(VIRA_BUILD_TESTS "Build tests using gtest" OFF)
option(VIRA_BUILD_VIRAPY "Build python bindings" OFF)
option(VIRA_BUILD_DOCS "Build documentation" OFF)

#option(VIRA_BUILD_VULKAN "Build Vulkan components. EXPERIMENTAL" OFF)
#option(VIRA_USE_GLSLC "Use GLSLC Instead of GlsLangValidator" ON)

option(VIRA_BUILD_SCRATCH "Build development programs" ON)

option(VIRA_KEEP_VCPKG_BUILDTREES "Keep the vcpkg buildtrees" OFF)
option(VIRA_ALL_SELECTED_WARNINGS "Turn on all selected warnings" OFF)

# Load modules from cmake/ directory
list(APPEND CMAKE_MODULE_PATH 
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/fetch"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/find"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/helpers"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/setup"
)
include(CustomVcpkgSetup)


#############################
### Configure the project ###
#############################
project(Vira LANGUAGES CXX VERSION 0.8.3)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message("Vira version ${PROJECT_VERSION}")

# Workaround for conda-forge LLVM 21+ with older macOS SDK
# See: https://conda-forge.org/docs/maintainer/knowledge_base.html#newer-c-features-with-old-sdk
if(APPLE)
    add_compile_definitions(_LIBCPP_DISABLE_AVAILABILITY)
endif()

# Ensure if Debug definitions:
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(NDEBUG)
endif()

if(VIRA_BUILD_VULKAN)
    add_compile_definitions(VIRA_BUILD_VULKAN)
endif()


# Set all outputs to go to the binary directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# TODO consider exporting specific symbols from the API:
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_compile_definitions(VIRA_VERSION="${CMAKE_PROJECT_VERSION}")


#############################
### Install Configuration ###
#############################
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Set installation paths
set(VIRA_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR})
set(VIRA_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
set(VIRA_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR})
set(VIRA_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/vira)

# Configure package config files
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/viraConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/viraConfig.cmake"
    INSTALL_DESTINATION ${VIRA_INSTALL_CMAKEDIR}
    PATH_VARS VIRA_INSTALL_INCLUDEDIR VIRA_INSTALL_LIBDIR VIRA_INSTALL_CMAKEDIR
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/viraConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install CMake config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/viraConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/viraConfigVersion.cmake"
    DESTINATION ${VIRA_INSTALL_CMAKEDIR}
)

# Install helper modules that downstream projects might need
install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/helpers/SimplePackageHelpers.cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/helpers/MarkSystemLibrary.cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/find/ViraFindProjDB.cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/fetch/ViraFetchIndicators.cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/fetch/ViraFetchPlog.cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/fetch/ViraFetchTermcolor.cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/fetch/ViraFetchCSPICE.cmake"
    DESTINATION ${VIRA_INSTALL_CMAKEDIR}
)


##########################
### Add Subdirectories ###
##########################
# Add the included directories:
include_directories(include)
include_directories(source)
include_directories(source/vira)

# Add source directories:
add_subdirectory(source/vira)

# Run custom compiler/warnings setup:
include(CustomCompilerFlags)
include(CustomWarnings)


#####################################################
### Add additional subdirs based on configuration ###
#####################################################
if(VIRA_BUILD_TOOLS)
    message("Building Vira command-line tools")
    add_subdirectory(source/tools)
endif()

if(VIRA_BUILD_EXAMPLES)
    message("Building Vira examples")
    add_subdirectory(examples/)
endif()

if(VIRA_BUILD_TESTS)
    message("Building Vira tests")
    add_subdirectory(tests)
endif()

if(VIRA_BUILD_VIRAPY)
    message("Building Vira python bindings")
    add_subdirectory(source/virapy)
endif()

if(VIRA_BUILD_DOCS)
    message("Building Vira documentation")
    set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
    add_subdirectory(docs)
endif()

if(VIRA_BUILD_VULKAN)
    message("Building Vira Vulkan components")
    add_subdirectory(source/shaders)
endif()

# Include all included Scratch directories:
if (VIRA_BUILD_SCRATCH)
    message("Building Included Scratch Code:")
    file(GLOB subdirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/scratch/ ${CMAKE_CURRENT_SOURCE_DIR}/scratch/*/)
    foreach(subdir ${subdirs})
        if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scratch/${subdir})
            message(" - Adding:  scratch/${subdir}")
            add_subdirectory(scratch/${subdir})
        endif()
    endforeach()
endif()