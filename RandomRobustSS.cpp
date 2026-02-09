//
// Created by hassan on 2/3/26.
//

#include "RandomRobustSS.h"

void NTLParams::SetV_pub(int m) {
    int r;
    for (int i=0; i< m; i++)
    {
        r = rand() % 10;
        while (r == 0)
        {
            r =  rand() % 256;
        }
        V_pub.push_back( GF256::byte(r));
        V_pub_ZZ[i] =r;
    }
}



 int RandomRobustSS::RRSS_Init(int min, int max)
{
    return 1;
}
//    static std::vector<std::string, std::vector<int>> ShareGen(int len, int t, int lambda, std::string Secret);
 std::vector<std::pair<std::string, std::vector<int>>> RandomRobustSS::ShareGen(int len, int threshold, NTLParams NTL_params, const std::string& secret)
{
    std::vector<std::pair<std::string, std::vector<int>>> FinalShare(
        len,
        std::make_pair(std::string{}, std::vector<int>(6 * len))
    );

    const unsigned int CHID_LENGTH = 4;
    CryptoPP::RandomPool rng;
    std::string seed = CryptoPP::IntToString(time(NULL));
    seed.resize(AES::DEFAULT_KEYLENGTH, ' '); //The defualt key length is 16
    // std::string b(AES::DEFAULT_KEYLENGTH, 0);
    // PRNG.GenerateBlock((CryptoPP::byte*) b.data(), b.size());
    rng.IncorporateEntropy((CryptoPP::byte*)seed.data(), strlen(seed.data()));



     CryptoPP::ChannelSwitch *channelSwitch;
     channelSwitch = NULLPTR;
     CryptoPP::StringSource source(secret, false,
                                  new CryptoPP::SecretSharing(rng, threshold, len,
                                                              channelSwitch = new CryptoPP::ChannelSwitch, false));


     CryptoPP::vector_member_ptrs<CryptoPP::StringSink> strSinks(len);
     std::string channel;

     // ********** Create Shares for the AES secret information "b".
     for (unsigned int i = 0; i < len; i++) {
         // strSinks[i].reset(new CryptoPP::StringSink(strShares[i]));
         strSinks[i].reset(new CryptoPP::StringSink(FinalShare[i].first));
         channel = CryptoPP::WordToString<word32>(i);
         strSinks[i]->Put((CryptoPP::byte*) channel.data(), CHID_LENGTH);
         channelSwitch->AddRoute(channel, *strSinks[i],DEFAULT_CHANNEL  ); //CryptoPP::BufferedTransformation::NULL_CHANNEL
     }
     source.PumpAll();



     // shamir::init();//to initialise the library functions.Must be used to use the library.
     scheme GF256SS(static_cast<int>(2*len),2*threshold); //For the security reasons we need to 2n shares with threshold 2*threshold
     // shamir::shares* GF256_shares = GF256SS.createShares("0");

     /*We need to generate the shares of V*/

     mat_ZZ_p V_test;

     // vector<vector<int>> V_Shares_int(len,vector<int>(2*_m));
     vector<vector<int>> V_Shares_int(len,vector<int>(2*NTL_params.m_V));

     // V_test.SetDims(NTL_params.m_V, 2*_len);


     int count_m = 0;

     // for (int i=0; i< NTL_params.m_V; i++)
     for (auto r:NTL_params.V_pub_ZZ)
     {
         vec_ZZ_p  GF256_shares_NTL= GF256SS.createShares_NTL_p( r);

         int count_n = 0;
         for (int j = 0; j<len; j++)
         {
             // conv(V_Shares_int[j][count_m], GF256_shares_NTL[count_n]);
             conv(FinalShare[j].second[count_m] , GF256_shares_NTL[count_n]);
             // conv(V_Shares_int[j][count_m+1], GF256_shares_NTL[count_n+ 1]);
             conv(FinalShare[j].second[count_m+1] , GF256_shares_NTL[count_n+1]);

             count_n = count_n+2;
         }


         count_m = count_m +2;
     }


    return FinalShare;
}

 std::string RandomRobustSS::SecretReconstruction(int len, int threshold,  NTLParams NTL_params, std::vector<std::pair<std::string, std::vector<int>>> const Shares)
{

    vector<vector<int>> Mat_shar_Ints(2*len, vector<int>(NTL_params.m_V) );
    std::vector<std::string> strShares;
    strShares.reserve(len);

    std::string secret;

    int rslt = 0;
    int lambda_GF256 =8;
    vector<int> V_Shares_int(2*NTL_params.m_V);
    mat_ZZ_p V_shares_NTL;
    V_shares_NTL.SetDims(NTL_params.m_V, 2*len);

    int j = 0;
    int i_Mat=0;
    for (int i= 0; i< len; i++) {
        int count_m =0;
        for (int c=0; c<NTL_params.m_V; c++)
        {
            conv(Mat_shar_Ints[i_Mat][c], Shares[i].second[count_m]);
            conv(Mat_shar_Ints[i_Mat+1][c], Shares[i].second[count_m+1]);
            count_m =count_m+2;
        }
        i_Mat=i_Mat+2;
        strShares.push_back(Shares[i].first);
    }

    for (int ii =0; ii<2*len; ii++)
    {
        for (int jj = 0; jj<NTL_params.m_V ; jj++)
        {
            conv(V_shares_NTL[jj][ii], Mat_shar_Ints[ii][jj]);
        }
        // cout <<"\n";
    }

    vector<int> ValidShareIndx;
    vec_ZZ_p x;
    bool rslt_Indx;

    x = ValidSharIndexFinder( V_shares_NTL,2*len, 2*threshold,NTL_params);



    int thshld =0;
    int indx = 0;
    int ret = 0;
    for (int k=0; k< 2*len; k=k+2)
    {
        if ((x[k] == 0 || x[k+1] ==0) && (thshld<threshold))
        {
            indx++;
            continue;
        }
        else if (thshld <threshold)
        {

            // cout << "k: (" << k << ")" <<"\t";
            indx = k/2;
            ValidShareIndx.push_back(indx);
            // indx++;
            cout <<indx<<"\t";
            thshld++;
        }


    }
    // cout <<"\n" <<ValidShareIndx.size() <<"\n";
    if (ValidShareIndx.size()< threshold)
    {
        cout <<"not enough shares\n";
        ret =  -1;
    }
    else {

        string recoverTheMainSecret;
        string recoveredMainSecret;
        string plaintext_rcv;

        bool ifCorrectShareVec = false;
        ifCorrectShareVec = RandomRobustSS::RecoverSecretFromValidShares (strShares, threshold, ValidShareIndx, recoverTheMainSecret );
        size_t key_size = recoverTheMainSecret.size();
        CryptoPP::StringSink ss_recoveredMainSecret(recoveredMainSecret);
        cout << "";
        auto reMainSecrtSize = ss_recoveredMainSecret.Put((const CryptoPP::byte*)recoverTheMainSecret.data(),  recoverTheMainSecret.size(), false);
        secret = recoveredMainSecret;

    }

    return secret;

}


