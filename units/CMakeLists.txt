# Set the project name
project (data_rate)

add_library(${PROJECT_NAME} data_rate.cc data_rate.h)
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER ${folders})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}
)