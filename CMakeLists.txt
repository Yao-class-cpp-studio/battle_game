cmake_minimum_required(VERSION 3.22)

project(battle_game)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(assets)

add_subdirectory(external/grassland)
add_subdirectory(external/abseil-cpp)

list(APPEND BATTLE_GAME_EXTERNAL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/abseil-cpp)
list(APPEND BATTLE_GAME_EXTERNAL_INCLUDE_DIRS ${GRASSLAND_INCLUDE_DIRS})

include_directories(external/asio/asio/include)

add_subdirectory(src)
