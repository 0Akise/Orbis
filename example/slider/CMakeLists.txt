cmake_minimum_required(VERSION 3.28)

add_executable(slider main.cpp)

target_link_libraries(slider PRIVATE Orbis)

add_custom_command(
    TARGET slider POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:slider>/res
    COMMENT "Copying resource files to output directory")