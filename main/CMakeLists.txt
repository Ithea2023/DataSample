# Set the project name
project (main)

add_executable(${PROJECT_NAME} main.cc)

target_link_libraries(${PROJECT_NAME}
        checks
        unit_base
        data_rate
        data_size 
        # bbr       
)   