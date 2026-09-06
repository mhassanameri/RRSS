//
// Created by hassan on 2/9/26.
//

#include "CondEncArbHamSemiHonest.h"

// int CondEncHamDistMal::NTL_params->m_V;
// vector<GF256::byte> CondEncHamDistMal::V_pub; // the publickey known vector will be used to recover the valid shares, if the predicate holds
// vec_ZZ_p CondEncHamDistMal::V_pub_ZZ;
// mpz_t CondEncHamDistMal::P_GF_OrigShare;
// mpz_t CondEncHamDistMal::MaxM;


bool CondEncArbHamSemiHonest::TestIfTheSahreAreValid(vector<string> &strShares, int threshold, vector<int> &selected) {
    string channel;
    const unsigned int CHID_LENGTH = 4;
    string recovered;
    CryptoPP::SecretRecovery recovery(threshold, new StringSink(recovered), false);

    CryptoPP::vector_member_ptrs<StringSource> strSources(threshold);
    channel.resize(CHID_LENGTH);
    for (unsigned int i = 0; i < threshold; i++) {
        strSources[i].reset(new StringSource(strShares[selected[i]], false));
        strSources[i]->Pump(CHID_LENGTH);
        strSources[i]->Get((CryptoPP::byte *) &channel[0], CHID_LENGTH);
        strSources[i]->Attach(new CryptoPP::ChannelSwitch(recovery, channel));
    }

    while (strSources[0]->Pump(256)) {
        for (unsigned int i = 1; i < threshold; i++)
            strSources[i]->Pump(256);
    }

    for (unsigned int i = 0; i < threshold; i++)
        strSources[i]->PumpAll();

    bool fail = false;
    // string subsRcvr= recovered.substr(0, 4);

    const char *Zero_str = "0\000\000\000";
    fail = (Zero_str[0] == recovered[0] && Zero_str[1] == recovered[1] && Zero_str[2] == recovered[2] && Zero_str[3] ==
            recovered[3]);
    // fail = (Zero_str[0] == recovered[0] );


    // fail = ("0000" == recovered);//for Optimized solution, TODO: make the non optimum without cancelling it
    // fail = ("0\000\000" == recovered.substr(0,3));//for Optimized soulution, TODO: make the non optimum without cancelling it


    //    fail  = true;
    return fail;
}


bool CondEncArbHamSemiHonest::TestIfTheSahreAreValid_GF256(shares *strShares, int threshold, vector<int> &selected,
                                                           int _len) {
    scheme GF256_SSscheme(_len, threshold);
    shares *GF256_shares = new shares(threshold);
    for (int i = 0; i < threshold; i++) {
        for (auto val: (*strShares)[selected[i]]) {
            (*GF256_shares)[i].push_back(val);
        }
    }

    string recovered = GF256_SSscheme.getSecret(GF256_shares);

    bool fail = false;
    fail = (recovered == "0");
    //    fail  = true;
    return fail;
}


void CondEncArbHamSemiHonest::makeCombiUtil(vector<vector<int> > &ans,
                                            vector<int> &tmp, int n, int left, int k) {
    // Pushing this vector to a vector of vector
    if (k == 0) {
        ans.push_back(tmp);
        return;
    }

    // i iterates from left to n. First time
    // left will be 1
    for (int i = left; i <= n; ++i) {
        tmp.push_back(i);
        makeCombiUtil(ans, tmp, n, i + 1, k - 1);

        // Popping out last inserted element
        // from the vector
        tmp.pop_back();
    }
}


bool CondEncArbHamSemiHonest::RecoverSecretFromValidShares(vector<string> &strShares,
                                                           int threshold,
                                                           vector<int> &selected,
                                                           string &RecoveredSecret) {
    string channel;
    const unsigned int CHID_LENGTH = 4;
    string recovered;
    CryptoPP::SecretRecovery recovery(threshold, new StringSink(recovered), false);

    CryptoPP::vector_member_ptrs<StringSource> strSources(threshold);
    channel.resize(CHID_LENGTH);

    for (unsigned int i = 0; i < threshold; i++) {
        strSources[i].reset(new StringSource(strShares[selected[i]], false));
        strSources[i]->Pump(CHID_LENGTH);
        strSources[i]->Get((CryptoPP::byte *) &channel[0], CHID_LENGTH);
        strSources[i]->Attach(new CryptoPP::ChannelSwitch(recovery, channel));
    }

    while (strSources[0]->Pump(256)) {
        for (unsigned int i = 1; i < threshold; i++)
            strSources[i]->Pump(256);
    }

    for (unsigned int i = 0; i < threshold; i++)
        strSources[i]->PumpAll();
    size_t keySize = recovered.size();
    size_t KeySizePut;
    //    memcpy(&RecoveredSecret, &recovered, recovered.size()-1 );
    //    RecoveredSecret =  recovered;

    CryptoPP::StringSink ss_RecoveredSecret(RecoveredSecret);
    cout << "";
    KeySizePut = ss_RecoveredSecret.Put((const CryptoPP::byte *) recovered.data(), recovered.size(), false);

    return true;
}


int CondEncArbHamSemiHonest::generatesubsets_GF256(vector<string> &MainstrShares, shares *strShares,
                                                   const string &DecoddCtxAE, string &recoveredMainSecret,
                                                   string &plaintext_rcv, vector<int> choices,
                                                   int current, int K, vector<int> selected,
                                                   vector<int> Valid_selected, int _len) {
    //    const string CTXT_AE = DecoddCtxAE;
    if (choices.size() - current < K - selected.size())
        return 0;
    if (selected.size() == K) {
        // vector<std::string> o[selected];
        //process subset
        bool pass;
        // auto start_checkShare = std::chrono::high_resolution_clock::now();
        pass = CondEncArbHamSemiHonest::TestIfTheSahreAreValid_GF256(strShares, K, selected, _len);

        // auto stop_CheckShare = std::chrono::high_resolution_clock::now();

        // auto duration_CondDec_HD = std::chrono::duration_cast<std::chrono::microseconds>(stop_CheckShare - start_checkShare);
        // cout  << "OPT Share Checking time: " << duration_CondDec_HD.count() << "\n";
        if (pass) {
            Valid_selected = selected;
            string recoverTheMainSecret;

            bool ifCorrectShareVec;
            ifCorrectShareVec = CondEncArbHamSemiHonest::RecoverSecretFromValidShares(
                MainstrShares, K, selected, recoverTheMainSecret);
            size_t key_size = recoverTheMainSecret.size();
            CryptoPP::StringSink ss_recoveredMainSecret(recoveredMainSecret);
            cout << "";
            auto reMainSecrtSize = ss_recoveredMainSecret.Put((const CryptoPP::byte *) recoverTheMainSecret.data(),
                                                              recoverTheMainSecret.size(), false);
            bool AEReslt = false;
            AEReslt = CryptoSymWrapperFunctions::Wrapper_AuthDecrypt(recoverTheMainSecret, DecoddCtxAE, plaintext_rcv);


            if (AEReslt) {
                // cout << "The recovered payload is = " << plaintext_rcv << "\n";
                return 1;
            }
        }
        return 0;
    }
    if (current == choices.size())
        return 0;

    selected.push_back(choices[current]);
    if (CondEncArbHamSemiHonest::generatesubsets_GF256(MainstrShares, strShares, DecoddCtxAE, recoveredMainSecret,
                                                       plaintext_rcv,
                                                       choices, current + 1, K, selected, Valid_selected, _len) == 1)
        return 1;
    selected.pop_back();
    if (CondEncArbHamSemiHonest::generatesubsets_GF256(MainstrShares, strShares, DecoddCtxAE, recoveredMainSecret,
                                                       plaintext_rcv,
                                                       choices, current + 1, K, selected, Valid_selected, _len) == 1)
        return 1;

    return 0;
}


int CondEncArbHamSemiHonest::generatesubsets(vector<string> &MainstrShares, vector<string> &strShares,
                                             const string &DecoddCtxAE, string &recoveredMainSecret,
                                             string &plaintext_rcv, vector<int> choices,
                                             int current, int K, vector<int> selected,
                                             vector<int> Valid_selected) {
    //    const string CTXT_AE = DecoddCtxAE;
    if (choices.size() - current < K - selected.size())
        return 0;
    if (selected.size() == K) {
        // vector<std::string> o[selected];
        //process subset
        bool pass;
        // auto start_checkShare = std::chrono::high_resolution_clock::now();
        // int _len = MainstrShares.size();
        pass = CondEncArbHamSemiHonest::TestIfTheSahreAreValid(strShares, K, selected); //For the optimized solution
        // pass = HamDistAtmostT::TestIfTheSahreAreValid_GF256(strShares, K, selected , _len);

        // auto stop_CheckShare = std::chrono::high_resolution_clock::now();

        // auto duration_CondDec_HD = std::chrono::duration_cast<std::chrono::microseconds>(stop_CheckShare - start_checkShare);
        // cout  << "OPT Share Checking time: " << duration_CondDec_HD.count() << "\n";
        if (pass) {
            Valid_selected = selected;
            string recoverTheMainSecret;

            bool ifCorrectShareVec;
            ifCorrectShareVec = CondEncArbHamSemiHonest::RecoverSecretFromValidShares(
                MainstrShares, K, selected, recoverTheMainSecret);
            size_t key_size = recoverTheMainSecret.size();
            CryptoPP::StringSink ss_recoveredMainSecret(recoveredMainSecret);
            cout << "";
            auto reMainSecrtSize = ss_recoveredMainSecret.Put((const CryptoPP::byte *) recoverTheMainSecret.data(),
                                                              recoverTheMainSecret.size(), false);
            bool AEReslt = false;
            AEReslt = CryptoSymWrapperFunctions::Wrapper_AuthDecrypt(recoverTheMainSecret, DecoddCtxAE, plaintext_rcv);


            if (AEReslt) {
                // cout << "The recovered payload is = " << plaintext_rcv << "\n";
                return 1;
            }
        }
        //            for(auto i:selected)
        //                cout<<i<<" ";
        //            cout<<endl;
        return 0;
    }
    if (current == choices.size())
        return 0;

    selected.push_back(choices[current]);
    if (CondEncArbHamSemiHonest::generatesubsets(MainstrShares, strShares, DecoddCtxAE, recoveredMainSecret,
                                                 plaintext_rcv,
                                                 choices, current + 1, K, selected, Valid_selected) == 1)
        return 1;
    selected.pop_back();
    if (CondEncArbHamSemiHonest::generatesubsets(MainstrShares, strShares, DecoddCtxAE, recoveredMainSecret,
                                                 plaintext_rcv,
                                                 choices, current + 1, K, selected, Valid_selected) == 1)
        return 1;

    return 0;
}


/*
 * This function takes as input the Base64 encoded (using b64Encode fucntion) and concetenated shares of and then
 * first endodce them as an integer and then runs RandEncode function on it. All the computations are helndle with
 * gmp library.
 * */
