# Random Robust Secret Sharing with Perfect Privacy and its Applications

This repository contains the implementation artifact for the ACM CCS 2026 paper:

> **Random Robust Secret Sharing with Perfect Privacy and its Applications**

> This artifact supports reproduction of the RRSS-fPAKE results reported in Table 1 and the conditional-encryption evaluation reported in Figure 2.

The artifact supports reproduction of the fPAKE results reported in **Table 1**, including a comparison between:

* **RSS-fPAKE**, the prior RSS-based baseline; and
* **RRSS-fPAKE**, our implementation based on Random Robust Secret Sharing (RRSS).

The two implementations are evaluated on the same fPAKE inputs and parameter settings. The benchmark reports separate Sender and Receiver computation time, communication time, overall time, and communication overhead.

## Artifact overview

This artifact supports reproduction of the RRSS-fPAKE results reported in Table 1 and the conditional-encryption evaluation reported in Figure 2.

The artifact provides:

- A C++ implementation of Random Robust Secret Sharing (RRSS).
- A Python binding, `rrss`, used by the RRSS-fPAKE benchmark.
- An implementation of the prior RSS-fPAKE baseline (as the prior work to compare with).
- An implementation of the proposed RRSS-fPAKE construction.
- Fixed RSS-fPAKE and RRSS-fPAKE benchmark configurations for security levels 128 and 244. The supplied configurations run the Sender and Receiver on the same machine using localhost.
- Statistics programs that aggregate Sender and Receiver timing and communication measurements.
- Automated scripts that start the Receiver and Sender locally, collect their benchmark results, calculate summary statistics, and reproduce Table 1.
- C++ implementations and Catch2 benchmark cases for the conditional-encryption constructions evaluated in Figure 2.
- An automated Figure 2 workflow that runs the conditional-encryption experiments, collects the raw measurements, exports normalized CSV files, and creates the figure in PDF and PNG formats.
- Manual reproduction commands for evaluators who prefer to run individual benchmarks or regenerate a figure from existing measurements.

The automated scripts use reduced benchmark repetition counts to keep artifact evaluation practical. These runs preserve the experimental configurations and trends used in the paper, while the paper's reported results were produced using larger experiment counts.


## Repository layout

```text
.
├── artifact/
│   ├── README.md
│   ├── scripts/
│   │   ├── install_dependencies_ubuntu24.sh
│   │   ├── reproduce_table1_RSSFPAKE.sh
│   │   ├── reproduce_table1_RRSSFPAKE.sh
│   │   ├── reproduce_table1_all.sh
│   │   ├── generate_table1.py
│   │   ├── reproduce_figure2.sh
│   │   └── generate_figure2.py
│   └── results/
│       └── reproduced/
├── download_rssfPAKE_data.sh
├── external/
│   └── fPAKE-benchmark/
│       ├── results128/
│       │   └── cache/
│       │       └── ... prepared JSON inputs ...
│       ├── results244/
│       │   └── cache/
│       │       └── ... prepared JSON inputs ...
│       ├── statisticBenchmarkRSSfPAKE.py
│       ├── statisticBenchmarkRRSSfPAKE.py
│       └── fPAKE/
│           ├── benchmarkRSS.py
│           ├── benchmarkRRSS.py
│           ├── config128.ini
│           └── config244.ini
├── CMakeLists.txt
└── build.sh
```

The artifact scripts and benchmark programs have the following roles:

| File | Purpose |
| --- | --- |
| `artifact/scripts/install_dependencies_ubuntu24.sh` | Installs the system and Python dependencies needed to build and evaluate the artifact on Ubuntu 24.04. |
| `artifact/scripts/reproduce_table1_RSSFPAKE.sh` | Runs the prior RSS-fPAKE benchmark for a selected security level. It starts the Receiver, runs the Sender, collects the generated JSON measurements, and invokes the RSS-fPAKE statistics program. |
| `artifact/scripts/reproduce_table1_RRSSFPAKE.sh` | Runs the proposed RRSS-fPAKE benchmark for a selected security level. It starts the Receiver, runs the Sender, collects the generated JSON measurements, and invokes the RRSS-fPAKE statistics program. |
| `artifact/scripts/reproduce_table1_all.sh` | Runs both Table 1 constructions for the supported security levels and gathers their statistics in one reproducible workflow. |
| `artifact/scripts/generate_table1.py` | Parses the collected statistics, organizes the RSS-fPAKE and RRSS-fPAKE measurements, and generates a formatted reproduction of Table 1. |
| `artifact/scripts/reproduce_figure2.sh` | Runs the two C++ conditional-encryption benchmark cases, collects their raw `.dat` files, and invokes the Figure 2 plotting program. |
| `artifact/scripts/generate_figure2.py` | Parses the raw conditional-encryption measurements, exports normalized CSV files, and creates Figure 2 in PDF and PNG formats. |
| `external/fPAKE-benchmark/statisticBenchmarkRSSfPAKE.py` | Computes Sender and Receiver timing and communication statistics for the RSS-fPAKE baseline. |
| `external/fPAKE-benchmark/statisticBenchmarkRRSSfPAKE.py` | Computes Sender and Receiver timing and communication statistics for RRSS-fPAKE. |
| `external/fPAKE-benchmark/fPAKE/benchmarkRSS.py` | Entry point for running the RSS-fPAKE Sender or Receiver benchmark. |
| `external/fPAKE-benchmark/fPAKE/benchmarkRRSS.py` | Entry point for running the RRSS-fPAKE Sender or Receiver benchmark. |
| `external/fPAKE-benchmark/fPAKE/config128.ini` | Fixed fPAKE benchmark configuration for security level 128. |
| `external/fPAKE-benchmark/fPAKE/config244.ini` | Fixed fPAKE benchmark configuration for security level 244. |

The two Table 1 reproduction scripts accept the requested security level and run both communication roles on the local machine. `reproduce_table1_all.sh` coordinates the complete Table 1 evaluation, while `generate_table1.py` can be called separately when benchmark results already exist and only the final table needs to be regenerated.

Similarly, `reproduce_figure2.sh` coordinates the expensive C++ experiments and then calls `generate_figure2.py`. If plotting fails after the measurements have been generated—for example, because Matplotlib was missing—`generate_figure2.py` can be run separately without repeating the C++ benchmarks.

# Requirements

The artifact was tested on Linux (Ubuntu 24.04) with:

* CMake and a C++20-compatible compiler
* Python 3 with development headers
* GMP, NTL, Crypto++, Protocol Buffers, pybind11, and Catch2
* The remaining native dependencies required by the top-level CMake project
* Matplotlib for creating the PDF and PNG plots
* NumPy, Cryptography, and PyCryptodome for the Python-based artifact components
* A PDF viewer, such as Evince, Okular, or another desktop PDF reader, if the generated PDF will be opened locally
* On Ubuntu 24.04, the plotting dependency can be installed with:

```bash
apt-get update
apt-get install -y python3-matplotlib
```


# How to Build 
Before starting to build the project, make sure that all the dependencies are installed. For that we can do as follows. 

**In a root-owned Ubuntu container**: 
```
bash artifact/scripts/install_dependencies_ubuntu24.sh
```

**On a regular Ubuntu installation**: 
```
sudo bash artifact/scripts/install_dependencies_ubuntu24.sh
```

Build RRSS and its Python binding from the repository root. The build process creates the build/ directory and places the compiled executables and libraries inside it.

```
bash build.sh
```

The RRSS-fPAKE benchmark imports the generated Python module `rrss`. The module must be compiled for the same Python version used to run the benchmark.

Before running the RRSS-fPAKE benchmark manually, verify that Python can import the module:

```bash
export PYTHONPATH="$PWD/build${PYTHONPATH:+:$PYTHONPATH}"
python3 -c "import rrss; print(rrss.__file__)"
```

The artifact script performs this check automatically.

### Script permissions
The reproduction commands below invoke each script with `bash`; therefore, no
additional permission changes are normally required.

If you prefer to run a script directly as `./artifact/scripts/<script>.sh`,
first make it executable:
```bash
chmod +x artifact/scripts/reproduce_table1_RSSFPAKE.sh
chmod +x artifact/scripts/reproduce_table1_RRSSFPAKE.sh
```

## Toy examples and functionality checks

This section provides two short smoke tests. After successfully building the project, evaluators can run these toy examples. The first demonstrates share generation, corruption, and recovery with Random Robust Secret Sharing. The second demonstrates conditional encryption for the arbitrary-Hamming-distance predicate. Both examples require the project to have been built successfully.

