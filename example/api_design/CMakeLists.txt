
cmake_minimum_required(VERSION 3.28)

add_executable(api_design main.cpp)

target_link_libraries(api_design PRIVATE Orbis)

add_custom_command(
    TARGET api_design POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:api_design>/res
    COMMENT "Copying resource files to output directory")