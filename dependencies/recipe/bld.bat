@echo off

echo Building %PKG_NAME% version %PKG_VERSION%

cmake -B build %CMAKE_ARGS% ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_INSTALL_PREFIX="%LIBRARY_PREFIX%" ^
    -DCMAKE_PREFIX_PATH="%LIBRARY_PREFIX%" ^
    -DVIRA_BUILD_TOOLS=OFF ^
    -DVIRA_BUILD_EXAMPLES=OFF ^
    -DVIRA_BUILD_TESTS=OFF ^
    -DVIRA_BUILD_SCRATCH=OFF ^
    -DVIRA_BUILD_DOCS=OFF
if errorlevel 1 exit 1

cmake --build build --config Release --parallel %CPU_COUNT%
if errorlevel 1 exit 1

cmake --install build --config Release
if errorlevel 1 exit 1