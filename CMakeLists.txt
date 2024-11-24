cmake_minimum_required(VERSION 3.10)

project(flash_cache_CDN LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(flash_cache_CDN src/main.cpp)

add_custom_target(run
        COMMAND flash_cache_CDN
        DEPENDS flash_cache_CDN
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY DEFAULT_TARGET run)

#target_link_libraries(flash_cache_CDN <library_name>)
