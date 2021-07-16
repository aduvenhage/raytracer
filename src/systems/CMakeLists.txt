PROJECT(systems)

SET(INCL_SRC
    frame.h
    trace.h
)

SET(LIB_SRC
    ${INCL_SRC}
)

add_custom_target(systems SOURCES ${LIB_SRC})
set_source_files_properties(${LIB_SRC} PROPERTIES HEADER_FILE_ONLY TRUE)

