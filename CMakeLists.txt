cmake_minimum_required(VERSION 3.28.3)
project(RRSS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

# -----------------------------
# Crypto++
# -----------------------------
find_path(CRYPTOPP_INCLUDE_DIR
        NAMES cryptopp/cryptlib.h
        PATHS /usr/include /usr/local/include
)

find_library(CRYPTOPP_LIBRARY
        NAMES cryptopp libcryptopp
        PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu
)

if(NOT CRYPTOPP_INCLUDE_DIR OR NOT CRYPTOPP_LIBRARY)
        message(FATAL_ERROR "Crypto++ not found. Install libcrypto++-dev")
endif()

# -----------------------------
# NTL
# -----------------------------
find_path(NTL_INCLUDE_DIR
        NAMES NTL/ZZ_pEX.h
        PATHS /usr/include /usr/local/include
)

find_library(NTL_LIBRARY
        NAMES ntl libntl
        PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu
)

if(NOT NTL_INCLUDE_DIR OR NOT NTL_LIBRARY)
        message(FATAL_ERROR "NTL not found. Install libntl-dev")
endif()

# -----------------------------
# GMP
# -----------------------------
find_path(GMP_INCLUDE_DIR
        NAMES gmp.h
        PATHS
        /usr/include
        /usr/include/x86_64-linux-gnu
        /usr/local/include
)

find_library(GMP_SHARED_LIB
        NAMES gmp libgmp
        PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu
)

find_library(GMP_STATIC_LIB
        NAMES libgmp.a gmp.a
        PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu
)

if(NOT GMP_INCLUDE_DIR OR NOT GMP_SHARED_LIB)
        message(FATAL_ERROR "GMP not found. Install libgmp-dev")
endif()

# -----------------------------
# Argon2
# -----------------------------
find_path(ARGON2_INCLUDE_DIR
        NAMES argon2.h
        PATHS /usr/include /usr/local/include
)

find_library(ARGON2_LIBRARY
        NAMES argon2 libargon2
        PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu
)

if(NOT ARGON2_INCLUDE_DIR OR NOT ARGON2_LIBRARY)
        message(FATAL_ERROR "Argon2 not found. Install libargon2-dev")
endif()

# -----------------------------
# Crypto-Suit
# -----------------------------
set(CRYPTO_SUITES_DIR "${CMAKE_SOURCE_DIR}/external/crypto-suites")

if(NOT EXISTS "${CRYPTO_SUITES_DIR}/crypto-bn/bn.h")
        message(FATAL_ERROR
                "Missing bundled dependency: external/crypto-suites/crypto-bn/bn.h")
endif()

include_directories("${CMAKE_SOURCE_DIR}/external")


# -----------------------------
# Main executable
# -----------------------------
add_executable(RRSS
        main.cpp
        RandomRobustSS.cpp
        RandomRobustSS.h
        GauessianElimFullRankMatrix.h
        GauessianElimFullRankMatrix.cpp
        ShamirSS/GF256.cpp
        ShamirSS/GF256.h
        ShamirSS/shamir.cpp
        ShamirSS/shamir.h
)


target_include_directories(RRSS PUBLIC
        ${CRYPTOPP_INCLUDE_DIR}
        ${NTL_INCLUDE_DIR}
        ${GMP_INCLUDE_DIR}
)

target_link_libraries(RRSS PUBLIC
        ${CRYPTOPP_LIBRARY}
        ${NTL_LIBRARY}
        ${GMP_SHARED_LIB}
)

#target_compile_options(RRSS PRIVATE
#        -O0
#        -g3
#        -fsanitize=address,undefined
#        -fno-omit-frame-pointer
#        -fno-inline
#)
#
#target_link_options(RRSS PRIVATE
#        -fsanitize=address,undefined
#)

#target_compile_options(RRSS PRIVATE -Og -g -fno-omit-frame-pointer)

# -----------------------------
# Python module
# -----------------------------
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
        PREFIX ""
)

target_include_directories(rrss PUBLIC
        ${CRYPTOPP_INCLUDE_DIR}
        ${NTL_INCLUDE_DIR}
        ${GMP_INCLUDE_DIR}
)

target_link_libraries(rrss PUBLIC
        ${CRYPTOPP_LIBRARY}
        ${NTL_LIBRARY}
        ${GMP_SHARED_LIB}
)

# -----------------------------
# Extra include dirs
# -----------------------------
include_directories("ShamirSS")
include_directories("paillier")

# These subdirectories expect ARGON2_LIBRARY and GMP_STATIC_LIB
add_subdirectory(srcCondEncArbHam)

include(CTest)
enable_testing()
add_subdirectory(testCondEncArbHam)

file(COPY PWDvsTyposDataSet DESTINATION testCondEncArbHam)