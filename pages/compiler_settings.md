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

- TODO: add debug info to release build for profiling
- TODO: include SSE2 optimisations


## OSx (XCode/Clang)
Cmake section to set options for XCode/Clang compiler:
```
IF(MAC)
    # assuming we are using XCode/Clang
    SET(CMAKE_XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym" CACHE STRING "")
    SET(CMAKE_XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "11" CACHE STRING "")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -ffast-math -flto -Wall -DNDEBUG")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall -DDEBUG")
ENDIF()
```

## Windows (MSVC)
Cmake section to set options for GCC compiler:
```
IF(WIN32)
    # assuming we are using MSVC
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox -DNDEBUG -D_CRT_SECURE_NO_WARNINGS /W4 /WX /MT /fp:fast")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -D_CRT_SECURE_NO_WARNINGS /W4 /WX /MTd")
ENDIF()
```

## Linux (GCC)
Cmake section to set options for XCode/Clang compiler:
```
IF(LINUX)
    # assuming we are using GCC
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -ffast-math -flto -Wall -DNDEBUG")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall -DDEBUG")
ENDIF()
```





