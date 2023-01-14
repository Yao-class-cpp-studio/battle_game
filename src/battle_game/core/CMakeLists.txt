set(lib_name battle_game_${COMPONENT_NAME}_lib)
list(APPEND LIBRARY_LIST ${lib_name})

add_library(${lib_name} units/Split_bullet_cannon.cpp units/Split_bullet_cannon.h bullets/split_bullet.cpp bullets/split_bullet.h bullets/laser.cpp units/Laser_Tank.cpp units/Laser_Tank.h units/Collision_tank.cpp units/Collision_tank.h)
file(GLOB_RECURSE source_files *.cpp *.h)
target_sources(${lib_name} PRIVATE ${source_files})
target_link_libraries(${lib_name} PRIVATE grassland)
target_include_directories(${lib_name} PRIVATE ${BATTLE_GAME_EXTERNAL_INCLUDE_DIRS} ${BATTLE_GAME_INCLUDE_DIR})

set(LIBRARY_LIST ${LIBRARY_LIST} PARENT_SCOPE)
