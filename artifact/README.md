# Random Robust Secret Sharing with Perfect Privacy and its Applications

This repository contains the implementation artifact for the ACM CCS 2026 paper:

> **Random Robust Secret Sharing with Perfect Privacy and its Applications**

The artifact currently supports reproduction of the fPAKE results reported in **Table 1**, including a comparison between:

* **RSS-fPAKE**, the prior RSS-based baseline; and
* **RRSS-fPAKE**, our implementation based on Random Robust Secret Sharing (RRSS).

The two implementations are evaluated on the same fPAKE inputs and parameter settings. The benchmark reports separate Sender and Receiver computation time, communication time, overall time, and communication overhead.

## Artifact overview

The artifact provides:

* A C++ implementation of Random Robust Secret Sharing.
* A Python binding, `rrss`, used by the RRSS-fPAKE benchmark.
* An implementation of the prior RSS-fPAKE baseline.
* An implementation of RRSS-fPAKE.
* Fixed benchmark configurations for security levels 128 and 244.
* Scripts that automatically start the Receiver and Sender locally, collect results, and run the corresponding statistics program.

## Repository layout

```text
.
├── artifact/
│   ├── scripts/
│   │   ├── reproduce_table1_RSSFPAKE.sh
│   │   └── reproduce_table1_RRSSFPAKE.sh
│   └── results/
│       └── reproduced/
├── external/
│   └── fPAKE-benchmark/
│       ├── statisticBenchmarkRSSfPAKE.py
│       ├── statisticBenchmarkRRSSfPAKE.py
│       └── fPAKE/
│           ├── benchmarkRSS.py
│           ├── benchmarkRRSS.py
│           ├── fPAKERSS.py
│           ├── fPAKERRSS.py
│           ├── config128.ini
│           └── config244.ini
├── RandomRobustSS.cpp
├── bindings.cpp
├── CMakeLists.txt
└── build.sh
```

## Requirements

The artifact was tested on Linux with:

* CMake and a C++20-compatible compiler
* Python 3 with development headers
* GMP, NTL, Crypto++, Protocol Buffers, pybind11, and Catch2
* The remaining native dependencies required by the top-level CMake project

Build RRSS and its Python binding from the repository root:

```bash
bash build.sh
```

The RRSS-fPAKE benchmark imports the generated Python module `rrss`. The module must be compiled for the same Python version used to run the benchmark.

Before running the RRSS-fPAKE benchmark manually, verify that Python can import the module:

```bash
export PYTHONPATH="$PWD/cmake-build-release${PYTHONPATH:+:$PYTHONPATH}"
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

## Reproducing Table 1: fPAKE Performance Evaluation
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

The CSV file contains the reproduced results in a machine-readable format. The Markdown file (`reproduced\table1.md`) is rendered directly by GitHub and follows the organization of Table 1 in the paper, including the algorithm, security level, input type, role, calculation time, network time, overall time, role communication overhead, and total communication overhead.

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

## Conditional-encryption experiments

Documentation and scripts for reproducing the conditional-encryption results will be added in the next artifact section.

## Citation

The full version of the paper is available at:

```text
https://eprint.iacr.org/2026/653
```
