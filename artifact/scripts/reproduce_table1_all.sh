#!/usr/bin/env bash
set -Eeuo pipefail

# Run from anywhere after installing dependencies and building the artifact.
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"

if [[ -f "${SCRIPT_DIR}/rrss_environment.sh" ]]; then
    source "${SCRIPT_DIR}/rrss_environment.sh"
fi

cd "${REPO_ROOT}"
trap 'echo "Table 1 reproduction failed at line ${LINENO}. See the preceding error and experiment logs." >&2' ERR

for filename in \
    reproduce_table1_RSSFPAKE.sh \
    reproduce_table1_RRSSFPAKE.sh \
    generate_table1.py
do
    if [[ ! -f "${SCRIPT_DIR}/${filename}" ]]; then
        echo "Missing required script: ${SCRIPT_DIR}/${filename}" >&2
        exit 1
    fi
done

echo "[1/5] Running RSS-fPAKE at security level 128..."
bash "${SCRIPT_DIR}/reproduce_table1_RSSFPAKE.sh" 128

echo "[2/5] Running RRSS-fPAKE at security level 128..."
bash "${SCRIPT_DIR}/reproduce_table1_RRSSFPAKE.sh" 128

echo "[3/5] Running RSS-fPAKE at security level 244..."
bash "${SCRIPT_DIR}/reproduce_table1_RSSFPAKE.sh" 244

echo "[4/5] Running RRSS-fPAKE at security level 244..."
bash "${SCRIPT_DIR}/reproduce_table1_RRSSFPAKE.sh" 244

echo "[5/5] Generating Table 1..."
/usr/bin/python3 "${SCRIPT_DIR}/generate_table1.py"

test -s "${REPO_ROOT}/artifact/results/reproduced/table1.csv"
test -s "${REPO_ROOT}/artifact/results/reproduced/table1.md"

echo
echo "Table 1 reproduction completed successfully."
echo "Outputs:"
echo "  ${REPO_ROOT}/artifact/results/reproduced/table1.csv"
echo "  ${REPO_ROOT}/artifact/results/reproduced/table1.md"
