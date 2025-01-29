cmake_minimum_required(VERSION 3.5.0)

include_directories(${PROJECT_SOURCE_DIR}/libcarla-install/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/libcarla-install/include/system)
link_directories(${PROJECT_SOURCE_DIR}/libcarla-install/lib)

add_library(Connection Connection.cpp listeners.cpp)

target_link_libraries(Connection PUBLIC 
    zenohcxx::zenohc
)

target_include_directories(Connection PUBLIC 
    ${PROJECT_SOURCE_DIR}/src/Connection
)
