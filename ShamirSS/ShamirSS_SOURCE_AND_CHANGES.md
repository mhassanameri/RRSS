# Shamir Secret-Sharing Source and Modifications

## Upstream source

The Shamir secret-sharing code for small field elements in this repository was adapted from the following public artifact:

- Repository: [CondEncCCS24Artifact](https://github.com/mhassanameri/CondEncCCS24Artifact)
- Original `shamir.h`: [`CondEncCPP/ShamirSS/src/shamir.h`](https://github.com/mhassanameri/CondEncCCS24Artifact/blob/4322cbee90dd0a85b8d03f2f66b8eb6b2a2cc820/CondEncCPP/ShamirSS/src/shamir.h)
- Original `GF256.h`: [`CondEncCPP/ShamirSS/src/GF256.h`](https://github.com/mhassanameri/CondEncCCS24Artifact/blob/4322cbee90dd0a85b8d03f2f66b8eb6b2a2cc820/CondEncCPP/ShamirSS/src/GF256.h)

The links above are pinned to upstream commit
[`4322cbee90dd0a85b8d03f2f66b8eb6b2a2cc820`](https://github.com/mhassanameri/CondEncCCS24Artifact/commit/4322cbee90dd0a85b8d03f2f66b8eb6b2a2cc820)
so that the referenced source remains identifiable even if the upstream repository changes.

The comment at the beginning of the upstream `shamir.h` also points to the following background discussion:
[“Shamir's Secret Share over the Reals”](https://crypto.stackexchange.com/questions/10701/shamirs-secret-share-over-the-reals).

## Treatment of `GF256.h`

`GF256.h` was copied into the new repository **without modification**. Its original location is:

```text
CondEncCCS24Artifact/CondEncCPP/ShamirSS/src/GF256.h
```

Accordingly, this file should be understood as reused upstream code rather than a modified component of the new implementation which support any arbitrary samll field element ranther thant just GF_256. 

## Changes to `shamir.h`

The original byte-oriented Shamir interface was retained, including:

- the `shamir` namespace;
- the `shares` alias based on `GF256::point`;
- the `scheme(int members, int threshold)` constructor;
- `createShares(std::string secret)` and `getSecret(shares* Kshares)`; and
- the namespace-level `init()` function.

The updated header extends that interface as follows.

### 1. Added NTL finite-field support

The header now imports NTL types and algorithms needed to operate over prime fields and binary extension fields:

```cpp
#include <NTL/ZZ_pEX.h>
#include <NTL/LLL.h>
#include <NTL/GF2E.h>
#include <NTL/GF2X.h>
#include <NTL/vec_GF2E.h>
#include <NTL/GF2XFactoring.h>
```

It also introduces aliases for vectors of NTL elements:

```cpp
typedef vec_ZZ_p shares_NTL_p;
typedef vec_ZZ shares_NTL;
```

### 2. Added scheme parameters for extension-field operation

Two private data members were added:

```cpp
int _lambda = 128;
long GF_2E_degree;
```

These members record the security-related parameter and the degree of the active `GF2E` extension field which is defined based on the number of required shares that we want to generate. For the Shamir secret sharing, if we want ot generate `n` shares, our field size should be atleast `|GF_2E| > n`. 

### 3. Added point representations for NTL fields

The updated class defines point types whose coordinates are NTL field elements:

```cpp
typedef struct point_p {
    ZZ_p x;
    ZZ_p y;
} point_p;

typedef struct point_GF2E {
    GF2E x;
    GF2E y;
} point_GF2E;
```

These supplement the original `GF256::point` representation.

### 4. Added share generation over additional fields

The following public methods were added:

```cpp
shares_NTL* createShares_NTL(std::string secret);
vec_ZZ_p createShares_NTL_p(ZZ_p secret);
vec_GF2E createShares_GF2E(const GF2E& secret);
```

Together, these declarations extend share generation beyond the original byte-based `GF(256)` interface to NTL integer/prime-field and binary-extension-field representations.

The older pointer-returning declaration for `createShares_NTL_p` remains in the header only as a commented-out line; the active declaration returns `vec_ZZ_p` by value.

### 5. Added binary-extension-field helper functions

The following utilities and usefull helper functions were added to realize the intented implementaion under arbitrary field:

```cpp
long smallest_lambda1_for_field_size(long required_points);
void init_GF2E_field(long lambda1);
static GF2E int_to_GF2E(unsigned long a);
static int GF2E_to_int(const GF2E& a);
long get_GF2E_degree() const;
long ceil_div(long a, long b);
```

They support choosing a sufficiently large binary extension field, initializing that field, converting between integers and `GF2E` elements, retrieving the configured field degree, and performing ceiling division.

## Summary of the adaptation

The main change is an extension of the original Shamir interface from its byte-oriented `GF(256)` representation to additional fields implemented with NTL, especially `GF(2^m)` through `GF2E`. The existing `GF256` API remains available for compatibility, while the new declarations provide field initialization, conversion utilities, and share generation using NTL vectors. `GF256.h` itself was reused unchanged from the cited upstream artifact.

## Scope

This document summarizes the changes made to `shamir.h`. Changes to the implementation in `shamir.cpp`, if any, are outside its scope.