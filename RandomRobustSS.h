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

// static CryptoPP::AutoSeededRandomPool PRNG;  // instantiate only one class

#include <cryptopp/channels.h>
using CryptoPP::ChannelSwitch;

#include <gmp.h>

#include <NTL/LLL.h>
#include <NTL/mat_ZZ_p.h>
using namespace NTL;

#include "ShamirSS/GF256.h"
#include "ShamirSS/shamir.h"
#include "GauessianElimFullRankMatrix.h"

using namespace GF256;
using namespace shamir;

#include "cryptopp/base64.h"


class NTLParams
{
public:
    vector<GF256::byte> V_pub; // the publickey known vector will be used to recover the valid shares, if the predicate holds
    vec_ZZ_p V_pub_ZZ;
    vec_GF2E V_pub_GF2E;
    bool is_RRSSNTLParamInitialized = false;
    scheme GF_2E_SS; //For the security reasons we need to 2n shares with threshold 2*threshold
    vector<int> R;
    mat_ZZ_p Debug;
    mpz_t P_GF_OrigShare;
    mpz_t MaxM;
    mpz_t MaxMEncoded;
    mpz_t L_PackingParam;
    mpz_t N_MaxMEncdo_Floor;
    int m_V; //The number of publicly know values or the numebr of the equations in the system of equations. We set it as m_V = 3*_len in which _len is the numebr of the shares.

    int lambda;
    int n_bits;
    int n_PackedPlain;
    int _d_pack; //indicating the number of elements after encoding to Paillier plaintext.

    /*
     * Let T = \dfrac{2^{\left\lfloor \log_2\sqrt{N}\right\rfloor-k}} {4\times 2^\lambda n (2 * m +1)}
     * then, d_LexpPack is maximized such that 2^d_LexpPack < T.
     */

    int d_LexpPack;
    int r_LexpPack;

    mpz_t exp_2_d_LexpPack; // stores 2 ^ d_LexpPack
    mpz_t exp_2_r_LexpPack; // stores 2 ^ r_LexpPack

    mpz_t RanEnc_Floor_d_LexpPack; // stores floor N/2^d_LexpPack
    mpz_t RanEnc_Floor_r_LexpPack; // stores floor N/2^r_LexpPack

    NTLParams(int m, int _len_, int _t_, int _lambda, int _n_bits ): GF_2E_SS(2*_len_,2*_t_)
    {
        n_bits = _n_bits;
        lambda = _lambda;
        long a = 257;
        ZZ_p::init(ZZ(a));
        m_V = m;
        V_pub_ZZ.SetLength(m);
        Debug.SetDims(m,_len_);

        // n_PackedPlain = PackedEncodingInitParams(_len_, m, _lambda, _n_bits);
        if (!is_RRSSNTLParamInitialized) {
            SetV_pub(m);
            is_RRSSNTLParamInitialized = true;
        }else {
            throw std::logic_error("RRSS params are already initialized");
        }

    }

    // int PackedEncodingInitParams(const mpz_t N, int _len, int m, int lambda, int k);

    void SetV_pub(int m);

    // static int compute_d_LexpPack(const mpz_t N, int lambda, int k, int  _len, int m);

    // void compute_floor_div_minus_one(mpz_t c, const mpz_t a, const mpz_t b);


};




class RandomRobustSS {
public:
    int _len;
    int _t;
    int lambda_;
    int _m; // The number of publicly know secret to be shares as the helper data. For this application and our paper we use _m =  3 *_len.
    int _n; // The number of the shares for identifying the (in)valid shares. It is enough to have $_n = 2* _len.
    NTLParams NTL_params;



    RandomRobustSS(int len, int t, int lambda): _len(len), _t(t),
    lambda_(lambda),
    _m(3 * len),
    _n(2* len),
    NTL_params(3*len,len, t, lambda, 3096){
    }

    static int RRSS_Init(int min, int max);
//    static std::vector<std::string, std::vector<int>> ShareGen(int len, int t, int lambda, std::string Secret);
    // static std::vector<std::pair<std::string, std::vector<int>>> ShareGen(int len, int t, NTLParams NTL_params, const std::string& secret);
    static std::vector<std::pair<std::string, std::vector<int>>> ShareGen(int len, int t, NTLParams& NTL_params, const std::string& secret);

    // static std::string SecretReconstruction(int len, int t,  NTLParams NTL_params, std::vector<std::pair<std::string, std::vector<int>>> const Shares);
    static std::string SecretReconstruction(int len, int t,  const NTLParams& NTL_params, const std::vector<std::pair<std::string, std::vector<int>>>& Shares);

    static vec_ZZ_p ValidSharIndexFinder(       mat_ZZ_p V_shares_NTL,  int _len, int threshold, NTLParams NTL_params);
    // static vec_GF2E ValidSharIndexFinder_GF2E(  mat_GF2E V_shares_GF2E, int _len, int threshold, NTLParams NTL_params);
    static vec_GF2E ValidSharIndexFinder_GF2E(const NTL::mat_GF2E& V_shares_GF2E, int _len, int threshold,const NTLParams& NTL_params);
    static bool RecoverSecretFromValidShares (const std::vector<std::string> &strShares,
                                                int threshold,
                                                const std::vector<int> &selected,
                                                std::string &RecoveredSecret);

};

#endif //RRSS_RANDOMROBUSTSS_H