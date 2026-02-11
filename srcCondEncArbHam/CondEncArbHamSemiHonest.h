//
// Created by hassan on 2/9/26.
//

#ifndef RRSS_CONDENCARBHAMSEMIHONEST_H
#define RRSS_CONDENCARBHAMSEMIHONEST_H


#if defined(CRYPTOPP_CXX11_NULLPTR) && !defined(NULLPTR)
# define NULLPTR nullptr
#elif !defined(NULLPTR)
# define NULLPTR NULL
#endif // CRYPTOPP_CXX11_NULLPTR



#include "PaillierWrapperFunctions.h"
#include "CryptoSymWrapperFunctions.h"
#include "PK_crypto.h"


#include "cryptopp/aes.h"
using CryptoPP::AES;


#include "cryptopp/modes.h"
using CryptoPP::CBC_Mode;

#include "cryptopp/pwdbased.h"
using CryptoPP::PKCS5_PBKDF2_HMAC;

#include "cryptopp/hmac.h"
using CryptoPP::HMAC;

#include <chrono>
#include <iostream>
#include <vector>
#include <random>

#include <NTL/LLL.h>
#include <NTL/mat_ZZ_p.h>
using namespace NTL;

#include "../RandomRobustSS.h"
// #include "../ShamirSS/GF256.h"
// #include "../ShamirSS/shamir.h"
// #include "../GauessianElimFullRankMatrix.h"

using namespace GF256;
using namespace shamir;

#include "cryptopp/base64.h"


using namespace std;

using CryptoPP::Base64URLEncoder;
using CryptoPP::Base64URLDecoder;
const size_t SSShareSizeMal = 20; //Indicating the secret sharing size;
const int Small_fieldSizeMal = 8;

struct RRSSShares {
	std::vector<std::pair<std::string, std::vector<int>>> shares;
};


class NTLParamsCondEncHam
{
public:

	RandomRobustSS RRSS;
	int _ell; //number of allowed errors in the hamming distance
	int _len; //length of the message
	int _threshold;
	int lambda_;
	int _n_bits; //number of bits in public key.
	NTLParamsCondEncHam(int len, int threshold, int _lambda, int n_bits): RRSS(len,threshold,_lambda) {
		_len = len;
		_ell = len -  threshold;
		_threshold = threshold;
		_n_bits = n_bits;
		lambda_ = _lambda;

	}

	int PackedEncodingInitParams_old(int _len, int m, int lambda, int n_bits);

	int PackedEncodingInitParams(const mpz_t N, int _len, int m, int lambda, int k);

	static int compute_d_LexpPack(const mpz_t N, int lambda, int k, int  _len, int m);

	void compute_floor_div_minus_one(mpz_t c, const mpz_t a, const mpz_t b);


};



/*
 * Class description:
 *          Conditional Encryption for Hamming Distance Two Predicate Using Shamir Secret Sharing
 * */
// public PaillerWrapperFunctions, public CryptoSymWrapperFunctions
class CondEncArbHamSemiHonest {

public:

	int _n_bits_;
	int _lambda;
	int _len;
	mpz_t _MinPQ;
	PkCrypto CondKeyPair;
	NTLParamsCondEncHam NTL_ArbHam_params;


	int _k; //Is the value used in size of PK for the packing-related optimizations

	// n_lambda is the number of bits requires for Paillier PK to provide lambda bit security levle.

	CondEncArbHamSemiHonest (int _len_, int threshold, int lambda, int n_bits, mpz_t minPQ, int k):_len(_len_),
	CondKeyPair(n_bits, minPQ, k),NTL_ArbHam_params(_len_, threshold, lambda, n_bits)
	{
		_n_bits_ = n_bits; //number of bits for the Paillier PK N.
		_lambda = lambda;
		// _l_NoSmallFactor = l_NoSmallFactor;
		// _eps_NoSmallFactor = eps_NoSmallFactor;
		// we have: minPK = 2^{2\lambda +2 } * n * (2 * m +1).

		if (minPQ == NULL) {
			mpz_t nmMult;
			mpz_init(nmMult); mpz_set_ui(nmMult, (_len_ * (2 * NTL_ArbHam_params.RRSS._m +1)));
			size_t log2_nmMult = mpz_sizeinbase(nmMult, 2) - 1; // floor(log2(n * (2 * m +1)))

			int exp_MinPQ = 2 * _lambda + 2 + log2_nmMult;
			mpz_init(_MinPQ);
			mpz_ui_pow_ui(_MinPQ, 2, exp_MinPQ);
			mpz_clear(nmMult);
		}else {
			mpz_init(_MinPQ);
			mpz_set( _MinPQ, minPQ);
		}


		_k = k; //For conditional Encryption for Hamming distnace we set it as k =4.

		CondKeyPair.initialize(n_bits, _MinPQ, _k); // if WithNIZK is true, then the keys are generated with NIZK implying PK is in GOODKEY
		NTL_ArbHam_params.PackedEncodingInitParams(CondKeyPair._ppk->n, _len, NTL_ArbHam_params.RRSS._m, _lambda, _k);
	}

