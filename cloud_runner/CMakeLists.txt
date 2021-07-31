PROJECT(cloud_runner)

# non-compiling pages files
SET(RUNNER_FILES
    ${CMAKE_SOURCE_DIR}/cloud_runner/runner.py
)

add_custom_target(cloud_runner SOURCES ${RUNNER_FILES})
set_source_files_properties(${RUNNER_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
