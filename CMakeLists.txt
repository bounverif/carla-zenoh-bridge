# carla icin pipten kullan elle build etme
# container olustur
# icinde sadece libcarla build etmeyi dene
# bu image i base olarak kullanalim


cmake_minimum_required(VERSION 3.5.0)
project(carla-zenoh-bridge VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(zenohc REQUIRED)
find_package(zenohcxx REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/libcarla-install/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/libcarla-install/include/system)
link_directories(${PROJECT_SOURCE_DIR}/libcarla-install/lib)

add_executable(carla-zenoh-bridge ${PROJECT_SOURCE_DIR}/src/carla-zenoh-bridge.cpp)
add_executable(control-data ${PROJECT_SOURCE_DIR}/src/control-data.cpp)
add_executable(spawner ${PROJECT_SOURCE_DIR}/src/spawner.cpp)
add_executable(test-listener ${PROJECT_SOURCE_DIR}/src/test-listener.cpp)

add_subdirectory(${PROJECT_SOURCE_DIR}/src/Connection)
target_link_libraries(carla-zenoh-bridge PRIVATE Connection)
target_include_directories(carla-zenoh-bridge PRIVATE 
                            ${PROJECT_SOURCE_DIR}/src
                            ${PROJECT_SOURCE_DIR}/src/Connection)

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
    PUBLIC zenohcxx::zenohc
)

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
    PUBLIC zenohcxx::zenohc
)

target_link_libraries(control-data PUBLIC 
    zenohcxx::zenohc
)

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

    PUBLIC
    zenohcxx::zenohc
)
