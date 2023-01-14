set(lib_name battle_game_${COMPONENT_NAME}_lib)
list(APPEND LIBRARY_LIST ${lib_name})

add_library(${lib_name})
file(GLOB_RECURSE source_files *.cpp *.h)
target_sources(${lib_name} PRIVATE ${source_files} "units/double_shot_tank.h" "units/double_shot_tank.cpp" "units/bullet_storage_tank.h" "units/bullet_storage_tank.cpp" "units/displacement_tank.h" "units/displacement_tank.cpp")
target_link_libraries(${lib_name} PRIVATE grassland)
target_include_directories(${lib_name} PRIVATE ${BATTLE_GAME_EXTERNAL_INCLUDE_DIRS} ${BATTLE_GAME_INCLUDE_DIR})

set(LIBRARY_LIST ${LIBRARY_LIST} PARENT_SCOPE)