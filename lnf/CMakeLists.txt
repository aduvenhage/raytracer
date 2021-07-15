PROJECT(lnf)

SET(INCL_SRC
    box.h
    bvh.h
    camera.h
    color.h
    constants.h
    default_materials.h
    example_scenes.h
    frame.h
    intersect.h
    jobs.h
    jpeg.h
    loader.h
    mandlebrot.h
    material.h
    marched_bubbles.h
    marched_mandle.h
    marched_materials.h
    marched_sphere.h
    mesh.h
    outputimage.h
    plane.h
    primitive.h
    profile.h
    queue.h
    random.h
    ray.h
    resource.h
    scene.h
    simple_camera.h
    simple_scene.h
    signed_distance_functions.h
    smoke_box.h
    sphere.h
    stats.h
    strutil.h
    trace.h
    uv.h
    vec3.h
    viewport.h
)

SET(LIB_SRC
    ${INCL_SRC}
)

add_custom_target(lnf SOURCES ${LIB_SRC})
set_source_files_properties(${LIB_SRC} PROPERTIES HEADER_FILE_ONLY TRUE)

