## CCS 2026 Artifact

The evaluator-facing documentation for the ACM CCS 2026 artifact is available in
[`artifact/README.md`](artifact/README.md).

Please begin with that document for the complete environment setup, build,
functionality checks, and instructions to reproduce the experiments supporting
the paper.


### Artifact availability

The permanent archival version of this artifact is available on Zenodo:

<https://doi.org/10.5281/zenodo.22802638>

For artifact evaluation, please use the Zenodo archive, which includes the
complete evaluator-facing source tree, reproduction scripts, documentation, and
prepared RRSS-fPAKE benchmark inputs. The actively maintained development
repository is available at <https://github.com/mhassanameri/RRSS>. If obtaining
the source from GitHub instead of Zenodo, follow the instructions in this README
to run `download_rssfPAKE_data.sh` and retrieve the prepared benchmark inputs.


# RRSS: Random Robust Secret sharing scheme

RRSS is a C++20 project that provides:
- **Robust secret sharing (RRSS)** primitives (Shamir-style + robustness/error-correction components).
- A **Python extension module** (via **pybind11**) to use RRSS from Python (used by the external **fPAKE** application).
- A **Conditional Encryption** application (e.g., “Arbitrary Hamming” / “AtMostT Hamming”, semi-honest setting) built as standalone C++ executables with tests.

This README focuses on **how to compile** and **how to run/use** the key deliverables.

---

## 1) Repository layout (high level)

- `CMakeLists.txt` — top-level build.
- `srcCondEncArbHam/` — Conditional Encryption application sources + its CMake.
- `testCondEncArbHam/` — tests for Conditional Encryption.
- `paillier/` — Paillier implementation used by Conditional Encryption.
- `ShamirSS/` — Shamir / GF256 utilities.
- `bindings.cpp` + `Wrapper*.cpp` — pybind11 bindings to build the Python module.

---

## 2) Requirements (Linux)

### Toolchain
- **CMake** ≥ 3.28
- **C++ compiler** with **C++20** support (GCC ≥ 11 or Clang ≥ 14 is typically fine)
- **Python 3** + development headers (for building the Python module)

### Native libraries (C/C++)
The project links against (names may vary by distro packages):

- **GMP** (GNU Multiple Precision Arithmetic)
- **NTL** (Number Theory Library)
- **Crypto++**
- **Protocol Buffers** (protobuf) — used in Conditional Encryption subproject
- **pybind11** — for the Python module build
- **Catch2** — for tests
- **SafeheronCryptoSuites** (shared library + headers)
- **Argon2** (only if enabled/linked in your Conditional Encryption target)

#### Using from your external fPAKE code
Your fPAKE Python implementation should import and call the RRSS binding module.
A typical pattern is:

> Notes on SafeheronCryptoSuites
>
> - Headers are typically installed under something like:
>   - `/usr/local/include/crypto-suites/...`
> - The shared library is typically:
>   - `/usr/local/lib/libSafeheronCryptoSuites.so`
>
> If you install it into a non-standard path, ensure your build system can find:
> - headers via include paths
> - the `.so` via link paths and the runtime loader

---

## 3) Installing dependencies (examples)
Below are common package names on Ubuntu/Debian-like systems. Adjust for your distro.
bash sudo apt-get update

- Build tools
 `sudo apt-get install -y build-essential cmake pkg-config` 
- Core math/crypto deps
`sudo apt-get install -y libgmp-dev libntl-dev libcrypto++-dev`
- Python + pybind11
`sudo apt-get install -y python3 python3-dev pybind11-dev`
- Protobuf
`sudo apt-get install -y protobuf-compiler libprotobuf-dev`
- Testing
`sudo apt-get install -y catch2'
- Optional (only if your build links them)
`sudo apt-get install -y libargon2-dev`

SafeheronCryptoSuites is often installed from source or as a custom package.
After installation, confirm:
- headers exist under `/usr/local/include/crypto-suites/...`
- `libSafeheronCryptoSuites.so` exists under `/usr/local/lib` (or another path you manage)

If the loader can’t find the Safeheron shared library at runtime, you may need to update the runtime search path:



---

## 4) Build instructions

### Option A — Standard CMake build (recommended)
First clone the project to your machine by doing: 

```bash
 $ git clone https://github.com/mhassanameri/RRSS.git
 $ cd RRSS
