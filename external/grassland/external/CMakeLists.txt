add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(spdlog)
add_subdirectory(freetype)

add_library(imgui)

target_sources(
        imgui
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_vulkan.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp")
target_include_directories(imgui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/imgui ${Vulkan_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include)
