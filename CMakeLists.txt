cmake_minimum_required(VERSION 3.31)
project(AppFrameworkDearImGuiSDL3)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_executable(${PROJECT_NAME} src/main.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

# Libraries

## SDL3

# By configuring CMake with -DUSE_SYSTEM_SDL3=ON/OFF,
# users can choose between downloading dependencies or using system libraries
option(USE_SYSTEM_SDL3 "Use the SDL3 install from the system" TRUE)

if (USE_SYSTEM_SDL3)
	### Find from system
	find_package("SDL3" REQUIRED)
else ()
	# FetchContent downloads and configures dependencies
	include(FetchContent)

	# SDL3
	FetchContent_Declare(
			SDL3
			GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
			GIT_TAG "main"
			EXCLUDE_FROM_ALL
	)
	FetchContent_MakeAvailable(SDL3)
endif ()

### Link
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

## Dear ImGui
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendored/imgui)
set(IMGUI_SOURCES
		${IMGUI_DIR}/imgui.cpp
		${IMGUI_DIR}/imgui_draw.cpp
		${IMGUI_DIR}/imgui_demo.cpp
		${IMGUI_DIR}/imgui_tables.cpp
		${IMGUI_DIR}/imgui_widgets.cpp
		# Specific implementations:
		${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
		${IMGUI_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_sources(imgui PUBLIC ${IMGUI_SOURCES})

### Set C++ version
target_compile_features(imgui PUBLIC cxx_std_20)
set_target_properties(imgui PROPERTIES CXX_STANDARD_REQUIRED ON)

### Include directories
target_include_directories(imgui SYSTEM PUBLIC ${IMGUI_DIR})
target_include_directories(imgui SYSTEM PUBLIC ${IMGUI_DIR}/backends)

### Link
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)