    /* API Documentation
	 * Algorithm description: Raw encryption scheme which uses the traditional encryption schemes (here we use Partially
     * homomorphic encryption scheme.
	 * Inputs:
	 *    paillier_pubkey_t* ppk                - the public key for our conditional encryption scheme
     *                                              (a pallier public key)
	 *    const string &msg                     - the input message msg of arbitrary length
     *  Output:
     *    string &tx                            - the output ciphertext will be written here.
     * */


    /*
     * API is similar to Enc, but the output ciphertext is pointer to byte array of unsigned char*
     * */
    static int Enc (paillier_pubkey_t* ppk, string &msg, char ctx_final[]);



    /*
     *
     * This function takes as input the public key ppk, the regualar ciphertext ctx, and the secret key psk and
     * the upper bound of the secret message size, and returns the plantext DecryptedMsg as the string. If the decryption
     * is successful, then it also outputs 1;
     *
     */
    static int RegDec(paillier_pubkey_t* ppk, char ctx [], paillier_prvkey_t* psk,
                            size_t _len, string &DecryptedMsg);






    /* API Documentation
	 * Algorithm description: Conditional Encryption algorithm which takes as input the traditional ctx and and then
     * based on the target predicate (hamming Distance two) generates the ciphertext using a secure secret sharing
     * scheme (here we use Shamir secret sharing which is provided by Crypto++ library).
	 * Inputs:
	 *    paillier_pubkey_t* ppk                - the public key for our conditional encryption scheme
     *                                              (a pallier public key)
	 *    const string& RlPwd_ctx_pull          - the input ciphertext of some (unknown) original message x
	 *    const string typo                     - input string which may or may not have ED(typo,x)<= 1
	 *    const string payload                  - the input message to be conditionally encypted based on the predicate ED(typo,x)<= 1
     *    size_t _len                           - Determines the size of padded message
     *  Output:
     *    string typo_ctx                       - the output ciphertext will be written here. If HD(typo,x) > 2
     *                                              the ciphertext reveals nothing about inputs typo or payload or
     *                                              otherwise if ED(typo,x)<=2 then the ciphertext can be decrypted to
     *                                              recover the string payload.     *
     * */


    /*
     * The functionality is similar to the CondEnc while the output is pointer to byte string of unsined char*
     *
     * Note: To achieve the Conditional Encryption for Hamming distance 2, we need to set the threshold value as
     * threshold  = _len - 2, and similarly for Hamming distance 2, we need threshold = _len - 3
     * */
    static int CondEnc(paillier_pubkey_t* ppk,
                        char RlPwd_ctx_pull[],
                        string& typo,
                        string& payload,
                        size_t _len,
                        int threshold,
                        char* ctx_final,
                        NTLParamsCondEncHam NTL_params);




    /* API Documenation
	 * Conditional Encryption for Hamming Distance Two Predicate Using Shamir Secret Sharing: Decryption Algorithm
	 * Inputs:
	 *    const string &typo_ctx                - the conditional ciphertext to decrypt
     *    paillier_pubkey_t* ppk                - the public key for our conditional encryption scheme
     *                                              (a pallier public key)
	 *    paillier_prvkey_t* psk                - the secrtet  key for our conditional encryption scheme
     *                                              (a pallier secret key)
     *    int threshold                         - the threshold number of requirecd valid shares
     *                                              to recover the shared secret
	 *    string recovered                      - the decrypted message will be written here (might be garbage )
     *    size_t _len                           - Determines the size of original padded message
     *
     *    Returns 1 if the message was successfully recovered; -1 if recovery failed
     * */

    static int CondDec(paillier_pubkey_t* ppk,
                        char* typo_ctx,
                        paillier_prvkey_t* psk,
                        int threshold,
                        string &recovered,
                        size_t _len,
                        NTLParamsCondEncHam NTL_params);


//private:

/*
 * Set of required wrapper function for Conditiona Encryption for Hamming Distance Two Predicate
 * Using Shamir Secret Sharing.
 *
 * TODO: I neeed to add the description of the API for the helper functions as well.
 * */

