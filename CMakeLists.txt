cmake_minimum_required(VERSION 3.10)
project(FlashCacheCDN)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


add_executable(client
    src/client/main.cpp
    src/client/concurrency.cpp
    src/client/usable.cpp
)

add_executable(edge_server
    src/edge_server/main.cpp
)

add_executable(EDNS_server
    src/EDNS_server/main.cpp
    src/EDNS_server/requests.cpp
)


add_executable(server
    src/server/main.cpp
)

target_include_directories(client PRIVATE src/client/include)
target_include_directories(EDNS_server PRIVATE src/EDNS_server/include)
target_include_directories(edge_server PRIVATE src/edge_server/include)
target_include_directories(server PRIVATE src/server/include)

find_package(CURL REQUIRED)

target_link_libraries(client PRIVATE CURL::libcurl)