vec_ZZ_p RandomRobustSS::ValidSharIndexFinder(mat_ZZ_p V_shares_NTL,int _len, int threshold, NTLParams NTL_params)
{
    mat_ZZ_p R_NTL;
    R_NTL.SetDims(NTL_params.m_V, _len); // Forming the lagrange coefficients to use in Solving system of the equations.


    // for (int i = 0; i< NTL_params.m_V;  i++)
    // {
    //     for (int j = 0; j<_len; j++)
    //     {
    //         ZZ_p K;
    //         K =1;
    //         int counter = 0;
    //         ZZ_p k_byte;
    //         ZZ_p j_byte;
    //         ZZ_p Aux_inv;
    //         for(int k = 1; k< threshold+1; k++)
    //         {
    //             if (k ==j+1) continue;
    //             if (counter < threshold)
    //             {
    //                 k_byte = k;
    //                 j_byte = j+1;
    //                 inv(Aux_inv, k_byte - j_byte);
    //                 K = K * k_byte * Aux_inv;
    //                 counter = counter +1;
    //             }
    //
    //         }
    //         R_NTL[i][j] = V_shares_NTL[i][j] * K;
    //     }
    // }
    vec_ZZ_p x;
    // x = gauss_jordan_NTL_p(R_NTL,NTL_params.V_pub_ZZ);
    x = gauss_jordan_NTL_p(V_shares_NTL,NTL_params.V_pub_ZZ);
    vec_ZZ_p RsltChk;
    RsltChk.SetLength(NTL_params.m_V);

    // mul(RsltChk, R_NTL, x);

    for (int j = 0; j < _len ; ++j) {
        if(x[j] == 0) continue;
        else{
            x[j] =1;
        }
    }
    return x;
}


