//
// Created by hassan on 2/9/26.
//

#include "testCondEncEvalArbHam.h"


int testCondEncArbHamDistSemiHonest(int n_bits, int lambda, int Num_tests, int _len, int _ell)
{

    int lambda_1 = 8; // Small field size.
    // int log_2_len =  static_cast<unsigned int>(std::ceil(std::log2(_len)));
    // int l_NoSmallFactor =  2 * log_2_len + 6 *_len * lambda_1 + 2 * lambda + 2;
    // int eps_NoSmallFactor = 2 * l_NoSmallFactor;

    int m = 3 *_len;
    int Threshold  =_len - _ell;
    mpz_t minPQ;
    int k = 5;

    // std::string filename = "PWDvsTyposDataSet/PWDvsTypoDataSetLessThan"+to_string(_len)+"OrNotHold.txt";
    // std::vector<std::pair<std::string, std::string>> data = LoadPWDvsTypoForTEST(filename);
    // std::string filename = "PWDvsTyposDataSet/PWDvsTypoDataSetLessThan"+to_string(_len)+"HamDisNotHold"+to_string(_ell)+".txt";
    std::string filename = "PWDvsTyposDataSet/PWDvsTypoDataSetLessThan"+to_string(_len)+".txt";
    std::vector<std::pair<std::string, std::string>> data = LoadPWDvsTypoForTEST(filename);
    double duration_KeyGen_Sum = 0;
    double duration_CondEnc_HD_Sum = 0;
    double duration_Enc_HD_Sum = 0;
    double duration_CondDec_HD_Sum = 0;
    double CondCtxSize_HD_Sum = 0;
    double ProofSize_Sum = 0;
    size_t TradCtxSize;


    string msg;
    string payload;
    string typo;

    size_t NumOfErrs = _len - Threshold + 1;



    string File  = "dataHamArbSemi.dat";

    std::ofstream dataFile(File, std::ios_base::app | std::ios_base::out);
    // dataFile << "n\td\tKeyGen\tEnc\tCondEnc\tCondDec\tCtxtSizeReg\tCtxtCodEnc\tProofSize\n";  //it considers the Proof of Good Key as part of the ctx as KB




    for(int T = 0; T< Num_tests; T++)
    {

        msg = data[T].first;
        typo = data[T].second;
         // msg = "Tes111";
         // typo = "Tes001";
        payload = CryptoSymWrapperFunctions::Wrapper_pad( typo, _len);
        // cout << msg.size() << "\t" <<typo.size() << "\n";
        // cout << msg << "\t" <<typo << "\n";

        payload =  typo;


        auto start_KeyGen = high_resolution_clock::now();
        // std::unique_ptr<CondEncHamDistMal> CondEncHamMal = std::make_unique<CondEncHamDistMal>(
        //         _len, m, n_lambda_, lambda, l_NoSmallFactor, eps_NoSmallFactor, WithNIZK
        //     );
        CondEncArbHamSemiHonest* CondEncArbHamSemi = new CondEncArbHamSemiHonest(_len, Threshold, lambda, n_bits,  NULL, k);

        auto stop_KeyGen = high_resolution_clock::now();

        auto duration_KeyGen = duration_cast<milliseconds>(stop_KeyGen - start_KeyGen);


        size_t PailCtxtSize =  PAILLIER_BITS_TO_BYTES(CondEncArbHamSemi->CondKeyPair._ppk->bits)*2;

        TradCtxSize = 2 * sizeof(size_t) + _len *  PailCtxtSize;


        size_t AE_CtxtSize = 2 * KEYSIZE_BYTES + _len;
        char HD_Char_ORigCTx [TradCtxSize];
        size_t CondCtxSize = 3 * sizeof(size_t) + AE_CtxtSize + (CondEncArbHamSemi->NTL_ArbHam_params.RRSS.NTL_params._d_pack * _len *  PailCtxtSize);
        char HD_ctx_typo_Bytes[CondCtxSize];
        CondCtxSize_HD_Sum = CondCtxSize_HD_Sum + CondCtxSize;

        string msg_pad  = CryptoSymWrapperFunctions::Wrapper_pad(msg, _len);
        string pad_typo = CryptoSymWrapperFunctions::Wrapper_pad(typo, _len);

        /*  Running the traditional Encryption of chosen*/
        auto start_Enc_HD = high_resolution_clock::now();
        int tradEncRslt =0;
        tradEncRslt = CondEncArbHamSemi->Enc(CondEncArbHamSemi->CondKeyPair._ppk, msg_pad, HD_Char_ORigCTx);
        auto stop_Enc_HD = high_resolution_clock::now();
        auto duration_Enc_HD = duration_cast<milliseconds>(stop_Enc_HD - start_Enc_HD);
       // auto ProofSize =  CondEncArbHamSemi->CondKeyPair.nizk_good_key._ProofSizGoodKey;

        ProofSize_Sum =  ProofSize_Sum;
        /*Running the Conditional Encryption*/
        auto start_CondEnc_HD = high_resolution_clock::now();

        auto ctx_final = CondEncArbHamSemi->CondEnc(CondEncArbHamSemi->CondKeyPair._ppk,
            HD_Char_ORigCTx, typo, payload,_len, Threshold, HD_ctx_typo_Bytes,
            CondEncArbHamSemi->NTL_ArbHam_params);
        auto stop_CondEnc_HD = high_resolution_clock::now();
        auto duration_CondEnc_HD = duration_cast<milliseconds>(stop_CondEnc_HD - start_CondEnc_HD);
        // cout <<"successful Cond encryption\n";
        /*Running the Conditional Decryption */
        auto start_CondDec_HD = high_resolution_clock::now();
        string recovered_hdBytes;
        int CondDecOut  = 0;
//        CondDecOut = HamDistAtmostT::CondDec(CondEncHamMal->CondKeyPairMal._ppk, HD_ctx_typo_Bytes, CondEncHamMal->CondKeyPairMal._psk, Threshold, recovered_hdBytes, _len);
         CondDecOut = CondEncArbHamSemi->CondDec(CondEncArbHamSemi->CondKeyPair._ppk,
             HD_ctx_typo_Bytes, CondEncArbHamSemi->CondKeyPair._psk,
             Threshold, recovered_hdBytes, _len, CondEncArbHamSemi->NTL_ArbHam_params);
       // assert(CondDecOut == -1);



        auto stop_CondDec_HD = high_resolution_clock::now();
        auto duration_CondDec_HD = duration_cast<milliseconds>(stop_CondDec_HD - start_CondDec_HD);


        duration_Enc_HD_Sum =  duration_Enc_HD_Sum + duration_Enc_HD.count();
        duration_CondEnc_HD_Sum =  duration_CondEnc_HD_Sum + duration_CondEnc_HD.count();
        duration_CondDec_HD_Sum =  duration_CondDec_HD_Sum + duration_CondDec_HD.count();
        duration_KeyGen_Sum = duration_KeyGen_Sum + duration_KeyGen.count();
        cout <<T << ",";

        paillier_freepubkey(CondEncArbHamSemi->CondKeyPair._ppk);
        paillier_freeprvkey(CondEncArbHamSemi->CondKeyPair._psk);


    }



    // string File1 = "OPT_HDdataL_T" + std::to_string(NumOfErrs -1) + ".dat";
    // string File2 = "OPT_HDdataL" + std::to_string(_len) + "_T.dat";

    // string File1 = "HDdataL_T" + std::to_string(NumOfErrs -1) + ".dat";
    // string File2 = "HDdataL" + std::to_string(_len) + "_T.dat";

    // string File1 = "OPT_HoldingHDdataL_T" + std::to_string(NumOfErrs -1) + ".dat";
    // string File2 = "OPT_HoldingHDdataL" + std::to_string(_len) + "_T.dat";

    // string File1 = "HoldingHDdataL_T" + std::to_string(NumOfErrs -1) + ".dat";
    // string File2 = "HoldingHDdataL" + std::to_string(_len) + "_T.dat";
    // string File1 = "OPTJustHam2HoldingHDdataL_T" + std::to_string(NumOfErrs -1) + ".dat";
    // string File2 = "OPTJustHam2HoldingHDdataL" + std::to_string(_len) + "_T.dat";


    // std::ofstream HDdataL(File1, std::ios_base::app | std::ios_base::out);
    // std::ofstream HDdataT(File2, std::ios_base::app | std::ios_base::out);
    // std::ofstream dataFile("dataMal.dat", std::ios_base::app | std::ios_base::out);
    // dataFile << "n\td\tKeyGen\tEnc\tCondEnc\tCondDec\tCtxtSizeReg\tCtxtCodEnc\tPkSize\n";


    size_t MaxDist  = _len - Threshold;


    dataFile << _len << "\t" << _ell << "\t" <<
        duration_KeyGen_Sum / (1000 * Num_tests) << "\t" <<
        duration_Enc_HD_Sum / (1000 * Num_tests) << "\t" <<
        duration_CondEnc_HD_Sum / (1000 * Num_tests) << "\t" <<
        duration_CondDec_HD_Sum / (1000 * Num_tests) << "\t" <<
        TradCtxSize/ (1024) + ProofSize_Sum/Num_tests << "\t" <<
        CondCtxSize_HD_Sum /(1024* Num_tests)  << "\t" << //it considers the Proof of Good Key as part of the ctx
        ProofSize_Sum/Num_tests<< "\n";
    dataFile.close();

    return 1;
}



