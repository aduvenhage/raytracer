PROJECT(core)

SET(INCL_SRC
    color.h
    constants.h
    image.h
    memory.h
    outputimage.h
    profile.h
    queue.h
    random.h
    ray.h
    scattered_ray.h
    stats.h
    strutil.h
    uv.h
    vec3.h
    viewport.h
)

SET(LIB_SRC
    ${INCL_SRC}
)

add_custom_target(core SOURCES ${LIB_SRC})
set_source_files_properties(${LIB_SRC} PROPERTIES HEADER_FILE_ONLY TRUE)

