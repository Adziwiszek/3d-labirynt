cmake_minimum_required(VERSION 4.0)
project(zad4)

set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(EPOXY REQUIRED epoxy)

include_directories(
        ${GLFW_INCLUDE_DIRS}
        ${EPOXY_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include
)

add_executable(main 
    main.cpp 
    common/shader.cpp
)
target_link_libraries(main
    ${GLFW_LIBRARIES}
    ${EPOXY_LIBRARIES}
    OpenGL::GL
    GLEW::GLEW
    glfw
    dl
    pthread
)