//
// Created by hassan on 2/3/26.
//

#ifndef RRSS_RANDOMROBUSTSS_H
#define RRSS_RANDOMROBUSTSS_H

#include <string>
#include <vector>


class RandomRobustSS {
public:
    int _len;
    int _t;
    int lambda_;

    RandomRobustSS(int len, int t, int lambda) {
        _len = len;
        _t = t;
        lambda_ = lambda;
    }

    static int RRSS_Init(int min, int max);
//    static std::vector<std::string, std::vector<int>> ShareGen(int len, int t, int lambda, std::string Secret);
    static std::vector<std::pair<std::string, std::vector<int>>> ShareGen(int len, int t, int lambda, const std::string& secret);

    static std::string SecretReconstruction(int len, int t, int lambda, std::vector<std::pair<std::string, std::vector<int>>> const Shares);
};

#endif //RRSS_RANDOMROBUSTSS_H