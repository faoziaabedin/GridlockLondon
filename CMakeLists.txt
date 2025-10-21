# code/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(GridlockLondon)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Core library - domain logic
add_library(gridlock_core
    core/Node.cpp
    core/Edge.cpp
    core/City.cpp
)

target_include_directories(gridlock_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core
)

# Test executable for City implementation
add_executable(test_city
    test_city.cpp
)

target_link_libraries(test_city
    gridlock_core
)