# Include FetchContent module
include(FetchContent)

# Suppress deprecation warnings from third-party libraries
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)

# Attempt to find dependencies and use FetchContent if not found:
include(ViraFetchCSPICE)
include(ViraFetchIndicators)
include(ViraFetchPlog)
include(ViraFetchTermcolor)
include(ViraFetchSTB)


# Restore deprecation warnings for your own code
set(CMAKE_WARN_DEPRECATED ON CACHE BOOL "" FORCE)

# Include our simple package finding helpers
set(CMAKE_FIND_PACKAGE_TARGETS_GLOBAL TRUE)

# Simple packages that work consistently across package managers
find_package(assimp CONFIG REQUIRED)
find_package(embree 3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(TBB CONFIG REQUIRED)

# Package finders using `cmake/find` modules in project root
include(ViraFindCFITSIO)
include(ViraFindFFTW3)
include(ViraFindGDAL)
include(ViraFindLZ4)
include(ViraFindTIFF)
include(ViraFindYAMLCPP)

# Create the Vira interface library:
add_library(vira INTERFACE)

# Mark all third-party libraries as SYSTEM
include(MarkSystemLibrary)
mark_as_system_libraries(
    assimp::assimp
    ${cfitsio_TARGET}
    embree
    ${FFTW3f_TARGET}
    ${GDAL_TARGET}
    glm::glm
    ${lz4_TARGET}
    TBB::tbb
    TBB::tbbmalloc
    ${tiff_TARGET}
    ${yaml-cpp_TARGET}
)

target_link_libraries(vira INTERFACE
    # Package manager libraries:
    assimp::assimp
    ${cfitsio_TARGET}
    embree
    ${FFTW3f_TARGET}
    ${GDAL_TARGET}
    glm::glm
    ${lz4_TARGET}
    TBB::tbb
    TBB::tbbmalloc
    ${tiff_TARGET}
    ${yaml-cpp_TARGET}
    
    # Required FetchContent libraries:
    cspice
    indicators::indicators
    plog::plog
    termcolor::termcolor
    stb::stb
)

# If FetchContent was used for CSPICE and csupport exists:
if(TARGET csupport)
    mark_as_system_library(csupport)
    target_link_libraries(vira INTERFACE csupport)
endif()

target_include_directories(vira INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(UNIX AND NOT APPLE)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.1)
        target_link_libraries(vira INTERFACE stdc++fs)
    endif()
endif()

# Find the proj.db file (implicitly required by GDAL)
include(ViraFindProjDB)


#######################
### Install Targets ###
#######################
install(TARGETS vira
    EXPORT ViraTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install headers (.hpp files from include/vira/ and .ipp files from include/implementation/)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING 
        PATTERN "*.h" 
        PATTERN "*.hpp"
        PATTERN "*.ipp"
)

# Export the targets
install(EXPORT ViraTargets
    FILE viraTargets.cmake
    NAMESPACE vira::
    DESTINATION ${VIRA_INSTALL_CMAKEDIR}
)