//
// Created by hassan on 2/8/25.
//

#include "GauessianElimFullRankMatrix.h"


using namespace std;

// Function to calculate the modular inverse of a under modulo p
int mod_inverse(int a, int p) {
    for (int x = 1; x < p; x++) {
        if ((a * x) % p == 0) {
            return x;
        }
    }
    throw std::invalid_argument("Modular inverse does not exist.");
}

// Function to check if the matrix A is full rank
bool is_full_rank(const vector<vector<int>>& A) {
    int m = A.size();
    int n = A[0].size();
    vector<vector<int>> mat = A;
    int rank = 0;

    for (int col = 0; col < n; col++) {
        int row;
        for (row = rank; row < m; row++) {
            if (mat[row][col] != 0) break;
        }
        if (row == m) continue;
        swap(mat[rank], mat[row]);
        int inv = mod_inverse(mat[rank][col], n);
        for (int j = 0; j < n; j++) mat[rank][j] = (mat[rank][j] * inv) % n;
        for (int i = 0; i < m; i++) {
            if (i != rank && mat[i][col] != 0) {
                int factor = mat[i][col];
                for (int j = 0; j < n; j++) {
                    mat[i][j] = (mat[i][j] - factor * mat[rank][j] + n) % n;
                }
            }
        }
        rank++;
    }
    return rank == min(m, n);
}




vector<GF256::byte> gauss_jordan_GF256(vector<vector<GF256::byte>> A, vector<GF256::byte> R)
{
    int m = A.size();
    int n = A[0].size();
    vector<vector<GF256::byte>> augmented(m, vector<GF256::byte>(n + 1));

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            augmented[i][j] = A[i][j];
        }
        augmented[i][n + 1] = R[i];
    }

    for (int i = 0; i < min(m, n); i++) {
        int pivot_row = -1;
        for (int row = i; row < m; row++) {
            if (augmented[row][i] != 0) {
                pivot_row = row;
                break;
            }
        }
        if (pivot_row == -1) continue;
        swap(augmented[i], augmented[pivot_row]);
        GF256::byte pivot = augmented[i][i];
        // int inv_pivot = mod_inverse(pivot, p);
        GF256::byte inv_pivot = GF256::byte(1) /  pivot;
        for (int j = 0; j < n + 1; j++) {
            augmented[i][j] = (augmented[i][j] * inv_pivot);
        }
        for (int row = i + 1; row < m; row++) {
            GF256::byte factor = augmented[row][i];
            for (int j = 0; j < n + 1; j++) {
                augmented[row][j] = (augmented[row][j] - factor * augmented[i][j]) ;
            }
        }
    }

    vector<GF256::byte> X(n, GF256::byte(0));
    for (int i = min(m, n) - 1; i >= 0; i--) {
        if (augmented[i][i].num == 0) continue;
        X[i] = augmented[i][n + 1];

        for (int row = i - 1; row >= 0; row--) {
            augmented[row][n + 1] = (augmented[row][n + 1] - augmented[row][i] * X[i]);
        }
    }
    return X;
}

vec_ZZ_p gauss_jordan_NTL_p(mat_ZZ_p A, vec_ZZ_p R)
{
    long m = A.NumRows();
    long n = A.NumCols();
    long p= 257;
    ZZ_p::init(ZZ(p));
//    vector<vector<GF256::byte>> augmented(m, vector<GF256::byte>(n + 1));
    mat_ZZ_p augmented;
    augmented.SetDims(m, n+1);

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            augmented[i][j] = A[i][j];
        }
        augmented[i][n] = R[i];
    }

    for (int i = 0; i < min(m, n); i++) {
        int pivot_row = -1;
        for (int row = i; row < m; row++) {
            if (augmented[row][i] != 0) {
                pivot_row = row;
                break;
            }
        }
        if (pivot_row == -1) continue;
        swap(augmented[i], augmented[pivot_row]);
        ZZ_p pivot = augmented[i][i];
        // int inv_pivot = mod_inverse(pivot, p);
        ZZ_p inv_pivot;
        inv(inv_pivot, pivot);
//        GF256::byte inv_pivot = GF256::byte(1) /  pivot;
        for (int j = 0; j < n + 1; j++) {
            augmented[i][j] = (augmented[i][j] * inv_pivot);
        }
        for (int row = i + 1; row < m; row++) {
            ZZ_p factor = augmented[row][i];
            for (int j = 0; j < n + 1; j++) {
                augmented[row][j] = (augmented[row][j] - factor * augmented[i][j]) ;
            }
        }

//        for(int i =0; i<m; i++)
//        {
//            for (int j=0; j<n+1; j++ )
//            {
//                cout << augmented[i][j] <<"\t";
//            }
//            cout <<"\n";
//        }
//        cout <<"The augmented matrix " <<endl;


    }

//    for(int i =0; i<m; i++)
//    {
//        for (int j=0; j<n+1; j++ )
//        {
//            cout << augmented[i][j] <<"\t";
//        }
//        cout <<"\n";
//    }
//    cout <<"The augmented matrix " <<endl;


    //    vector<GF256::byte> X(n, GF256::byte(0));
    vec_ZZ_p X;
    X.SetLength(n);
    for (int i = min(m, n) - 1; i >= 0; i--) {
        if (augmented[i][i] == 0) continue;
        X[i] = augmented[i][n];

        for (int row = i - 1; row >= 0; row--) {
            augmented[row][n] = (augmented[row][n] - augmented[row][i] * X[i]);
        }
    }

    /*Alternative Solution*/

    return X;
}


