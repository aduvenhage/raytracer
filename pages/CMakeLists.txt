PROJECT(pages)

# non-compiling pages files
SET(PAGES_FILES
    ${CMAKE_SOURCE_DIR}/README.md
    ${CMAKE_SOURCE_DIR}/pages/axis.md
    ${CMAKE_SOURCE_DIR}/pages/bvh.md
    ${CMAKE_SOURCE_DIR}/pages/docker-machine.md
    ${CMAKE_SOURCE_DIR}/pages/jobs.md
    ${CMAKE_SOURCE_DIR}/pages/qt.md
    ${CMAKE_SOURCE_DIR}/pages/random.md
    ${CMAKE_SOURCE_DIR}/pages/raytracing.md
    ${CMAKE_SOURCE_DIR}/pages/raytracing_advanced.md
)

add_custom_target(pages SOURCES ${PAGES_FILES})
set_source_files_properties(${PAGES_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
