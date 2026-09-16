#!/usr/bin/env bash
set -Eeuo pipefail

# Install all RRSS artifact dependencies on a fresh Ubuntu 24.04 system.
# Run this script from anywhere inside the cloned RRSS repository.

PROTOBUF_VERSION="3.20.3"
SAFEHERON_COMMIT="c05c0907ac6875ce4e1f5bbb4fcababcca5c71cf"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
if [[ -f "${SCRIPT_DIR}/../../CMakeLists.txt" ]]; then
    REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
elif [[ -f "${PWD}/CMakeLists.txt" ]]; then
    REPO_ROOT="${PWD}"
else
    echo "Error: run this script from the RRSS repository, or place it in artifact/scripts/." >&2
    exit 1
fi

if [[ ! -r /etc/os-release ]]; then
    echo "Error: cannot identify the operating system." >&2
    exit 1
fi

# shellcheck disable=SC1091
source /etc/os-release
if [[ "${ID:-}" != "ubuntu" || "${VERSION_ID:-}" != "24.04" ]]; then
    echo "Error: this installer supports Ubuntu 24.04 only." >&2
    echo "Detected: ${PRETTY_NAME:-unknown operating system}" >&2
    exit 1
fi

if [[ "$(id -u)" -eq 0 ]]; then
    APT=(apt-get)
    INSTALL=(cmake --install)
    LDCONFIG=(ldconfig)
else
    if ! command -v sudo >/dev/null 2>&1; then
        echo "Error: run as root or install sudo." >&2
        exit 1
    fi
    APT=(sudo apt-get)
    INSTALL=(sudo cmake --install)
    LDCONFIG=(sudo ldconfig)
fi

BUILD_JOBS="$(nproc 2>/dev/null || echo 2)"
BUILD_TMP="$(mktemp -d)"
trap 'rm -rf -- "${BUILD_TMP}"' EXIT

echo "[1/5] Installing Ubuntu packages..."
export DEBIAN_FRONTEND=noninteractive
"${APT[@]}" update
"${APT[@]}" install -y --no-install-recommends \
    build-essential \
    ca-certificates \
    catch2 \
    cmake \
    curl \
    git \
    gnuplot-nox \
    libargon2-dev \
    libcrypto++-dev \
    libgmp-dev \
    libntl-dev \
    libssl-dev \
    libzstd-dev \
    ninja-build \
    pkg-config \
    pybind11-dev \
    python3 \
    python3-cryptography \
    python3-matplotlib \
    python3-dev \
    python3-numpy \
    python3-pip \
    python3-venv \
    unzip \

    zlib1g-dev

echo "[2/5] Creating the repository-local Python environment..."
python3 -m venv --system-site-packages "${REPO_ROOT}/.venv"
"${REPO_ROOT}/.venv/bin/python" -m pip install --upgrade pip
"${REPO_ROOT}/.venv/bin/python" -m pip install "pycryptodome==3.20.0"

echo "[3/5] Building Protocol Buffers ${PROTOBUF_VERSION}..."
git clone --depth 1 --branch "v${PROTOBUF_VERSION}" \
    --recurse-submodules --shallow-submodules \
    https://github.com/protocolbuffers/protobuf.git \
    "${BUILD_TMP}/protobuf"

cmake -S "${BUILD_TMP}/protobuf/cmake" \
    -B "${BUILD_TMP}/protobuf-build" \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
    -Dprotobuf_BUILD_EXAMPLES=OFF \
    -Dprotobuf_BUILD_SHARED_LIBS=ON \
    -Dprotobuf_BUILD_TESTS=OFF

cmake --build "${BUILD_TMP}/protobuf-build" --parallel "${BUILD_JOBS}"
"${INSTALL[@]}" "${BUILD_TMP}/protobuf-build"
"${LDCONFIG[@]}"

if [[ "$(/usr/local/bin/protoc --version)" != "libprotoc ${PROTOBUF_VERSION}" ]]; then
    echo "Error: Protocol Buffers ${PROTOBUF_VERSION} was not installed correctly." >&2
    exit 1
fi

echo "[4/5] Building the pinned SafeheronCryptoSuites revision..."
mkdir -p "${BUILD_TMP}/safeheron"
git -C "${BUILD_TMP}/safeheron" init
git -C "${BUILD_TMP}/safeheron" remote add origin \
    https://github.com/Safeheron/safeheron-crypto-suites-cpp.git
git -C "${BUILD_TMP}/safeheron" fetch --depth 1 origin "${SAFEHERON_COMMIT}"
git -C "${BUILD_TMP}/safeheron" checkout --detach FETCH_HEAD

cmake -S "${BUILD_TMP}/safeheron" \
    -B "${BUILD_TMP}/safeheron-build" \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DCMAKE_PREFIX_PATH=/usr/local \
    -DOPENSSL_ROOT_DIR=/usr \
    -DBUILD_SHARED_LIBS=ON \
    -DENABLE_TESTS=OFF

cmake --build "${BUILD_TMP}/safeheron-build" --parallel "${BUILD_JOBS}"
"${INSTALL[@]}" "${BUILD_TMP}/safeheron-build"
"${LDCONFIG[@]}"

echo "[5/5] Verifying the installation..."
test -f /usr/local/lib/libSafeheronCryptoSuites.so
test -f /usr/local/include/crypto-suites/crypto-bn/bn.h
"${REPO_ROOT}/.venv/bin/python" -c \
    "import Crypto.Util.number, cryptography, numpy; print('Python dependencies: OK')"

echo
echo "All RRSS dependencies were installed successfully."
echo "Next commands:"
echo "  cd ${REPO_ROOT}"
echo "  source .venv/bin/activate"
echo "  ./build.sh"
