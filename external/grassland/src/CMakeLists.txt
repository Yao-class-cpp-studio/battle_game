file(GLOB grassland_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} LIST_DIRECTORIES true grassland/*)

foreach(grassland_file ${grassland_files})
    if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${grassland_file})
        add_subdirectory(${grassland_file})
    endif()
endforeach()

add_library(grassland grassland/grassland.cpp grassland/grassland.h)

foreach(grassland_lib ${GRASSLAND_LIBS})
    message("Using sublib ${grassland_lib}.")
endforeach()

target_link_libraries(grassland PRIVATE ${GRASSLAND_LIBS})
