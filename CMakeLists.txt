cmake_minimum_required(VERSION 3.28.3)
project(RRSS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

#Finding the cryptoPP package.
find_path(CRYPTOPP_INCLUDE_DIR
        NAMES cryptopp
        PATHS /usr/include /usr/local/include)

find_library(CRYPTOPP_LIBRARY
        NAMES cryptopp libcryptopp
        PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu)

if(NOT CRYPTOPP_INCLUDE_DIR OR NOT CRYPTOPP_LIBRARY)
        message(FATAL_ERROR "Crypto++ not found. Install libcrypto++-dev")
endif()

find_library(NTL_STATIC_LIB libntl.a NTL_LIBRARIES)
link_directories(${NTL_LIBRARIES})
include_directories(${NTL_INCLUDE_DIRS})

link_directories(${GMP_LIBRARIES})
include_directories(${GMP_INCLUDE_DIRS})


add_executable(RRSS main.cpp
        RandomRobustSS.cpp
        RandomRobustSS.h
        GauessianElimFullRankMatrix.h
        GauessianElimFullRankMatrix.cpp
        ShamirSS/GF256.cpp
        ShamirSS/GF256.h
        ShamirSS/shamir.cpp
        ShamirSS/shamir.h
        #        bindings.cpp
        )


pybind11_add_module(rrss
        WrapperPackShares.cpp
        WrapperPackShares.h
        bindings.cpp
        RandomRobustSS.cpp
        RandomRobustSS.h
        GauessianElimFullRankMatrix.h
        GauessianElimFullRankMatrix.cpp
        ShamirSS/GF256.cpp
        ShamirSS/GF256.h
        ShamirSS/shamir.cpp
        ShamirSS/shamir.h
)


set_target_properties(rrss PROPERTIES
        OUTPUT_NAME "rrss"
        PREFIX ""   # ensures it’s rrss*.so, not librrss*.so
)

#set_target_properties(rrss PROPERTIES
#        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
#)


#Finding the cryptoPP package.
find_path(CRYPTOPP_INCLUDE_DIR
        NAMES cryptopp
        PATHS /usr/include /usr/local/include)

find_library(CRYPTOPP_LIBRARY
        NAMES cryptopp libcryptopp
        PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu)

if(NOT CRYPTOPP_INCLUDE_DIR OR NOT CRYPTOPP_LIBRARY)
        message(FATAL_ERROR "Crypto++ not found. Install libcrypto++-dev")
endif()

target_include_directories(rrss PUBLIC ${CRYPTOPP_INCLUDE_DIR})
target_link_libraries(rrss PUBLIC ${CRYPTOPP_LIBRARY})
target_include_directories(rrss PUBLIC ${NTL_INCLUDE_DIRS})
target_link_libraries(rrss PUBLIC ${NTL_LIBRARIES})
target_link_libraries(rrss PUBLIC ntl gmp)
target_link_libraries(rrss  ${GMP_STATIC_LIB})
#target_link_libraries(condenccpp ${${GMP_SHARED_LIB}})
target_link_libraries(rrss  ${GMP_LIBRAIES})

target_include_directories(RRSS PUBLIC ${CRYPTOPP_INCLUDE_DIR})
target_link_libraries(RRSS PUBLIC ${CRYPTOPP_LIBRARY})

target_include_directories(RRSS PUBLIC ${NTL_INCLUDE_DIRS})
target_link_libraries(RRSS PUBLIC ${NTL_LIBRARIES})
target_link_libraries(RRSS PUBLIC ntl)

#target_include_directories(RRSS PRIVATE ${GMP_INCLUDE_DIRS})
#target_link_libraries(RRSS PRIVATE ${GMP_LIBRARIES})

target_link_libraries(RRSS  ${GMP_STATIC_LIB})
#target_link_libraries(condenccpp ${${GMP_SHARED_LIB}})
target_link_libraries(RRSS  ${GMP_LIBRAIES})

include_directories("ShamirSS")

# Ensure the compiled Python extension is placed somewhere importable.
# Putting it in the project root makes: `python -c "import rrss"` work from root.
#set_target_properties(rrss PROPERTIES
#        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake-build-debug"
#        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake-build-debug"
#        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake-build-debug"
#)