cmake_minimum_required(VERSION 3.28.3)
project(RRSS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

add_executable(RRSS main.cpp
        RandomRobustSS.cpp
        RandomRobustSS.h
#        bindings.cpp
        )



pybind11_add_module(rrss
        bindings.cpp
        RandomRobustSS.cpp
)



include_directories("ShamirSS")

# Ensure the compiled Python extension is placed somewhere importable.
# Putting it in the project root makes: `python -c "import rrss"` work from root.
#set_target_properties(rrss PROPERTIES
#        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake-build-debug"
#        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake-build-debug"
#        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake-build-debug"
#)