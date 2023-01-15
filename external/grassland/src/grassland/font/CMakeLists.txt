file(GLOB_RECURSE source_files *.cpp *.h)

file(RELATIVE_PATH dirname ${CMAKE_CURRENT_SOURCE_DIR}/../ ${CMAKE_CURRENT_SOURCE_DIR} )
set(CURRENT_LIB_NAME grassland_${dirname}_lib)

list(APPEND GRASSLAND_LIBS ${CURRENT_LIB_NAME})
set(GRASSLAND_LIBS ${GRASSLAND_LIBS} PARENT_SCOPE)

add_library(${CURRENT_LIB_NAME} ${source_files})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${GRASSLAND_INCLUDE_DIR} ${GRASSLAND_EXTERNAL_INCLUDE_DIRS})
target_link_libraries(${CURRENT_LIB_NAME} PRIVATE freetype)
