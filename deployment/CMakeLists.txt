PROJECT(deployment)

# non-compiling pages files
SET(DEPLOYMENT_FILES
    raytracer_cli/Dockerfile
    raytracer_cli/Dockerfile.base
    raytracer_cli/run.sh
)

add_custom_target(deployment SOURCES ${DEPLOYMENT_FILES})
set_source_files_properties(${DEPLOYMENT_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
