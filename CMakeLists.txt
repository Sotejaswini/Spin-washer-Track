cmake_minimum_required(VERSION 3.12)
project(WashingMachineSystem CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(WashingMachineSystem
    src/main.cpp
    src/Scheduler.cpp
    src/WashingMachine.cpp
    src/User.cpp
)
target_include_directories(WashingMachineSystem PRIVATE src third_party)
if(UNIX) target_link_libraries(WashingMachineSystem pthread) endif()
