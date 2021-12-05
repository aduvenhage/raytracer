# Compiler Settings (CMAKE)
This project uses the Cmake build system, but we still need some compiler specific settings for OSx (Clang/XCode), Windows (MSVC) and Linux (GCC).

This is how we currently check which system/compiler we are using (with CMAKE):
``` 
# check system/compiler
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    SET(MAC ON)
    MESSAGE("Running on OSX / Clang")
ENDIF()
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    SET(WIN32 ON)
    MESSAGE("Running on Win32 / MSVC")
ENDIF()
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    SET(LINUX ON)
    MESSAGE("Running on Linux / GCC")
ENDIF()
```

For now the config is very simple, tying the compilers to the system name, and we can only use the default compiler on each system. 
These are the compiler settings we are aiming for:
- switch on all warnings
- use fast foating point math in release version
- add debug info to release build for profiling
- include SSE2 optimisations (SSE/SSE2 used by default in x64)

*NOTE*: Clang and GCC compiler options are generally the same


## Windows (MSVC)
Cmake section to set options for GCC compiler:
```
IF(WIN32)
    # assuming we are using MSVC
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 -DNDEBUG -D_CRT_SECURE_NO_WARNINGS /W4 /WX /MT /fp:fast")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -D_CRT_SECURE_NO_WARNINGS /W4 /WX /MTd")
ENDIF()
```

- `-DDEBUG / -DNDEBUG`: set DEBUG and NDEBUG compiler defines
- `/W4 /WX`: enable all warnings and also break compilation on any warnings
- `/MTd & /MT`: use multithreaded (non DLL) version of the runtime
- `/fp:fast`: faster floating point (relaxed rule set)
- `/O2`: maximise execution speed

On MSVC I also had to enable compiling for x64 with `SET(CMAKE_GENERATOR_PLATFORM "x64")` during CMAKE toolchain selection:
```
IF(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  MESSAGE("Using VCPKG Toolchain")
  SET(CMAKE_GENERATOR_PLATFORM "x64")
  SET(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
  SET(VCPKG_TARGET_TRIPLET "x64-windows-static")
ENDIF()
```

The `x64-windows-static` option above was also required to select the specific VCPKG version of QT (with static linking) that I needed.


## OSx (XCode/Clang)
Cmake section to set options for XCode/Clang compiler:
```
IF(MAC)
    # assuming we are using XCode/Clang
    SET(CMAKE_XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym" CACHE STRING "")
    SET(CMAKE_XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "11" CACHE STRING "")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -Ofast -march=native -flto -Wall -DNDEBUG")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall -DDEBUG")
ENDIF()
```

- `-Ofast`: includes `-O3` plus `-ffast-math` and more
- `-Wall`: enables all warnings
- `-flto`: enable link time optimisations
- `-g`: include debug info
- `-march=native`: optimise for local machine/architecture
- `-DDEBUG / -DNDEBUG`: set DEBUG and NDEBUG compiler defines


## Linux (GCC)
Cmake section to set options for XCode/Clang compiler:
```
IF(LINUX)
    # assuming we are using GCC
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -ffast-math -flto -Wall -DNDEBUG")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall -DDEBUG")
ENDIF()
```

- `-Ofast`: includes `-O3` plus `-ffast-math` and more
- `-Wall`: enables all warnings
- `-flto`: enable link time optimisations
- `-g`: include debug info
- `-march=native`: optimise for local machine/architecture
- `-DDEBUG / -DNDEBUG`: set DEBUG and NDEBUG compiler defines




