cmake_minimum_required(VERSION 3.31)
project(pixcl)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenCL REQUIRED)

set(STB_IMAGE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/libs/stb_image/include)

add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/image.cpp
    src/image.h)

target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL::OpenCL)

target_include_directories(${PROJECT_NAME} PUBLIC ${STB_IMAGE_INCLUDE_DIRS})