//    vector<paillier_ciphertext_t*> Enc_SecrtShr(std::vector<std::string> strShares,  paillier_pubkey_t* ppk) const {
int CondEncArbHamSemiHonest::Enc_SecrtShr(vector<string> strShares, paillier_pubkey_t *ppk,
                                          vector<paillier_ciphertext_t *> &Shar_Ctxt, size_t ShareSize,
                                          size_t _len) {
    /*Computing the maximum value of the share*/


    mpz_t P_GF;
    mpz_init(P_GF);
    size_t ShareStreamSizer = ShareSize;
    int max_power_int = (ShareStreamSizer * 8) + 1;
    mpz_ui_pow_ui(P_GF, 2, max_power_int);
    mpz_sub_ui(P_GF, P_GF, 1); // computing the value of P_GF based on the size of the input value.
    mpz_t N_p_floor;
    mpz_init(N_p_floor);
    mpz_fdiv_q(N_p_floor, ppk->n, P_GF); // Here, we have P_GF = 2^(36 * 8 +1 ) -1
    mpz_sub_ui(N_p_floor, N_p_floor, 1);
    // We have N_p_floor = floor(N/P_GF - 1) (This is correct with the assumption that p = 2 ^32.
    //    paillier_plaintext_t* m_rcv_aftermod;
    //    m_rcv_aftermod = (paillier_plaintext_t*) malloc(sizeof(paillier_plaintext_t));
    //    mpz_init(m_rcv_aftermod->m);
    mpz_t a_i;
    mpz_init(a_i);
    paillier_plaintext_t *m;
    m = (paillier_plaintext_t *) malloc(sizeof(paillier_plaintext_t));
    mpz_init(m->m);
    //    mpz_t aux;
    //    mpz_init(aux);
    //    paillier_plaintext_t* m_Rand;
    for (int i = 0; i < _len; i++) {
        //        paillier_plaintext_t* m_Rand;
        auto &str_Shrs = strShares[i];
        unsigned char a[ShareSize];
        for (int j = 0; j < ShareSize; ++j) {
            a[j] = str_Shrs[j];
        }
        mpz_import(m->m, ShareSize, -1, 1, 0, 0, &a[0]);
        //        mpz_init_set(m->m, aux);

        //        m = paillier_plaintext_from_bytes(&a[0],ShareSize );
        gmp_randstate_t rand_i;
        gmp_randinit_mt(rand_i);
        mpz_urandomm(a_i, rand_i, N_p_floor);
        // Generates a random number in range [1, N_p_floor] in which N_p_floor =  floor(N/p - 1).
        mpz_mul(a_i, a_i, P_GF); //multiplying a_i with P_GF and set as m_Rand->m;
        mpz_add(m->m, m->m, a_i); // Computing m_Rand = m_i + a_i * p mod N.
        Shar_Ctxt[i] = paillier_enc(NULL, ppk, m, paillier_get_rand_devrandom);
        gmp_randclear(rand_i);
    }

    //    paillier_freeplaintext(m_rcv_aftermod);
    paillier_freeplaintext(m);
    mpz_clear(a_i);
    mpz_clear(P_GF);
    mpz_clear(N_p_floor);
    //    mpz_clear(aux);
    return 1;
}

vector<paillier_ciphertext_t *> CondEncArbHamSemiHonest::Enc_SecrtShr_V2(
    vector<string> strShares, paillier_pubkey_t *ppk,
    size_t ShareSize, size_t _len) {
    vector<paillier_ciphertext_t *> Shar_Ctxt(_len);

    mpz_t P_GF;
    mpz_init(P_GF);
    int max_power_int = (ShareSize * 8) + 1;
    mpz_ui_pow_ui(P_GF, 2, max_power_int);
    mpz_sub_ui(P_GF, P_GF, 1); // computing the value of P_GF based on the size of the input value.
    mpz_t N_p_floor;
    mpz_init(N_p_floor);
    mpz_fdiv_q(N_p_floor, ppk->n, P_GF); // Here, we have P_GF = 2^(36 * 8 +1 ) -1
    mpz_sub_ui(N_p_floor, N_p_floor, 1);
    // We have N_p_floor = floor(N/P_GF - 1) (This is correct with the assumption that p = 2 ^32.

    for (int i = 0; i < _len; i++) {
        paillier_plaintext_t *m;
        m = RandEncod(strShares[i], ShareSize, N_p_floor, P_GF);
        Shar_Ctxt[i] = paillier_enc(NULL, ppk, m, paillier_get_rand_devrandom);
        paillier_freeplaintext(m);
    }

    mpz_clear(P_GF);
    mpz_clear(N_p_floor);
    return Shar_Ctxt;
}

paillier_plaintext_t *CondEncArbHamSemiHonest::RandEncod(string &share, size_t ShareSize, mpz_t N_p_floor, mpz_t P_GF) {
    mpz_t a_i;
    mpz_init(a_i);
    auto &str_Shrs = share;
    //    unsigned char a[ShareSize];
    char a[ShareSize];
    //    unsigned long a[ShareSize];

    for (int j = 0; j < ShareSize; ++j) {
        a[j] = str_Shrs[j];
    }
    paillier_plaintext_t *m;
    m = (paillier_plaintext_t *) malloc(sizeof(paillier_plaintext_t));
    mpz_init(m->m);
    mpz_import(m->m, ShareSize, -1, 1, 0, 0, a);
    //    mpz_import(m->m, ShareSize, -1, 1, 0, 0, &a[0]);

    gmp_randstate_t rand_i;
    gmp_randinit_mt(rand_i);
    mpz_urandomm(a_i, rand_i, N_p_floor);
    // Generates a random number in range [1, N_p_floor] in which N_p_floor =  floor(N/p - 1).
    mpz_mul(a_i, a_i, P_GF); //multiplying a_i with P_GF and set as m_Rand->m;
    mpz_add(m->m, m->m, a_i); // Computing m_Rand = m_i + a_i * p mod N.

    gmp_randclear(rand_i);
    mpz_clear(a_i);
    return m;
}


int CondEncArbHamSemiHonest::RandEnc(mpz_t &y_p, mpz_t y, mpz_t N_p_floor, mpz_t MaxM) {
    mpz_t a_i;
    mpz_init(a_i);

    if (!y_p) {
        mpz_init(y_p);
    }


    gmp_randstate_t rand_i;
    gmp_randinit_mt(rand_i);
    mpz_urandomm(a_i, rand_i, N_p_floor);
    // Generates a random number in range [1, N_p_floor] in which N_p_floor =  floor(N/p - 1).
    mpz_mul(a_i, a_i, MaxM); //multiplying a_i with P_GF and set as m_Rand->m;
    mpz_add(y_p, y, a_i); // Computing m_Rand = m_i + a_i * p mod N.

    gmp_randclear(rand_i);
    mpz_clear(a_i);
    return 1;
}

int CondEncArbHamSemiHonest::RandDec(mpz_t &y, mpz_t y_p, mpz_t MaxM) {
    if (!y) {
        mpz_init(y);
    }
    mpz_mod(y, y_p, MaxM);
    return 1;
}


//RandomNumberGenerator & GlobalRNG()
//{
//    static CryptoPP::OFB_Mode<AES>::Encryption s_globalRNG;
//    return dynamic_cast<RandomNumberGenerator&>(s_globalRNG);
//}


int CondEncArbHamSemiHonest::Enc(paillier_pubkey_t *ppk, string &msg, char ctx_final[]) {
    auto &str = msg;
    string s(begin(str), end(str));
    size_t Ctxt_Vec_size = s.size();
    assert(s==msg);
    size_t Ctxt_Byte_size = PAILLIER_BITS_TO_BYTES(ppk->bits) * 2;
    int EncBytesRslt = 0;
    //     char* ctx_final;
    //     char* ctx_final =(char*) malloc ((Ctxt_Vec_size + 1 ) * sizeof(size_t) + Ctxt_Vec_size *  Ctxt_Byte_size);
    EncBytesRslt = PaillerWrapperFunctions::Enc_Byte_By_Byte(s, ppk, ctx_final);

    return 1;
}


int CondEncArbHamSemiHonest::RegDec(paillier_pubkey_t *ppk, char ctx[], paillier_prvkey_t *psk,
                                    size_t _len, string &DecryptedMsg) {
    int ret = 0;
    paillier_plaintext_t *dec;
    vector<paillier_ciphertext_t *> vctx(_len);
    vctx = PaillerWrapperFunctions::Pail_Parse_Ctx_size(ppk, ctx);
    string DecreyptedCharByCha;
    for (int j = 0; j < _len; j++) {
        dec = paillier_dec(NULL, ppk, psk, vctx[j]);
        DecreyptedCharByCha += paillier_plaintext_to_str_NegOrd(dec);
        paillier_freeplaintext(dec);
    }
    DecryptedMsg = CryptoSymWrapperFunctions::Wrapper_unpad(DecreyptedCharByCha);
    //orig_typo TODO: make sure the correct input is added here perviously was orig_typo extract from encoded typo. It should be handled outside this fubnction

    ret = 1;

    return 1;
}


