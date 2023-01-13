set(lib_name battle_game_${COMPONENT_NAME}_lib)
list(APPEND LIBRARY_LIST ${lib_name})

add_library(${lib_name} units/Kirima_Kokona.h units/Kirima_Kokona.cpp   "bullets/laser.h" "bullets/laser.cpp" "units/laser_tank.h" "units/laser_tank.cpp" "units/machine_tank.cpp" "units/machine_tank.h" "bullets/colorful_ball.h" "bullets/colorful_ball.cpp"   "obstacles/swamp.h" "obstacles/swamp.cpp")
file(GLOB_RECURSE source_files *.cpp *.h)
target_sources(${lib_name} PRIVATE ${source_files})
target_link_libraries(${lib_name} PRIVATE grassland)
target_include_directories(${lib_name} PRIVATE ${BATTLE_GAME_EXTERNAL_INCLUDE_DIRS} ${BATTLE_GAME_INCLUDE_DIR})

set(LIBRARY_LIST ${LIBRARY_LIST} PARENT_SCOPE)
