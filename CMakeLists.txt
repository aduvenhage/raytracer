CMAKE_MINIMUM_REQUIRED(VERSION 3.2.0)

# MSVC VCPKG setup (using x64 build)
IF(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  MESSAGE("Using VCPKG Toolchain")
  SET(CMAKE_GENERATOR_PLATFORM "x64")
  SET(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
  SET(VCPKG_TARGET_TRIPLET "x64-windows-static")
ENDIF()

# default project build type
IF(DEFINED CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: Debug Release.")
ELSE()
    SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: Debug Release.")
ENDIF()
MESSAGE("Build Type: " ${CMAKE_BUILD_TYPE})

PROJECT(Raytracer)

# build/configuration
MESSAGE("System: " ${CMAKE_SYSTEM})
MESSAGE("System name: " ${CMAKE_SYSTEM_NAME})
MESSAGE("Build tool: " ${CMAKE_BUILD_TOOL})
SET(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING INTERNAL FORCE)

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

# set language features
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# default compiler config
SET(LNF_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)

IF(MAC)
    # assuming we are using XCode/Clang
    SET(CMAKE_XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym" CACHE STRING "")
    SET(CMAKE_XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "11" CACHE STRING "")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -O3 -march=native -ffast-math -flto -Wall -DNDEBUG")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall -DDEBUG")
ENDIF()

IF(WIN32)
    # assuming we are using MSVC
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi /Ox -DNDEBUG -D_CRT_SECURE_NO_WARNINGS /W4 /WX /MT /fp:fast /GL")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -D_CRT_SECURE_NO_WARNINGS /W4 /WX /MTd")
ENDIF()

IF(LINUX)
    # assuming we are using GCC
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -O3 -march=native -ffast-math -flto -Wall -DNDEBUG")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall -DDEBUG")
ENDIF()

# sub-projects
add_subdirectory("src/core")
add_subdirectory("src/base")
add_subdirectory("src/detail")
add_subdirectory("src/systems")

# targets
IF(MAC)
    add_subdirectory("raytracer")
    add_subdirectory("raytracer_cli")
ENDIF()
IF(WIN32)
    add_subdirectory("raytracer")
    add_subdirectory("raytracer_cli")
ENDIF()
IF(LINUX)
    add_subdirectory("raytracer_cli")
ENDIF()

# non-compiling project files
SET(PROJ_FILES
    ${CMAKE_SOURCE_DIR}/_config.yml
    ${CMAKE_SOURCE_DIR}/README.md
    ${CMAKE_SOURCE_DIR}/.gitignore
    ${CMAKE_SOURCE_DIR}/.dockerignore
    ${CMAKE_SOURCE_DIR}/docker-compose.yml
)

add_custom_target(proj_files SOURCES ${PROJ_FILES})
set_source_files_properties(${PROJ_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

add_subdirectory("pages")
add_subdirectory("deployment")
add_subdirectory("cloud_runner")


