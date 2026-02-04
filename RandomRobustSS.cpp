//
// Created by hassan on 2/3/26.
//

#include "RandomRobustSS.h"

 int RandomRobustSS::RRSS_Init(int min, int max)
{
    return 1;
}
//    static std::vector<std::string, std::vector<int>> ShareGen(int len, int t, int lambda, std::string Secret);
 std::vector<std::pair<std::string, std::vector<int>>> RandomRobustSS::ShareGen(int len, int t, int lambda, const std::string& secret)
{
    std::vector<std::pair<std::string, std::vector<int>>> a;
    return a;
}

 std::string RandomRobustSS::SecretReconstruction(int len, int t, int lambda, std::vector<std::pair<std::string, std::vector<int>>> const Shares)
{
    return "out";
}