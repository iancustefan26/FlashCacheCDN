cmake_minimum_required(VERSION 3.10)
project(FlashCacheCDN)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


add_executable(client
    src/client/main.cpp
        src/client/protocol.cpp
        src/utils/utils.cpp
        src/edge_server/include/client_protocol.h
        src/utils/utils.cpp
)

add_executable(edge_server
    src/edge_server/main.cpp
        src/edge_server/client_protocol.cpp
        src/edge_server/include/main_server_protocol.h
        src/edge_server/main_server_protocol.cpp
        src/edge_server/include/concurrency.h
        src/utils/utils.cpp
        src/edge_server/concurrency.cpp
)

add_executable(EDNS_server
    src/EDNS_server/main.cpp
    src/EDNS_server/requests.cpp
        src/EDNS_server/include/cache.h
        src/EDNS_server/cache.cpp
        src/utils/utils.cpp
)


add_executable(server
    src/server/main.cpp
        src/server/include/edge_protocol.h
        src/server/include/dns_server_protocol.h
        src/utils/utils.cpp
        src/server/edge_protocol.cpp
        src/server/dns_server_protocol.cpp
)


set(AVAILABLE_RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/edge_server/available_resources")
file(GLOB RESOURCE_SOURCES "${AVAILABLE_RESOURCES_DIR}/*.cpp")

foreach(SOURCE_FILE ${RESOURCE_SOURCES})
    get_filename_component(TARGET_NAME ${SOURCE_FILE} NAME_WE)
    add_executable(${TARGET_NAME} ${SOURCE_FILE})
endforeach()



target_include_directories(client PRIVATE src/client/include src/utils)
target_include_directories(EDNS_server PRIVATE src/EDNS_server/include src/utils)
target_include_directories(edge_server PRIVATE src/edge_server/include src/utils)
target_include_directories(server PRIVATE src/server/include src/utils)

find_package(CURL REQUIRED)
find_library(configparser
        NAME configparser
        HINTS [${LIBDIR} ${INCLUDEDIR}]
)

target_link_libraries(client PRIVATE CURL::libcurl configparser)
target_link_libraries(edge_server PRIVATE CURL::libcurl configparser)
target_link_libraries(server PRIVATE CURL::libcurl configparser)
target_link_libraries(EDNS_server PRIVATE CURL::libcurl configparser)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(EDNS_server PRIVATE nlohmann_json::nlohmann_json)


