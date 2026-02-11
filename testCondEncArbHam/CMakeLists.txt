find_library(GMP_STATIC_LIB libgmp.a GMP_LIBRARIES)
#find_library(GMP_SHARED_LIB  libgmp.so GMP_LIBRARIES)
message(STATUS ">>> GMP LIBRARY: (${GMP_SHARED_LIB})")
#message(STATUS ">>> GMP INCLUDE: (${GMP_INCLUDE_DIRS})")
include_directories(${GMP_INCLUDE_DIRS})

find_library(NTL_STATIC_LIB libntl.a NTL_LIBRARIES)


include_directories("/usr/include/NTL")

set(TEST_SRCS
        ../GauessianElimFullRankMatrix.h
        ../GauessianElimFullRankMatrix.cpp
        ../RandomRobustSS.h
        ../RandomRobustSS.cpp
#        ../srcCondEncArbHam/CondEncArbHamSemiHonest.h
#        ../srcCondEncArbHam/CondEncArbHamSemiHonest.cpp
#        ../srcCondEncArbHam/CryptoSymWrapperFunctions.cpp
#        ../srcCondEncArbHam/CryptoSymWrapperFunctions.h
#        ../srcCondEncArbHam/PaillierWrapperFunctions.cpp
#        ../srcCondEncArbHam/PaillierWrapperFunctions.h
        ../paillier/pailliercpp.cpp
        ../paillier/pailliercpp.h
        ../srcCondEncArbHam/PK_crypto.cpp
        ../srcCondEncArbHam/PK_crypto.h
        ../srcCondEncArbHam/CondEncAtMostTHamSemiHonest.cpp
        ../srcCondEncArbHam/CondEncAtMostTHamSemiHonest.h
)

add_executable(tests ${TEST_SRCS}
        testCondEncEvalArbHam.h
        testCondEncEvalArbHam.cpp
        ../srcCondEncArbHam/CondEncArbHamSemiHonest.h
        ../srcCondEncArbHam/CondEncArbHamSemiHonest.cpp
        ../srcCondEncArbHam/CryptoSymWrapperFunctions.cpp
        ../srcCondEncArbHam/CryptoSymWrapperFunctions.h
        ../srcCondEncArbHam/PaillierWrapperFunctions.cpp
        ../srcCondEncArbHam/PaillierWrapperFunctions.h
        ../paillier/pailliercpp.cpp
        ../paillier/pailliercpp.h
        ../srcCondEncArbHam/PK_crypto.cpp
        ../srcCondEncArbHam/PK_crypto.h
        ../ShamirSS/shamir.h
        ../ShamirSS/shamir.cpp
        ../ShamirSS/GF256.h
        ../ShamirSS/GF256.cpp
        ../srcCondEncArbHam/CondEncAtMostTHamSemiHonest.cpp
        ../srcCondEncArbHam/CondEncAtMostTHamSemiHonest.h
)

target_link_libraries(tests
        PUBLIC
        ntl
        ${GMP_STATIC_LIB}
        ${GMP_LIBRARIES}
        ${CRYPTOPP_LIBRARY}
        /usr/local/lib/libSafeheronCryptoSuites.so
        ${ARGON2_LIBRARY}
)