    /*
     * API Documentation:
     * Inputs:
     *      std::vector<std::string> &strShares                     - the string vector of the generated shares
     *      int threshold                                           - the threshold number of requirecd valid shares
     *      vector<int> &selected                                   - subset of selected shares of size threshold:
     *                                                                  We check to see if recovered small secre is
     *                                                                  valid or not. if yes, we recover the main share
     *                                                                  based on the index of the selected shares.
     *  Output:
     *      boolian value which is True if there exists a subset of valid shares; otherwise the ouput is False.
     *
     * */
//    void combinationUtil(int arr[], int n, int r,
//                         int index, int data[], int i);

    static bool TestIfTheSahreAreValid (vector<string> &strShares,
                                        int threshold, vector<int> &selected);

    static bool TestIfTheSahreAreValid_GF256 (shares* strShares,
                                        int threshold, vector<int> &selected, int _len);

    static tuple<vector<paillier_ciphertext_t*>, string> Pail_Parse_Ctx_size_AECtx(paillier_pubkey_t* ppk,
                                                              char* ctx);

    static int  Pail_Parse_Ctx_size_AECtx2(paillier_pubkey_t* ppk, char* ctx, string& CtxtAEStr, vector<paillier_ciphertext_t*>& Vect_Ctx );

    /*
     * TODO: The API description will be added
     * */

    static void makeCombiUtil(vector<vector<int>>& ans, vector<int>& tmp, int n, int left, int k);



    /*
     * Informal Function Description:
     *      The function checks that if the selectes set of shares of the main secret is valid by recovering the secret
     *      and checking the Authenticated Decryption algorithm.
     *      TODO: Formal API will be added
     * */

    static bool RecoverSecretFromValidShares (vector<string> &strShares, int threshold,
                                              vector<int> &selected,
                                              string &RecoveredSecret );

    /*
     * TODO: The API description will be added
     * */

    static int generatesubsets(vector<string> &MainstrShares, vector<string> &strShares,
                        const string &DecoddCtxAE, string &recoveredMainSecret, string &plaintext_rcv, vector<int> choices,
                        int current, int K, vector<int> selected=vector<int>(),
                        vector<int> Valid_selected = vector<int>());

   static int generatesubsets_GF256(vector<string> &MainstrShares,shares* strShares,
                     const string& DecoddCtxAE, string &recoveredMainSecret, string &plaintext_rcv, vector<int> choices,
                     int current, int K, vector<int> selected,
                     vector<int> Valid_selected, int _len);
    /*
     * The folowing function, recursively extract all possible combinatiobs of n choose threshold in the
     * optimized way that we described.
     * The API is described as follows:
     *  Inputs:
     *      MainstsharesL:  the string vector of the shares
     *      strShares:      The shares of the small secret, i.e., "0000".
     *      DecoddCtxAE:    The ctxt of the authenticated encryption, which encryptes the actual payload using the secret key extracted from the secret value.
     *  OutputL
     *      recoveredMainSecret:    The fianl valid secret and we use it to recover the secret key.
     *      plainteext_rcv:         The final payload which is recovered by successful AuthenDecrypt.
     *      choices:                The vector of the current choices which will be updated, recuresively.
     *      current:                The index of the vector which implies the current index of the vector element
     *      K:                      The size current selected eleement.
     *      l_m:                    The size of the original message
     *
     * */
    static int generatesubsets_Optimized(vector<string> &MainstrShares, vector<string> &strShares,
                               const string &DecoddCtxAE, string &recoveredMainSecret, string &plaintext_rcv, vector<int> choices,
                               int current, size_t l_m, int threshold, vector<int> selected=vector<int>(),
                               vector<int> Valid_selected = vector<int>());


    static int generatesubsets_NonSmallFieldCheck(vector<string> &MainstrShares,
                    const string& DecoddCtxAE, string &recoveredMainSecret, string &plaintext_rcv, vector<int> choices,
                    int current, int K, vector<int> selected,
                    vector<int> Valid_selected= vector<int>());


    /*
     * API Documention for the helper functions Enc_SecretShr
     * Inputs:
     *      std::vector<std::string> strShares                      - The string vector of the generates share
     *                                                                  of the secret [Shamir secret Sharing is used]
     *      paillier_pubkey_t* ppk                                  - the public key for our conditional
     *                                                                  encryption scheme (a pallier public key)
     *      size_t ShareSize                                        - Size of the shares which are field elements of a
     *                                                                  specific size.
     *      size_t _len                                             - Determines the size of original padded message
     *
     *  Output:
     *      vector<paillier_ciphertext_t*> &Shar_Ctxt               - the generated encryption of each share will
     *                                                                  written as one of the vector elements of output
     *                                                                  vector ciphertext, i.e., Shar_Ctxt
     *      For successful execution of encryption algorithm it returns integer 1.
     *
     *
     * */
    static int Enc_SecrtShr(vector<string> strShares,  paillier_pubkey_t* ppk,
                     vector<paillier_ciphertext_t*> &Shar_Ctxt, size_t ShareSize, size_t _len );

