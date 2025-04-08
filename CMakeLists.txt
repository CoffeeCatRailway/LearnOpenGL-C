cmake_minimum_required(VERSION 3.30)
project(LearnOpenGL C)

find_package(OpenGL REQUIRED)

set(CMAKE_C_STANDARD 11)

# Include GLFW
add_subdirectory(glfw)
include_directories(glfw/include)

# Include GLAD & other dependencies
include_directories(glad/include)
include_directories(dependencies)

set(SOURCE_FILES src/main.c
        glad/src/glad.c
        src/util.c
        src/util.h
        src/shader.c
        src/shader.h
        src/camera.c
        src/camera.h
        src/model.c
        src/model.h)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

add_custom_target(COPY_RESOURCES ALL
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${PROJECT_BINARY_DIR}/resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/resources
        ${PROJECT_BINARY_DIR}/resources
        COMMENT "Copying resources into binary directory")
add_dependencies(${CMAKE_PROJECT_NAME} COPY_RESOURCES)

target_link_libraries(${CMAKE_PROJECT_NAME} glfw ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES})
