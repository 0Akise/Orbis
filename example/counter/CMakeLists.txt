cmake_minimum_required(VERSION 3.28)

add_executable(counter main.cpp)

target_link_libraries(counter PRIVATE Orbis)

add_custom_command(
    TARGET counter POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:counter>/res
    COMMENT "Copying resource files to output directory")