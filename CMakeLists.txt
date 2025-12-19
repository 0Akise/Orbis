cmake_minimum_required(VERSION 3.28)
project(Orbis VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ORBIS_BUILD_EXAMPLE "Build example projects" ${PROJECT_IS_TOP_LEVEL})
option(ORBIS_BUILD_TEST "Build tests" OFF)
option(ORBIS_BUILD_DOCS "Build documentation" OFF)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(Orbis INTERFACE)

include(FetchContent)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

target_include_directories(Orbis INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
target_compile_features(Orbis INTERFACE cxx_std_20)

FetchContent_Declare(SFML GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_BRANCH master GIT_SHALLOW ON EXCLUDE_FROM_ALL SYSTEM)
FetchContent_MakeAvailable(SFML)

set(SFML_LIBRARIES sfml-system sfml-window sfml-graphics sfml-audio sfml-network)

target_link_libraries(Orbis INTERFACE ${SFML_LIBRARIES})

if(MSVC)
    target_compile_options(Orbis INTERFACE /W4)
    target_compile_definitions(Orbis INTERFACE NOMINMAX WIN32_LEAN_AND_MEAN)
else()
    target_compile_options(Orbis INTERFACE -Wall -Wextra -Wpedantic -Werror)
endif()

if(ORBIS_BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

if(ORBIS_BUILD_TEST)
    add_subdirectory(test)
endif()

install(TARGETS Orbis EXPORT OrbisTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

write_basic_package_version_file("OrbisConfigVersion.cmake" VERSION ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion)
configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/OrbisConfig.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/OrbisConfig.cmake" INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Orbis)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/OrbisConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/OrbisConfigVersion.cmake" DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Orbis)
install(EXPORT OrbisTargets FILE OrbisTargets.cmake NAMESPACE Orbis:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Orbis)