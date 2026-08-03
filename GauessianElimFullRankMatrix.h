//
// Created by hassan on 2/8/25.
//

#ifndef GAUESSIANELIMFULLRANKMATRIX_H
#define GAUESSIANELIMFULLRANKMATRIX_H


#include <vector>
#include <stdexcept>
#include "ShamirSS/GF256.h"
#include "ShamirSS/shamir.h"
#include <NTL/ZZ_pEX.h>

// For supporing arbitrary GF_2E
#include <NTL/GF2E.h>
#include <NTL/GF2X.h>
#include <NTL/GF2XFactoring.h>
#include <NTL/mat_GF2E.h>
#include <NTL/vec_GF2E.h>

#include <NTL/LLL.h>
using namespace NTL;

// #include "NTL/ntl-11.5.1/include/NTL/mat_poly_ZZ_p.h"
// #include "NTL/ntl-11.5.1/include/NTL/ZZ_p.h"
#include <iostream>

using namespace std;
// using namespace NTL;

int mod_inverse(int a, int p);
bool is_full_rank(const vector<vector<int>>& A);
vector<vector<int>> gauss_jordan_mod(vector<vector<int>> A, vector<vector<int>> R, int p);
// vector<vector<GF256::byte>> gauss_jordan_GF256(vector<vector<GF256::byte>> A, vector<vector<GF256::byte>> R);
vector<GF256::byte> gauss_jordan_GF256(vector<vector<GF256::byte>> A, vector<GF256::byte> R);
vec_ZZ_p gauss_jordan_NTL_p(mat_ZZ_p A, vec_ZZ_p R);
vec_GF2E gauss_jordan_GF2E(const mat_GF2E& A, const vec_GF2E& R);

vector<vector<GF256::byte>> MatMult(vector<vector<GF256::byte>> A, vector<vector<GF256::byte>> B);
vector<GF256::byte> MatMultVec( vector<vector<GF256::byte>> A,  vector<GF256::byte> B);

#endif //GAUESSIANELIMFULLRANKMATRIX_H
