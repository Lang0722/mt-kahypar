# Building Mt-KaHyPar on Windows

This guide explains how to build Mt-KaHyPar on Windows using pre-installed **TBB** and **Boost** libraries, instead of downloading them automatically or using vcpkg.

## Prerequisites

1.  **Visual Studio 2022** (or newer) with C++ Desktop Development workload.
2.  **CMake** (3.26 or newer).
3.  **Intel oneTBB** (installed via installer or zip).
4.  **Boost** (installed via installer or zip).

## Step 1: Locate Your Dependencies

Before running CMake, you need to find the installation paths where `Config.cmake` or `lib` files are located.

### TBB
Look for the directory containing `TBBConfig.cmake`.
*   Example: `C:\Program Files (x86)\Intel\oneAPI\tbb\2021.5.0\lib\cmake\tbb`
*   If you installed via zip, it might be: `C:\path\to\tbb\my_tbb\cmake`

### Boost
Look for the root directory of your Boost installation.
*   Example: `C:\local\boost_1_82_0`
*   Or: `C:\Program Files\boost\boost_1_82_0`

## Step 2: Open Command Prompt

Open the **x64 Native Tools Command Prompt for VS 2022** from your Start Menu. This ensures all MSVC compilers are in your PATH.

## Step 3: Configure with CMake

Navigate to the project root directory and run the following command.

**Replace the paths below with your actual paths found in Step 1.**

```cmd
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 ^
  -DTBB_DIR="C:\Program Files (x86)\Intel\oneAPI\tbb\2021.5.0\lib\cmake\tbb" ^
  -DBoost_ROOT="C:\local\boost_1_82_0" ^
  -DKAHYPAR_DOWNLOAD_TBB=OFF ^
  -DKAHYPAR_DOWNLOAD_BOOST=OFF
```

### Explanation of Flags:
*   `-G "Visual Studio 17 2022" -A x64`: Generates a Visual Studio solution for 64-bit architecture.
*   `-DTBB_DIR="..."`: Tells CMake where to find the TBB configuration file.
*   `-DBoost_ROOT="..."`: Tells CMake where to find the Boost root directory.
*   `-DKAHYPAR_DOWNLOAD_...=OFF`: Prevents the build system from trying to download dependencies automatically.


## Step 4: Build the Project

You can build the project directly from the command line:

```cmd
cmake --build build --config Release
```

Or you can open the generated solution file in Visual Studio.
*   **For VS 2022 and older:** Open `build\MtKaHyPar.sln`.
*   **For VS Preview / Newer Versions (VS 18+):** You might see a **`.slnx`** file (e.g., `MtKaHyPar.slnx`) instead of `.sln`. This is the new XML-based solution format. You can open this file in Visual Studio just like a `.sln` file.

## Troubleshooting

### No `.sln` file generated (Only `.slnx`)
If you are using a very recent or preview version of Visual Studio (e.g., "Visual Studio 18" generator), CMake may generate the modern `.slnx` format. This is expected. Simply open the `MtKaHyPar.slnx` file.

### "Could not find a package configuration file provided by 'TBB'"

*   Double-check that the `-DTBB_DIR` path actually contains `TBBConfig.cmake`.

### "Boost not found"
*   Ensure `-DBoost_ROOT` points to the folder containing `lib` and `include` (or the versioned folder).
*   Add `-DBoost_DEBUG=ON` to the cmake command to see detailed search output.

### Linker Errors (LNK1181, etc.)
*   If you encounter linker errors about missing `.lib` files, ensure your TBB/Boost installation matches your compiler (MSVC vs MinGW) and architecture (x64).