double testCondEncAtmostTHamDistSemihonest(int n_bits, int lambda, int Num_tests, int _len, int MaxHam, std::string& FileName)
{
    size_t Threshold = _len - MaxHam;
    int k=5;

    string File  = "dataHamAtmostTSemi.dat";

    std::ofstream HamdDisSemi(FileName, std::ios_base::app | std::ios_base::out);
    // HamdDisSemi << "n\td\tKeyGen\tEnc\tCondEnc\tCondDec\tCtxtSizeReg\tCtxtCodEnc\tProofSize\n";  //it considers the Proof of Good Key as part of the ctx as KB

    // std::string filename = "PWDvsTyposDataSet/PWDvsTypoDataSetLessThan"+to_string(_len)+"OrNotHold.txt";
    // std::vector<std::pair<std::string, std::string>> data = LoadPWDvsTypoForTEST(filename);
    // std::string filename = "PWDvsTyposDataSet/PWDvsTypoDataSetLessThan"+to_string(_len)+"HamDisNotHold"+to_string(MaxHam)+".txt";
    std::string filename = "PWDvsTyposDataSet/PWDvsTypoDataSetLessThan"+to_string(_len)+".txt";
    std::vector<std::pair<std::string, std::string>> data = LoadPWDvsTypoForTEST(filename);

    // PkCrypto pkobj(n_lambda, l_NoSmallFactor, eps_NoSmallFactor);
    // mpz_t _MinPK;
    // mpz_init(_MinPK);
    // mpz_ui_pow_ui(_MinPK, 2, l_NoSmallFactor);
    // pkobj.initialize(n_lambda_, WithNIZK, _MinPK);

    double duration_KeyGen_HD_Sum = 0;
    double duration_CondEnc_HD_Sum = 0;
    double duration_Enc_HD_Sum = 0;
    double duration_CondDec_HD_Sum = 0;
    double CondCtxSize_HD_Sum = 0;
    double ProofSize_Sum = 0;




    string msg;
    string payload;
    string typo;
    size_t TradCtxSize;
    size_t PailCtxtSize;

    size_t NumOfErrs = _len - Threshold + 1;

    for(int T = 0; T< Num_tests; T++)
    {
        auto start_KeyGen_HD = high_resolution_clock::now();
        // HamDistAtmostTMal* CondEncHamAtMostTMal = new HamDistAtmostTMal(n_lambda,  l_NoSmallFactor,  eps_NoSmallFactor, WithNIZK);
        // auto CondEncHamAtMostSemiHon= std::make_unique<CondEncAtMostTHamSemiHonest>(_len, Threshold, lambda, n_bits, NULL, k);
        CondEncAtMostTHamSemiHonest* CondEncHamAtMostSemiHon = new CondEncAtMostTHamSemiHonest(_len, Threshold, lambda, n_bits, NULL, k);

        auto stop_KeyGen_HD = high_resolution_clock::now();
        auto duration_KeyGen_HD = duration_cast<milliseconds>(stop_KeyGen_HD - start_KeyGen_HD);
        // auto ProofSize =  CondEncHamAtMostSemiHon->CondKeyPairMal.nizk_good_key._ProofSizGoodKey;
        ProofSize_Sum =  ProofSize_Sum;

        PailCtxtSize =  PAILLIER_BITS_TO_BYTES(CondEncHamAtMostSemiHon->CondKeyPair._ppk->bits)*2;
        TradCtxSize = 2 * sizeof(size_t) + _len *  PailCtxtSize;

        msg = data[T].first;
        typo = data[T].second;
        // msg = "pwdws";
        // typo = "1";
        payload = CryptoSymWrapperFunctions::Wrapper_pad( typo, _len);
        // cout << msg.size() << "\t" <<typo.size() << "\n";
        // cout << msg << "\t" <<typo << "\n";

        payload =  typo;

        size_t AE_CtxtSize = 2 * KEYSIZE_BYTES + _len;
        // char HD_Char_ORigCTx [TradCtxSize];
        char HD_Char_ORigCTx [TradCtxSize];
        size_t CondCtxSize = 3 * sizeof(size_t) + AE_CtxtSize + (_len *  PailCtxtSize);
        // char HD_ctx_typo_Bytes[CondCtxSize];
        char HD_ctx_typo_Bytes[CondCtxSize];
        CondCtxSize_HD_Sum = CondCtxSize_HD_Sum + CondCtxSize;

        string msg_pad  = CryptoSymWrapperFunctions::Wrapper_pad(msg, _len);
        string pad_typo = CryptoSymWrapperFunctions::Wrapper_pad(typo, _len);

        /*  Running the traditional Encryption of chosen*/
        auto start_Enc_HD = high_resolution_clock::now();
        int tradEncRslt =0;
        tradEncRslt = CondEncHamAtMostSemiHon->Enc(CondEncHamAtMostSemiHon->CondKeyPair._ppk, msg_pad, HD_Char_ORigCTx);
        auto stop_Enc_HD = high_resolution_clock::now();
        auto duration_Enc_HD = duration_cast<milliseconds>(stop_Enc_HD - start_Enc_HD);


        /*Running the Conditional Encryption*/
        auto start_CondEnc_HD = high_resolution_clock::now();

        auto ctx_final = CondEncHamAtMostSemiHon->CondEnc(CondEncHamAtMostSemiHon->CondKeyPair._ppk, HD_Char_ORigCTx, typo, payload,_len, Threshold, HD_ctx_typo_Bytes);

        auto stop_CondEnc_HD = high_resolution_clock::now();
        auto duration_CondEnc_HD = duration_cast<milliseconds>(stop_CondEnc_HD - start_CondEnc_HD);
        // cout <<"successful Cond encryption\n";
        /*Running the Conditional Decryption */
        auto start_CondDec_HD = high_resolution_clock::now();
        string recovered_hdBytes;
        int CondDecOut  = 0;
        // CondDecOut = HamDistAtmostT::CondDec(pkobj._ppk, HD_ctx_typo_Bytes, pkobj._psk, Threshold, recovered_hdBytes, _len);
        CondDecOut = CondEncHamAtMostSemiHon->CondDec_SmallGF256(CondEncHamAtMostSemiHon->CondKeyPair._ppk, HD_ctx_typo_Bytes, CondEncHamAtMostSemiHon->CondKeyPair._psk, Threshold, recovered_hdBytes, _len);
        // CondDecOut = HamDistAtmostT::CondDec_Optimized(pkobj._ppk, HD_ctx_typo_Bytes, pkobj._psk, Threshold, recovered_hdBytes, _len, SizeShare, msg.size());
        // CondDecOut = HamDistAtmostT::CondDec_Optimized_UnknownMsgLength(pkobj._ppk, HD_ctx_typo_Bytes, pkobj._psk, Threshold, recovered_hdBytes, _len);
        // CondDecOut = HamDistAtmostT::CondDec_NewOPT(pkobj._ppk, HD_ctx_typo_Bytes, pkobj._psk, Threshold, recovered_hdBytes, _len);
        // assert(CondDecOut == -1);
        // CondDecOut = HamDistAtmostT::CondDec_NonSmallFieldCheck(pkobj._ppk, HD_ctx_typo_Bytes, pkobj._psk, Threshold, recovered_hdBytes, _len, SizeShare);
        // CondDecOut = HamDistAtmostT::CondDec_2dif(pkobj._ppk, HD_ctx_typo_Bytes, pkobj._psk, Threshold, recovered_hdBytes, _len, SizeShare);



        auto stop_CondDec_HD = high_resolution_clock::now();
        auto duration_CondDec_HD = duration_cast<milliseconds>(stop_CondDec_HD - start_CondDec_HD);

        duration_KeyGen_HD_Sum =  duration_KeyGen_HD_Sum + duration_KeyGen_HD.count();
        duration_Enc_HD_Sum =  duration_Enc_HD_Sum + duration_Enc_HD.count();
        duration_CondEnc_HD_Sum =  duration_CondEnc_HD_Sum + duration_CondEnc_HD.count();
        duration_CondDec_HD_Sum =  duration_CondDec_HD_Sum + duration_CondDec_HD.count();
        cout <<T << ",";
        // delete[] HD_Char_ORigCTx;
        // delete[] HD_ctx_typo_Bytes;
        // recovered_hdBytes.clear();

        // paillier_freepubkey(CondEncHamAtMostTMal->CondKeyPairMal._ppk);
        // paillier_freeprvkey(CondEncHamAtMostTMal->CondKeyPairMal._psk);
        // delete CondEncHamAtMostTMal;

    }


    size_t MaxDist  = _len - Threshold;

    HamdDisSemi <<_len << "\t" << MaxDist << "\t" << duration_KeyGen_HD_Sum / (1000 * Num_tests) << "\t"
            << duration_Enc_HD_Sum / (1000* Num_tests) << "\t"
            << duration_CondEnc_HD_Sum / (1000 * Num_tests) << "\t"
            << duration_CondDec_HD_Sum / (1000*Num_tests) << "\t"
            << TradCtxSize /(1024) +   ProofSize_Sum/Num_tests << "\t" // In KB, The proof size is part of the regulart encryption
            << CondCtxSize_HD_Sum /(1024* Num_tests) << "\t" //it considers the Proof of Good Key as part of the ctx as KB
            << ProofSize_Sum/Num_tests<< "\n";

    HamdDisSemi.close();
    return (duration_CondDec_HD_Sum / Num_tests);
    // return 1;
}

