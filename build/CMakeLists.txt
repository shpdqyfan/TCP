cmake_minimum_required(VERSION 3.0)
project(TCP)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11 -O0 -Wall -g2 -ggdb")

message(------------------------------------------------)
find_package(Boost
             1.57.0
             COMPONENTS
             REQUIRED)
             
message(project_name: ${PROJECT_NAME})
message(project_src_dir: ${PROJECT_SOURCE_DIR})
message(project_bin_dir: ${PROJECT_BINARY_DIR})

add_subdirectory(../lib bin/baselib)

set(SERVER_SRC ../server/TcpServer.cpp) 
add_executable(tcpServer ${SERVER_SRC}) 
target_link_libraries(tcpServer base pthread)

set(CLIENT_SRC ../client/TcpClient.cpp) 
add_executable(tcpClient ${CLIENT_SRC}) 
target_link_libraries(tcpClient base)

include_directories("../lib/include")

