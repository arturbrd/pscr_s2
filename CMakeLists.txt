cmake_minimum_required(VERSION 3.10)
project(pscr_s2)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)
find_package(PahoMqttCpp REQUIRED)

add_executable(app.exe 
    src/main.cpp
    src/threads.cpp
)


target_include_directories(app.exe
    PRIVATE ${CMAKE_SOURCE_DIR}/inc
)
target_link_libraries(app.exe
    PahoMqttCpp::paho-mqttpp3
    Threads::Threads
)
message(STATUS "Paho include dirs: ${PahoMqttCpp_INCLUDE_DIRS}")