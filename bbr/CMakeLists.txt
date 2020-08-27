# # Set the project name
# project (bbr)

# # Add a library with the above sources
# add_library(${PROJECT_NAME} src/sublib1.cpp)
# add_library(sub::lib1 ALIAS ${PROJECT_NAME})
# set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER Samples)

# target_include_directories( ${PROJECT_NAME}
#     PUBLIC ${PROJECT_SOURCE_DIR}/include
# )
