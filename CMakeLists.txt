CMAKE_MINIMUM_REQUIRED(VERSION 3.2.0)

# MSVC VCPKG setup
IF(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  MESSAGE("Using VCPKG Toolchain")
  SET(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
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

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    SET(MAC ON)
    MESSAGE("Running on MAC")
ENDIF()
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    SET(WIN32 ON)
    MESSAGE("Running on Win32")
ENDIF()
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    SET(LINUX ON)
    MESSAGE("Running on Linux")
ENDIF()

SET(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING INTERNAL FORCE)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# pull in libs
FIND_PACKAGE(JPEG REQUIRED)

# default compiler config
SET(LNF_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src)

IF(MAC)
    SET(CMAKE_XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym" CACHE STRING "")
    SET(CMAKE_XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -ffast-math -Wall -DNDEBUG")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall  -DDEBUG")
ENDIF()

IF(WIN32)
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -D_CRT_SECURE_NO_WARNINGS")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -D_CRT_SECURE_NO_WARNINGS")
ENDIF()

IF(LINUX)
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -ffast-math -flto -Wall -DNDEBUG")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall  -DDEBUG")
ENDIF()

# sub-projects
add_subdirectory("src/core")
add_subdirectory("src/base")
add_subdirectory("src/detail")
add_subdirectory("src/systems")
add_subdirectory("src/utils")

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