// vec_GF2E gauss_jordan_GF2E(mat_GF2E A, vec_GF2E R)
vec_GF2E gauss_jordan_GF2E(
    const mat_GF2E& A,
    const vec_GF2E& R
)
{
    long m = A.NumRows();
    long n = A.NumCols();

    if (R.length() != m) {
        throw std::runtime_error("Dimension mismatch in gauss_jordan_GF2E.");
    }



    mat_GF2E augmented;

    GF2X currentModulus = GF2E::modulus();


    augmented.SetDims(m, n + 1);


    for (long i = 0; i < m; i++) {
        for (long j = 0; j < n; j++) {
            augmented[i][j] = A[i][j];
        }
        augmented[i][n] = R[i];
    }


    vec_long pivot_col;
    pivot_col.SetLength(std::min(m, n));

    long rank = 0;

    for (long col = 0; col < n && rank < m; col++) {
        long pivot_row = -1;

        for (long row = rank; row < m; row++) {
            if (!IsZero(augmented[row][col])) {
                pivot_row = row;
                break;
            }
        }

        if (pivot_row == -1) {
            continue;
        }

        if (pivot_row != rank) {
            swap(augmented[rank], augmented[pivot_row]);
        }

        GF2E pivot = augmented[rank][col];
        GF2E inv_pivot;
        inv(inv_pivot, pivot);

        for (long j = col; j < n + 1; j++) {
            augmented[rank][j] *= inv_pivot;
        }

        for (long row = rank + 1; row < m; row++) {
            GF2E factor = augmented[row][col];

            if (!IsZero(factor)) {
                for (long j = col; j < n + 1; j++) {
                    augmented[row][j] -= factor * augmented[rank][j];
                }
            }
        }

        pivot_col[rank] = col;
        rank++;
    }

    vec_GF2E X;
    X.SetLength(n);

    for (long i = 0; i < n; i++) {
        clear(X[i]);
    }

    for (long r = rank - 1; r >= 0; r--) {
        long col = pivot_col[r];

        GF2E value = augmented[r][n];

        for (long j = col + 1; j < n; j++) {
            value -= augmented[r][j] * X[j];
        }

        X[col] = value;

        if (r == 0) break; // avoid long underflow issue
    }

    return X;
}

// Function to perform Gaussian elimination in the field Z_p
vector<vector<int>> gauss_jordan_mod(vector<vector<int>> A, vector<vector<int>> R, int p) {
    int m = A.size();
    int n = A[0].size();
    vector<vector<int>> augmented(m, vector<int>(n + R[0].size()));

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            augmented[i][j] = A[i][j];
        }
        for (int j = 0; j < R[0].size(); j++) {
            augmented[i][n + j] = R[i][j];
        }
    }

    for (int i = 0; i < min(m, n); i++) {
        int pivot_row = -1;
        for (int row = i; row < m; row++) {
            if (augmented[row][i] % p != 0) {
                pivot_row = row;
                break;
            }
        }
        if (pivot_row == -1) continue;
        swap(augmented[i], augmented[pivot_row]);
        int pivot = augmented[i][i];
        int inv_pivot = mod_inverse(pivot, p);
        for (int j = 0; j < n + R[0].size(); j++) {
            augmented[i][j] = (augmented[i][j] * inv_pivot) % p;
        }
        for (int row = i + 1; row < m; row++) {
            int factor = augmented[row][i];
            for (int j = 0; j < n + R[0].size(); j++) {
                augmented[row][j] = (augmented[row][j] - factor * augmented[i][j] + p) % p;
            }
        }
    }

    vector<vector<int>> X(n, vector<int>(R[0].size(), 0));
    for (int i = min(m, n) - 1; i >= 0; i--) {
//        if (augmented[i][i] == 0) continue;
        if (augmented[i][i] == 0) continue;
        for (int j = 0; j < R[0].size(); j++) {
            X[i][j] = augmented[i][n + j] % p;
        }
        for (int row = i - 1; row >= 0; row--) {
            for (int j = 0; j < R[0].size(); j++) {
                augmented[row][n + j] = (augmented[row][n + j] - augmented[row][i] * X[i][j] + p) % p;
            }
        }
    }
    return X;
}


vector<vector<GF256::byte>> MatMult( vector<vector<GF256::byte>> A,  vector<vector<GF256::byte>> B)
{

        int rowsA = A.size();       // Number of rows in A
        int colsA = A[0].size();    // Number of columns in A
        int colsB = B[0].size();    // Number of columns in B

        // Result matrix of size rowsA x colsB, initialized with 0s
        std::vector<std::vector<GF256::byte>> C(rowsA, std::vector<GF256::byte>(colsB, 0));

        // Matrix multiplication logic: C = A * B
        for (int i = 0; i < rowsA; ++i) {
            for (int j = 0; j < colsB; ++j) {
                for (int k = 0; k < colsA; ++k) {
                    C[i][j] = C[i][j] + ( A[i][k] * B[k][j]);
                }
            }
        }
        return C;

}

vector<GF256::byte> MatMultVec( vector<vector<GF256::byte>> A,  vector<GF256::byte> B)
{

    int rowsA = A.size();       // Number of rows in A
    int colsA = A[0].size();    // Number of columns in A
    int colsB = B.size();    // Number of columns in B

    // Result matrix of size rowsA x colsB, initialized with 0s
    std::vector<GF256::byte> C(rowsA, 0);

    // Matrix multiplication logic: C = A * B
    for (int i = 0; i < rowsA; ++i) {

            for (int k = 0; k < colsA; ++k)
            {
                C[k] = C[i] + ( A[i][k] * B[k]);
            }
    }
    return C;

}
