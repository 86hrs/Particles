cmake_minimum_required(VERSION 4.2.3)
project(main)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/obj)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp src/*.h src/*.c)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_compile_options(${PROJECT_NAME} PRIVATE
    -O3
    -ffast-math
    -march=native
    -flto
    -Wall
    -Wextra
    -fno-rtti             # If you don’t use dynamic_cast or typeid
)

target_link_libraries(${PROJECT_NAME}
    ${OPENGL_LIBRARIES}
    glfw
)

set(SHADER_FILES
    ${CMAKE_SOURCE_DIR}/src/vertex.glsl
    ${CMAKE_SOURCE_DIR}/src/fragment.glsl
)
add_custom_target(copy_resources ALL
    COMMENT "Copying shaders and resources to output directory"
)
foreach(RESOURCE_FILE ${SHADER_FILES})
    get_filename_component(RESOURCE_FILENAME ${RESOURCE_FILE} NAME)
    add_custom_command(
        POST_BUILD
        TARGET copy_resources
        COMMAND ${CMAKE_COMMAND} -E copy
        ${RESOURCE_FILE}
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${RESOURCE_FILENAME}
    )
endforeach()
add_dependencies(${PROJECT_NAME} copy_resources)

