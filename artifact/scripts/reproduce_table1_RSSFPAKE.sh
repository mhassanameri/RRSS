#!/usr/bin/env bash
#
# Reproduce Table 1 results for the RRSS-fPAKE implementation from our paper. Like the prior work, we consider
# two security level 128, 244. In the following we show how to use this file. For the ease of Artifact Evaluation
# reviewers we provided this .sh script to make the result more straightforward. Based on the security leve (128 | 244)
# this script runs two parallel benchmark, one for sender and the other one as receiver and these two program will be
# communicate and we measure execution times and the communication overheads and stors the average values in the
# "statistics.txt" generated for each execution as the output file.
#
#
# Usage:
#   bash artifact/scripts/reproduce_table1_RSSFPAKE.sh 128
#   bash artifact/scripts/reproduce_table1_RSSFPAKE.sh 244

set -Eeuo pipefail

LEVEL="${1:-}"

if [[ "$LEVEL" != "128" && "$LEVEL" != "244" ]]; then
    echo "Usage: bash artifact/scripts/reproduce_table1_RSSFPAKE.sh {128|244}"
    exit 1
fi

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BENCH_DIR="$ROOT_DIR/external/fPAKE-benchmark/fPAKE"
STATS_DIR="$ROOT_DIR/external/fPAKE-benchmark"
OUT_DIR="$ROOT_DIR/artifact/results/reproduced/RSSFPAKE-${LEVEL}"

CONFIG_FILE="$BENCH_DIR/config${LEVEL}.ini"

mkdir -p "$OUT_DIR"

cd "$BENCH_DIR"

echo "Starting RSS-fPAKE Receiver for lambda = $LEVEL ..."
python3 benchmarkRSS.py Receiver --config "$CONFIG_FILE" \
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

echo "Starting RSS-fPAKE Sender for lambda = $LEVEL ..."
python3 benchmarkRSS.py Sender --config "$CONFIG_FILE" \
    | tee "$OUT_DIR/sender.log"

echo "Waiting for the Receiver ..."
wait "$RECEIVER_PID"

echo "Computing RSS-fPAKE statistics ..."
cd "$STATS_DIR"
python3 statisticBenchmarkRSSfPAKE.py "$LEVEL" \
    | tee "$OUT_DIR/statistics.txt"

echo "RSS-fPAKE benchmark for lambda = $LEVEL completed."