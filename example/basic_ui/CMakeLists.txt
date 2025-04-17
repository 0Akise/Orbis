
cmake_minimum_required(VERSION 3.28)

add_executable(basic_ui main.cpp)

target_link_libraries(basic_ui PRIVATE Orbis)

add_custom_command(
    TARGET basic_ui POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:basic_ui>/res
    COMMENT "Copying resource files to output directory")