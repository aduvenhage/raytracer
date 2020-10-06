PROJECT(raytracer)

# source files
SET(APP_SRC
	main.cpp
)

# compiler settings
INCLUDE_DIRECTORIES(${LNF_INCLUDE_DIRS})
  
# linker settings
LINK_DIRECTORIES(${LNF_LIB_DIRS})

set(targetname "raytracer")
ADD_EXECUTABLE(${targetname} ${APP_SRC})

IF(MAC)
    TARGET_LINK_LIBRARIES(${targetname} "-framework CoreFoundation -framework Foundation -framework AVFoundation")
    TARGET_LINK_LIBRARIES(${targetname} Qt5::Widgets -ljpeg)
    TARGET_LINK_LIBRARIES(${targetname} "-stdlib=libc++")
ENDIF(MAC)