int GenerateEsitmatedDateCondDecHamSemi(vector<int>  n_values, std::string DatFileName, vector<double> C) {
    vector<double> a;


    // Define the n and d arrays.
    // vector<int> n_values = {8, 16, 32, 64, 128};
    // vector<int > d_values= {1, 2,3,4, 5, 6, 7};
    // vector<vector<int>> d_values = {{2, 4, 8, 16, 32}, {4, 8, 16, 32, 64}};

    // Open file to write the computed data.
    std::ofstream HamdDisAtMostDEstimate(DatFileName, std::ios_base::app | std::ios_base::out);
    // HamdDisAtMostDEstimate << "n\td\tCondDec\n";  //it considers the Proof of Good Key as part of the ctx as KB

    // ofstream dataFile(DatFileName);
    // if (!dataFile) {
    //     cerr << "Error: Could not create data.txt file." << endl;
    //     return 1;
    // }

    // dataFile << "n\td\tCondDec\n";

    // For each d, compute the value for each n and output to the file.
    // A blank line is added between series so GNUplot treats them as separate curves.
    for (auto n : n_values) {
        int d= 4;
        vector<int> d_values;
        while (d<n)
        {
            d_values.push_back(d);
            d = d*2;
        }
        for (auto d : d_values) {
            a  = TestTimingSSAE(n, d);
            double value = C[0] + n * C[1] + (binom(n,d) * (a[0] + a[1]/255) + a[1] + a[2])/1000000;
            HamdDisAtMostDEstimate << n << " " << d << " " << value << "\n";
        }
        // dataFile << "\n"; // Separate series for GNUplot
    }
    HamdDisAtMostDEstimate.close();

    // Create a GNUplot script to plot the data.
    ofstream gp("plot.gp");
    if (!gp) {
        cerr << "Error: Could not create plot.gp file." << endl;
        return 1;
    }
    gp << "set terminal pdf\n";
    gp << "set output 'chart.pdf'\n";
    gp << "set title 'Plot of [n choose d] * a1 + a2 + a3'\n";
    gp << "set xlabel 'n'\n";
    gp << "set ylabel 'Value'\n";
    gp << "set logscale y\n";  // Enable logarithmic scale on y-axis

    gp << "set grid y\n";       // Add grid lines on the logarithmic y-axis
    gp << "set key outside\n";


    gp << "set style line 1 lt 1 lw 2 dt 2\n";  // Dashed pattern 1
    gp << "set style line 2 lt 1 lw 2 dt 3\n";  // Dashed pattern 2
    gp << "set style line 3 lt 1 lw 2 dt 4\n";  // Dashed pattern 3
    gp << "set style line 4 lt 1 lw 2 dt 5\n";  // Dashed pattern 4
    gp << "set style line 5 lt 1 lw 2 dt 6\n";  // Dashed pattern 5

    // Plot each series (each d value) using GNUplot's index feature.
    gp << "plot ";
    // for (size_t i = 0; i < d_values.size(); ++i) {
    //     if (i > 0)
    //         gp << ", ";
    //     // Plot horizontal step lines connecting nodes for this d_value.
    //     gp << "'data.txt' index " << i
    //        << " using 1:3 with steps ls " << (i+1)
    //        << " title 'd = " << d_values[i] << "', ";
    //     // Overlay the nodes (data points) for clarity.
    //     gp << "'data.txt' index " << i
    //        << " using 1:3 with points ls " << (i+1) << " notitle";
    // }


    gp.close();

    // Call GNUplot to generate the PDF.
    system("gnuplot plot.gp");

    cout << "Chart generated as chart.pdf" << endl;
    return 0;
}

