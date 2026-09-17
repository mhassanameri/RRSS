#!/usr/bin/env bash
set -euo pipefail

ZENODO_RECORD="22802638"
ARCHIVE="RRSS-CCS2026-Artifact.zip"
EXPECTED_SHA256="b555f60328e209173fa565f5058412ad2956f8e51cee6767dd1652140ac0ce9d"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
TARGET_DIR="${REPO_ROOT}/external/fPAKE-benchmark"
TEMP_DIR="$(mktemp -d)"

trap 'rm -rf "${TEMP_DIR}"' EXIT

curl -L --fail --retry 3 \
  "https://zenodo.org/records/${ZENODO_RECORD}/files/${ARCHIVE}?download=1" \
  -o "${TEMP_DIR}/${ARCHIVE}"

echo "${EXPECTED_SHA256}  ${TEMP_DIR}/${ARCHIVE}" | sha256sum --check -

unzip -q "${TEMP_DIR}/${ARCHIVE}" \
  "RRSS-CCS2026-Artifact/external/fPAKE-benchmark/results128/*" \
  "RRSS-CCS2026-Artifact/external/fPAKE-benchmark/results244/*" \
  -d "${TEMP_DIR}"

cp -a \
  "${TEMP_DIR}/RRSS-CCS2026-Artifact/external/fPAKE-benchmark/results128" \
  "${TARGET_DIR}/"

cp -a \
  "${TEMP_DIR}/RRSS-CCS2026-Artifact/external/fPAKE-benchmark/results244" \
  "${TARGET_DIR}/"

echo "RSS-fPAKE data installed successfully:"
echo "  ${TARGET_DIR}/results128"
echo "  ${TARGET_DIR}/results244"