int CondEncArbHamSemiHonest::CondEnc(paillier_pubkey_t *ppk,
                                     char RlPwd_ctx_pull[],
                                     string &typo,
                                     string &payload,
                                     size_t _len,
                                     int threshold,
                                     char *ctx_final,
                                     NTLParamsCondEncHam NTL_params) {
    string seed = CryptoPP::IntToString(time(NULL));
    seed.resize(AES::DEFAULT_KEYLENGTH, ' '); //The defualt key length is 16
    CryptoPP::RandomPool rng;
    rng.IncorporateEntropy((CryptoPP::byte *) seed.data(), strlen(seed.data()));
    int shares = _len;
    const unsigned int CHID_LENGTH = 4;
    bool fail, pass;
    string cipherText, encoded;

    // size_t AECtxSize = 24; // for 128  bit, the output is 192 bit as the size of the EncryptedKey.
    size_t AECtxSize = 2 * KEYSIZE_BYTES + payload.size();

    size_t Ctxt_Vec_size = NTL_params.RRSS.NTL_params._d_pack * _len;
    //1 for the numeber of elemements, 2 for the AE and its lenght, and 2 * _len PaillerCtxt samples
    size_t Ctxt_Byte_size = PAILLIER_BITS_TO_BYTES(ppk->bits) * 2;
    memcpy(ctx_final, &Ctxt_Vec_size, sizeof(size_t));
    memcpy(ctx_final + sizeof(size_t), &AECtxSize, sizeof(size_t));
    memcpy(ctx_final + 2 * sizeof(size_t), &Ctxt_Byte_size, sizeof(size_t));
    /*
* Randomly selecting 16 bytes of secret and derive the AES key from it for Authenticated encryption.
* */

    std::string *EncrypteKey = new std::string[1];
    string b(AES::DEFAULT_KEYLENGTH, 0);
    PRNG.GenerateBlock((CryptoPP::byte *) b.data(), b.size());
    bool kEncCtxtRst;
    kEncCtxtRst = CryptoSymWrapperFunctions::Wrapper_AuthEncrypt(b, payload, EncrypteKey[0]);
    size_t sizeEncKey = EncrypteKey[0].size();
    assert(sizeEncKey == AECtxSize);

    // string AE_ctx =  EncrypteKey[0];
    memcpy(ctx_final + 3 * sizeof(size_t), EncrypteKey[0].c_str(), sizeof(char) * sizeEncKey);
    //Appending the AECtxt to the begining of the ctxt vector.
    //    memcpy(ctx_final + 3 * sizeof(size_t), &EncrypteKey[0],  sizeof(char) * AECtxSize); //Appending the AECtxt to the begining of the ctxt vector.
    // free(EncrypteKey);
    delete[] EncrypteKey;


    size_t ShareSize = SSShareSizeMal;
    string msg = CryptoSymWrapperFunctions::Wrapper_pad(typo, _len);
    //orig_typo TODO: make sure the correct input is added here perviously was orig_typo extract from encoded typo. It should be handled outside this fubnction
    vector<paillier_ciphertext_t *> vctx(_len);
    vector<paillier_ciphertext_t *> vctx1(_len); //Used for encrypting the chars of typo.
    int VecSize;
    string Ctxt_0;

    vctx = PaillerWrapperFunctions::Pail_Parse_Ctx_size(ppk, RlPwd_ctx_pull);
    // Extracting the ctxt of each char of the original meesage using the parsing function desined in Paillier Wrapper functions.

    vctx1 = PaillerWrapperFunctions::Enc_Vec_Typo(msg, ppk);
    string message = CryptoSymWrapperFunctions::Wrapper_pad(typo, _len);
    // pad(typo);  TODO: Double check if we need to make sure that we need pad here?


    std::vector<std::pair<std::string, std::vector<int> > > rrssShares;
    rrssShares = NTL_params.RRSS.ShareGen(_len, threshold, NTL_params.RRSS.NTL_params, b);
    // int lambda_GF256 = 8;
    int lambda_GF_2E = static_cast<int>(NTL_params.RRSS.NTL_params.GF_2E_SS.get_GF2E_degree());

    CryptoPP::vector_member_ptrs<CryptoPP::StringSink> strSinks(shares);



    int out_d;
    int j = 0;

    for (int i = 0; i < _len; i++) {
        paillier_ciphertext_t *Aux_Ctx;
        paillier_ciphertext_t *Aux_Ctx1;
        paillier_plaintext_t *R;
        vector<paillier_plaintext_t *> PailVectPlain(NTL_params.RRSS.NTL_params._d_pack);
        mpz_t PackedEncodedInt;
        mpz_t RandEncodedBigInt;
        mpz_t Aux_mpz;

        mpz_init(PackedEncodedInt);
        mpz_init(RandEncodedBigInt);
        mpz_init(Aux_mpz);
        size_t str_Shr_Legth = rrssShares[i].first.length();
        char *byteCtxt1;
        int rslt = 0;
        // rslt = PackedEncodeMpz_t(strShares[i], 20,  V_Shares_int[i], NTL_params.P_GF_OrigShare,
        //                         lambda_GF256, NTL_params.MaxM,PackedEncodedInt);
        //
        // rslt = RandEnc(RandEncodedBigInt,PackedEncodedInt, NTL_params.N_MaxMEncdo_Floor,NTL_params.MaxM);
        // rslt = PackedMsgEncoding(RandEncodedBigInt, NTL_params.L_PackingParam, PailVectPlain);

        // rslt = PackedEncodeMpz_t_BaseL(strShares[i], 20,  V_Shares_int[i], NTL_params.P_GF_OrigShare,
        // lambda_GF256, PailVectPlain, NTL_params);

        rslt = PackedEncodeMpz_t_packBits(rrssShares[i].first, 20, rrssShares[i].second,
                                          NTL_params.RRSS.NTL_params.P_GF_OrigShare,
                                          lambda_GF_2E, PailVectPlain, NTL_params);
        Aux_Ctx = PaillerWrapperFunctions::Pail_Subtct(ppk, vctx[i], vctx1[i]);
        R = PaillerWrapperFunctions::Rand_Plain_Pail(ppk);
        //I need to describe a function to generate random number in plaintext.
        Aux_Ctx1 = PaillerWrapperFunctions::Pail_Mult_PtxCtx(ppk, Aux_Ctx, R);
        for (int k = 0; k < NTL_params.RRSS.NTL_params._d_pack; k++) {
            // paillier_enc(Aux_Ctx, ppk,PailVectPlain[k], paillier_get_rand_devrandom);
            paillier_ciphertext_t *Ctxt_final;
            Ctxt_final = paillier_create_enc_zero();
            // Ctxt_final = PaillerWrapperFunctions::Pail_Add(ppk, Aux_Ctx1, Aux_Ctx);


            mpz_powm(Aux_mpz, ppk->n_plusone, PailVectPlain[k]->m, ppk->n_squared);
            mpz_mul(Ctxt_final->c, Aux_Ctx1->c, Aux_mpz);
            mpz_mod(Ctxt_final->c, Ctxt_final->c, ppk->n_squared);

            byteCtxt1 = (char *) paillier_ciphertext_to_bytes(Ctxt_Byte_size, Ctxt_final); //TODO: Temproray
            memcpy(ctx_final + 3 * sizeof(size_t) + AECtxSize + j * Ctxt_Byte_size, byteCtxt1, Ctxt_Byte_size);
            j++;
            paillier_freeciphertext(Ctxt_final);
            paillier_freeplaintext(PailVectPlain[k]);
        }

        paillier_freeciphertext(Aux_Ctx);
        paillier_freeciphertext(Aux_Ctx1);
        paillier_freeplaintext(R);
        mpz_clear(PackedEncodedInt);
        mpz_clear(RandEncodedBigInt);
        mpz_clear(Aux_mpz);
    }

    for (int i_f = 0; i_f < _len; i_f++) {
        paillier_freeciphertext(vctx[i_f]);
        paillier_freeciphertext(vctx1[i_f]);
    }


    return 1;
}


tuple<vector<paillier_ciphertext_t *>, string> CondEncArbHamSemiHonest::Pail_Parse_Ctx_size_AECtx(
    paillier_pubkey_t *ppk,
    char *ctx) {
    tuple<vector<paillier_ciphertext_t *>, string> tuple_out;
    size_t size;
    memcpy(&size, ctx, sizeof(size_t));
    size_t AE_Ctx_size;
    memcpy(&AE_Ctx_size, ctx + sizeof(size_t), sizeof(size_t));
    size_t Ctxt_Element_Size;

    string CtxtAEStr;
    //    string* CtxtAEStr = (string*) malloc(AE_Ctx_size);
    //    string* CtxtAEStr = new string("");
    memcpy(&CtxtAEStr, ctx + (size + 1) * sizeof(size_t), AE_Ctx_size);
    //    memcpy(CtxtAEStr, ctx + (size + 1) * sizeof(size_t), AE_Ctx_size);


    //    tuple<vector<paillier_ciphertext_t*>, string> Output_Tuple;
    //    string CTXAE = CtxtAEStr;
    //    string CTXAE = CtxtAEStr[0];
    //    assert(CTXAE == CtxtAEStr);
    //    CtxAE= CtxtAEStr;
    //    size_t* msg_size =(size_t*) malloc(sizeof(size_t));
    //    free(msg_size);


    //    size_t* Elements_Size =(size_t*) malloc(sizeof(size_t));
    //    free(Elements_Size);

    //    size_t* AECtxtSize =(size_t*) malloc(sizeof(size_t));
    //    free((size_t*)AECtxtSize);

    size_t Accum_ptr = 0;
    //    memcpy(&msg_size, ctx , sizeof(size_t));
    //    const size_t size = (size_t) msg_size;
    //    memcpy(&AECtxtSize, ctx  + sizeof(size_t), sizeof(size_t));
    //    const size_t AE_Ctx_size = (size_t) AECtxtSize;

    //    string* CtxAE_Char = (string*) malloc(AE_Ctx_size);

    //    memcpy(CtxAE_Char, ctx + (size + 1) * sizeof(size_t), AE_Ctx_size);
    //    CtxAE = CtxAE_Char[0];
    //    free(CtxAE_Char);
    Accum_ptr = Accum_ptr + AE_Ctx_size;

    vector<paillier_ciphertext_t *> Vect_Ctx(size - 1);


    //    char *byteCtxt1 = new char [PAILLIER_BITS_TO_BYTES(ppk->bits)*2];

    //    delete [] byteCtxt1;
    //    char byteCtxt1[PAILLIER_BITS_TO_BYTES(ppk->bits)*2];
    //    void* byteCtxt1 = malloc(PAILLIER_BITS_TO_BYTES(ppk->bits)*2);
    char *byteCtxt1 = (char *) malloc(PAILLIER_BITS_TO_BYTES(ppk->bits) * 2);

    //    paillier_ciphertext_t* ctxt1;
    for (int i = 0; i < size - 1; i++) {
        //
        memcpy(&Ctxt_Element_Size, ctx + (i + 2) * sizeof(size_t), sizeof(size_t));
        //        size_t Ctxt_Elemnt_Size = (size_t) Elements_Size;
        memcpy(byteCtxt1, ctx + (1 + size) * sizeof(size_t) + Accum_ptr, Ctxt_Element_Size);
        Accum_ptr = Accum_ptr + Ctxt_Element_Size;
        Vect_Ctx[i] = paillier_ciphertext_from_bytes((void *) byteCtxt1, PAILLIER_BITS_TO_BYTES(ppk->bits) * 2);

        //        Vect_Ctx[i]  = ctxt1;
    }

    //    Output_Tuple =  make_tuple(Vect_Ctx,CtxtAEStr);

    //    for(int i =0; i< size-1; i++ )
    //    {
    //        paillier_freeciphertext(Vect_Ctx[i]);
    //    }
    tuple_out = make_tuple(Vect_Ctx, CtxtAEStr);
    //TODO Free the variables
    free(byteCtxt1);
    for (int i = 0; i < size - 1; i++) {
        paillier_freeciphertext(Vect_Ctx[i]);
    }
    //    free(ctx);
    //    free(&CtxtAEStr[0]);
    //    free(CtxtAEStr);
    //    delete CtxtAEStr;
    return tuple_out;
}


int CondEncArbHamSemiHonest::Pail_Parse_Ctx_size_AECtx2(paillier_pubkey_t *ppk,
                                                        char *ctx, string &CtxtAEStr,
                                                        vector<paillier_ciphertext_t *> &Vect_Ctx) {
    //    tuple<vector<paillier_ciphertext_t*>, string> tuple_out;
    size_t size;
    memcpy(&size, ctx, sizeof(size_t));
    size_t AE_Ctx_size;
    memcpy(&AE_Ctx_size, ctx + sizeof(size_t), sizeof(size_t));
    size_t Ctxt_Element_Size;
    memcpy(&Ctxt_Element_Size, ctx + 2 * sizeof(size_t), sizeof(size_t));

    // cout << "PayloadCtxtSize = "<<AE_Ctx_size<<" \n" << endl;
    // cout << Ctxt_Element_Size << endl;


    //    void* vp = malloc (24);
    //    memcpy(vp, ctx + 3 * sizeof(size_t), 24);
    //    std::string *sp = static_cast<std::string*>(vp);
    //    CtxtAEStr = *sp;
    //    delete sp;
    CryptoPP::StringSink ss(CtxtAEStr);
    std::string *CtxtAEStrPre = new std::string[1];
    CtxtAEStrPre[0].resize(AE_Ctx_size);
    memcpy(&CtxtAEStrPre[0][0], ctx + 3 * sizeof(size_t), AE_Ctx_size * sizeof(char)); //Correct
    // memcpy(&CtxtAEStr[0], ctx + 3 * sizeof(size_t),  AE_Ctx_size * sizeof(char)); //Correct
    // cout << "Dec pre Parsing  ...\n";

    ss.Put((const CryptoPP::byte *) CtxtAEStrPre[0].data(), CtxtAEStrPre[0].size(), false);
    //    string A = CtxtAEStrPre[0];
    //    CtxtAEStr = CtxtAEStrPre[0];
    // free(CtxtAEStrPre);
    delete [] CtxtAEStrPre;


    //    byte a[AE_Ctx_size];
    //    memcpy(a, ctx + 3 * sizeof(size_t),  AE_Ctx_size); //Correct
    //    CryptoPP::StringSource ss(a,AE_Ctx_size, true /*pumpAll*/,new StringSink(CtxtAEStr)); // StringSink


    //    string* CtxtAEStrPre = (string*) malloc(sizeof(char) * 24);
    //    memcpy(&CtxtAEStrPre[0], ctx + (3 * sizeof(size_t)), sizeof(char) * 24);


    //    memcpy(&CtxtAEStr, ctx + 3 * sizeof(size_t), sizeof(char) * 24);
    //    CtxtAEStr = CtxtAEStrPre[0];

    //    memcpy(&CtxtAEStr, &CtxtAEStrPre[0], AE_Ctx_size);
    //    CtxtAEStr = CtxtAEStrPre[0];
    //    free(CtxtAEStrPre);
    //    CtxtAEStr = &CtxtAEStrPre[0];
    size_t Accum_ptr = 0;

    Accum_ptr = Accum_ptr + AE_Ctx_size;

    //    vector<paillier_ciphertext_t*> Vect_Ctx(size-1  );

    char *byteCtxt1 = (char *) malloc(PAILLIER_BITS_TO_BYTES(ppk->bits) * 2);
    //    void* byteCtxt = malloc(PAILLIER_BITS_TO_BYTES(ppk->bits)*2);

    for (int i = 0; i < size; i++) {
        //
        //        memcpy(&Ctxt_Element_Size, ctx + (i + 2) * sizeof(size_t), sizeof(size_t));
        //        size_t Ctxt_Elemnt_Size = (size_t) Elements_Size;
        //        memcpy(byteCtxt1,  ctx + (1 + size) * sizeof(size_t) + Accum_ptr, Ctxt_Element_Size);
        //        memcpy(byteCtxt1,  ctx + (1 + size) * sizeof(size_t) + Accum_ptr, Ctxt_Element_Size);
        memcpy(byteCtxt1, ctx + 3 * sizeof(size_t) + Accum_ptr, Ctxt_Element_Size);

        Accum_ptr = Accum_ptr + Ctxt_Element_Size;
        //        memcpy(&Vect_Ctx[i], paillier_ciphertext_from_bytes((void*)byteCtxt1, PAILLIER_BITS_TO_BYTES(ppk->bits)*2), PAILLIER_BITS_TO_BYTES(ppk->bits)*2);
        //        Vect_Ctx[i] = paillier_ciphertext_from_bytes((void*)byteCtxt1, PAILLIER_BITS_TO_BYTES(ppk->bits)*2);
        Vect_Ctx[i] = paillier_ciphertext_from_bytes(byteCtxt1, PAILLIER_BITS_TO_BYTES(ppk->bits) * 2);

        //        Vect_Ctx[i]  = ctxt1;
    }


    //TODO Free the variables
    free(byteCtxt1);
    //    free(ctx);
    //    free(ctx);
    //    free(&CtxtAEStr[0]);
    //    free(CtxtAEStr);
    //    delete CtxtAEStr;
    return 1;
}


