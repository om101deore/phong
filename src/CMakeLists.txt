cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(run VERSION 1.0)

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../include/*.cpp 
                            ${CMAKE_CURRENT_SOURCE_DIR}/../include/custom/*.cpp 
                            ${CMAKE_CURRENT_SOURCE_DIR}/../include/stb/*.cpp 
                            ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
			    ${CMAKE_CURRENT_SOURCE_DIR}/../include/GLFW/*.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/../include/glad/*.c)                           

add_executable(run main.cpp ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} GL dl glfw)
target_include_directories(run PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include/
                                      ${CMAKE_CURRENT_SOURCE_DIR}/)
