# Set the project name
project(code_tests CXX)

add_library(${PROJECT_NAME} 
common_code.cc 
fec_test.cc
common_code.h 
fec_test.h)
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC 
            ${PROJECT_SOURCE_DIR}
)