cmake_minimum_required(VERSION 3.30)
project(LearnOpenGL)

find_package(OpenGL REQUIRED)

set(CMAKE_C_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)

# Include dependencies
add_subdirectory(glfw)
add_subdirectory(cglm EXCLUDE_FROM_ALL)
include_directories(
        glfw/include
        cimgui
        cimgui/imgui
        glad/include
        dependencies)

file(GLOB CIMGUI_SOURCES
        cimgui/*.cpp
        cimgui/imgui/*.cpp
        cimgui/imgui/backends/imgui_impl_glfw.cpp
        cimgui/imgui/backends/imgui_impl_opengl3.cpp)

set(SOURCE_FILES src/main.c
        glad/src/glad.c
        src/util.c
        src/util.h
        src/shader.c
        src/shader.h
        src/camera.c
        src/camera.h
        src/model.c
        src/model.h
        src/framebuffer.c
        src/framebuffer.h
)

add_library(cimgui STATIC ${CIMGUI_SOURCES})
add_compile_definitions("IMGUI_IMPL_OPENGL_LOADED_GL3W")
target_compile_definitions(cimgui PUBLIC "-DIMGUI_IMPL_API=extern \"C\"")

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})
target_compile_definitions(${PROJECT_NAME} PUBLIC -DCIMGUI_USE_OPENGL3 -DCIMGUI_USE_GLFW)

add_custom_target(COPY_RESOURCES ALL
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${PROJECT_BINARY_DIR}/resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/resources
        ${PROJECT_BINARY_DIR}/resources
        COMMENT "Copying resources into binary directory")
add_dependencies(${CMAKE_PROJECT_NAME} COPY_RESOURCES)

target_link_libraries(${CMAKE_PROJECT_NAME}
        glfw ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES}
        cglm_headers
        cimgui)
