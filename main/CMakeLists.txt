# Set the project name
project (main)

add_executable(${PROJECT_NAME} main.cc)

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER main)

target_link_libraries(${PROJECT_NAME}
        checks
        unit_base
        units 
        # bbr       
)   