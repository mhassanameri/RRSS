#include <iostream>
#include <vector>
#include <random>
#include "GF256.h"
#include "shamir.h"



using namespace std;
using namespace GF256;
using namespace shamir;

typedef vector<vector<point>> shares;

scheme::scheme(int members,int threshold){
    n = members;
    k = threshold;
    GF_2E_degree =  smallest_lambda1_for_2n(n); // for example, when n = 128, we have GF_2E_degree = 9, and we get  GF_2^9
    init_GF2E_field(GF_2E_degree);

}

/*Creating of shares shamir (n,k) scheme*/

shares* scheme::createShares(string secret) {
  random_device device;
  default_random_engine generator(device());
  uniform_int_distribution<int> distribution(0,255);
  shares* allShares = new shares(n);
  for(char data:secret) {
  GF256::byte coeff[k];
  coeff[0] = (int)data;
    for(int i=1;i<k;i++) {
        coeff[i] = distribution(generator);
    }
    point temp;
    GF256::byte x,y;
    for(int i=0;i<n;i++) {
        x = (int)(i + 1);
        y = 0;
        for(int j=0;j<k;j++) {
            y = y + (coeff[j] * power(x,j));
        }
        temp.x = x;
        temp.y = y;
        (*allShares)[i].push_back(temp);
      }
    }
    return allShares;
  }

string scheme::getSecret(shares* Kshares) {
    if((*Kshares).size() < k) {
      cout << "Atleast " << k << " shares are required out of " << n << " shares" << endl;
      return "";
    }
    string secret = "";
    int secretSize = (*Kshares)[0].size();
    vector<point> curr_shares(k);
    for(int di=0;di<secretSize;di++) {
        for(int i=0;i<k;i++) {
            curr_shares[i] = (*Kshares)[i][di];
        }
        secret += interpolate(curr_shares);
    }
    return secret;
}


void shamir::init() {
  gen_multipletable();
}


shares_NTL* scheme::createShares_NTL(string secret) {
    // long a = 256;
    // ZZ_p::init(ZZ(a));
    random_device device;
    default_random_engine generator(device());
    // uniform_int_distribution<int> distribution(0,255);
    uniform_int_distribution<int> distribution(0,255);
    shares_NTL* allShares = new shares_NTL;
    allShares->SetLength(n);
    for(char data:secret) {
        ZZ coeff[k];
        coeff[0] = (int)data;
        for(int i=1;i<k;i++) {
            coeff[i] = distribution(generator);
        }
        point temp;
        ZZ x,y;
        for(int i=0;i<n;i++) {
            x = (int)(i + 1);
            y = 0;
            for(int j=0;j<k;j++) {
                y = y + (coeff[j] * power(x,j));
            }
            // temp.x = x;
            // temp.y = y;
            // (*allShares)[i].push_back(y);
            (*allShares)[i] = y;
        }
    }
    return allShares;
}


vec_ZZ_p scheme::createShares_NTL_p(ZZ_p secret) {
    // long a = 257;
    // ZZ_p::init(ZZ(a));
    random_device device;
    default_random_engine generator(device());
    uniform_int_distribution<int> distribution(0,255);
    // shares_NTL_p* allShares = new shares_NTL_p[n];
    vec_ZZ_p allShares;
    allShares.SetLength(n);
    ZZ_p coeff[k];
    coeff[0] = secret;
    for(int i=1;i<k;i++) {
        coeff[i] = distribution(generator);
    }
    point_p temp;
    ZZ_p x,y;
    for(int i=0;i<n;i++) {
        x = i + 1;
        y = 0;
        for(int j=0;j<k;j++) {
            y = y + (coeff[j] * power(x,j));
        }
        temp.x = x;
        temp.y = y;
        // (*allShares)[i].push_back(temp);
        // (*allShares)[i] = y;
        allShares[i] = y;
    }


    return allShares;
}


void scheme::init_GF2E_field(long lambda1) {
    GF2X P;
    BuildIrred(P, lambda1);   // irreducible polynomial of degree lambda1
    GF2E::init(P);
    // field_degree = lambda1;
}



vec_GF2E scheme::createShares_GF2E(const GF2E& secret, long numShares, long threshold) {
    if (numShares > ((1L << GF_2E_degree) - 1)) {
        throw std::runtime_error("Not enough nonzero field elements for Shamir evaluation points.");
    }

    vec_GF2E allShares;
    allShares.SetLength(numShares);

    vec_GF2E coeff;
    coeff.SetLength(threshold);

    coeff[0] = secret;
    for (long j = 1; j < threshold; j++) {
        coeff[j] = random_GF2E();   // uniform in GF(2^lambda1)
    }

    for (long i = 0; i < numShares; i++) {
        GF2E x = ulong_to_GF2E(i + 1); // distinct nonzero elements
        GF2E y;
        clear(y);

        GF2E xpow;
        set(xpow); // xpow = 1

        for (long j = 0; j < threshold; j++) {
            y += coeff[j] * xpow;
            xpow *= x;
        }

        allShares[i] = y;
    }

    return allShares;
}

long scheme::smallest_lambda1_for_2n(long n) {
    if (n <= 0) {
        throw std::invalid_argument("n must be positive");
    }

    unsigned long long target = 2ULL * static_cast<unsigned long long>(n);

    long lambda1 = 0;
    unsigned long long field_size = 1ULL; // 2^lambda1

    while (field_size <= target) {        // need 2^lambda1 > 2n
        if (field_size > (std::numeric_limits<unsigned long long>::max() >> 1)) {
            throw std::overflow_error("n is too large for unsigned long long");
        }

        field_size <<= 1;                 // multiply by 2
        lambda1++;
    }

    return lambda1;
}