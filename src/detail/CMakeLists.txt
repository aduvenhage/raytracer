PROJECT(detail)

SET(INCL_SRC
    box.h
    basic_materials.h
    example_scenes.h
    mandlebrot.h
    marched_blob.h
    marched_bubbles.h
    marched_mandle.h
    marched_sphere.h
    marched_torus.h
    mesh.h
    plane.h
    tex_materials.h
    scatter_materials.h
    simple_camera.h
    simple_scene.h
    signed_distance_functions.h
    smoke_box.h
    special_materials.h
    sphere.h
    
)

SET(LIB_SRC
    ${INCL_SRC}
)

add_custom_target(detail SOURCES ${LIB_SRC})
set_source_files_properties(${LIB_SRC} PROPERTIES HEADER_FILE_ONLY TRUE)

