cmake_minimum_required(VERSION 3.31)
project(pixcl)

set(CMAKE_CXX_STANDARD 20)

if (APPLE)
    add_compile_options(-gdwarf-4)
endif ()

find_package(OpenCL REQUIRED)

set(STB_IMAGE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/libs/stb_image/include)

set(SOURCES
        src/image.cpp src/image.h
        src/clPipeline.cpp src/clPipeline.h)

add_executable(${PROJECT_NAME} src/main.cpp ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL::OpenCL)

target_include_directories(${PROJECT_NAME} PUBLIC ${STB_IMAGE_INCLUDE_DIRS})