```
As an optional step, before to make the build directory, it may need to run `bash$ conda deactivate` (if the the cmake failed to build the project.).
Then, creat the build directory and go inside the directory to build the program as follows:

```bash
 $ mkdir cmake-build-release && cd cmake-build-release 
 $ cmake .. -DCMAKE_BUILD_TYPE=Release
 $ cmake --build . -j14  # You may replace 14  with the number of processors that your system supports. 
```

### What gets built?
Depending on configuration, after successfull exeutuion of the `bash$ cmake --build` command, typical outputs include the following compiled binary files:

- **`RRSS`** (C++ executable) — built from `main.cpp`. After buidling and compiling the project, it is located in the build directory (e.g., `RRSS/cmake-build-release`). You can modify the main function to use our RRSS as you want based on your desired application or for checcking if RRSS is compiled properly and linked to your project. 

- **`rrss`** (Python extension module) — built via `pybind11_add_module`. Its corresponding `*.so` libriary is located in the build dicrectory (e.g., `RRSS/cmake-build-release`) and its name is something like `rrss*.so` (e.g., `rrss.cpython-313-x86_64-linux-gnu.so`). As the application of our RRSS, we use the compiled RRSS in our `fPAKERRSS.py` application located in `RRSS/external/fPAKE-benchmark/fPAKE` directory.  
- **Conditional Encryption executables** — from `RRSS/cmake-build-release/srcCondEncArbHam/` the resulting compiled binary is `condencarbham`
- **Tests** — from `RRSS/cmake-build-release/testCondEncArbHam/` the resulting compiled binary for our functionality tests is `tests`. 

---

## 5) Running / using the deliverables

### 5.1 RRSS C++ executable
After building, run (example for Release build dir):

```bash
 $ ./cmake-build-release/RRSS
```
### 5.2 RRSS Python extension module (for external fPAKE usage)
The build produces a Python extension named similar to:
- `rrss*.so` (no `lib` prefix)

To use it from Python, you need Python to be able to import it.
Common approaches:

1) Run Python from the directory containing the compiled module:
```bash
 $ cd cmake-build-release 
 $ python3 -c "import rrss; print(rrss)"
 ```
 This command in terminal actually checks if you can import our rrss library successfully to our Python. 
2) Or add the build directory to `PYTHONPATH`:
```bash
 $ export PYTHONPATH="(pwd)/cmake-build-release:{PYTHONPATH}" 
 $ python3 -c "import rrss; print(rrss)"
```

#### Using from your external fPAKE code
Your fPAKE Python implementation should import and call the RRSS binding module.
A typical pattern is:


> If fPAKE lives in another repo/folder, the easiest method is:
> - build RRSS
> - export `PYTHONPATH` to point at the directory that contains `rrss*.so`
> - run your fPAKE script

---

### 5.3 Conditional Encryption application(s)

Build is handled by CMake as part of the top-level build (the project adds the `srcCondEncArbHam/` subdirectory).

After compiling, run the corresponding executable from the build directory, e.g.:
```bash
 $./cmake-build-release/srcCondEncArbHam/condencarbham
```


If your build produces a different name (e.g., `CondEncAtMostTHamSemiHonest`), run that executable instead:
```bash
 $ ./cmake-build-release/srcCondEncArbHam/condencarbham
```
---

### 5.4 Tests

Configure + build, then run CTest from the build directory:

```bash
 $ cd cmake-build-release ctest --output-on-failure
```

To run the **Catch2** test case named **`HDAtmostTSemiEval`** from your build folder, you should execute the **test binary** (`tests`) and pass the test name as a filter.
From the directory you mentioned:

```bash
 $ cd RRSS/cmake-build-release/testCondEncArbHam
 $ ./tests "HDAtmostTSemiEval"
