cmake_minimum_required(VERSION 3.22)

project(grassland)

if (MSVC AND ${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
    message("Using msvc-clang compiler")
    add_compile_options(/EHsc)
endif()

set(CMAKE_CXX_STANDARD 17)

set(GRASSLAND_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(GRASSLAND_INCLUDE_DIR ${GRASSLAND_SOURCE_DIR}/src)
set(GRASSLAND_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

find_package(Vulkan REQUIRED)
if (Vulkan_FOUND)
    message("Vulkan found: ${Vulkan_VERSION}")
    message("Vulkan include dir: ${Vulkan_INCLUDE_DIRS}")
    message("Vulkan libraries: ${Vulkan_LIBRARIES}")
    list(APPEND GRASSLAND_EXTERNAL_INCLUDE_DIRS ${Vulkan_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Vulkan SDK not found!")
endif()

set(FT_DISABLE_HARFBUZZ ON CACHE INTERNAL "" FORCE)

add_subdirectory(external)

list(APPEND GRASSLAND_EXTERNAL_INCLUDE_DIRS ${GLFW_SOURCE_DIR}/include)
list(APPEND GRASSLAND_EXTERNAL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/glm)
list(APPEND GRASSLAND_EXTERNAL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/spdlog/include)
list(APPEND GRASSLAND_EXTERNAL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/freetype/include)
list(APPEND GRASSLAND_EXTERNAL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/stb)
list(APPEND GRASSLAND_EXTERNAL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)

add_subdirectory(src)

if(GRASSLAND_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    add_subdirectory(assets)
    add_subdirectory(demo)
else()
    set(GRASSLAND_INCLUDE_DIRS ${GRASSLAND_INCLUDE_DIR} ${GRASSLAND_EXTERNAL_INCLUDE_DIRS} PARENT_SCOPE)
    message("Grassland is included as subdirectory, not building demos.")
endif()
