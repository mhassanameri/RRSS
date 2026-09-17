link_directories("/usr/lib/x86_64-linux-gnu")


find_package(Protobuf REQUIRED)
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS db.proto)
include_directories(${PROTOBUF_INCLUDE_DIRS})
message(STATUS ">>> PROTOBUF_INCLUDE_DIRS: (${PROTOBUF_INCLUDE_DIRS})")

if(APPLE)
    find_library(PROTOBUF_STATIC_LIB libprotobuf-c.a PROTOBUF_LIBRARIES)
else()
    find_library(PROTOBUF_STATIC_LIB libprotobuf.a PROTOBUF_LIBRARIES)
endif()
#    find_library(PROTOBUF_STATIC_LIB libprotobuf.a PROTOBUF_LIBRARIES)
find_library(PROTOBUF_SHARED_LIB libprotobuf.so PROTOBUF_LIBRARIES)

find_library(GMP_STATIC_LIB libgmp.a GMP_LIBRARIES)
message(STATUS ">>> GMP LIBRARY: (${GMP_STATIC_LIB})")

find_library(GMP_SHARED_LIB  libgmp.so GMP_LIBRARIES)
message(STATUS ">>> GMP LIBRARY: (${GMP_SHARED_LIB})")
message(STATUS ">>> GMP INLCUDE: (${GMP_INCLUDE_DIRS})")

find_library(NTL_STATIC_LIB libntl.a NTL_LIBRARIES)
link_directories(${NTL_LIBRARIES})
include_directories(${NTL_INCLUDE_DIRS})

link_directories(${GMP_LIBRARIES})
include_directories(${GMP_INCLUDE_DIRS})
#link_libraries(${GMP_LIBRARIES})

find_package(Catch2 REQUIRED)
include_directories(${Catch2_INCLUDE_DIRS})


if(NOT PROTOBUF_STATIC_LIB AND NOT PROTOBUF_SHARED_LIB)
    message( FATAL_ERROR "Cannot find any suitable protobuf library." )
endif()


#cryptoPP
find_path(CRYPTOPP_INCLUDE_DIR
        NAMES cryptopp
        PATHS /usr/include /usr/local/include)

find_library(CRYPTOPP_LIBRARY
        NAMES cryptopp libcryptopp
        PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu)

if(NOT CRYPTOPP_INCLUDE_DIR OR NOT CRYPTOPP_LIBRARY)
    message(FATAL_ERROR "Crypto++ not found. Install libcrypto++-dev")
endif()


add_executable(condencarbham main.cpp
        ../RandomRobustSS.cpp
        ../RandomRobustSS.h
        ../GauessianElimFullRankMatrix.h
        ../GauessianElimFullRankMatrix.cpp
        ../ShamirSS/shamir.h
        ../ShamirSS/shamir.cpp
        ../ShamirSS/GF256.h
        ../ShamirSS/GF256.cpp
        CondEncArbHamSemiHonest.cpp
        CondEncArbHamSemiHonest.h
        CryptoSymWrapperFunctions.cpp
        CryptoSymWrapperFunctions.h
        PaillierWrapperFunctions.cpp
        PaillierWrapperFunctions.h
        ../paillier/pailliercpp.cpp
        ../paillier/pailliercpp.h
        PK_crypto.cpp
        PK_crypto.h
        CondEncAtMostTHamSemiHonest.cpp
        CondEncAtMostTHamSemiHonest.h

)

#add_library(condencarbham STATIC
#        ../RandomRobustSS.cpp
#        ../RandomRobustSS.h
#        ../GauessianElimFullRankMatrix.h
#        ../GauessianElimFullRankMatrix.cpp
#        ../ShamirSS/shamir.h
#        ../ShamirSS/shamir.cpp
#        ../ShamirSS/GF256.h
#        ../ShamirSS/GF256.cpp
#        CondEncArbHamSemiHonest.cpp
#        CondEncArbHamSemiHonest.h
#        CryptoSymWrapperFunctions.cpp
#        CryptoSymWrapperFunctions.h
#        PaillierWrapperFunctions.cpp
#        PaillierWrapperFunctions.h
#        ../paillier/pailliercpp.cpp
#        ../paillier/pailliercpp.h
#        PK_crypto.cpp
#        PK_crypto.h
#)

### Link libraries. These are configured through 3rdparty/${lib_name}

find_library(ARGON2_LIBRARY argon2 PATHS ${CMAKE_SOURCE_DIR}/argon2/phcargon2)
#include_directories("/usr/local/include/crypto-suites")

target_link_libraries(condencarbham
        PUBLIC
        ntl
        ${GMP_STATIC_LIB}
        ${GMP_LIBRARIES}
        ${CRYPTOPP_LIBRARY}
        /usr/local/lib/libSafeheronCryptoSuites.so
        ${ARGON2_LIBRARY}
)

target_include_directories(condencarbham
        PUBLIC
        ${CRYPTOPP_INCLUDE_DIR}
)

include_directories( ${GMP_INCLUDE_DIRS})
include_directories( ${NTL_INCLUDE_DIRS})


