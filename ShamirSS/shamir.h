//https://crypto.stackexchange.com/questions/10701/shamirs-secret-share-over-the-reals

#ifndef SHAMIR
#define SHAMIR

#include <NTL/ZZ_pEX.h>
#include <NTL/LLL.h>
using namespace NTL;

namespace shamir{
/*datatype to store the shares of each participant for each byte of secret.
  point is defined as
  stuct point {
    byte x,
    byte y,
  }
  this structure is used to store the x-coordinate and y-coordinate of polynomial.
}
*/
typedef std::vector<std::vector<GF256::point>> shares;
typedef vec_ZZ_p shares_NTL_p;
typedef vec_ZZ shares_NTL;

  class scheme{
    int n,k;
  public:
    typedef struct point_p{
      ZZ_p x;
      ZZ_p y;
    }point_p;
    scheme(int members,int threshold);
    shares* createShares(std::string secret);
    shares_NTL* createShares_NTL(std::string secret);
    // shares_NTL_p* createShares_NTL_p(ZZ_p secret);
    vec_ZZ_p createShares_NTL_p(ZZ_p secret);

    std::string getSecret(shares* Kshares);
  };
  /*
  this function should be called whenever the shamir library is required to used
  it calls the function gen_multipletable and srand for rand function.
  */
  void init();
}

#endif
