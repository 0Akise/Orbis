cmake_minimum_required(VERSION 3.28)

add_executable(graph main.cpp)

target_link_libraries(graph PRIVATE Orbis)

add_custom_command(
    TARGET graph POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:graph>/res
    COMMENT "Copying resource files to output directory")