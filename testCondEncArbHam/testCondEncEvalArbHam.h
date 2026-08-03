//
// Created by hassan on 2/9/26.
//

#ifndef RRSS_TESTCONDENCEVALARBHAM_H
#define RRSS_TESTCONDENCEVALARBHAM_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <chrono>


#define CATCH_CONFIG_MAIN // This should come **before** including the 'catch.hpp'.
#include "catch.hpp"
// #include "gtest/gtest.h"

#include "../srcCondEncArbHam/CondEncArbHamSemiHonest.h"
#include "../srcCondEncArbHam/CondEncAtMostTHamSemiHonest.h"


using namespace std::chrono;
using std::vector;

std::vector<std::pair<std::string, std::string>> LoadPWDvsTypoForTEST(const std::string& FileName);
// int testCondEncHamDist(int n_lambda, int Num_tests, int _len, int MaxHam);
int testCondEncArbHamDistSemiHonest(int n_lambda, int lambda, int Num_tests, int _len, int MaxHam);
double testCondEncAtmostTHamDistSemihonest(int n_lambda, int lambda, int Num_tests, int _len, int MaxHam, std::string& FileName);

vector<double> TestTimingSSAE(int _len, int MaxDis);
int GenerateEsitmatedDateCondDecHamSemi(vector<int>  n_values, std::string DatFileName, vector<double> C);
double binom(int n, int d);
int  a_VectAverageCalc(int Num_tests);
int plot();





TEST_CASE("HDArbSemiEval") {

    int  n_lambda_ =3072;
    int lambda =128; //the security level.
    int Num_tests = 5;
    double rslt = 0;

    string File  = "dataHamArbSemi.dat";

    std::ofstream dataFile(File, std::ios_base::app | std::ios_base::out);
    dataFile << "n\td\tKeyGen\tEnc\tCondEnc\tCondDec\tCtxtSizeReg\tCtxtCodEnc\tProofSize\n";  //it considers the Proof of Good Key as part of the ctx as KB
    dataFile.close();


    vector<int> len = {8,16, 32, 64, 128};
    // vector<int> len = {8,16,};
    vector<int> _ell = {2, 4, 8, 16, 32, 64};

    for (auto l:len)
    {
        int count = 1;
        for (auto j:_ell)
        {
            if (j< l) {
                rslt  = testCondEncArbHamDistSemiHonest(n_lambda_, lambda, Num_tests, l, j);
            }
            cout << "padded length: " << l << "\t" << "ell: " << j << "\n";
        }

    }


}


TEST_CASE("HDAtmostTSemiEval") {
    int  n_lambda_ =3072;
    int lambda =128; //the security level.
    int Num_tests =20;
    double rslt = 0;

    // vector<int> len = {8, 16, 32, 64};
    // vector<int> len = {128};
    vector<int> len = {32};
    // vector<int> _ell = {1,2,3,4};
    vector<int> _ell = {8};

    string File  = "dataMalAtmostT.dat";

    std::ofstream HamdDisSemi(File, std::ios_base::app | std::ios_base::out);
    HamdDisSemi << "n\td\tKeyGen\tEnc\tCondEnc\tCondDec\tCtxtSizeReg\tCtxtCodEnc\tProofSize\n";  //it considers the Proof of Good Key as part of the ctx as KB
    HamdDisSemi.close();

    for (auto l:len)
    {
        int count = 1;

        for (auto j:_ell)
        {
            rslt =testCondEncAtmostTHamDistSemihonest(n_lambda_, lambda, Num_tests, l, j, File );
            cout << "padded length: " << l << "\t" << "ell: " << j << "\n";

        }
    }

}



/*
 * Aribitrary Hamming distance Simple Functionality Check.
 */
TEST_CASE("HDArbSemiFuncCheck") {
    int  n_bits =3072;
    int lambda =128; //the security level.
    int Num_tests = 20;
    double rslt = 0;
    int len = 64;
    int ell = 4;
    int m = 3 *len;
    int Threshold  =len - ell;
    // mpz_t minPQ;
    int k = 5;

    string msg = "Tes111";
    string typo = "T00000";
    string payload = CryptoSymWrapperFunctions::Wrapper_pad( typo, len);
    cout << msg.size() << "\t" <<typo.size() << "\n";
    cout << msg << "\t" <<typo << "\n";
    size_t AE_CtxtSize = 2 * KEYSIZE_BYTES + len;

    payload =  "This is good Thing to see!";

    CondEncArbHamSemiHonest* CondEncArbHamSemi = new CondEncArbHamSemiHonest(len, Threshold, lambda, n_bits,  NULL, k);

    size_t PailCtxtSize =  PAILLIER_BITS_TO_BYTES(CondEncArbHamSemi->CondKeyPair._ppk->bits)*2;

    size_t TradCtxSize = 2 * sizeof(size_t) + len *  PailCtxtSize;

    char HD_Char_ORigCTx [TradCtxSize];
    size_t CondCtxSize = 3 * sizeof(size_t) + AE_CtxtSize + (CondEncArbHamSemi->NTL_ArbHam_params.RRSS.NTL_params._d_pack * len *  PailCtxtSize);
    char HD_ctx_typo_Bytes[CondCtxSize];

    string msg_pad  = CryptoSymWrapperFunctions::Wrapper_pad(msg, len);
    string pad_typo = CryptoSymWrapperFunctions::Wrapper_pad(typo, len);

    int tradEncRslt =0;
    tradEncRslt = CondEncArbHamSemi->Enc(CondEncArbHamSemi->CondKeyPair._ppk, msg_pad, HD_Char_ORigCTx);
    auto ctx_final = CondEncArbHamSemi->CondEnc(CondEncArbHamSemi->CondKeyPair._ppk,
    HD_Char_ORigCTx, typo, payload, len, Threshold, HD_ctx_typo_Bytes,
        CondEncArbHamSemi->NTL_ArbHam_params);

    int CondDecOut  = 0;
    string recovered_hdBytes;

    CondDecOut = CondEncArbHamSemi->CondDec(CondEncArbHamSemi->CondKeyPair._ppk,
        HD_ctx_typo_Bytes, CondEncArbHamSemi->CondKeyPair._psk,
        Threshold, recovered_hdBytes, len, CondEncArbHamSemi->NTL_ArbHam_params);

    cout << "the recoverd payload is: " << recovered_hdBytes <<"\n";

}


