cmake_minimum_required(VERSION 3.10)
project(pscr_s2)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)

add_executable(app.exe src/main.cpp)
target_link_libraries(app.exe Threads::Threads)