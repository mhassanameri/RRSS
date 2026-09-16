#!/usr/bin/env bash
# Reproduce Figure 2 (arbitrary Hamming-distance CE evaluation).
#
# Run from anywhere:
#   bash artifact/scripts/reproduce_figure2.sh
#
# Optional: override the CMake build directory:
#   RRSS_BUILD_DIR=/path/to/RRSS/build/testCondEncArbHam \
#     bash artifact/scripts/reproduce_figure2.sh

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
BUILD_DIR="${RRSS_BUILD_DIR:-${REPO_ROOT}/build/testCondEncArbHam}"
TEST_BINARY="${BUILD_DIR}/tests"
RESULT_DIR="${REPO_ROOT}/artifact/results/reproduced"
PLOT_SCRIPT="${SCRIPT_DIR}/generate_figure2.py"

OURS_RAW="${BUILD_DIR}/dataHamArbSemi.dat"
AB24_RAW="${BUILD_DIR}/dataMalAtmostT.dat"

fail() { echo "Error: $*" >&2; exit 1; }

[[ -x "${TEST_BINARY}" ]] || fail "Cannot execute ${TEST_BINARY}. Build the project first or set RRSS_BUILD_DIR."
[[ -f "${PLOT_SCRIPT}" ]] || fail "Missing plotting script: ${PLOT_SCRIPT}"

mkdir -p "${RESULT_DIR}"

# Timestamps make sure that the data copied below were produced by this run,
# rather than being stale files from an earlier invocation.
MARKER="$(mktemp)"
trap 'rm -f "${MARKER}"' EXIT

echo "[1/3] Running our arbitrary-Hamming-distance evaluation..."
touch "${MARKER}"
( cd "${BUILD_DIR}" && ./tests "HDArbSemiEval" )
[[ -f "${OURS_RAW}" && "${OURS_RAW}" -nt "${MARKER}" ]] || \
  fail "Expected a newly generated ${OURS_RAW} after HDArbSemiEval."
cp "${OURS_RAW}" "${RESULT_DIR}/dataHamArbSemi.dat"

echo "[2/3] Running the AB24 comparison evaluation..."
touch "${MARKER}"
( cd "${BUILD_DIR}" && ./tests "HDAtmostTSemiEval" )
[[ -f "${AB24_RAW}" && "${AB24_RAW}" -nt "${MARKER}" ]] || \
  fail "Expected a newly generated ${AB24_RAW} after HDAtmostTSemiEval."
cp "${AB24_RAW}" "${RESULT_DIR}/dataMalAtmostT.dat"

echo "[3/3] Creating normalized CSV files and Figure 2..."
python3 "${PLOT_SCRIPT}" \
  --ours "${RESULT_DIR}/dataHamArbSemi.dat" \
  --ab24 "${RESULT_DIR}/dataMalAtmostT.dat" \
  --output-dir "${RESULT_DIR}"

echo
echo "Figure 2 reproduced successfully:"
echo "  ${RESULT_DIR}/figure2.pdf"
echo "  ${RESULT_DIR}/figure2.png"
echo "  ${RESULT_DIR}/figure2_ours.csv"
echo "  ${RESULT_DIR}/figure2_ab24.csv"