```

### Useful Catch2 commands (to discover and run the “proper” case)
**List available test cases** (so you can see all exact names defined in ): `testCondEncEvalArbHam.h`
`bash$ ./tests --list-test-cases`
**Run with verbose output** (handy for debugging / seeing timing prints):
`bash$ ./tests "HDAtmostTSemiEval" -s`
`bash$ ./tests -c "HDAtmostTSemiEval"`

### Test cases visible in `testCondEncEvalArbHam.h`
According to this test header, the following names are valid filters:
- `HDAtmostTSemiEval`
- `HDArbSemiFuncCheck`
- `HDLMalEvalEstim`
- So, for example:
  ```bash
  $ ./tests "HDArbSemiFuncCheck"
  ```
---

## 6) Specification / design notes

Use this section to document protocol/security/parameter choices and interfaces.

### 6.1 RRSS specification
- Threshold / reconstruction requirements: Our random robust secret Sharing is closing the gap between the robustness and the perfect privacy parameter whene the corrupted shares are chosen uniformly at random not maliciously. 
- Error model and robustness guarantees: In our `(n,t)-RRSS` if we are given `n` shares containing at most `n-t` *randomly* corrupted shares, our robust secret sharing can identify the (in)valid shares with high probability (negligibly less than 1) and reconstruct the secret using the valid shares.    
- Share size, encoding, field parameters: The shares size is O(n^2) where n is the number of shares and the field size can can be set in the implementation. We use Regualar Shamir Secret Sharing for sharing the original secret (with larger field size) and for the identification part, we also use Shamir Secret sharing with smaller field (like F_2^{8}) for more faster computations. See the paper with more detials on the RRSS construction and the parameter selection. 
- Complexity notes:

### 6.2 fPAKE integration notes
- Expected RRSS API surface exposed to Python:
- Password representation assumptions: The passwords are n characters for n \in {32, 64, 128}
- Security parameter settings: we provide 128 bit security level
- Network expectations: The application stablishes a TCP ip connection, however for the test we consider the same machine stablishes connection among two different terminals. So the application has the ability to form TCP connection via two machines and they can communicate by using proper choice of ip addressa and the port number.  

### 6.3 Conditional Encryption specification
- Security model: The construction is secure in the Semi-honest setting which means that the public keys are safe and generated honestly, and the parties follows the algorithm steps. 
- Supported predicates: Our construction provides conditional encryption for Arbitrary Hamming distance as well as well Conditional Encryption for small Hamming distances.  For Hamming distance smaller thatn 2, it is more efficient to use prior work of Conditional encryption [Ameri & Blocki, CCS24] without RRSS. However it is more practical to use the construciton using RRSS when the maximum Hamming distance is arbitrary big, e.g., `\ell \in {16, 32, 64}` .  
 [Ameri & Blocki, CCS24]: https://arxiv.org/pdf/2409.06128 
- Cryptographic assumptions and parameter sizes:
- Public-key components used (Paillier, etc.): We use Paillier Public key scheme as the underlying public key encryption as the building blocks since it provides the ciphertext addition homomorphic and ciphertext-plaintext multiplication property. 
---

### 8) Paper, authors, and citation

This repository contains the open-source implementation accompanying:

> **Random Robust Secret Sharing with Perfect Privacy and its Applications**  
> Mohammad Hassan Ameri and Jeremiah Blocki  
> ACM Conference on Computer and Communications Security (CCS 2026)

The paper has been accepted to CCS 2026. The full version is available on the IACR Cryptology ePrint Archive:

<https://eprint.iacr.org/2026/653>

The final ACM Digital Library version and citation information will be added once they become available.

#### Artifact archive

A permanent Zenodo archive for the CCS 2026 artifact evaluation, including the exact evaluated release and reproducibility materials, will be added here before the artifact-submission deadline.

<!-- Zenodo DOI: to be added -->

#### How to cite

```bibtex
@inproceedings{AmeriBlocki2026RRSS,
  author    = {Mohammad Hassan Ameri and Jeremiah Blocki},
  title     = {Random Robust Secret Sharing with Perfect Privacy and its Applications},
  booktitle = {Proceedings of the ACM Conference on Computer and Communications Security (CCS)},
  year      = {2026},
  note      = {To appear}
}
