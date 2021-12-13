PROJECT(raytracer)

# source files
SET(APP_SRC
	main.cpp
)

# extra compiler settings
INCLUDE_DIRECTORIES(${LNF_INCLUDE_DIRS})
LINK_DIRECTORIES(${LNF_LIB_DIRS})

set(targetname "raytracer_cli")
ADD_EXECUTABLE(${targetname} ${APP_SRC})

IF(MAC)
    TARGET_LINK_LIBRARIES(${targetname} "-framework CoreFoundation -framework Foundation -framework AVFoundation")
    TARGET_LINK_LIBRARIES(${targetname} assimp::assimp)
    TARGET_LINK_LIBRARIES(${targetname} "-stdlib=libc++")
ENDIF()

IF(WIN32)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd26451")
	TARGET_LINK_OPTIONS(${targetname} PUBLIC /DEBUG /LTCG)
    TARGET_LINK_LIBRARIES(${targetname} assimp::assimp)
ENDIF()

IF(LINUX)
    TARGET_LINK_LIBRARIES(${targetname} assimp::assimp)
    TARGET_LINK_LIBRARIES(${targetname} pthread stdc++fs)
ENDIF()

