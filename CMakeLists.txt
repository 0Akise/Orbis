cmake_minimum_required(VERSION 3.28)
project(Orbis LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)

FetchContent_MakeAvailable(SFML)

find_package(Vulkan REQUIRED)

set(SFML_LIBRARIES sfml-system sfml-window sfml-graphics sfml-audio sfml-network)
set(VULKAN_LIBRARIES Vulkan::Vulkan)

file(GLOB_RECURSE HEADERS "include/*.hpp")
file(GLOB SOURCES "src/*.cpp")

add_executable(Orbis ${HEADERS} ${SOURCES})

target_include_directories(Orbis PRIVATE include)
target_link_libraries(Orbis PRIVATE ${SFML_LIBRARIES} ${VULKAN_LIBRARIES})
target_compile_features(Orbis PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(Orbis PRIVATE /W4 /WX)
    target_compile_definitions(
        Orbis PRIVATE
        NOMINMAX
        WIN32_LEAN_AND_MEAN
    )
else()
    target_compile_options(Orbis PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

install(TARGETS Orbis)