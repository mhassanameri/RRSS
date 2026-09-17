#!/usr/bin/env bash
#
# Reproduce Table 1 results for the RRSS-fPAKE implementation from our paper. Like the prior work, we consider
# two securiy level 128, 244. In the following we show how to use this file.
#
# Usage:
#   bash artifact/scripts/reproduce_table1_RRSSFPAKE.sh 128
#   bash artifact/scripts/reproduce_table1_RRSSFPAKE.sh 244

set -Eeuo pipefail

LEVEL="${1:-}"

if [[ "$LEVEL" != "128" && "$LEVEL" != "244" ]]; then
    echo "Usage: bash artifact/scripts/reproduce_table1_RRSSFPAKE.sh {128|244}"
    exit 1
fi

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BENCH_DIR="$ROOT_DIR/external/fPAKE-benchmark/fPAKE"
STATS_DIR="$ROOT_DIR/external/fPAKE-benchmark"
BUILD_DIR="$ROOT_DIR/cmake-build-py312"
OUT_DIR="$ROOT_DIR/artifact/results/reproduced/RRSSFPAKE-${LEVEL}"

CONFIG_FILE="$BENCH_DIR/config${LEVEL}.ini"

mkdir -p "$OUT_DIR"

export PYTHONPATH="$BUILD_DIR${PYTHONPATH:+:$PYTHONPATH}"

BUILD_DIR="$ROOT_DIR/cmake-build-release"

export PYTHONPATH="$BUILD_DIR${PYTHONPATH:+:$PYTHONPATH}"

echo "Checking the RRSS Python module (for being called as the generated libraray from the C++ complied project) ..."
python3 -c "import rrss; print('RRSS module:', rrss.__file__)"

cd "$BENCH_DIR"

echo "Starting RRSS-fPAKE Receiver for lambda = $LEVEL ..."
python3 benchmarkRRSS.py Receiver --config "$CONFIG_FILE" \
    > "$OUT_DIR/receiver.log" 2>&1 &
RECEIVER_PID=$!

cleanup() {
    if kill -0 "$RECEIVER_PID" 2>/dev/null; then
        kill "$RECEIVER_PID" 2>/dev/null || true
    fi
}
trap cleanup EXIT

sleep 2

if ! kill -0 "$RECEIVER_PID" 2>/dev/null; then
    echo "Receiver stopped unexpectedly. See $OUT_DIR/receiver.log"
    exit 1
fi

echo "Starting RRSS-fPAKE Sender for lambda = $LEVEL ..."
python3 benchmarkRRSS.py Sender --config "$CONFIG_FILE" \
    | tee "$OUT_DIR/sender.log"

echo "Waiting for the Receiver ..."
wait "$RECEIVER_PID"

echo "Computing RRSS-fPAKE statistics ..."
cd "$STATS_DIR"
python3 statisticBenchmarkRRSSfPAKE.py "$LEVEL" \
    | tee "$OUT_DIR/statistics.txt"

echo "RRSS-fPAKE benchmark for lambda = $LEVEL completed."