    static vector<paillier_ciphertext_t*> Enc_SecrtShr_V2(vector<string> strShares,  paillier_pubkey_t* ppk,
                                                          size_t ShareSize, size_t _len );

    static vector<int> GnereateVectorOfIntegeres (int _len);
    static paillier_plaintext_t* RandEncod(string& share, size_t ShareSize, mpz_t N_p_floor, mpz_t P_GF);
    static paillier_plaintext_t* RandDecod(string& share, size_t ShareSize, mpz_t N_p_floor, mpz_t P_GF);



	static int PackedEncodeMpz_t(string &msg, size_t msgSize, vec_ZZ_p V, mpz_t P_GF, int lambda, mpz_t MaxM,mpz_t &PackedEncodedInt);
	static int PackedEncodeMpz_t(string &msg, size_t msgSize, vector<int> V, mpz_t P_GF, int lambda, mpz_t MaxM,mpz_t &PackedEncodedInt);

	static  vector<int> PackedDecodeMpz_t(string  &DecodedMsg, mpz_t PackedEncodedInt, mpz_t P_GF, int lambda, int m_V);
	// static char* PackedDecodeMpz_t(string& DecodedMsg, mpz_t PackedEncodedInt, mpz_t P_GF, int lambda, vector<int>& VecOut);

	static int PackedEncode(string& msg, vector<int> V, mpz_t P_GF, int lambda, mpz_t MaxM, mpz_t N, int& out_d, vector<paillier_plaintext_t*> &PailVectPlain);
	static vector<int> PackedDecode(string& DecodedMsg, vector<paillier_plaintext_t*> CodedMsg,
									int d, mpz_t P_GF, int lambda, mpz_t N);
	static int RandEnc(mpz_t &y_p, mpz_t y, mpz_t N_p_floor, mpz_t MaxM);
	static int RandDec(mpz_t &y, mpz_t y_p, mpz_t MaxM);

	static int PackedMsgEncoding(const mpz_t Msg, mpz_t L_PackingParam, vector<paillier_plaintext_t*> &PailPackedMsg);
	static int PackedMsgDecoding(mpz_t &Msg, mpz_t L_PackingParam, const vector<paillier_plaintext_t*> PailPackedMsg);


	static paillier_plaintext_t* StringToPailPlaintext (string& msg);

	static  vec_ZZ_p ValidSharIndexFinder(mat_ZZ_p V_shares_NTL,int _len, int threshold, NTLParamsCondEncHam NTL_params);

	static void get_digits_baseL(const mpz_t m, const mpz_t L, size_t d_pack, vector<mpz_t> &m_d);
	static int set_from_digits_baseL(const vector<mpz_t> &m_d, const mpz_t L,
											  size_t d_pack, mpz_t& m_out);

	static int PackedEncodeMpz_t_BaseL( string &msg, size_t msgSize, const vector<int> V,
											mpz_t P_GF, int lambda, vector<paillier_plaintext_t*> &PailVectPlain,
											NTLParamsCondEncHam NTL_params);

	static vector<int> PackedDecodeMpz_t_BaseL(string &DecodedMsg, vector<paillier_plaintext_t*> PailVectPlain,
	mpz_t P_GF, int lambda, int m_V, NTLParamsCondEncHam NTL_params);


	/*
	 * In the following we will define two functions packed encoding and packed decoding which
	 * based on the values of NTLParams::d_LexpPack and r_LexpPack as well as _d_Pack to map
	 * (2m*\lambda_1 + \lambda)-bit string to _d_Pack bit strings and then tranfer them to
	 * an integer for randomized encdoing in the conditional encryption.
	 *
	 */


	static vector<mpz_class> packBits(const mpz_class x, NTLParamsCondEncHam NTL_params);
	static mpz_class unpackBits(const std::vector<mpz_class> &parts, NTLParamsCondEncHam NTL_params);

	static int PackedEncodeMpz_t_packBits( string &msg, size_t msgSize, const vector<int> V,
										mpz_t P_GF, int lambda, vector<paillier_plaintext_t*> &PailVectPlain,
										NTLParamsCondEncHam NTL_params);

	static vector<int> PackedDecodeMpz_t_packBits(string &DecodedMsg, vector<paillier_plaintext_t*> PailVectPlain,
	mpz_t P_GF, int lambda, int m_V, NTLParamsCondEncHam NTL_params);
};


#endif //RRSS_CONDENCARBHAMSEMIHONEST_H