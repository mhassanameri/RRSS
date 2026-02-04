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