# CMakeList.txt : CMake project for WahlSoBlue, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("WahlSoBlue")

file(GLOB cpu_source_files "${CMAKE_SOURCE_DIR}/*.cpp")
file(GLOB gpu_source_files "${CMAKE_SOURCE_DIR}/*.cu")
file(GLOB cpu_header_files "${CMAKE_SOURCE_DIR}/*.h")
file(GLOB gpu_header_files "${CMAKE_SOURCE_DIR}/*.cuh")

# Add source to this project's executable.
add_executable (WahlSoBlue ${cpu_source_files} ${cpu_header_files} ${gpu_source_files} ${gpu_header_files})

# TODO: Add tests and install targets if needed.
