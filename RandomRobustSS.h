//
// Created by hassan on 2/3/26.
//

#ifndef RRSS_RANDOMROBUSTSS_H
#define RRSS_RANDOMROBUSTSS_H

#include <string>
#include <vector>


class RandomRobustSS {
public:
    int _len,_t, _lambda;
    RandomRobustSS(int len, int t, int lambda) {
        _len = len;
        _t = t;
        _lambda = lambda;
    }

    static int rand_int(int min, int max);
    static std::vector<std::string, std::vector<int>> SahreGen(int len, int t, int lambda, std::string Secret);
    static std::string ShareReconst(int len, int t, int lambda, std::vector<std::string, std::vector<int>> Shares);
};

#endif //RRSS_RANDOMROBUSTSS_H