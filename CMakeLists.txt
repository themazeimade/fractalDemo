# Minimum CMake version required
cmake_minimum_required(VERSION 3.30)

# Project name and version
project(Test)

# Set the C++ standard to C++11 (or newer)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --target=x86_64-w64-mingw32")
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --target=x86_64-w64-mingw32")
set(IMGUI_DIR ./externals/imgui/)
set(IMPLOT_DIR ./externals/implot/)
file(GLOB IMGUI_SRC "${IMGUI_DIR}/*.cpp" "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp" "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp" )
file(GLOB IMPLOT_SRC "${IMPLOT_DIR}/*.cpp")
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.c")

# Add the executable (the source file is main.cpp)
add_executable(ThreadPlot ${SOURCES} ${IMGUI_SRC} ${IMPLOT_SRC})
target_include_directories(ThreadPlot PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_include_directories(ThreadPlot PRIVATE ${IMGUI_DIR} ${IMGUI_DIR}/backends/ ${IMPLOT_DIR})

# find_package(Threads REQUIRED)
# target_link_libraries(Test1 PRIVATE Threads::Threads)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.4 REQUIRED)
find_package(glm REQUIRED)
target_link_libraries(ThreadPlot glfw glm::glm)
target_link_libraries(ThreadPlot OpenGL::GL)
