#include <iostream>
#include "RandomRobustSS.h"

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {
    // TIP Press <shortcut actionId="RenameElement"/> when your caret is at the <b>lang</b> variable name to see how CLion can help you rename it.

    RandomRobustSS RRSS(128, 120, 128);

    string Secret  = "HelloWorld";
    vector<pair<string, vector<int>>> Shares;

    Shares = RRSS.ShareGen(RRSS._n, RRSS._t, RRSS.NTL_params, Secret);

    string Recovered;

   Recovered =  RRSS.SecretReconstruction(RRSS._n, RRSS._t, RRSS.NTL_params, Shares);

    cout << Recovered;


    return 0;
}