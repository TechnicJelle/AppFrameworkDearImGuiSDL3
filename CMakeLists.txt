cmake_minimum_required(VERSION 3.31)
project(TransparentWindowSDL)

add_executable(${PROJECT_NAME} src/main.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