std::vector<std::pair<std::string, std::string>> LoadPWDvsTypoForTEST(const std::string& FileName)
{
    std::vector<std::pair<std::string, std::string>> data;
    std::ifstream file(FileName);
    if (!file)
    {
        std::cerr <<"Unable to open the PWD and Typo pair related file:" << FileName << std::endl;
        return data;
    }
    std::string line;
    const std::string delimiter = "\t\t****\t\t";

    while (std::getline(file, line)) {
        // Find the delimiter position
        size_t pos = line.find(delimiter);
        if (pos != std::string::npos) {
            // Extract the two columns
            std::string column1 = line.substr(0, pos);
            std::string column2 = line.substr(pos + delimiter.length());
            // Store the pair in the vector
            data.emplace_back(column1, column2);
        }
    }


    file.close();
    return data;
}

vector<double> TestTimingSSAE(int _len, int MaxDis)
{


    string seed = CryptoPP::IntToString(time(NULL));
    seed.resize(AES::DEFAULT_KEYLENGTH, ' '); //The defualt key length is 16
    CryptoPP::RandomPool rng;
    rng.IncorporateEntropy((CryptoPP::byte*)seed.data(), strlen(seed.data()));
    int shares = _len;
    // int _len = 32;
    // int MaxDis = 4;
    int threshold = _len - MaxDis;
    const unsigned int CHID_LENGTH = 4;
    bool fail, pass;
    string cipherText, encoded;


    string TestMSG = "GGGGGG";
    std::string* EncrypteKey = new std::string[1];
    string b(AES::DEFAULT_KEYLENGTH, 0);
    PRNG.GenerateBlock((CryptoPP::byte*) b.data(), b.size());
    bool kEncCtxtRst;
    kEncCtxtRst = CryptoSymWrapperFunctions::Wrapper_AuthEncrypt(b, TestMSG, EncrypteKey[0]);
    size_t sizeEncKey = EncrypteKey[0].size();
    bool AEReslt;
    string RecoveredK;
    string rsvMsg;


    auto tik_AEDec = high_resolution_clock::now();
    AEReslt = CryptoSymWrapperFunctions::Wrapper_AuthDecrypt(b, EncrypteKey[0],rsvMsg );
    auto tok_AEDec= high_resolution_clock::now();
    auto AEDecTime = duration_cast<microseconds>(tok_AEDec - tik_AEDec);




    CryptoPP::ChannelSwitch *channelSwitch;
    channelSwitch = NULLPTR;
    CryptoPP::ChannelSwitch *channelSwitch_Zero;
    channelSwitch_Zero = NULLPTR;
    //        CryptoPP::StringSource source(message.c_str(), false, new CryptoPP::SecretSharing(rng, threshold, shares,
    //                                                                                          channelSwitch = new CryptoPP::ChannelSwitch));
    CryptoPP::StringSource source(b, false,
                                  new CryptoPP::SecretSharing(rng, threshold, shares,
                                                              channelSwitch = new CryptoPP::ChannelSwitch, false));

    vector<string> strShares(shares);
    CryptoPP::vector_member_ptrs<CryptoPP::StringSink> strSinks(shares);
    string channel;
    string RecoveredSecret;
    for (unsigned int i = 0; i < shares; i++) {
        strSinks[i].reset(new CryptoPP::StringSink(strShares[i]));
        channel = CryptoPP::WordToString<word32>(i);
        strSinks[i]->Put((CryptoPP::byte*) channel.data(), CHID_LENGTH);
        channelSwitch->AddRoute(channel, *strSinks[i],DEFAULT_CHANNEL  ); //CryptoPP::BufferedTransformation::NULL_CHANNEL
    }
    source.PumpAll();

    vector<int> selected;

    for (int i=0; i<threshold; i++)
    {
        selected.push_back(i);
    }

    auto tik_MainShare = high_resolution_clock::now();

    CondEncAtMostTHamSemiHonest::RecoverSecretFromValidShares (strShares, threshold,
                                                selected,RecoveredSecret);

    auto tok_MainShare = high_resolution_clock::now();
    auto MainShareRecover = duration_cast<microseconds>(tok_MainShare - tik_MainShare);


    scheme GF256SS(static_cast<int>(_len),threshold); //creating a scheme with 8 shares and 5 threshold.
    shamir::shares* GF256_shares = GF256SS.createShares("0");




    auto tik_SmallShare = high_resolution_clock::now();
    string recovered = GF256SS.getSecret(GF256_shares);
    auto tok_SmallShare = high_resolution_clock::now();
    auto SmallShareRecover = duration_cast<microseconds>(tok_SmallShare - tik_SmallShare);


    // cout << "T_MailShareSSRecover(mu Sec): " << MainShareRecover.count() << "\n";
    // cout << "T_AEDecryption (mu sec): " << AEDecTime.count() << "\n";
    // cout << "T_SmallShareRecover(mu sec)" << SmallShareRecover.count() << "\n";

    vector<double> T_Outs(3);
    T_Outs[0] =  SmallShareRecover.count();
    T_Outs[1] =  MainShareRecover.count();
    T_Outs[2] =  AEDecTime.count() ;


    return T_Outs;
}

