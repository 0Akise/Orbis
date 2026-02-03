cmake_minimum_required(VERSION 3.28)

add_executable(playground main.cpp)

target_link_libraries(playground PRIVATE Orbis)

add_custom_command(
    TARGET playground POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:playground>/res
    COMMENT "Copying resource files to output directory")