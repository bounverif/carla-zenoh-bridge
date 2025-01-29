# carla icin pipten kullan elle build etme
# container olustur
# icinde sadece libcarla build etmeyi dene
# bu image i base olarak kullanalim


cmake_minimum_required(VERSION 3.5.0)
project(carla-zenoh-bridge VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

# TODO Version 1.1.1 check
find_package(zenohc REQUIRED)
message(STATUS "Zenoh-C Version ${zenohc_VERSION}")
find_package(zenohcxx REQUIRED)
message(STATUS "Zenoh-C++ Version ${zenohcxx_VERSION}")


add_subdirectory(${SRC_DIR}/Connection)
add_subdirectory(${PROJECT_SOURCE_DIR}/protobuf)

include_directories(${PROJECT_SOURCE_DIR}/libcarla-install/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/libcarla-install/include/system)
link_directories(${PROJECT_SOURCE_DIR}/libcarla-install/lib)

add_executable(carla-zenoh-bridge ${SRC_DIR}/carla-zenoh-bridge.cpp)
target_link_libraries(carla-zenoh-bridge PRIVATE Connection)
target_link_libraries(carla-zenoh-bridge PUBLIC
    -Wl,-Bstatic
    carla_client
    rpc
    boost_filesystem
    -Wl,-Bdynamic
    png
    tiff
    jpeg
    Recast
    Detour
    DetourCrowd
    zenohcxx::zenohc
)
target_include_directories(carla-zenoh-bridge PRIVATE ${SRC_DIR}/Connection)
                
add_executable(spawner ${SRC_DIR}/spawner.cpp)
target_link_libraries(spawner PUBLIC
    -Wl,-Bstatic
    carla_client
    rpc
    boost_filesystem
    -Wl,-Bdynamic
    png
    tiff
    jpeg
    Recast
    Detour
    DetourCrowd
    zenohcxx::zenohc
)

add_executable(control-data 
    ${SRC_DIR}/control-data.cpp
)
target_link_libraries(control-data
    PUBLIC zenohcxx::zenohc
    PRIVATE ExampleIncoming
)


add_executable(test-listener ${SRC_DIR}/test-listener.cpp)
target_link_libraries(test-listener PUBLIC
    -Wl,-Bstatic
    carla_client
    rpc
    boost_filesystem
    -Wl,-Bdynamic
    png
    tiff
    jpeg
    Recast
    Detour
    DetourCrowd
    zenohcxx::zenohc
    PRIVATE ExampleIncoming
)
