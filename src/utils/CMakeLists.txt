PROJECT(utils)

SET(INCL_SRC
    jpeg.h    mandlebrot.h    signed_distance_functions.h
)

SET(LIB_SRC
    ${INCL_SRC}
)

add_custom_target(utils SOURCES ${LIB_SRC})
set_source_files_properties(${LIB_SRC} PROPERTIES HEADER_FILE_ONLY TRUE)

