//
// Created by hassan on 2/3/26.
//

#include "RandomRobustSS.h"

 int RandomRobustSS::RRSS_Init(int min, int max)
{
    return 1;
}
//    static std::vector<std::string, std::vector<int>> ShareGen(int len, int t, int lambda, std::string Secret);
 std::vector<std::pair<std::string, std::vector<int>>> RandomRobustSS::ShareGen(int len, int threshold, int lambda, const std::string& secret)
{

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

     std::vector<std::string> strShares(len);
     CryptoPP::vector_member_ptrs<CryptoPP::StringSink> strSinks(len);
     std::string channel;

     // ********** Create Shares for the AES secret information "b".
     for (unsigned int i = 0; i < len; i++) {
         strSinks[i].reset(new CryptoPP::StringSink(strShares[i]));
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
     vector<vector<int>> V_Shares_int(len,vector<int>(2*3*len));

     // V_test.SetDims(NTL_params.m_V, 2*_len);


     int count_m = 0;

     // for (int i=0; i< NTL_params.m_V; i++)
     for (auto r:NTL_params.V_pub_ZZ)
     {
         vec_ZZ_p  GF256_shares_NTL= GF256SS.createShares_NTL_p( r);

         int count_n = 0;
         for (int j = 0; j<_len; j++)
         {
             conv(V_Shares_int[j][count_m], GF256_shares_NTL[count_n]);
             conv(V_Shares_int[j][count_m+1], GF256_shares_NTL[count_n+ 1]);
             count_n = count_n+2;
         }
         // cout <<"\n";
         count_m = count_m +2;
     }



     std::vector<std::pair<std::string, std::vector<int>>> a;
    return a;
}

 std::string RandomRobustSS::SecretReconstruction(int len, int t, int lambda, std::vector<std::pair<std::string, std::vector<int>>> const Shares)
{
    return "out";
}