<div align="center">

# Kodgen

[![License: MIT](https://img.shields.io/github/license/jsoysouvanh/Kodgen)](https://github.com/jsoysouvanh/Kodgen/blob/master/LICENSE.md)
[![Latest Release](https://img.shields.io/github/v/tag/jsoysouvanh/kodgen?label=release)](https://github.com/jsoysouvanh/Kodgen/releases/latest)
[![Tests](https://github.com/jsoysouvanh/Kodgen/actions/workflows/build_and_tests.yml/badge.svg?branch=master)](https://github.com/jsoysouvanh/Kodgen/actions/workflows/build_and_tests.yml)

</div>

Kodgen is a C++20 library based on libclang. It provides tools to parse C++ source files and generate files from the parsed data. The C++ entities parsed in the current implementation are namespaces, structs/classes, non-member and member variables, non-member and member functions, enums and enum values.

## Getting Started
### Requirements:
- CMake 3.15.0+ (if you build the library from source).
- A compatible compiler: MSVC Platform Toolset v141+ / GCC8.0.0+ / Clang 7.0.0+.

### Clone the repository
Clone the git repository **with its submodules**:

```shell
> git clone https://github.com/jsoysouvanh/Kodgen.git
>
> cd Kodgen
```

### Generate the project

```shell
> cmake -B Build/Release -DCMAKE_BUILD_TYPE=Release -G "<Generator>"
```

Most common generators include:
  - Visual Studio 15 2017
  - Visual Studio 16 2019
  - Unix Makefiles
  - Ninja
  - Type cmake -G for more information

> **Note:** If you use Visual Studio 15 2017 generator, the default target platform is x86 so you should specify it explicitely. See the [generator documentation](https://cmake.org/cmake/help/v3.15/generator/Visual%20Studio%2016%202019.html) for more information.
> ```shell
> > cmake -B Build/Release -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 15 2017" -A x64
> ```

### Build the library

```shell
> cmake --build Build/Release --config Release --target Kodgen
```

You will find built executables / shared libraries in **Kodgen/Build/Release/Bin** and static libraries in **Kodgen/Build/Release/Lib**.

## Cross-platform compatibility
This library has been tested and is stable on the following configurations:
- Microsoft Windows Server | MSVC 19.29.30133.0
- Linux 20.04 | Clang 7.0.1, Clang 8.0.1, Clang 9.0.1, Clang 10.0.0, Clang 11.0.0
- Linux 20.04 | GNU 8.4.0, GNU 9.3.0, GNU 10.3.0, GNU 11.1.0
- Mac OS X 10.15.7 | AppleClang 12.0.0