# Set the project name
project (unit_base CXX)

add_library(${PROJECT_NAME} unit_base.cc unit_base.h checks.h)
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}
)
