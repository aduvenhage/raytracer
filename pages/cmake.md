
## Notes on using CMake
I have something like this in my main CMakeLists.txt file
```

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)

IF(MAC)
    FIND_PACKAGE(Qt6 REQUIRED COMPONENTS Widgets)
ENDIF(MAC)
IF(WIN32)
    FIND_PACKAGE(Qt5 REQUIRED COMPONENTS Widgets)
ENDIF(WIN32)

```

With Qt installed correctly CMake should just find it 🤞
You can then link in Qt in your target CMakeLists.txt file like this:

```

TARGET_LINK_LIBRARIES(${targetname} Qt6::Widgets)

```

Also, made use of Brew (https://brew.sh) on OSx and vcpkg (https://vcpkg.io) on Windows to install Qt and other libs.


## Using VCPKG
[vcpkg](https://vcpkg.io) is a free C/C++ package manager for acquiring and managing libraries.
Choose from over 1500 open source libraries to download and build in a single step or add your own private libraries to simplify your build process.
Maintained by the Microsoft C++ team and open source contributors.


VCPKG setup in CMAKE file:
```

# MSVC VCPKG setup (using x64 build)
IF(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  MESSAGE("Using VCPKG Toolchain")
  SET(CMAKE_GENERATOR_PLATFORM "x64")
  SET(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
  SET(VCPKG_TARGET_TRIPLET "x64-windows-static")
ENDIF()

```

Using VCPKG is fairly straightforward, with the following notes:
- The `x64-windows-static` option above was required to select the specific VCPKG version of QT (with static linking) that I needed.
- You also have to install all your packages with this option, for example:
  `vcpkg install assimp --triplet x64-windows-static`
- I had to define `VCPKG_ROOT` (location on disk of `vcpkg` install) and add it to the list of system environment variables.