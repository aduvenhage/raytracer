PROJECT(raytracer)

# source files
SET(APP_SRC
	main.cpp
)

# extra compiler settings
INCLUDE_DIRECTORIES(${LNF_INCLUDE_DIRS} ${JPEG_INCLUDE_DIR})
LINK_DIRECTORIES(${LNF_LIB_DIRS})


set(targetname "raytracer")
ADD_EXECUTABLE(${targetname} ${APP_SRC})

IF(MAC)
    TARGET_LINK_LIBRARIES(${targetname} "-framework CoreFoundation -framework Foundation -framework AVFoundation")
    TARGET_LINK_LIBRARIES(${targetname} Qt5::Widgets ${JPEG_LIBRARIES})
    TARGET_LINK_LIBRARIES(${targetname} "-stdlib=libc++")
ELSE()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd26451")
    TARGET_LINK_LIBRARIES(${targetname} Qt5::Widgets ${JPEG_LIBRARIES})
ENDIF(MAC)
