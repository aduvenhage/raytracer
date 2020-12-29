PROJECT(lnf)

SET(INCL_SRC
    box.h
    bvh.h
    camera.h
    color.h
    constants.h
    default_materials.h
    frame.h
    intersect.h
    jobs.h
    jpeg.h
    mandlebrot.h
    material.h
    marched_materials.h
    marched_node.h
    mesh.h
    node.h
    outputimage.h
    plane.h
    profile.h
    queue.h
    ray.h
    scene.h
    smoke_box.h
    sphere.h
    stats.h
    strutil.h
    trace.h
    transform.h
    triangle.h
    uv.h
    vec3.h
    viewport.h
)

SET(LIB_SRC
    ${INCL_SRC}
)

add_custom_target(lnf SOURCES ${LIB_SRC})
set_source_files_properties(${LIB_SRC} PROPERTIES HEADER_FILE_ONLY TRUE)