int CondEncArbHamSemiHonest::CondDec(paillier_pubkey_t *ppk,
                                     char *typo_ctx,
                                     paillier_prvkey_t *psk,
                                     int threshold,
                                     string &recovered,
                                     size_t _len,
                                     NTLParamsCondEncHam NTL_params) {
    int ret = 0;
    string CtxAE;
    vector<paillier_ciphertext_t *> V_ctx_typo(_len * NTL_params.RRSS.NTL_params._d_pack);
    int pars_rslt = 0;
    // Mat_V_Shares.SetDims(NTL_params.m_V, _len);


    pars_rslt = CondEncArbHamSemiHonest::Pail_Parse_Ctx_size_AECtx2(ppk, typo_ctx, CtxAE, V_ctx_typo);
    vector<string> strShares_Main(_len);

    vector<paillier_plaintext_t *> dec( NTL_params.RRSS.NTL_params._d_pack);
    const unsigned int CHID_LENGTH = 4;


    int rslt = 0;
    // int lambda_GF256 = 8;
    vector<int> V_Shares_int(2 * NTL_params.RRSS.NTL_params.m_V);
    // mat_ZZ_p V_shares_NTL;
    // V_shares_NTL.SetDims(NTL_params.RRSS.NTL_params.m_V, 2 * _len);
    // vector<vector<int> > Mat_shar_Ints(2 * _len, vector<int>(NTL_params.RRSS.NTL_params.m_V));

    int lambda_GF_2E = static_cast<int>(NTL_params.RRSS.NTL_params.GF_2E_SS.get_GF2E_degree());
    mat_GF2E V_shares_GF2E;
    V_shares_GF2E.SetDims(NTL_params.RRSS.NTL_params.m_V, 2 * _len);

    vector<vector<int> > Mat_shar_Ints(
        2 * _len,
        vector<int>(NTL_params.RRSS.NTL_params.m_V)
    );

    int j = 0;
    int i_Mat = 0;
    for (int i = 0; i < _len; i++) {
        mpz_t RandDecodedBigInt;
        mpz_t DecodedBigInt;
        mpz_init(RandDecodedBigInt);
        mpz_init(DecodedBigInt);
        for (int k = 0; k <  NTL_params.RRSS.NTL_params._d_pack; k++) {
            dec[k] = paillier_dec(NULL, ppk, psk, V_ctx_typo[j]);
            j++;
        }
        // rslt = PackedMsgDecoding(RandDecodedBigInt, NTL_params.L_PackingParam, dec);
        // rslt = RandDec(DecodedBigInt, RandDecodedBigInt,NTL_params.MaxM);
        // V_Shares_int = PackedDecodeMpz_t( strShares_Main[i], DecodedBigInt, NTL_params.P_GF_OrigShare, lambda_GF256, 2*NTL_params.m_V);

        // V_Shares_int = PackedDecodeMpz_t_BaseL(strShares_Main[i], dec, NTL_params.P_GF_OrigShare, lambda_GF256, 2*NTL_params.m_V, NTL_params);
        V_Shares_int = PackedDecodeMpz_t_packBits(strShares_Main[i], dec, NTL_params.RRSS.NTL_params.P_GF_OrigShare, lambda_GF_2E,
                                                  2 * NTL_params.RRSS.NTL_params.m_V, NTL_params);
        int count_m = 0;
        for (int c = 0; c < NTL_params.RRSS.NTL_params.m_V; c++) {

            // conv(Mat_shar_Ints[i_Mat][c], V_Shares_int[count_m]);
            // conv(Mat_shar_Ints[i_Mat + 1][c], V_Shares_int[count_m + 1]);

            Mat_shar_Ints[i_Mat][c]     = V_Shares_int[count_m];
            Mat_shar_Ints[i_Mat + 1][c] = V_Shares_int[count_m + 1];

            count_m = count_m + 2;
        }
        i_Mat = i_Mat + 2;


        for (int k_f = 0; k_f <  NTL_params.RRSS.NTL_params._d_pack; k_f++) {
            paillier_freeplaintext(dec[k_f]);
        }
        mpz_clear(RandDecodedBigInt);
        mpz_clear(DecodedBigInt);
    }
    // cout <<"\n";

    for (int ii = 0; ii < 2 * _len; ii++) {
        for (int jj = 0; jj < NTL_params.RRSS.NTL_params.m_V; jj++) {
            V_shares_GF2E[jj][ii] = NTL_params.RRSS.NTL_params.GF_2E_SS.int_to_GF2E(Mat_shar_Ints[ii][jj]);
            // conv(V_shares_NTL[jj][ii], Mat_shar_Ints[ii][jj]);
        }
        // cout <<"\n";
    }


    string recoveredMainSecret;
    vector<int> ValidShareIndx;
    // vec_ZZ_p x;
    vec_GF2E x;
    bool rslt_Indx;

    // x = NTL_params.RRSS.ValidSharIndexFinder(V_shares_NTL, 2 * _len, 2 * threshold, NTL_params.RRSS.NTL_params);

    x = NTL_params.RRSS.ValidSharIndexFinder_GF2E( V_shares_GF2E, 2 * _len, 2 * threshold,NTL_params.RRSS.NTL_params);


    // cout << "the recovered x is: ";
    // for (int h =0; h<2*_len; h++)
    // {
    //     cout << x[h] <<"\t";
    // }
    // cout <<"\n";
    int thshld = 0;
    int indx = 0;
    for (int k = 0; k < 2 * _len; k = k + 2) {
        if ((x[k] == 0 || x[k + 1] == 0) && (thshld < threshold)) {
            indx++;
            continue;
        } else if (thshld < threshold) {
            indx = k/2;
            ValidShareIndx.push_back(indx);
            indx++;
            thshld++;
        }
    }
    if (ValidShareIndx.size() < threshold) {
        cout << "not enough shares\n";
        ret = -1;
    } else {
        string recoverTheMainSecret;

        string plaintext_rcv;

        bool ifCorrectShareVec = false;
        ifCorrectShareVec = CondEncArbHamSemiHonest::RecoverSecretFromValidShares(
            strShares_Main, threshold, ValidShareIndx, recoverTheMainSecret);
        size_t key_size = recoverTheMainSecret.size();
        CryptoPP::StringSink ss_recoveredMainSecret(recoveredMainSecret);
        cout << "";
        auto reMainSecrtSize = ss_recoveredMainSecret.Put((const CryptoPP::byte *) recoverTheMainSecret.data(),
                                                          recoverTheMainSecret.size(), false);

        bool AEReslt = false;
        AEReslt = CryptoSymWrapperFunctions::Wrapper_AuthDecrypt(recoveredMainSecret, CtxAE, plaintext_rcv);

        if (AEReslt) {
            recovered = plaintext_rcv;
            // cout <<"Valid shares founCorrect\n";
            ret = 1;
        } else {
            //        recovered = ""; //(The original agreed i one)
            recovered = "bot";
            // cout <<"Valid shares foundbot\n";
            ret = -1;
        }
    }
    return ret;
}


vector<int> CondEncArbHamSemiHonest::GnereateVectorOfIntegeres(int _len) {
    vector<int> Result;
    for (int i = 0; i < _len; i++) {
        Result.push_back(i);
    }

    return Result;
}

int CondEncArbHamSemiHonest::PackedEncodeMpz_t(string &msg, size_t msgSize, const vec_ZZ_p V,
                                               mpz_t P_GF, int lambda, mpz_t MaxM,
                                               mpz_t &PackedEncodedInt) {
    mpz_t a_i;
    mpz_init(a_i);
    auto &str = msg;
    char a[msgSize];
    for (int j = 0; j < msgSize; ++j) {
        a[j] = str[j];
    }
    mpz_import(PackedEncodedInt, msgSize, -1, 1, 0, 0, a);

    size_t V_size = V.length();

    mpz_t exp_lambda; //Exp_lambad = 2^lambda.  If we use GF256 as our secret sharing, the value is 2^8.
    // mpz_t p_lambda;
    mpz_t V_aux;
    mpz_t P_k_Lambda;
    mpz_init(P_k_Lambda);
    mpz_init(exp_lambda);
    // mpz_init(p_lambda);
    mpz_init(V_aux);

    // int exp_lambda = (msg_l * 8) + 1;
    mpz_ui_pow_ui(exp_lambda, 2, lambda);
    mpz_add_ui(exp_lambda, exp_lambda, 1);
    // Setting as 257 make sure is bigger than all the generated shares. ZZ_p is defined over 257.
    mpz_set(P_k_Lambda, P_GF);
    int k = 0;
    // mpz_import(V_aux, 1, 1, sizeof(V[k]), 0,0, &V[k]);
    mpz_set_ui(V_aux, conv<unsigned long>(V[k]));
    mpz_mul(V_aux, V_aux, P_GF);
    mpz_add(PackedEncodedInt, PackedEncodedInt, V_aux);
    k++;
    while (k < V_size) {
        mpz_mul(P_k_Lambda, P_k_Lambda, exp_lambda);
        // mpz_import(V_aux, 1, 1, sizeof(V[k]), 0,0, &V[k]);
        mpz_set_ui(V_aux, conv<unsigned long>(V[k]));
        mpz_mul(V_aux, V_aux, P_k_Lambda);
        mpz_add(PackedEncodedInt, PackedEncodedInt, V_aux);
        k++;
    }
    assert(k == V_size);

    mpz_clear(V_aux);
    // mpz_clear(p_lambda);
    mpz_clear(exp_lambda);
    mpz_clear(P_k_Lambda);

    return 1;
}


