#include <cassert>
#include <iostream>
#include "RandomRobustSS.h"

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.

int main() {
    RandomRobustSS RRSS(40, 38, 8);

    std::string Secret = "HelloWorldHelloWorldHelloWorldHelloWorld";

    assert(Secret.size() == static_cast<size_t>(RRSS._len));

    auto Shares =
        RRSS.ShareGen(RRSS._len, RRSS._t, RRSS.NTL_params, Secret);

    assert(Shares.size() == static_cast<size_t>(RRSS._len));

    // Corrupt shares at indices 3 and 4.
    for (size_t j = 3; j < 5; ++j) {
        auto& v = Shares.at(j).second;

        for (auto& value : v) {
            value = 66;
        }
    }

    // std::cerr << "Before reconstruction:"
    //           << " len=" << RRSS._len
    //           << " threshold=" << RRSS._t
    //           << " shares=" << Shares.size()
    //           << '\n';

    std::string Recovered = RRSS.SecretReconstruction(
        RRSS._len,
        RRSS._t,
        RRSS.NTL_params,
        Shares
    );

    std::cout << "Original:  " << Secret << '\n';
    std::cout << "Recovered: " << Recovered << '\n';
    std::cout << "Match: " << std::boolalpha
              << (Recovered == Secret) << '\n';

    return Recovered == Secret ? 0 : 1;
}