
IF(DEFINED CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: Debug Release.")
ELSE()
    SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: Debug Release.")
ENDIF()

PROJECT(Lost-N-Found)
CMAKE_MINIMUM_REQUIRED(VERSION 3.2.0)
SET(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING INTERNAL FORCE)

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)

# NOTE: CMAKE_PREFIX_PATH=/usr/local/Cellar/qt/5.15.0
FIND_PACKAGE(Qt5 COMPONENTS Widgets REQUIRED)

MESSAGE("Build tool: " ${CMAKE_BUILD_TOOL})

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    SET(LNF_INCLUDE_DIRS "/usr/local/include")
    SET(LNF_LIB_DIRS "/usr/local/lib")

    SET(CMAKE_XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym" CACHE STRING "")
    SET(CMAKE_XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -mfpmath=sse -Wall ")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall  -DDEBUG")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

INCLUDE_DIRECTORIES(${LNF_INCLUDE_DIRS})
LINK_DIRECTORIES(${LNF_LIB_DIRS})

SET(PROJ_FILES
    README.md
)

add_custom_target(proj_files SOURCES ${PROJ_FILES})
set_source_files_properties(${PROJ_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

SET(APP_SRC
    main.cpp
)

SET(LNF_HEADERS
    headers/box.h
    headers/camera.h
    headers/color.h
    headers/constants.h
    headers/default_materials.h
    headers/frame.h
    headers/intersect.h
    headers/jobs.h
    headers/jpeg.h
    headers/mandlebrot.h
    headers/material.h
    headers/outputimage.h
    headers/plane.h
    headers/profile.h
    headers/ray.h
    headers/scene.h
    headers/shape.h
    headers/shape_transform.h
    headers/sphere.h
    headers/trace.h
    headers/uv.h
    headers/vec3.h
    headers/viewport.h
)

SET(targetname "lostnfound")
ADD_EXECUTABLE(${targetname} ${PRJ_FILES_SRC} ${APP_SRC} ${LNF_HEADERS})

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    TARGET_LINK_LIBRARIES(${targetname} "-framework CoreFoundation -framework Foundation -framework AVFoundation")
    TARGET_LINK_LIBRARIES(${targetname} Qt5::Widgets -ljpeg)
    TARGET_LINK_LIBRARIES(${targetname} "-stdlib=libc++")

ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
