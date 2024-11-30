cmake_minimum_required(VERSION 3.10)
project(FlashCacheCDN)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(client
    src/client/main.cpp
)

add_executable(edge_server
    src/edge_server/main.cpp
)

add_executable(EDNS_server
    src/EDNS_server/main.cpp
)

# Add executable for the server
add_executable(server
    src/server/main.cpp
)