int CondEncArbHamSemiHonest::PackedEncodeMpz_t(string &msg, size_t msgSize, const vector<int> V,
                                               mpz_t P_GF, int lambda, mpz_t MaxM,
                                               mpz_t &PackedEncodedInt) {
    mpz_t a_i;
    mpz_init(a_i);
    auto &str = msg;
    //    unsigned char a[ShareSize];
    char a[msgSize];
    //    unsigned long a[ShareSize];

    for (int j = 0; j < msgSize; ++j) {
        a[j] = str[j];
    }
    // paillier_plaintext_t* m;
    // m = (paillier_plaintext_t*) malloc(sizeof(paillier_plaintext_t));
    // mpz_init(m->m);
    mpz_import(PackedEncodedInt, msgSize, -1, 1, 0, 0, a);


    // // mpz_init(PackedEncodedInt);
    size_t V_size = V.size();
    // size_t msg_l = msg.length();
    // mpz_import(PackedEncodedInt, msg_l, 1, 1, 0, 0, &msg[0]);
    // string str_m;
    // size_t len_str;
    // mpz_export(&str_m[0],&len_str,1,1,0,0, PackedEncodedInt);


    mpz_t exp_lambda; //Exp_lambad = 2^lambda.  If we use GF256 as our secret sharing, the value is 2^8.
    // mpz_t p_lambda;
    mpz_t V_aux;
    mpz_t P_k_Lambda;
    mpz_init(P_k_Lambda);
    mpz_init(exp_lambda);
    // mpz_init(p_lambda);
    mpz_init(V_aux);

    // int exp_lambda = (msg_l * 8) + 1;
    mpz_ui_pow_ui(exp_lambda, 2, lambda);
    mpz_add_ui(exp_lambda, exp_lambda, 1);
    // Setting as 257 make sure is bigger than all the generated shares. ZZ_p is defined over 257.
    // mpz_mul_ui(P_k_Lambda, P_GF,1);
    mpz_set(P_k_Lambda, P_GF);
    // assert(mpz_cmp(P_k_Lambda,P_GF)== 0);
    // mpz_sub_ui(exp_lambda, exp_lambda, 1);
    int k = 0;
    // mpz_import(V_aux, 1, 1, sizeof(V[k]), 0,0, &V[k]);
    mpz_set_ui(V_aux, static_cast<unsigned long>(V[k]));
    mpz_mul(V_aux, V_aux, P_GF);
    mpz_add(PackedEncodedInt, PackedEncodedInt, V_aux);
    k++;
    // while ( mpz_cmp(PackedEncodedInt, MaxM) < 0 && k <V_size)
    while (k < V_size) {
        mpz_mul(P_k_Lambda, P_k_Lambda, exp_lambda);
        // mpz_import(V_aux, 1, 1, sizeof(V[k]), 0,0, &V[k]);
        mpz_set_ui(V_aux, static_cast<unsigned long>(V[k]));
        mpz_mul(V_aux, V_aux, P_k_Lambda);
        mpz_add(PackedEncodedInt, PackedEncodedInt, V_aux);
        k++;
    }
    assert(k == V_size);
    // assert( mpz_cmp(PackedEncodedInt, MaxM) < 0);

    mpz_clear(V_aux);
    // mpz_clear(p_lambda);
    mpz_clear(exp_lambda);
    mpz_clear(P_k_Lambda);

    return 1;
}

vector<int> CondEncArbHamSemiHonest::PackedDecodeMpz_t(string &DecodedMsg, mpz_t PackedEncodedInt, mpz_t P_GF,
                                                       int lambda, int m_V) {
    mpz_t m;
    mpz_init(m);
    size_t msg_size;
    vector<int> VecOut(m_V);
    int V_i;
    size_t V_i_count;

    mpz_t exp_lambda; //Exp_lambad = 2^lambda.  If we use GF256 as our secret sharing, the value is 2^8.
    mpz_t V_aux;
    mpz_init(exp_lambda);
    mpz_init(V_aux);

    mpz_ui_pow_ui(exp_lambda, 2, lambda); //this is actually the value of p.
    mpz_add_ui(exp_lambda, exp_lambda, 1); // mpz_mod(m, PackedEncodedInt, P_GF);

    // char* buf;
    // size_t len_str;
    // buf = (char*) mpz_export(0, &len_str, 1, 1, 0, 0, m);
    // // buf = (char*) realloc(buf, len_str + 1);
    // // buf[len_str] = 0;
    // CryptoPP::StringSink ss_RecoveredSecret(DecodedMsg);
    // int size_share;
    // size_share =  ss_RecoveredSecret.Put((const CryptoPP::byte*)buf,  len_str, false);
    // free(buf);


    int ShareSize = 20;
    void *ByteDec;
    paillier_plaintext_t *p_m;
    p_m = (paillier_plaintext_t *) malloc(sizeof(paillier_plaintext_t));
    mpz_init(p_m->m);
    vector<CryptoPP::byte> ab(ShareSize);
    mpz_mod(p_m->m, PackedEncodedInt, P_GF);
    ByteDec = paillier_plaintext_to_bytes_NegOrd(ShareSize, p_m);
    memcpy(&ab[0], ByteDec, ShareSize);
    string s(ab.begin(), ab.end());
    DecodedMsg = s.substr(0, 20);
    free(ByteDec);


    mpz_sub(V_aux, PackedEncodedInt, p_m->m);
    mpz_tdiv_q(V_aux, V_aux, P_GF);

    int count = 0;
    // while (mpz_cmp(V_aux, exp_lambda)>0 && count <m_V-1)
    for (int c = 0; c < m_V; c++) {
        mpz_mod(m, V_aux, exp_lambda);
        // mpz_export(&V_i, &V_i_count, 1, sizeof(V_i), 0,0, m);
        V_i = (int) mpz_get_ui(m);
        VecOut[c] = V_i;
        // VecOut.push_back(V_i);
        mpz_sub(V_aux, V_aux, m);
        mpz_tdiv_q(V_aux, V_aux, exp_lambda);
        // count++;
    }


    // assert(mpz_cmp_ui(V_aux,  0)== 0);


    // mpz_mod(m, V_aux, exp_lambda);
    // mpz_export(&V_i, &V_i_count, 1, sizeof(V_i), 0,0, m);
    // V_i = (int) mpz_get_ui(m);
    // VecOut.push_back(V_i);
    // VecOut[count] = V_i;
    // assert(count <= 255);
    // assert(VecOut.size() == m_V);
    // gmp_printf("Value of x after mpz_init(): %Zd\n",V_aux);

    mpz_clear(V_aux);
    mpz_clear(exp_lambda);
    mpz_clear(m);
    paillier_freeplaintext(p_m);


    return VecOut;
}

int CondEncArbHamSemiHonest::PackedEncode(string &msg, vector<int> V, mpz_t P_GF, int lambda, mpz_t MaxM
                                          , mpz_t N, int &out_d, vector<paillier_plaintext_t *> &PailVectPlain) {
    size_t V_size = V.size();
    // vector<paillier_plaintext_t*> PailVectPlain;
    paillier_plaintext_t *m_0;
    m_0 = paillier_plaintext_from_str(&msg[0]);
    // m_0 =  StringToPailPlaintext(msg);
    size_t msg_l = msg.length();

    mpz_t exp_lambda; //Exp_lambad = 2^lambda.  If we use GF256 as our secret sharing, the value is 2^8.
    mpz_t p_lambda;
    mpz_t V_aux;
    mpz_t P_k_Lambda;
    mpz_init(P_k_Lambda);
    mpz_init(exp_lambda);
    mpz_init(p_lambda);
    mpz_init(V_aux);

    // int exp_lambda = (msg_l * 8) + 1;
    mpz_ui_pow_ui(exp_lambda, 2, lambda);
    // mpz_sub_ui(exp_lambda, exp_lambda, 1);
    int k = 0;
    int d = 1;
    int i = 0;
    mpz_import(V_aux, 1, 1, sizeof(V[k]), 0, 0, &V[k]);
    mpz_mul(V_aux, V_aux, P_GF);
    mpz_add(m_0->m, m_0->m, V_aux);
    k++;
    while (mpz_cmp(m_0->m, MaxM) < 0 && k <= V_size) {
        mpz_pow_ui(p_lambda, exp_lambda, k);
        mpz_mul(P_k_Lambda, p_lambda, P_GF);
        mpz_import(V_aux, 1, 1, sizeof(V[k]), 0, 0, &V[k]);
        mpz_mul(V_aux, V_aux, P_k_Lambda);
        mpz_add(m_0->m, m_0->m, V_aux);
        k++;
    }

    PailVectPlain.push_back(m_0);

    out_d = 1;

    while (k <= V_size) {
        mpz_import(m_0->m, 1, 1, sizeof(V[k]), 0, 0, &V[k]);
        i++;
        while (mpz_cmp(m_0->m, MaxM) < 0) {
            mpz_pow_ui(p_lambda, exp_lambda, i);
            mpz_import(V_aux, 1, 1, sizeof(V[k]), 0, 0, &V[k]);
            mpz_mul(V_aux, V_aux, p_lambda);
            mpz_add(m_0->m, m_0->m, V_aux);
            k++;
            i++;
        }
        PailVectPlain.push_back(m_0);
        out_d++;
    }
    // paillier_freeplaintext(m_0);
    mpz_clear(V_aux);
    mpz_clear(p_lambda);
    mpz_clear(exp_lambda);
    mpz_clear(P_k_Lambda);

    return 1;
}

vector<int> CondEncArbHamSemiHonest::PackedDecode(string &DecodedMsg, vector<paillier_plaintext_t *> CodedMsg,
                                                  int d, mpz_t P_GF, int lambda, mpz_t N) {
    paillier_plaintext_t *m;
    m = (paillier_plaintext_t *) malloc(sizeof(paillier_plaintext_t));
    mpz_init(m->m);
    size_t msg_size;

    vector<int> VecOut;
    int V_i;
    size_t V_i_count;

    mpz_t exp_lambda; //Exp_lambad = 2^lambda.  If we use GF256 as our secret sharing, the value is 2^8.
    mpz_t exp_lambda_inv;
    mpz_t V_aux;
    mpz_t P_k_Lambda;
    mpz_t P_GF_inv;
    mpz_init(P_k_Lambda);
    mpz_init(exp_lambda);
    mpz_init(exp_lambda_inv);
    mpz_init(V_aux);
    mpz_init(P_GF_inv);
    int r_inv;
    r_inv = mpz_invert(P_GF_inv, P_GF, N);
    mpz_ui_pow_ui(exp_lambda, 2, lambda); //this is aactrually the value of p.
    // mpz_sub_ui(exp_lambda, exp_lambda, 1);
    r_inv = mpz_invert(exp_lambda_inv, exp_lambda, N);

    gmp_printf("Value of x after mpz_init(): %Zd\n", exp_lambda_inv);

    mpz_mod(m->m, CodedMsg[0]->m, P_GF);
    DecodedMsg = paillier_plaintext_to_str(m);

    gmp_printf("Value of x after mpz_init(): %Zd\n", m->m);


    mpz_sub(V_aux, CodedMsg[0]->m, m->m);
    mpz_mul(V_aux, V_aux, P_GF_inv);
    mpz_mod(V_aux, V_aux, N);
    while (mpz_cmp(V_aux, exp_lambda) > 0) {
        mpz_mod(m->m, V_aux, exp_lambda);
        mpz_export(&V_i, &V_i_count, 1, sizeof(V_i), 0, 0, m->m);
        VecOut.push_back(V_i);
        mpz_sub(V_aux, V_aux, m->m);
        mpz_mul(V_aux, V_aux, exp_lambda_inv);
        mpz_mod(V_aux, V_aux, N);
    }

    mpz_export(&V_i, &V_i_count, 1, sizeof(V_i), 0, 0, m->m);
    VecOut.push_back(V_i);

    gmp_printf("Value of x after mpz_init(): %Zd\n", V_aux);


    if (d > 1) {
        for (int i = 1; i < CodedMsg.size(); i++) {
            while (mpz_cmp(CodedMsg[i]->m, exp_lambda) > 0) {
                mpz_mod(V_aux, CodedMsg[i]->m, exp_lambda);
                mpz_export(&V_i, &V_i_count, 1, sizeof(V_i), 0, 0, V_aux);
                VecOut.push_back(V_i);
                mpz_sub(CodedMsg[i]->m, CodedMsg[i]->m, V_aux);
                mpz_mul(CodedMsg[i]->m, CodedMsg[i]->m, exp_lambda_inv);
                mpz_mod(CodedMsg[i]->m, CodedMsg[i]->m, N);
            }

            mpz_export(&V_i, &V_i_count, 1, sizeof(V_i), 0, 0, CodedMsg[i]->m);
            VecOut.push_back(V_i);
        }
    }

    mpz_clear(V_aux);
    mpz_clear(exp_lambda);
    mpz_clear(exp_lambda_inv);
    paillier_freeplaintext(m);
    return VecOut;
}

