cmake_minimum_required(VERSION 3.28)

add_executable(basics main.cpp)

target_link_libraries(basics PRIVATE Orbis)

add_custom_command(
    TARGET basics POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:basics>/res
    COMMENT "Copying resource files to output directory")