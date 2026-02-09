#include <iostream>
#include "RandomRobustSS.h"

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {
    // TIP Press <shortcut actionId="RenameElement"/> when your caret is at the <b>lang</b> variable name to see how CLion can help you rename it.

    RandomRobustSS RRSS(128, 120, 128);

    string Secret  = "HelloWorld";
    vector<pair<string, vector<int>>> Shares;

    Shares = RRSS.ShareGen(RRSS._len, RRSS._t, RRSS.NTL_params, Secret);

    // For shares 0..5 (NOT 1..5 unless you intentionally skip 0)
    for (size_t j = 3; j < 8; ++j) {
        auto &v = Shares[j].second;
        for (size_t i = 0; i < v.size(); ++i) {
            v[i] = 66;
        }
    }

    string Recovered;

   Recovered =  RRSS.SecretReconstruction(RRSS._len, RRSS._t, RRSS.NTL_params, Shares);

    cout << Recovered;


    return 0;
}