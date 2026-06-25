cmake_minimum_required(VERSION 3.16)

project(Lazy_Sequence VERSION 1.0.0 LANGUAGES CXX)

enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

add_subdirectory(include/Sequences)
add_subdirectory(tests)
add_subdirectory(src)