bool RandomRobustSS::RecoverSecretFromValidShares(
    const std::vector<std::string> &strShares,
    int threshold,
    const std::vector<int> &selected,
    std::string &RecoveredSecret)
{
    try {
        if (threshold <= 0) return false;
        if ((size_t)threshold > selected.size()) return false;

        const unsigned int CHID_LENGTH = 4;

        // SecretRecovery will write into recovered (still in memory),
        // but we will immediately move it into RecoveredSecret and wipe.
        std::string recovered;
        CryptoPP::SecretRecovery recovery(
            threshold,
            new CryptoPP::StringSink(recovered),
            false
        );

        CryptoPP::vector_member_ptrs<CryptoPP::StringSource> strSources(threshold);

        // Use a fixed-size buffer for channel to avoid weird string aliasing
        CryptoPP::SecByteBlock channel(CHID_LENGTH);

        for (int i = 0; i < threshold; i++) {
            int idx = selected[i];
            if (idx < 0 || (size_t)idx >= strShares.size()) return false;

            const std::string &share = strShares[idx];
            if (share.size() < CHID_LENGTH) return false;

            strSources[i].reset(new CryptoPP::StringSource(share, false));

            // Read the 4-byte channel id
            strSources[i]->Pump(CHID_LENGTH);
            strSources[i]->Get(channel, CHID_LENGTH);

            // Attach to channel switch feeding SecretRecovery
            // NOTE: ChannelSwitch expects a "channel name" string;
            // Crypto++ uses channel names as strings, so we construct it.
            std::string channelName(reinterpret_cast<const char*>(channel.data()), CHID_LENGTH);
            strSources[i]->Attach(new CryptoPP::ChannelSwitch(recovery, channelName));
        }

        // Pump all sources in lockstep
        while (strSources[0]->Pump(256)) {
            for (int i = 1; i < threshold; i++)
                strSources[i]->Pump(256);
        }
        for (int i = 0; i < threshold; i++)
            strSources[i]->PumpAll();

        // If recovery failed, recovered might be empty; you can decide policy here.
        // For now: treat empty as failure.
        if (recovered.empty()) return false;

        // Output: avoid extra StringSink copy; assign directly.
        RecoveredSecret.assign(recovered.data(), recovered.size());

        // Best-effort wipe of temporary buffer (not perfect with std::string SSO)
        std::fill(recovered.begin(), recovered.end(), '\0');
        recovered.clear();
        recovered.shrink_to_fit(); // may or may not actually release/wipe

        return true;
    }
    catch (const CryptoPP::Exception &) {
        // Don’t leak detailed error info from here; just fail.
        return false;
    }
    catch (...) {
        return false;
    }
}




// bool RandomRobustSS::RecoverSecretFromValidShares (vector<string> &strShares,
//                                                 int threshold,
//                                                 vector<int> &selected,
//                                                 string &RecoveredSecret)
// {
//
//     string channel;
//     const unsigned int CHID_LENGTH = 4;
//     string recovered;
//     CryptoPP::SecretRecovery recovery(threshold, new CryptoPP::StringSink(recovered), false);
//
//     CryptoPP::vector_member_ptrs<CryptoPP::StringSource> strSources(threshold);
//     channel.resize(CHID_LENGTH);
//
//     for (unsigned int i=0; i<threshold; i++)
//     {
//         strSources[i].reset(new CryptoPP::StringSource(strShares[selected[i]], false));
//         strSources[i]->Pump(CHID_LENGTH);
//         strSources[i]->Get((CryptoPP::byte*)&channel[0], CHID_LENGTH);
//         strSources[i]->Attach(new CryptoPP::ChannelSwitch(recovery, channel));
//     }
//
//     while (strSources[0]->Pump(256))
//     {
//         for (unsigned int i=1; i<threshold; i++)
//             strSources[i]->Pump(256);
//     }
//
//     for (unsigned int i=0; i<threshold; i++)
//         strSources[i]->PumpAll();
//     size_t keySize = recovered.size();
//     size_t KeySizePut;
//     //    memcpy(&RecoveredSecret, &recovered, recovered.size()-1 );
//     //    RecoveredSecret =  recovered;
//
//     CryptoPP::StringSink ss_RecoveredSecret(RecoveredSecret);
//     cout << "";
//     KeySizePut =  ss_RecoveredSecret.Put((const CryptoPP::byte*)recovered.data(),  recovered.size(), false);
//
//     return true;
// }