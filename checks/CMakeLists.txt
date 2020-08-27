# Set the project name
project (checks)

add_library(${PROJECT_NAME} checks.cc checks.h)
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}
)
