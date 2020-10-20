CMAKE_MINIMUM_REQUIRED(VERSION 3.2.0)

# default project build type
IF(DEFINED CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: Debug Release.")
ELSE()
    SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: Debug Release.")
ENDIF()

PROJECT(Lost-N-Found)

# build/configuration
MESSAGE("Build tool: " ${CMAKE_BUILD_TOOL})

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    SET(MAC ON)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

SET(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING INTERNAL FORCE)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# pull in Qt
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_PREFIX_PATH /usr/local/Cellar/qt/5.15.0 CACHE STRING INTERNAL FORCE)
FIND_PACKAGE(Qt5 REQUIRED COMPONENTS Widgets)

# default compiler config
IF (MAC)
    SET(LNF_INCLUDE_DIRS "/usr/local/include" ${CMAKE_CURRENT_SOURCE_DIR})
    SET(LNF_LIB_DIRS "/usr/local/lib" "/opt/local/lib")

    SET(CMAKE_XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym" CACHE STRING "")
    SET(CMAKE_XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -O3 -march=native -ffast-math -Wall ")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall  -DDEBUG")
ENDIF(MAC)

# sub-projects
add_subdirectory("lnf")
add_subdirectory("raytracer")

# non-compiling project files
SET(PROJ_FILES
    ${CMAKE_SOURCE_DIR}/README.md
    ${CMAKE_SOURCE_DIR}/.gitignore
)

add_custom_target(proj_files SOURCES ${PROJ_FILES})
set_source_files_properties(${PROJ_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