double binom(int n, int d) {
    if (d > n)
        return 0.0;
    if (d > n - d)
        d = n - d;
    double res = 1.0;
    for (int i = 1; i <= d; ++i)
        res = res * (n - d + i) / i;
    return res;
}

int  a_VectAverageCalc(int Num_tests) {

    std::vector<double> sum_a16(3, 0.0);
    std::vector<double> sum_a32(3, 0.0);

    for (int itr = 0; itr < Num_tests; itr++) {
        std::vector<double> a_16 = TestTimingSSAE(16, 3);  // Replace with actual call
        std::vector<double> a_32 = TestTimingSSAE(32, 3);  // Replace with actual call

        for (int i = 0; i < 3; i++) {
            sum_a16[i] += a_16[i];
            sum_a32[i] += a_32[i];
        }
    }

    // Compute the average
    std::vector<double> avg_a16(3);
    std::vector<double> avg_a32(3);

    for (int i = 0; i < 3; i++) {
        avg_a16[i] = sum_a16[i] / Num_tests;
        avg_a32[i] = sum_a32[i] / Num_tests;
    }

    // Print the averages
    std::cout << "Average a_16: ";
    for (double val : avg_a16) std::cout << val << " ";
    std::cout << "\n";

    std::cout << "Average a_32: ";
    for (double val : avg_a32) std::cout << val << " ";
    std::cout << "\n";

    return 0;
}