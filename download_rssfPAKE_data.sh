#!/usr/bin/env bash
set -euo pipefail

ZENODO_RECORD="REPLACE_WITH_YOUR_RECORD_NUMBER"
ARCHIVE="RSSFPAKE-prepared-data-v1.tar.gz"
EXPECTED_SHA256="REPLACE_WITH_THE_SHA256_VALUE"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
TARGET_DIR="${REPO_ROOT}/external/fPAKE-benchmark"
TEMP_DIR="$(mktemp -d)"

trap 'rm -rf "${TEMP_DIR}"' EXIT

curl -L --fail --retry 3 \
  "https://zenodo.org/records/${ZENODO_RECORD}/files/${ARCHIVE}?download=1" \
  -o "${TEMP_DIR}/${ARCHIVE}"

echo "${EXPECTED_SHA256}  ${TEMP_DIR}/${ARCHIVE}" |
  sha256sum --check -

tar -xzf "${TEMP_DIR}/${ARCHIVE}" -C "${TARGET_DIR}"

echo "RSS-fPAKE data installed successfully:"
echo "  ${TARGET_DIR}/results128"
echo "  ${TARGET_DIR}/results244"