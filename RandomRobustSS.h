//
// Created by hassan on 2/3/26.
//

#ifndef RRSS_RANDOMROBUSTSS_H
#define RRSS_RANDOMROBUSTSS_H

#include <string>
#include <vector>

#include  <cryptopp/cryptlib.h>
using CryptoPP::lword;
using CryptoPP::word32;
using CryptoPP::word64;
using CryptoPP::Exception;
using CryptoPP::DEFAULT_CHANNEL;
using CryptoPP::AAD_CHANNEL;
using CryptoPP::PrivateKey;
using CryptoPP::PublicKey;


#include <cryptopp/ida.h>
#include <cryptopp/aes.h>
using CryptoPP::AES;
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
using CryptoPP::RandomNumberGenerator;

static CryptoPP::AutoSeededRandomPool PRNG;  // instantiate only one class

#include <cryptopp/channels.h>
using CryptoPP::ChannelSwitch;



#include <NTL/LLL.h>
#include <NTL/mat_ZZ_p.h>
using namespace NTL;

#include "ShamirSS/GF256.h"
#include "ShamirSS/shamir.h"
#include "GauessianElimFullRankMatrix.h"

using namespace GF256;
using namespace shamir;

#include "cryptopp/base64.h"


class RandomRobustSS {
public:
    int _len;
    int _t;
    int lambda_;

    int _m; // The number of publicly know secret to be shares as the helper data. For this application and our paper we use _m =  3 *_len.

    int _n; // The number of the shares for identifying the (in)valid shares. It is enough to have $_n = 2* _len.

    RandomRobustSS(int len, int t, int lambda) {
        _len = len;
        _t = t;
        _n =  2 * _len;
        _m = 3 * _len; // Based on the paper, we decided to have 3*_len according to the security analysis.
        lambda_ = lambda;
    }

    static int RRSS_Init(int min, int max);
//    static std::vector<std::string, std::vector<int>> ShareGen(int len, int t, int lambda, std::string Secret);
    static std::vector<std::pair<std::string, std::vector<int>>> ShareGen(int len, int t, int lambda, const std::string& secret);

    static std::string SecretReconstruction(int len, int t, int lambda, std::vector<std::pair<std::string, std::vector<int>>> const Shares);
};

#endif //RRSS_RANDOMROBUSTSS_H