cmake_minimum_required(VERSION 3.25)
set(NAME BoostServer)
project(${NAME})

set(CMAKE_CXX_STANDARD 20)

# BOOST
set(Boost_USE_STATIC_LIBS ON)

find_package(Boost REQUIRED system)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

include_directories("include/")

add_executable(${NAME}
        src/main.cpp
        src/TCPConn.cpp
        src/TCPServer.cpp)
#target_link_libraries(${NAME} include/TCPConn.h)
target_link_libraries(${NAME} ${Boost_LIBRARIES})