int CondEncArbHamSemiHonest::PackedMsgEncoding(const mpz_t Msg, mpz_t L_PackingParam,
                                               vector<paillier_plaintext_t *> &PailPackedMsg) {
    int d = PailPackedMsg.size();

    for (int i = 0; i < d; i++) {
        if (!PailPackedMsg[i]) {
            PailPackedMsg[i] = (paillier_plaintext_t *) malloc(sizeof(paillier_plaintext_t));
            mpz_init(PailPackedMsg[i]->m);
        }
    }


    /*
     * Computing Pail_0, Pail_1 s.t. Msg = Pail_0 + Pail_1 * L.
     */
    if (d == 2) {
        // mpz_tdiv_qr (PailPackedMsg[1]->m, PailPackedMsg[0]->m, Msg, L_PackingParam);
        mpz_mod(PailPackedMsg[0]->m, Msg, L_PackingParam);
        mpz_tdiv_q(PailPackedMsg[1]->m, Msg, L_PackingParam);
    } else if (d == 1) {
        mpz_mod(PailPackedMsg[0]->m, Msg, L_PackingParam);
    } else if (d == 3) {
        mpz_mod(PailPackedMsg[0]->m, Msg, L_PackingParam);
        mpz_sub(PailPackedMsg[1]->m, Msg, PailPackedMsg[0]->m);
        mpz_tdiv_q(PailPackedMsg[2]->m, PailPackedMsg[1]->m, L_PackingParam);
        mpz_mod(PailPackedMsg[1]->m, PailPackedMsg[2]->m, L_PackingParam);
        mpz_sub(PailPackedMsg[2]->m, PailPackedMsg[2]->m, PailPackedMsg[1]->m);
        mpz_tdiv_q(PailPackedMsg[2]->m, PailPackedMsg[2]->m, L_PackingParam);
    }


    // assert(mpz_cmp(PailPackedMsg[1]->m, L_PackingParam) < 0);

    return 1;
}

int CondEncArbHamSemiHonest::PackedMsgDecoding(mpz_t &Msg, mpz_t L_PackingParam,
                                               const vector<paillier_plaintext_t *> PailPackedMsg) {
    if (!Msg) {
        mpz_init(Msg);
    }


    if (PailPackedMsg.size() == 2) {
        mpz_mul(Msg, PailPackedMsg[1]->m, L_PackingParam);
        mpz_add(Msg, Msg, PailPackedMsg[0]->m);
    } else if (PailPackedMsg.size() == 1) {
        mpz_mod(Msg, PailPackedMsg[0]->m, L_PackingParam);
    } else if (PailPackedMsg.size() == 3) {
        mpz_mul(Msg, PailPackedMsg[2]->m, L_PackingParam);
        mpz_mul(Msg, Msg, L_PackingParam);
        mpz_mul(PailPackedMsg[1]->m, PailPackedMsg[1]->m, L_PackingParam);
        mpz_add(Msg, Msg, PailPackedMsg[1]->m);
        mpz_add(Msg, Msg, PailPackedMsg[0]->m);
    }

    return 1;
}

paillier_plaintext_t *CondEncArbHamSemiHonest::StringToPailPlaintext(string &msg) {
    paillier_plaintext_t *m;
    m = (paillier_plaintext_t *) malloc(sizeof(paillier_plaintext_t));
    size_t msg_l = msg.length();
    char a[msg_l];

    for (int j = 0; j < msg_l; ++j) {
        a[j] = msg[j];
    }

    mpz_init(m->m);
    mpz_import(m->m, msg_l, 1, 1, 0, 0, a);
    return m;
}

vec_ZZ_p CondEncArbHamSemiHonest::ValidSharIndexFinder(mat_ZZ_p V_shares_NTL, int _len, int threshold,
                                                       NTLParamsCondEncHam NTL_params) {

    vec_ZZ_p x;
    x = gauss_jordan_NTL_p(V_shares_NTL, NTL_params.RRSS.NTL_params.V_pub_ZZ);
    x = gauss_jordan_NTL_p(V_shares_NTL, NTL_params.RRSS.NTL_params.V_pub_ZZ);
    // for(auto f:x)
    // {
    //     cout << f << "\t";
    // }
    // cout << "\n";
    vec_ZZ_p RsltChk;
    RsltChk.SetLength(NTL_params.RRSS.NTL_params.m_V);

    // mul(RsltChk, R_NTL, x);

    for (int j = 0; j < _len; ++j) {
        if (x[j] == 0) continue;
        else {
            x[j] = 1;
        }
    }
    // for(auto f:x)
    // {
    //     cout << f << "\t";
    // }
    // cout << "\n";
    // int tt =0;
    // for(auto f:RsltChk)
    // {
    //     cout << f << " Vs " << NTL_params.V_pub_ZZ[tt] << "\t";
    //     tt++;
    // }
    // cout << "\n";
    return x;
}


int CondEncArbHamSemiHonest::PackedEncodeMpz_t_packBits(string &msg, size_t msgSize, const vector<int> V,
                                                        mpz_t P_GF, int lambda,
                                                        vector<paillier_plaintext_t *> &PailVectPlain,
                                                        NTLParamsCondEncHam NTL_params) {
    mpz_t PackedEncodedInt;
    mpz_init(PackedEncodedInt);
    mpz_t a_i;
    mpz_init(a_i);
    auto &str = msg;
    char a[msgSize];
    for (int j = 0; j < msgSize; ++j) {
        a[j] = str[j];
    }
    mpz_import(PackedEncodedInt, msgSize, -1, 1, 0, 0, a);

    size_t V_size = V.size();

    mpz_t exp_lambda; //Exp_lambad = 2^lambda.  If we use GF256 as our secret sharing, the value is 2^8.
    mpz_t V_aux;
    mpz_t P_k_Lambda;
    mpz_t d_R_i; //RandEncoded d_i mapped to mod N
    mpz_init(P_k_Lambda);
    mpz_init(exp_lambda);
    // mpz_init(p_lambda);
    mpz_init(V_aux);
    mpz_init(d_R_i);

    // int exp_lambda = (msg_l * 8) + 1;
    mpz_ui_pow_ui(exp_lambda, 2, lambda);
    mpz_add_ui(exp_lambda, exp_lambda, 1);
    // Setting as 257 make sure is bigger than all the generated shares. ZZ_p is defined over 257.
    mpz_set(P_k_Lambda, P_GF);
    int k = 0;
    mpz_set_ui(V_aux, static_cast<unsigned long>(V[k]));
    mpz_mul(V_aux, V_aux, P_GF);
    mpz_add(PackedEncodedInt, PackedEncodedInt, V_aux);
    k++;
    while (k < V_size) {
        mpz_mul(P_k_Lambda, P_k_Lambda, exp_lambda);
        // mpz_import(V_aux, 1, 1, sizeof(V[k]), 0,0, &V[k]);
        mpz_set_ui(V_aux, static_cast<unsigned long>(V[k]));
        mpz_mul(V_aux, V_aux, P_k_Lambda);
        mpz_add(PackedEncodedInt, PackedEncodedInt, V_aux);
        k++;
    }
    assert(k == V_size);

    vector<mpz_t> m_d(NTL_params.RRSS.NTL_params._d_pack);
    vector<mpz_class> m_d_class(NTL_params.RRSS.NTL_params._d_pack);

    mpz_class PackedEncodedInt_class(PackedEncodedInt);


    m_d_class = CondEncArbHamSemiHonest::packBits(PackedEncodedInt_class, NTL_params);

    for (int ii = 0; ii < NTL_params.RRSS.NTL_params._d_pack; ii++) {
    }


    for (int i = 0; i < NTL_params.RRSS.NTL_params._d_pack; i++) {
        if (i == NTL_params.RRSS.NTL_params._d_pack - 1) {
            mpz_init(m_d[i]);
            mpz_set(m_d[i], m_d_class[i].get_mpz_t());

            RandEnc(d_R_i, m_d[i], NTL_params.RRSS.NTL_params.RanEnc_Floor_r_LexpPack, NTL_params.RRSS.NTL_params.exp_2_r_LexpPack);
            PailVectPlain[i] = (paillier_plaintext_t *) malloc(sizeof(paillier_plaintext_t));
            mpz_init(PailVectPlain[i]->m);
            mpz_set(PailVectPlain[i]->m, d_R_i);
            mpz_clear(m_d[i]);
        }
        mpz_init(m_d[i]);
        mpz_set(m_d[i], m_d_class[i].get_mpz_t());

        RandEnc(d_R_i, m_d[i], NTL_params.RRSS.NTL_params.RanEnc_Floor_d_LexpPack, NTL_params.RRSS.NTL_params.exp_2_d_LexpPack);
        PailVectPlain[i] = (paillier_plaintext_t *) malloc(sizeof(paillier_plaintext_t));
        mpz_init(PailVectPlain[i]->m);
        mpz_set(PailVectPlain[i]->m, d_R_i);
        mpz_clear(m_d[i]);
    }


    // assert( mpz_cmp(PackedEncodedInt, MaxM) < 0);
    // paillier_freeplaintext(m_pail_aux);
    mpz_clears(PackedEncodedInt, V_aux, exp_lambda, P_k_Lambda, a_i, NULL);
    // mpz_clear(V_aux);
    // // mpz_clear(p_lambda);
    // mpz_clear(exp_lambda);
    // mpz_clear(P_k_Lambda);

    return 1;
}

