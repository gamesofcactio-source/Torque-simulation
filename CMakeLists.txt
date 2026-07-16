cmake_minimum_required(VERSION 3.12)

project(torque-simulation VERSION 1.0)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_executable(torque-simulation src/main.cpp)

target_link_libraries(torque-simulation
    SDL2::SDL2
    SDL2_ttf::SDL2_ttf
)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
