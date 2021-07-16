PROJECT(base)

SET(INCL_SRC
    camera.h
    intersect.h    loader.h    material.h    primitive.h    resource.h    scene.h
)

SET(LIB_SRC
    ${INCL_SRC}
)

add_custom_target(base SOURCES ${LIB_SRC})
set_source_files_properties(${LIB_SRC} PROPERTIES HEADER_FILE_ONLY TRUE)

