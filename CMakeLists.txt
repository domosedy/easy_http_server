cmake_minimum_required(VERSION 3.25)
set(NAME BoostServer)
project(${NAME})

set(CMAKE_CXX_STANDARD 23)

# BOOST
set(Boost_USE_STATIC_LIBS ON)

find_package(Boost REQUIRED system)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()


add_compile_options(-fsanitize=address)
add_compile_options(-fsanitize=undefined)
add_link_options(-fsanitize=address)
add_link_options(-fsanitize=undefined)

include_directories("include/")

####### PROJECT #########
add_executable(${NAME}
        src/main.cpp
        src/TCPConn.cpp
        src/TCPServer.cpp
        src/html_parser.cpp
        src/jinger.cpp)
#target_link_libraries(${NAME} include/TCPConn.h)
target_link_libraries(${NAME} ${Boost_LIBRARIES})
##########################


###### JINJA #######
add_executable(jinger
               src/main_jinger.cpp
        src/jinger.cpp
        include/jinger.hpp)
####################