TEST_CASE("HDLSemiEvalEstim")
{
    /*
     * In this Test case, we compare the estimated running time with the actual running time required to execute the
     * conditional decryption of the first construction of Conditional Encryption for Hamming distance suggested in
     * [Ameri,Blocki,CCS24].
     */

    cout << binom(64, 60) << "\n";
    int n_lambda =3072;
    int lambda =128; //the security level.
    int l_NoSmallFactor = 256;
    int eps_NoSmallFactor = 512;
    int _len = 32;
    //    int m = 256;
    int Threshold  =_len - 3;
    int MaxHam = 4;
    bool WithNIZK = true;
    string msg = "Tes11111";
    string typo= "Tes22222";
    int Num_tests = 30;

    int rslt = 0;
    // rslt  = testCondEncHamDist(n_lambda_, Num_tests, _len, MaxHam);
    vector<double> T_Outs(3);

    string File  = "dataSemiAtmostTEstimate.dat";

    std::ofstream HamdDisAtMostDEstimate(File, std::ios_base::app | std::ios_base::out);
    HamdDisAtMostDEstimate << "n\td\tCondDec\n";  //it considers the Proof of Good Key as part of the ctx as KB
    HamdDisAtMostDEstimate.close();


    // ofstream dataFile("dataSemi.txt");
    // string DatFileNameEstimation = "dataSemiEstimation.txt";
    // dataFile << "n\td\tEnc\tCondEnc\tCondDec\tCtxtSizeReg\tCtxtCodEnc\n";
    // vector<int> len = {8, 16, 32, 64, 128};
    vector<int> len = {8, 16,32, 64, 128};
    vector<int> d = {1,2,3,4};

    string FileDataCollector = "DataCollector.dat";

    vector<int> len_EqSys = { 32, 64};
    vector<int> len_Real = {8, 16,32, 64};
    vector<int> d_Real = {1,2,3};
    vector<int> d_EqSys = {4};
    vector<double> t_HD(2);

    t_HD[0]= testCondEncAtmostTHamDistSemihonest(n_lambda, Num_tests,
        lambda, len_EqSys[0], d_EqSys[0], FileDataCollector)/1000; // in sec
    t_HD[1]= testCondEncAtmostTHamDistSemihonest(n_lambda, Num_tests,
        lambda, len_EqSys[1], d_EqSys[0], FileDataCollector)/1000; // in sec
    //
    // dataFile << t_HD[0] << "\t" << t_HD[1] << " (The values of t_HD[0], t_HD[1])\n";


    // t_HD[0] =0.11428;
    // t_HD[1] = 0.24025;

    vector<double> a;
    double value;

    a  = TestTimingSSAE(len_EqSys[0], d_EqSys[0]);
    value = (binom(len_EqSys[0],d_EqSys[0]) * (a[0] + a[1]/256) + a[1]+ a[2])/1000000;
    t_HD[0] = t_HD[0] - value;

    a  = TestTimingSSAE(len_EqSys[1], d_EqSys[0]);
    value = (binom(len_EqSys[1],d_EqSys[0]) * (a[0] + a[1]/256) + a[1]+ a[2])/1000000;
    t_HD[1] = t_HD[1] - value;

    /*
     * We have: t_HD[0] = C_0 + len_EqSys[0] C_1
     *          t_HD[1] = C_0 + len_EqSys[1] C_1
     * wher both C_0, C_1 are unknown (C_1 is associated with the pailler decryption and C_0 is related to other
     * computations).
     */

    vector<double> C(2);
    C[1] = (t_HD[0] - t_HD[1])/(len_EqSys[0] - len_EqSys[1]);
    C[0] = t_HD[0] - len_EqSys[0] * C[1];

    a_VectAverageCalc(Num_tests);



    GenerateEsitmatedDateCondDecHamSemi(len, File, C);

    cout << "Finishing the Estimated runing time, And Starting the Real time evaluation\n";

    // for (auto l:len_Real)
    // {
    //     int count = 1;
    //     for (auto j:d_Real)
    //     {
    //         rslt  = testCondEncHamDistMalAtmostT(n_lambda, lambda,  Num_tests, l, j);
    //         // GenerateEsitmatedDateCondDecHamSemi(len, DatFileNameEstimation, C);
    //     }
    //     cout <<  "End of test for _len_Real corresponding to d_realp[1,2,3,4]: " << l << "\n";
    // }

    // plot();

    //



}

#endif //RRSS_TESTCONDENCEVALARBHAM_H