### Random Robust Secret Sharing example

The root-level `main.cpp` contains a small RRSS demonstration. It uses the following 40-character secret:

```text
HelloWorldHelloWorldHelloWorldHelloWorld
```

The program generates RRSS shares of the secret, deliberately corrupts two shares, reconstructs the secret from the resulting share vector, and compares the recovered value with the original value.

From the repository root, run:

```bash
./build/RRSS
```

Alternatively:

```bash
cd build
./RRSS
cd ..
```

The expected output is similar to:

```text
Original:  HelloWorldHelloWorldHelloWorldHelloWorld
Recovered: HelloWorldHelloWorldHelloWorldHelloWorld
Match: true
```

`Match: true` confirms that the demonstration recovered the original secret even after the example corrupted two shares.

The executable is compiled from the root-level `main.cpp` and uses the RRSS implementation in `RandomRobustSS.cpp`. Evaluators can experiment with a different secret or corruption pattern by modifying `main.cpp` and rebuilding the executable. The configured RRSS length must remain consistent with the byte length of `Secret`; otherwise, the length assertion in the example will fail.

After modifying `main.cpp`, rebuild and rerun the example:

```bash
cmake --build build --target RRSS --parallel "$(nproc)"
./build/RRSS
```

This example is a functionality demonstration and is not used to produce the performance results in Table 1 or Figure 2.

### Conditional encryption for arbitrary Hamming distance

The Catch2 test executable contains a small functionality check named `HDArbSemiFuncCheck`. It demonstrates conditional encryption for the arbitrary-Hamming-distance predicate using:

```text
m1      = "Test1"
m2      = "Test0"
payload = "This is good Thing to see!"
```

The strings `Test1` and `Test0` differ in one position, so their Hamming distance is 1. In the supplied example, the accepted distance is `ell = 4`. Because `1 <= 4`, the predicate is satisfied and conditional decryption recovers the payload.

From the repository root, run:

```bash
cd build/testCondEncArbHam
./tests "HDArbSemiFuncCheck"
cd ../..
```

The quotation marks around the test name are recommended because Catch2 interprets the argument as a test filter.

The output should include lines similar to:

```text
5       5
Test1   Test0
the recovered payload is: This is good Thing to see!
===============================================================================
test cases: 1 | 1 passed
```

The first line reports the two unpadded message lengths. The second line prints the two messages. The recovered-payload line demonstrates that conditional decryption succeeded because the Hamming-distance predicate was true.

The example pads the two short messages to the configured internal message length before applying the conditional-encryption construction. It then performs the construction's key generation, ordinary encryption, conditional encryption, and conditional decryption steps.

Evaluators can experiment by changing `msg`, `typo`, `payload`, or `ell` in the `HDArbSemiFuncCheck` test case in:

```text
testCondEncArbHam/testCondEncEvalArbHam.h
```

After changing the test inputs, rebuild the test target and rerun the selected case:

```bash
cmake --build build --target tests --parallel "$(nproc)"

cd build/testCondEncArbHam
./tests "HDArbSemiFuncCheck"
cd ../..
```

If the Hamming distance between the two messages is at most `ell`, the payload should be recovered. If the distance is greater than `ell`, the predicate is false and the original payload should not be recovered.

> **Note:** The supplied Catch2 case is primarily a terminal-output demonstration. As shown by Catch2's `assertions: - none -` message, successful payload recovery is currently checked by inspecting the printed output. It should not be described as an assertion-based correctness test unless the source is updated to include an explicit Catch2 assertion comparing the recovered value with the original payload.

These toy examples are intended as quick functionality checks. The Table 1 and Figure 2 scripts described below run the full artifact evaluation workflows.

---


# Reproducing Table 1: fPAKE Performance Evaluation
Table 1 compares the prior RSS-fPAKE implementation with RRSS-fPAKE.

The benchmark uses:
* Password/fingerprint length: `n = 36`
* Maximum Hamming distance: `ℓ = 3`
* Input types: `acc_h-bar` and `acc_h-gyrW`
* Three executions per benchmark instance
* Security levels: `λ = 128` and `λ = 244`

This section reproduces the fPAKE performance comparison in Table 1 between the prior RSS-based construction (**RSS-fPAKE**) and our Random Robust Secret Sharing-based fPAKE construction (**RRSS-fPAKE**).

