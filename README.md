# RRSS

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

### Build tools
 `sudo apt-get install -y build-essential cmake pkg-config` 
### Core math/crypto deps
`sudo apt-get install -y libgmp-dev libntl-dev libcrypto++-dev`
### Python + pybind11
`sudo apt-get install -y python3 python3-dev pybind11-dev`
### Protobuf
`sudo apt-get install -y protobuf-compiler libprotobuf-dev`
### Testing
`sudo apt-get install -y catch2'
### Optional (only if your build links them)
`sudo apt-get install -y libargon2-dev`

SafeheronCryptoSuites is often installed from source or as a custom package.
After installation, confirm:
- headers exist under `/usr/local/include/crypto-suites/...`
- `libSafeheronCryptoSuites.so` exists under `/usr/local/lib` (or another path you manage)

If the loader can’t find the Safeheron shared library at runtime, you may need to update the runtime search path:



---

## 4) Build instructions

### Option A — Standard CMake build (recommended)

From the project root:
`bash$ cmake -S . -B cmake-build-release -DCMAKE_BUILD_TYPE=Release cmake --build cmake-build-release -j`

### What gets built?
Depending on configuration, typical outputs include:

- **`RRSS`** (C++ executable) — built from `main.cpp`
- **`rrss`** (Python extension module) — built via `pybind11_add_module`
- **Conditional Encryption executables** — from `srcCondEncArbHam/`
- **Tests** — from `testCondEncArbHam/` (when configured)

---

## 5) Running / using the deliverables

### 5.1 RRSS C++ executable
After building, run (example for Release build dir):

`bash ./cmake-build-release/RRSS`
### 5.2 RRSS Python extension module (for external fPAKE usage)
The build produces a Python extension named similar to:
- `rrss*.so` (no `lib` prefix)

To use it from Python, you need Python to be able to import it.
Common approaches:

1) Run Python from the directory containing the compiled module:
`bash$ cd cmake-build-release python3 -c "import rrss; print(rrss)"`
2) Or add the build directory to `PYTHONPATH`:
`bash$ export PYTHONPATH="(pwd)/cmake-build-release:{PYTHONPATH}" python3 -c "import rrss; print(rrss)"`

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
`bash ./cmake-build-release/srcCondEncArbHam/condencarbham`


If your build produces a different name (e.g., `CondEncAtMostTHamSemiHonest`), run that executable instead:
`bash ./cmake-build-release/srcCondEncArbHam/<YOUR_EXECUTABLE_NAME>`


---

### 5.4 Tests

Configure + build, then run CTest from the build directory:

`bash cd cmake-build-release ctest --output-on-failure`

To run the **Catch2** test case named **`HDAtmostTSemiEval`** from your build folder, you should execute the **test binary** (`tests`) and pass the test name as a filter.
From the directory you mentioned:
cd RRSS/cmake-build-release/testCondEncArbHam
`bash$ ./tests "HDAtmostTSemiEval"`
### Useful Catch2 commands (to discover and run the “proper” case)
**List available test cases** (so you can see all exact names defined in ): `testCondEncEvalArbHam.h`
`bash$ ./tests --list-test-cases`
**Run with verbose output** (handy for debugging / seeing timing prints):
`bash$ ./tests "HDAtmostTSemiEval" -s`
`bash$ ./tests -c "HDAtmostTSemiEval"`

### Test cases visible in `testCondEncEvalArbHam.h`
From what’s visible in that header, these names are valid filters:
- `HDAtmostTSemiEval`
- `HDArbSemiFuncCheck`
- `HDLMalEvalEstim`

So, for example:\
`bash$ ./tests "HDArbSemiFuncCheck"`
---

## 7) Specification / design notes (fill in)

Use this section to document protocol/security/parameter choices and interfaces.

### 7.1 RRSS specification
- Threshold / reconstruction requirements:
- Error model and robustness guarantees:
- Share size, encoding, field parameters:
- Complexity notes:

### 7.2 fPAKE integration notes
- Expected RRSS API surface exposed to Python:
- Password representation assumptions:
- Security parameter settings (e.g., 128/256-bit modes):
- Network / serialization expectations:

### 7.3 Conditional Encryption specification
- Security model (e.g., semi-honest):
- Supported predicates (e.g., AtMostT Hamming / Arbitrary Hamming):
- Cryptographic assumptions and parameter sizes:
- Public-key components used (Paillier, etc.):

---

## 8) How to cite / authorship
(Currently empty and the paper is under review and we share the anonympus ripository to the reviewers.)

---