vector<int> CondEncArbHamSemiHonest::PackedDecodeMpz_t_packBits(string &DecodedMsg,
                                                                vector<paillier_plaintext_t *> PailVectPlain,
                                                                mpz_t P_GF, int lambda, int m_V, NTLParamsCondEncHam NTL_params) {
    mpz_t m;
    mpz_init(m);
    size_t msg_size;
    vector<int> VecOut(m_V);
    int V_i;
    size_t V_i_count;

    mpz_t exp_lambda; //Exp_lambad = 2^lambda.  If we use GF256 as our secret sharing, the value is 2^8.
    mpz_t V_aux;
    mpz_init(exp_lambda);
    mpz_init(V_aux);

    mpz_ui_pow_ui(exp_lambda, 2, lambda); //this is actually the value of p.
    mpz_add_ui(exp_lambda, exp_lambda, 1); // mpz_mod(m, PackedEncodedInt, P_GF);
    size_t d = NTL_params.RRSS.NTL_params._d_pack;

    mpz_t m_d;
    vector<mpz_class> m_d_class(d);
    for (int i = 0; i < d; i++) {
        if (i == d - 1) {
            mpz_init(m_d);
            mpz_mod(m_d, PailVectPlain[i]->m, NTL_params.RRSS.NTL_params.exp_2_r_LexpPack);
            mpz_class m_d_class_i(m_d);
            m_d_class[i] = m_d_class_i;
            mpz_clear(m_d);
        }

        mpz_init(m_d);
        mpz_mod(m_d, PailVectPlain[i]->m, NTL_params.RRSS.NTL_params.exp_2_d_LexpPack);
        mpz_class m_d_class_i(m_d);
        m_d_class[i] = m_d_class_i;
        mpz_clear(m_d);
    }


    mpz_t PackedEncodedInt;
    mpz_init(PackedEncodedInt);


    int rslt = 0;

    mpz_class PackedEncodedInt_class;
    PackedEncodedInt_class = unpackBits(m_d_class, NTL_params);
    mpz_set(PackedEncodedInt, PackedEncodedInt_class.get_mpz_t());


    int ShareSize = 20;
    void *ByteDec;
    paillier_plaintext_t *p_m;
    p_m = (paillier_plaintext_t *) malloc(sizeof(paillier_plaintext_t));
    mpz_init(p_m->m);
    vector<CryptoPP::byte> ab(ShareSize);
    mpz_mod(p_m->m, PackedEncodedInt, P_GF);
    ByteDec = paillier_plaintext_to_bytes_NegOrd(ShareSize, p_m);
    memcpy(&ab[0], ByteDec, ShareSize);
    string s(ab.begin(), ab.end());
    DecodedMsg = s.substr(0, 20);
    free(ByteDec);


    mpz_sub(V_aux, PackedEncodedInt, p_m->m);
    mpz_tdiv_q(V_aux, V_aux, P_GF);

    int count = 0;
    // while (mpz_cmp(V_aux, exp_lambda)>0 && count <m_V-1)
    for (int c = 0; c < m_V; c++) {
        mpz_mod(m, V_aux, exp_lambda);
        // mpz_export(&V_i, &V_i_count, 1, sizeof(V_i), 0,0, m);
        V_i = (int) mpz_get_ui(m);
        VecOut[c] = V_i;
        // VecOut.push_back(V_i);
        mpz_sub(V_aux, V_aux, m);
        mpz_tdiv_q(V_aux, V_aux, exp_lambda);
        // count++;
    }

    mpz_clears(m, PackedEncodedInt, V_aux, exp_lambda, NULL);
    // mpz_clear(V_aux);
    // mpz_clear(exp_lambda);
    // mpz_clear(m);
    paillier_freeplaintext(p_m);

    return VecOut;
}

int CondEncArbHamSemiHonest::PackedEncodeMpz_t_BaseL(string &msg, size_t msgSize, const vector<int> V,
                                                     mpz_t P_GF, int lambda,
                                                     vector<paillier_plaintext_t *> &PailVectPlain,
                                                     NTLParamsCondEncHam NTL_params) {
    mpz_t PackedEncodedInt;
    mpz_init(PackedEncodedInt);
    mpz_t a_i;
    mpz_init(a_i);
    auto &str = msg;
    //    unsigned char a[ShareSize];
    char a[msgSize];
    //    unsigned long a[ShareSize];

    for (int j = 0; j < msgSize; ++j) {
        a[j] = str[j];
    }
    // paillier_plaintext_t* m;
    // m = (paillier_plaintext_t*) malloc(sizeof(paillier_plaintext_t));
    // mpz_init(m->m);
    mpz_import(PackedEncodedInt, msgSize, -1, 1, 0, 0, a);


    // // mpz_init(PackedEncodedInt);
    size_t V_size = V.size();
    // size_t msg_l = msg.length();
    // mpz_import(PackedEncodedInt, msg_l, 1, 1, 0, 0, &msg[0]);
    // string str_m;
    // size_t len_str;
    // mpz_export(&str_m[0],&len_str,1,1,0,0, PackedEncodedInt);


    mpz_t exp_lambda; //Exp_lambad = 2^lambda.  If we use GF256 as our secret sharing, the value is 2^8.
    // mpz_t p_lambda;
    mpz_t V_aux;
    mpz_t P_k_Lambda;
    mpz_t d_R_i; //RandEncoded d_i mapped to mod N
    mpz_init(P_k_Lambda);
    mpz_init(exp_lambda);
    // mpz_init(p_lambda);
    mpz_init(V_aux);
    mpz_init(d_R_i);

    // int exp_lambda = (msg_l * 8) + 1;
    mpz_ui_pow_ui(exp_lambda, 2, lambda);
    mpz_add_ui(exp_lambda, exp_lambda, 1);
    // Setting as 257 make sure is bigger than all the generated shares. ZZ_p is defined over 257.
    // mpz_mul_ui(P_k_Lambda, P_GF,1);
    mpz_set(P_k_Lambda, P_GF);
    // assert(mpz_cmp(P_k_Lambda,P_GF)== 0);
    // mpz_sub_ui(exp_lambda, exp_lambda, 1);
    int k = 0;
    // mpz_import(V_aux, 1, 1, sizeof(V[k]), 0,0, &V[k]);
    mpz_set_ui(V_aux, static_cast<unsigned long>(V[k]));
    mpz_mul(V_aux, V_aux, P_GF);
    mpz_add(PackedEncodedInt, PackedEncodedInt, V_aux);
    k++;
    // while ( mpz_cmp(PackedEncodedInt, MaxM) < 0 && k <V_size)
    while (k < V_size) {
        mpz_mul(P_k_Lambda, P_k_Lambda, exp_lambda);
        // mpz_import(V_aux, 1, 1, sizeof(V[k]), 0,0, &V[k]);
        mpz_set_ui(V_aux, static_cast<unsigned long>(V[k]));
        mpz_mul(V_aux, V_aux, P_k_Lambda);
        mpz_add(PackedEncodedInt, PackedEncodedInt, V_aux);
        k++;
    }
    assert(k == V_size);

    vector<mpz_t> m_d(NTL_params.RRSS.NTL_params._d_pack);

    CondEncArbHamSemiHonest::get_digits_baseL(PackedEncodedInt, NTL_params.RRSS.NTL_params.L_PackingParam, NTL_params.RRSS.NTL_params._d_pack, m_d);
    // We represent m_0 as integers base L

    // paillier_plaintext_t* m_pail_aux;
    // m_pail_aux = (paillier_plaintext_t*) malloc(sizeof(paillier_plaintext_t));
    // mpz_init(m_pail_aux->m);


    for (int i = 0; i < NTL_params.RRSS.NTL_params._d_pack; i++) {
        RandEnc(d_R_i, m_d[i], NTL_params.RRSS.NTL_params.N_MaxMEncdo_Floor, NTL_params.RRSS.NTL_params.L_PackingParam);
        PailVectPlain[i] = (paillier_plaintext_t *) malloc(sizeof(paillier_plaintext_t));
        mpz_init(PailVectPlain[i]->m);
        mpz_set(PailVectPlain[i]->m, d_R_i);
        mpz_clear(m_d[i]);
    }


    // assert( mpz_cmp(PackedEncodedInt, MaxM) < 0);
    // paillier_freeplaintext(m_pail_aux);
    mpz_clears(PackedEncodedInt, V_aux, exp_lambda, P_k_Lambda, a_i, NULL);
    // mpz_clear(V_aux);
    // // mpz_clear(p_lambda);
    // mpz_clear(exp_lambda);
    // mpz_clear(P_k_Lambda);

    return 1;
}

vector<int> CondEncArbHamSemiHonest::PackedDecodeMpz_t_BaseL(string &DecodedMsg,
                                                             vector<paillier_plaintext_t *> PailVectPlain,
                                                             mpz_t P_GF, int lambda, int m_V, NTLParamsCondEncHam NTL_params) {
    mpz_t m;
    mpz_init(m);
    size_t msg_size;
    vector<int> VecOut(m_V);
    int V_i;
    size_t V_i_count;

    mpz_t exp_lambda; //Exp_lambad = 2^lambda.  If we use GF256 as our secret sharing, the value is 2^8.
    mpz_t V_aux;
    mpz_init(exp_lambda);
    mpz_init(V_aux);

    mpz_ui_pow_ui(exp_lambda, 2, lambda); //this is actually the value of p.
    mpz_add_ui(exp_lambda, exp_lambda, 1); // mpz_mod(m, PackedEncodedInt, P_GF);


    vector<mpz_t> m_d(NTL_params.RRSS.NTL_params._d_pack);

    for (int i = 0; i < NTL_params.RRSS.NTL_params._d_pack; i++) {
        mpz_init(m_d[i]);
        mpz_mod(m_d[i], PailVectPlain[i]->m, NTL_params.RRSS.NTL_params.L_PackingParam);
    }
    mpz_t PackedEncodedInt;
    mpz_init(PackedEncodedInt);

    mpz_t L;
    mpz_init(L);
    mpz_set(L, NTL_params.RRSS.NTL_params.L_PackingParam);
    size_t d = NTL_params.RRSS.NTL_params._d_pack;

    int rslt = 0;
    rslt = CondEncArbHamSemiHonest::set_from_digits_baseL(m_d, L, d, PackedEncodedInt);

    int ShareSize = 20;
    void *ByteDec;
    paillier_plaintext_t *p_m;
    p_m = (paillier_plaintext_t *) malloc(sizeof(paillier_plaintext_t));
    mpz_init(p_m->m);
    vector<CryptoPP::byte> ab(ShareSize);
    mpz_mod(p_m->m, PackedEncodedInt, P_GF);
    ByteDec = paillier_plaintext_to_bytes_NegOrd(ShareSize, p_m);
    memcpy(&ab[0], ByteDec, ShareSize);
    string s(ab.begin(), ab.end());
    DecodedMsg = s.substr(0, 20);
    free(ByteDec);


    mpz_sub(V_aux, PackedEncodedInt, p_m->m);
    mpz_tdiv_q(V_aux, V_aux, P_GF);

    int count = 0;
    // while (mpz_cmp(V_aux, exp_lambda)>0 && count <m_V-1)
    for (int c = 0; c < m_V; c++) {
        mpz_mod(m, V_aux, exp_lambda);
        // mpz_export(&V_i, &V_i_count, 1, sizeof(V_i), 0,0, m);
        V_i = (int) mpz_get_ui(m);
        VecOut[c] = V_i;
        // VecOut.push_back(V_i);
        mpz_sub(V_aux, V_aux, m);
        mpz_tdiv_q(V_aux, V_aux, exp_lambda);
        // count++;
    }

    mpz_clears(m, PackedEncodedInt, V_aux, exp_lambda, NULL);
    // mpz_clear(V_aux);
    // mpz_clear(exp_lambda);
    // mpz_clear(m);
    paillier_freeplaintext(p_m);

    return VecOut;
}


void CondEncArbHamSemiHonest::get_digits_baseL(const mpz_t m,
                                               const mpz_t L,
                                               size_t d_pack,
                                               vector<mpz_t> &m_d) /* caller allocates an array of p mpz_t’s */
{
    mpz_t n, q, r;
    mpz_inits(n, q, r, NULL);
    mpz_set(n, m); /* working copy of the input integer  */

    for (size_t i = 0; i < d_pack; ++i) {
        if (mpz_cmp_ui(n, 0) == 0) {
            /* no more significant digits */
            mpz_set_ui(m_d[i], 0);
            continue;
        }

        /* n = q·L + r,   0 ≤ r < L */
        mpz_tdiv_qr(q, r, n, L);

        mpz_set(m_d[i], r); /* d_i  ←  r  (the remainder) */
        mpz_set(n, q); /* n    ←  ⌊n / L⌋ for next digit */
    }

    mpz_clears(n, q, r, NULL);
}

int CondEncArbHamSemiHonest::set_from_digits_baseL(const vector<mpz_t> &m_d, const mpz_t L,
                                                   size_t d_pack,
                                                   mpz_t &m_out) {
    mpz_t powL; /* will hold   L^i  during the loop   */
    mpz_t aux; /* working copy of the input integer  */
    mpz_init_set_ui(powL, 1); /* powL = L^0 = 1                     */
    mpz_set_ui(m_out, 0); /* m_out = 0                          */
    mpz_init(aux);

    for (size_t i = 0; i < d_pack; ++i) {
        /* m_out += m_d[i] * powL */
        // mpz_addmul(m_out, m_d[i], powL);
        mpz_mul(aux, m_d[i], powL);
        mpz_add(m_out, m_out, aux);

        /* powL *= L   for next digit */
        mpz_mul(powL, powL, L);
    }

    mpz_clear(powL);
    mpz_clear(aux);
    return 1;
}