The experiments measure the Sender and Receiver separately for the two input types `acc_h-bar` and `acc_h-gyrW`, using password length \(n=36\), maximum Hamming distance \(\ell=3\), and security levels \(\lambda\in\{128,244\}\). For each benchmark instance, the implementation performs three executions and reports mean calculation time, network time, overall time, and communication overhead.

Both protocol parties execute on the same machine and communicate through a local TCP connection. The reproduction scripts automatically start the Receiver in the background, run the Sender, wait for both processes to complete, and invoke the corresponding statistics program.

### Obtaining the Table 1 benchmark inputs

The RSS-fPAKE and RRSS-fPAKE experiments in Table 1 require prepared JSON input files for security levels 128 and 244. These files are not generated by the C++ build process.

The benchmark programs expect the data at:

```text
external/fPAKE-benchmark/results128/cache/
external/fPAKE-benchmark/results244/cache/
```


The original Zenodo record identifies the deposit as the evaluation results for the MobiSys 2021 FastZIP paper and provides `fastzip-results.zip`. [FastZIP Zenodo dataset](https://zenodo.org/records/4911390)

| `download_rssfPAKE_data.sh` | Downloads the prepared 128-bit and 244-bit fPAKE input datasets from the RRSS artifact Zenodo record, verifies the archive checksum, and extracts the JSON files into the locations expected by `config128.ini` and `config244.ini`. |

The complete Zenodo artifact archive already contains the `results128` and
`results244` directories. The download script is required only when obtaining
the source code from GitHub without the large benchmark-input directories.
## Repository-layout update

Add the download script and both input directories to the tree:



### Running the benchmarks

Run the following commands from the repository root:

```bash
bash artifact/scripts/reproduce_table1_RSSFPAKE.sh 128
bash artifact/scripts/reproduce_table1_RRSSFPAKE.sh 128
```

These commands reproduce the RSS-fPAKE baseline and RRSS-fPAKE results, respectively, for security level \(\lambda=128\).

Next, reproduce the experiments for security level \(\lambda=244\):

```bash
bash artifact/scripts/reproduce_table1_RSSFPAKE.sh 244
bash artifact/scripts/reproduce_table1_RRSSFPAKE.sh 244
```
We can generate all data by simply executing the following command. 
```bash
bash artifact/scripts/reproduce_table1_all.sh
```

The RSS-fPAKE script invokes `benchmarkRSS.py` and `statisticBenchmarkRSSfPAKE.py`. The RRSS-fPAKE script invokes `benchmarkRRSS.py` and `statisticBenchmarkRRSSfPAKE.py`.

The RRSS-fPAKE script also verifies that the compiled `rrss` Python module can be imported before starting the benchmark. Therefore, the RRSS C++ project and Python binding must be built with the same Python version used by `python3`.

### Benchmark configurations

The experiments use two fixed configuration files:

| Configuration file | Security-level selector | Input directory       |
| ------------------ | ----------------------: | --------------------- |
| `config128.ini`    |          `SECPARAM = 0` | `../results128/cache` |
| `config244.ini`    |          `SECPARAM = 1` | `../results244/cache` |

Each Sender/Receiver pair uses the same configuration file. The scripts execute one benchmark pair at a time, avoiding conflicts between TCP ports or result directories.

### Generated benchmark outputs

Each reproduction script stores its output under `artifact/results/reproduced/`:

```text
artifact/results/reproduced/RSSFPAKE-128/
artifact/results/reproduced/RRSSFPAKE-128/
artifact/results/reproduced/RSSFPAKE-244/
artifact/results/reproduced/RRSSFPAKE-244/
```

Each directory contains:

```text
receiver.log
sender.log
statistics.txt
```

The `statistics.txt` file reports the mean and standard deviation of calculation time, network time, and overall time for each role, as well as the Sender and Receiver communication overhead.

### Generating the reproduced Table 1

After running the benchmark scripts, generate a machine-readable and GitHub-rendered version of Table 1 from the collected statistics files.

To generate a partial table containing only the completed 128-bit experiments, run:

```bash
python3 artifact/scripts/generate_table1.py --levels 128
```

After completing all four experiments, generate the complete table:

```bash
python3 artifact/scripts/generate_table1.py
```

The generator reads the statistics files from the four output directories listed above and produces:

```text
artifact/results/reproduced/table1.csv
artifact/results/reproduced/table1.md
```

The CSV file contains the reproduced results in a machine-readable format. The Markdown file (`artifact/results/reproduced/table1.md`) is rendered directly by GitHub and follows the organization of Table 1 in the paper, including the algorithm, security level, input type, role, calculation time, network time, overall time, role communication overhead, and total communication overhead.

For each algorithm, security level, and input type, the total communication overhead is computed as the sum of the Sender and Receiver communication overhead.

Absolute timings may differ across machines because of hardware, system load, and compiler or Python-version differences. A successful reproduction should preserve the functionality and the qualitative performance comparison between RSS-fPAKE and RRSS-fPAKE.



### Manual benchmark execution for Table 1

The artifact scripts above are the recommended method. For debugging, the benchmark roles can also be started manually from:

```text
external/fPAKE-benchmark/fPAKE
```

For example, for RRSS-fPAKE at security level 128, first start the Receiver:

```bash
python3 benchmarkRRSS.py Receiver --config config128.ini
```

Then start the Sender in another terminal:

```bash
python3 benchmarkRRSS.py Sender --config config128.ini
```

After both roles finish, run the matching statistics program from:

```text
external/fPAKE-benchmark
```

```bash
python3 statisticBenchmarkRRSSfPAKE.py 128
```

## Troubleshooting

### `ModuleNotFoundError: No module named 'rrss'`

The RRSS Python binding is not visible to Python, or it was compiled for a different Python version.

Rebuild the project using the same Python interpreter used by `python3`, then ensure that the build directory containing `rrss*.so` is included in `PYTHONPATH`.

### Receiver stops unexpectedly

Check the saved file:

```text
artifact/results/reproduced/<experiment>/receiver.log
```

A common cause is that another benchmark process is already using the configured TCP port. Stop the earlier Sender/Receiver pair before starting another experiment.

### Statistics program finds no result files

Run the statistics program from:

```text
external/fPAKE-benchmark
```

The statistics programs use paths relative to that directory. The artifact scripts automatically change to the correct directory before invoking them.

# Reproducing Figure 2: Conditional-Encryption Evaluation

Figure 2 compares the conditional-encryption construction in this paper with the Hamming-distance-at-most-`t` construction of AB24/CCS24. The artifact measures conditional-decryption time and ciphertext size over several message lengths and Hamming-distance thresholds.

The paper's reported measurements were collected using a larger number of repetitions. To keep artifact-evaluation time practical, the supplied artifact uses fewer repetitions while preserving the same experimental structure and trends. In the current artifact configuration:

- `HDArbSemiEval` evaluates our construction with security parameter 128, a 3072-bit Paillier modulus, message lengths `n` in `{8, 16, 32, 64, 128}`, and applicable Hamming-distance values `ell` in `{2, 4, 8, 16, 32, 64}`, with `ell < n`.
- `HDAtmostTSemiEval` evaluates the AB24/CCS24 construction with security parameter 128 and a 3072-bit Paillier modulus. For `n = 128`, the artifact evaluates Hamming distance at most `t = 3` and for `n = {8, 16, 32, 64}` we have `t=4`. 

The smaller artifact configuration is intended to verify reproducibility within a reasonable evaluation time; it is not meant to replace the higher-repetition measurements reported in the paper.

## Prerequisites

Build the repository before running this experiment. From the repository root:

```bash
./build.sh
```

The expected test executable is:

```text
build/testCondEncArbHam/tests
```

Figure generation also requires Matplotlib:

```bash
apt-get update
apt-get install -y python3-matplotlib
```

For non-root users, use `sudo apt-get` instead of `apt-get`.

## Automatic reproduction

From the repository root (e.g., `user/Github/RRSS$`), run:

```bash
bash artifact/scripts/reproduce_figure2.sh
```

The script performs the following steps:

1. Locates the test executable under `build/testCondEncArbHam/`.
2. Removes raw measurement files left by earlier runs, preventing duplicated rows.
3. Runs the Catch2 test case `HDArbSemiEval` for the construction in this paper.
4. Runs the Catch2 test case `HDAtmostTSemiEval` for the AB24/CCS24 comparison.
5. Copies the generated raw `.dat` files to `artifact/results/reproduced/`.
6. Calls `generate_figure2.py` to normalize the measurements, export CSV files, and create the final plots.

Benchmark execution can take time because it performs public-key cryptographic operations for multiple parameter combinations. (Estimated time to get the results: TBA)

## Manual reproduction

The same benchmarks can be run manually. Starting at the repository root:

```bash
cd build/testCondEncArbHam

# Remove measurements from earlier runs because the tests append to these files.
rm -f dataHamArbSemi.dat dataMalAtmostT.dat

# Our arbitrary-Hamming-distance construction.
./tests "HDArbSemiEval"

# AB24/CCS24 Hamming-distance-at-most-t construction.
./tests "HDAtmostTSemiEval"
```

After both tests finish, return to the repository root, collect the raw files, and generate the figure:

```bash
cd ../..
mkdir -p artifact/results/reproduced

cp build/testCondEncArbHam/dataHamArbSemi.dat \
   artifact/results/reproduced/

cp build/testCondEncArbHam/dataMalAtmostT.dat \
   artifact/results/reproduced/

python3 artifact/scripts/generate_figure2.py \
  --ours artifact/results/reproduced/dataHamArbSemi.dat \
  --ab24 artifact/results/reproduced/dataMalAtmostT.dat \
  --output-dir artifact/results/reproduced
```

Catch2 accepts the quoted test names as filters, so only the requested benchmark case is executed.

## Output files

Successful reproduction creates the following files under `artifact/results/reproduced/`:

| File | Description |
| --- | --- |
| `dataHamArbSemi.dat` | Raw measurements for the construction in this paper |
| `dataMalAtmostT.dat` | Raw measurements for the AB24/CCS24 construction |
| `figure2_ours.csv` | Parsed and normalized measurements for our construction |
| `figure2_ab24.csv` | Parsed and normalized measurements for AB24/CCS24 |
| `figure2.pdf` | Publication-quality vector version of Figure 2 |
| `figure2.png` | Raster preview of Figure 2 |

The three Figure 2 panels show:

1. Conditional-decryption time as a function of message length `n`, with one curve per Hamming-distance value.
2. Conditional-decryption time as a function of Hamming distance, with one curve per message length.
3. Regular and conditional ciphertext sizes.

To open the generated PDF on a Linux desktop:

```bash
xdg-open artifact/results/reproduced/figure2.pdf
```

On a headless machine or container, copy `figure2.pdf` or `figure2.png` to the host and open it there.

## Recovering from a plotting-only failure

If both C++ tests complete but the shell script stops because Matplotlib is missing, do not rerun the expensive benchmarks. Install Matplotlib and invoke only the plotting program:

```bash
apt-get update
apt-get install -y python3-matplotlib

python3 artifact/scripts/generate_figure2.py \
  --ours artifact/results/reproduced/dataHamArbSemi.dat \
  --ab24 artifact/results/reproduced/dataMalAtmostT.dat \
  --output-dir artifact/results/reproduced
```

For non-root users, prefix the package-installation commands with `sudo`.

If the raw files were generated in the build directory but were not copied before the script stopped, copy them first:

```bash
mkdir -p artifact/results/reproduced

cp build/testCondEncArbHam/dataHamArbSemi.dat \
   artifact/results/reproduced/

cp build/testCondEncArbHam/dataMalAtmostT.dat \
   artifact/results/reproduced/
```

Then run `generate_figure2.py` using the command above.

## Troubleshooting

### `ModuleNotFoundError: No module named 'matplotlib'`

Install Matplotlib for the same Python interpreter used to run the plotting script:

```bash
apt-get install -y python3-matplotlib
python3 -c "import matplotlib; print(matplotlib.__version__)"
```

### The plot contains duplicate points

The C++ benchmarks append to their output files. Remove the previous `.dat` files and rerun the tests:

```bash
rm -f build/testCondEncArbHam/dataHamArbSemi.dat \
      build/testCondEncArbHam/dataMalAtmostT.dat
```

### The test executable cannot be found

Rebuild the project from the repository root:

```bash
./build.sh
```

Then verify:

```bash
ls -l build/testCondEncArbHam/tests
```

---


## Citation

The full version of the paper is available at:

```text
https://eprint.iacr.org/2026/653
```
