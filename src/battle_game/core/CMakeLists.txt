set(lib_name battle_game_${COMPONENT_NAME}_lib)
list(APPEND LIBRARY_LIST ${lib_name})

add_library(${lib_name})
file(GLOB_RECURSE source_files *.cpp *.h)
target_sources(${lib_name} PRIVATE ${source_files} "units/qx_tank.cpp" "units/qx_tank.h" "units/crit_tank_qxhwj.cpp" "units/crit_tank_qxhwj.h" "bullets/random_ball.cpp" "bullets/random_ball.h" "units/distank.h" "units/distank.cpp" "bullets/disball.cpp" "bullets/disball.h" "bullets/rangeball.h" "bullets/rangeball.cpp" "units/qx_tank_1.h" "units/qx_tank_1.cpp")
target_link_libraries(${lib_name} PRIVATE grassland)
target_include_directories(${lib_name} PRIVATE ${BATTLE_GAME_EXTERNAL_INCLUDE_DIRS} ${BATTLE_GAME_INCLUDE_DIR})

set(LIBRARY_LIST ${LIBRARY_LIST} PARENT_SCOPE)