vector<mpz_class> CondEncArbHamSemiHonest::packBits(const mpz_class x, NTLParamsCondEncHam NTL_params) {
    vector<mpz_class> parts;

    mpz_class tmp = x;

    mpz_class mask = (mpz_class(1) << NTL_params.RRSS.NTL_params.d_LexpPack) - 1;

    for (size_t i = 0; i < NTL_params.RRSS.NTL_params._d_pack - 1; i++) {
        parts.push_back(tmp & mask);
        tmp >>= NTL_params.RRSS.NTL_params.d_LexpPack;
    }

    mpz_class mask_r = (mpz_class(1) << NTL_params.RRSS.NTL_params.r_LexpPack) - 1;
    parts.push_back(tmp & mask_r);

    return parts;
}

mpz_class CondEncArbHamSemiHonest::unpackBits(const std::vector<mpz_class> &parts, NTLParamsCondEncHam NTL_params) {
    mpz_class result = 0;

    for (size_t idx = parts.size(); idx-- > 0;) {
        if (idx == parts.size() - 1) {
            result <<= NTL_params.RRSS.NTL_params.r_LexpPack;
            result += parts[idx];
        } else {
            result <<= NTL_params.RRSS.NTL_params.d_LexpPack;
            result += parts[idx];
        }
    }
    return result;
}


int NTLParamsCondEncHam::compute_d_LexpPack(const mpz_t N, int lambda, int k, int _len, int m) {
    size_t log2N = mpz_sizeinbase(N, 2) - 1; // floor(log2(N))
    size_t floor_log2_sqrtN = log2N / 2;

    // Step 2: numerator = 2^(floor_log2_sqrtN - k)
    mpz_t numerator;
    mpz_init(numerator);
    mpz_ui_pow_ui(numerator, 2, floor_log2_sqrtN - k);

    // Step 3: denominator = 4 * 2^lambda * n * (6n+1)
    mpz_t denominator;
    mpz_init(denominator);

    mpz_ui_pow_ui(denominator, 2, lambda + 2); // denominator = 2^lambda * 4
    mpz_mul_ui(denominator, denominator, _len); // *= n
    mpz_mul_ui(denominator, denominator, (2 * m + 1));
    // *= (2 * m+1). In the paper we have m  = 3_len, and we have 6 * _len+1

    // Step 4: T = numerator / denominator
    mpz_t T;
    mpz_init(T);
    mpz_fdiv_q(T, numerator, denominator); // floor division

    // Step 5: find d such that 2^d < T
    size_t bits = mpz_sizeinbase(T, 2); // ~ floor(log2(T)) + 1
    mpz_t power;
    mpz_init(power);
    mpz_ui_pow_ui(power, 2, bits - 1);

    int d;
    if (mpz_cmp(power, T) < 0) {
        d = bits - 1;
    } else {
        d = bits - 2;
    }

    // Clear
    mpz_clear(numerator);
    mpz_clear(denominator);
    mpz_clear(T);
    mpz_clear(power);

    return d;
}

void NTLParamsCondEncHam::compute_floor_div_minus_one(mpz_t c, const mpz_t a, const mpz_t b) {
    mpz_t q;
    mpz_init(q);
    // q = floor(a / b)
    mpz_fdiv_q(q, a, b);
    // c = q - 1
    mpz_sub_ui(c, q, 1);
    mpz_clear(q);
}



int NTLParamsCondEncHam::PackedEncodingInitParams(const mpz_t N, int _len, int m, int lambda, int lambda_1, int k) {
    mpz_init(RRSS.NTL_params.P_GF_OrigShare);
    mpz_init(RRSS.NTL_params.MaxM);
    mpz_init(RRSS.NTL_params.MaxMEncoded);
    mpz_init(RRSS.NTL_params.L_PackingParam);
    mpz_init(RRSS.NTL_params.N_MaxMEncdo_Floor);
    // int  log_P_GF = AES::DEFAULT_KEYLENGTH *8 + 1;
    int log_P_GF = 20 * 8 + 1; //20 is the string length of each share of the secret key.
    // int log_MaxM = 2 * (m + 1) * (8 + 2) + log_P_GF;
    int log_MaxM = 2 * (m + 1) * (lambda_1 + 2) + log_P_GF;
    // 2*m because of 2n shares we decided to use for the security reason
    // int  log_MaxM =  m * (8+2) + log_P_GF; // 2*m because of 2n shares we decided to use for the security reason

    mpz_ui_pow_ui(RRSS.NTL_params.P_GF_OrigShare, 2, log_P_GF);
    mpz_sub_ui(RRSS.NTL_params.P_GF_OrigShare, RRSS.NTL_params.P_GF_OrigShare, 1);
    mpz_ui_pow_ui(RRSS.NTL_params.MaxM, 2, log_MaxM);

    RRSS.NTL_params.d_LexpPack = compute_d_LexpPack(N, lambda, k, _len, m);


    mpz_init(RRSS.NTL_params.exp_2_d_LexpPack);
    mpz_init(RRSS.NTL_params.RanEnc_Floor_d_LexpPack);

    mpz_ui_pow_ui(RRSS.NTL_params.exp_2_d_LexpPack, 2, RRSS.NTL_params.d_LexpPack);
    compute_floor_div_minus_one(RRSS.NTL_params.RanEnc_Floor_d_LexpPack, N, RRSS.NTL_params.exp_2_d_LexpPack);


    size_t c = log_MaxM / RRSS.NTL_params.d_LexpPack;
    size_t r = log_MaxM % RRSS.NTL_params.d_LexpPack;
    if (r == 0) {
        r = RRSS.NTL_params.d_LexpPack;
        c -= 1;
    }
    RRSS.NTL_params._d_pack = c + 1;
    RRSS.NTL_params.r_LexpPack = r;

    mpz_init(RRSS.NTL_params.exp_2_r_LexpPack);
    mpz_init(RRSS.NTL_params.RanEnc_Floor_r_LexpPack);

    mpz_ui_pow_ui(RRSS.NTL_params.exp_2_r_LexpPack, 2, RRSS.NTL_params.r_LexpPack);
    compute_floor_div_minus_one(RRSS.NTL_params.RanEnc_Floor_r_LexpPack, N, RRSS.NTL_params.exp_2_r_LexpPack);

    return RRSS.NTL_params._d_pack;
}

int NTLParamsCondEncHam::PackedEncodingInitParams_old(int _len, int m, int lambda, int n_bits) {
    mpz_init(RRSS.NTL_params.P_GF_OrigShare);
    mpz_init(RRSS.NTL_params.MaxM);
    mpz_init(RRSS.NTL_params.MaxMEncoded);
    mpz_init(RRSS.NTL_params.L_PackingParam);
    mpz_init(RRSS.NTL_params.N_MaxMEncdo_Floor);
    // int  log_P_GF = AES::DEFAULT_KEYLENGTH *8 + 1;
    int log_P_GF = 20 * 8 + 1; //20 is the string length of each share of the secret key.
    int log_MaxM = 2 * (m + 1) * (8 + 2) + log_P_GF;
    // 2*m because of 2n shares we decided to use for the security reason
    // int  log_MaxM =  m * (8+2) + log_P_GF; // 2*m because of 2n shares we decided to use for the security reason

    mpz_ui_pow_ui(RRSS.NTL_params.P_GF_OrigShare, 2, log_P_GF);
    mpz_sub_ui(RRSS.NTL_params.P_GF_OrigShare, RRSS.NTL_params.P_GF_OrigShare, 1);
    mpz_ui_pow_ui(RRSS.NTL_params.MaxM, 2, log_MaxM);


    /*
     *	Set L =  sqrt(N)/(2^\lambda * 10 )
     *	such that L^(_d_Pack - 1) < MaxM <= L^(_d_Pack)
     *
     *	Since N is a number with n_bits, so we can set  \log L = (n_bits/2 - \lambda - \log_2 10)
     *	Then we can compute (_d_Pack -1 ) \log L < \log_2 MaxM < _d_Pack \log L
     *	_d_Pack < 1 + (\log MaxM) / (log L)
     *	_d_Pack > \log MaxM/ \log L
     *
     *	So we can fix d = \floor( 1 + (\log MaxM) / (log L) )
     *
     *	And for L we can set L = 2^( n_bits/2 - \lambda - \log_2 10)
     *
     *
     *
     *
     */
    int epx_L_pack = (n_bits / 2 - lambda - 4);
    mpz_set_ui(RRSS.NTL_params.L_PackingParam, 1);
    mpz_mul_2exp(RRSS.NTL_params.L_PackingParam, RRSS.NTL_params.L_PackingParam, epx_L_pack);
    long d = 1 + (log_MaxM + lambda) / (epx_L_pack);
    RRSS.NTL_params._d_pack = floor(d);
    mpz_ui_pow_ui(RRSS.NTL_params.N_MaxMEncdo_Floor, 2, (n_bits / 2 + lambda - 4));
    // computing \floor( N/L) -1, for our randomized encoding, mapping digit d_i to a random number in [N].
    mpz_sub_ui(RRSS.NTL_params.N_MaxMEncdo_Floor, RRSS.NTL_params.N_MaxMEncdo_Floor, 1);
    int d_int = RRSS.NTL_params._d_pack;
    return d_int;

    ///*********////


    /*
     * The following coding is use
     *
     */
    // mpz_mul_ui(MaxM,MaxM, m);
    // mpz_ui_pow_ui(MaxMEncoded, 2, lambda);
    // mpz_mul(MaxMEncoded, MaxMEncoded, MaxM);
    //
    // /*
    //  *Computing the RandEnc Parameters: y = y' + a (MaxM), a is in the range 1< a < [2^lambda MaxM/N] N -1
    //  * it is roughly 2^lambda
    //  */
    // mpz_ui_pow_ui(N_MaxMEncdo_Floor, 2,lambda);
    // mpz_sub_ui(N_MaxMEncdo_Floor, N_MaxMEncdo_Floor, 1 );
    // /*Computing MaxEncodedM when the max value of m is 512*/
    //
    // long d = (log_MaxM + lambda)/(n_bits);
    // int d_int = static_cast<int>(std::round(d));
    // _d_pack = d_int+1; //indicating the number of elements after encoding to Paillier plaintext.
    // // _d_pack = 2;
    // // assert(_d_pack == 2);
    // if (_d_pack==2)
    // {
    // 	long exp_d =  (log_MaxM + lambda)/2;
    // 	int epx_L_pack = static_cast<int>(std::round(exp_d));
    // 	mpz_set_ui(L_PackingParam, 1);
    // 	mpz_mul_2exp(L_PackingParam, L_PackingParam, epx_L_pack);
    // 	// gmp_printf("The valud L:  %Zd\n",L_PackingParam);
    //
    //
    // 	// mpz_ui_pow_ui(L_PackingParam, 2,epx_L_pack);
    // }else if (_d_pack == 1)
    // {
    // 	long exp_d =  (log_MaxM + lambda);
    // 	int epx_L_pack = static_cast<int>(std::round(exp_d));
    // 	mpz_set_ui(L_PackingParam, 1);
    // 	mpz_mul_2exp(L_PackingParam, L_PackingParam, epx_L_pack);
    // 	// gmp_printf("The valud L:  %Zd\n",L_PackingParam);
    // }else if (_d_pack == 3) {
    // 	long exp_d =  (log_MaxM + lambda)/3;
    // 	int epx_L_pack = static_cast<int>(std::round(exp_d));
    // 	mpz_set_ui(L_PackingParam, 1);
    // 	mpz_mul_2exp(L_PackingParam, L_PackingParam, epx_L_pack);
    // }
    // return d_int;
}
