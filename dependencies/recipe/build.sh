#!/bin/bash
set -ex

echo "Building ${PKG_NAME} version ${PKG_VERSION}"

cmake -B build \
    -G Ninja \
    ${CMAKE_ARGS} \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
    -DCMAKE_PREFIX_PATH="${PREFIX}" \
    -DVIRA_BUILD_TOOLS=OFF \
    -DVIRA_BUILD_EXAMPLES=OFF \
    -DVIRA_BUILD_TESTS=OFF \
    -DVIRA_BUILD_SCRATCH=OFF \
    -DVIRA_BUILD_DOCS=OFF

cmake --build build --parallel ${CPU_COUNT}

cmake --install build