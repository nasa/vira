# Moon Example
This example demonstrates how to render an image using the Level-of-Detail system.  This includes fetching the raw Digital Elevation Map (DEM) and Albedo data, pre-processing it into the required Vira format, and then writing an executable to use the Vira API to render an image.

## 1. Download the Data
In the `scripts/` directory (in the root of the Vira repository), there is a `download_dems` script (available both as a `.bat` for Windows, and `.sh` for Linux/MacOS).  This will download all of the required `.TIF` amd `.LBL/.TIF` to a new directory located at `data/dems/` and `data/albedo/`.

Also in the `data/` directory is a `download_kernels` script (again both a `.bat` and `.sh`).  This will download all required SPICE kernels to a new directory located at `data/kernels/`.

## 2. Build the Vira command line tools
We'll need the `vira_dem2qld` command line tool to pre-process the map (`.IMG` and `.LBL/.IMG`) files we just downloaded.  To do this, run the CMake configuration with the `-DVIRA_BUILD_TOOLS=ON` option.  So from the root directory, you can run:

```
mkdir build
cd build
cmake ../ -DCMAKE_TOOLCHAIN_FILE=<you/path/to/vcpkg>/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release -DVIRA_BUILD_TOOLS=ON -DVIRA_BUILD_EXAMPLES=ON
cmake --build . -j
```

Optionally, to install the tools on your system, you can install after building by running the following command:

```
cmake --install .
```

*NOTE: The `-DVIRA_BUILD_EXAMPLES=ON` option will build the example programs, including the `vira_moon` executable for this example.  The `-j` option in `cmake --build . -j` configures the build process to use all available cores in parallel.*

## 2. Pre-process the map files
The `vira_dem2qld` executable takes in all of the map files, processes them, and saves the data to a special format specific to Vira known as the "quipu level-of-detail" (`.qld`).  Quipu files are binary files used by Vira to quickly access data required for rendering.

To perform this pre-processing we simply provide `vira_dem2qld` a `dem_config.yml` file, and optionally the directory where the data has been saved.  We have provided a `dem_config.yml` in this example.  Simply run:

```
vira_dem2qld dem_config.yml -r <path/to/data/>
```

## 3. Rendering an Image
If you built Vira with the `-DVIRA_BUILD_EXAMPLES=ON` option, then the `vira_moon` executable will have already been built to your build directory.  Simply run that executable, while providing the path to the generated `qpu` directory.