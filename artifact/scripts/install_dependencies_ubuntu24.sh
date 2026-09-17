#!/usr/bin/env bash
set -Eeuo pipefail

# RRSS artifact dependencies for Ubuntu 24.04.
# Uses /usr/bin/python3 and Ubuntu Python packages. No venv or pip.
# Place this file in artifact/scripts/ and run with bash.
PROTOBUF_VERSION="3.20.3"
SAFEHERON_COMMIT="c05c0907ac6875ce4e1f5bbb4fcababcca5c71cf"

# Keep Conda and active Python environments out of dependency builds.
unset PYTHONHOME PYTHONPATH VIRTUAL_ENV CONDA_PREFIX
unset CC CXX CPP LD AR AS NM RANLIB
unset CFLAGS CXXFLAGS CPPFLAGS LDFLAGS
unset CMAKE_PREFIX_PATH CMAKE_TOOLCHAIN_FILE CMAKE_GENERATOR
unset LIBRARY_PATH CPATH C_INCLUDE_PATH CPLUS_INCLUDE_PATH LD_LIBRARY_PATH
export PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
export CC="/usr/bin/gcc"
export CXX="/usr/bin/g++"
hash -r

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
if [[ -f "${SCRIPT_DIR}/../../CMakeLists.txt" ]]; then
    REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
elif [[ -f "${PWD}/CMakeLists.txt" ]]; then
    REPO_ROOT="${PWD}"
else
    echo "Error: place this script in artifact/scripts/ or run from the repository root." >&2
    exit 1
fi

if [[ ! -r /etc/os-release ]]; then
    echo "Error: cannot identify the operating system." >&2
    exit 1
fi
# shellcheck disable=SC1091
source /etc/os-release
if [[ "${ID:-}" != "ubuntu" || "${VERSION_ID:-}" != "24.04" ]]; then
    echo "Error: Ubuntu 24.04 is required. Detected: ${PRETTY_NAME:-unknown}" >&2
    exit 1
fi

PRIV=()
if [[ "$(id -u)" -ne 0 ]]; then
    if ! command -v sudo >/dev/null 2>&1; then
        echo "Error: run as root or install sudo." >&2
        exit 1
    fi
    PRIV=(sudo)
fi

# Default to at most four jobs to limit memory use in Docker.
BUILD_JOBS="${BUILD_JOBS:-$(nproc)}"
if [[ ! "${BUILD_JOBS}" =~ ^[1-9][0-9]*$ ]]; then
    echo "Error: BUILD_JOBS must be a positive integer." >&2
    exit 1
fi
if (( BUILD_JOBS > 4 )); then BUILD_JOBS=4; fi

BUILD_TMP="$(mktemp -d)"
trap 'rm -rf -- "${BUILD_TMP}"' EXIT
trap 'echo "Installation failed at line ${LINENO}. See the preceding error." >&2' ERR

echo "[1/5] Installing Ubuntu packages..."
"${PRIV[@]}" env DEBIAN_FRONTEND=noninteractive apt-get update
"${PRIV[@]}" env DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
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
    libgtest-dev \
    libntl-dev \
    libssl-dev \
    libzstd-dev \
    ninja-build \
    pkg-config \
    pybind11-dev \
    python3 \
    python3-cryptography \
    python3-dev \
    python3-matplotlib \
    python3-numpy \
    python3-pycryptodome \
    unzip \
    zip \
    zlib1g-dev

echo "[2/5] Building Protocol Buffers ${PROTOBUF_VERSION}..."
git clone --depth 1 --branch "v${PROTOBUF_VERSION}" \
    --recurse-submodules --shallow-submodules \
    https://github.com/protocolbuffers/protobuf.git \
    "${BUILD_TMP}/protobuf"

cmake -S "${BUILD_TMP}/protobuf/cmake" \
    -B "${BUILD_TMP}/protobuf-build" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER=/usr/bin/gcc \
    -DCMAKE_CXX_COMPILER=/usr/bin/g++ \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
    -Dprotobuf_BUILD_EXAMPLES=OFF \
    -Dprotobuf_BUILD_SHARED_LIBS=ON \
    -Dprotobuf_BUILD_TESTS=OFF

cmake --build "${BUILD_TMP}/protobuf-build" --parallel "${BUILD_JOBS}"
"${PRIV[@]}" /usr/bin/cmake --install "${BUILD_TMP}/protobuf-build"
"${PRIV[@]}" /sbin/ldconfig

if [[ "$(/usr/local/bin/protoc --version)" != "libprotoc ${PROTOBUF_VERSION}" ]]; then
    echo "Error: Protocol Buffers version verification failed." >&2
    exit 1
fi

echo "[3/5] Building the pinned SafeheronCryptoSuites revision..."
mkdir -p "${BUILD_TMP}/safeheron"
git -C "${BUILD_TMP}/safeheron" init
git -C "${BUILD_TMP}/safeheron" remote add origin \
    https://github.com/Safeheron/safeheron-crypto-suites-cpp.git
git -C "${BUILD_TMP}/safeheron" fetch --depth 1 origin "${SAFEHERON_COMMIT}"
git -C "${BUILD_TMP}/safeheron" checkout --detach FETCH_HEAD

cmake -S "${BUILD_TMP}/safeheron" \
    -B "${BUILD_TMP}/safeheron-build" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER=/usr/bin/gcc \
    -DCMAKE_CXX_COMPILER=/usr/bin/g++ \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DCMAKE_PREFIX_PATH=/usr/local \
    -DProtobuf_PROTOC_EXECUTABLE=/usr/local/bin/protoc \
    -DProtobuf_INCLUDE_DIR=/usr/local/include \
    -DProtobuf_LIBRARY=/usr/local/lib/libprotobuf.so \
    -DOPENSSL_ROOT_DIR=/usr \
    -DBUILD_SHARED_LIBS=ON \
    -DENABLE_TESTS=OFF

cmake --build "${BUILD_TMP}/safeheron-build" --parallel "${BUILD_JOBS}"
"${PRIV[@]}" /usr/bin/cmake --install "${BUILD_TMP}/safeheron-build"
"${PRIV[@]}" /sbin/ldconfig

echo "[4/5] Configuring artifact build and runtime scripts..."

# This file is sourced by build/reproduction scripts, so settings take effect
# even when the calling terminal still has a virtual environment activated.
cat > "${REPO_ROOT}/artifact/scripts/rrss_environment.sh" <<'ENV'
#!/usr/bin/env bash
unset PYTHONHOME PYTHONPATH VIRTUAL_ENV CONDA_PREFIX
unset CC CXX CPP LD AR AS NM RANLIB
unset CFLAGS CXXFLAGS CPPFLAGS LDFLAGS
unset CMAKE_PREFIX_PATH CMAKE_TOOLCHAIN_FILE CMAKE_GENERATOR
unset LIBRARY_PATH CPATH C_INCLUDE_PATH CPLUS_INCLUDE_PATH LD_LIBRARY_PATH
export PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
export CC=/usr/bin/gcc
export CXX=/usr/bin/g++
export MPLBACKEND=Agg
RRSS_ENV_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
export PYTHONPATH="${RRSS_ENV_ROOT}/build"
hash -r
ENV

# Preserve the supplied build script for reference before replacing it.
if [[ -f "${REPO_ROOT}/build.sh" && ! -e "${REPO_ROOT}/build.sh.before-system-python" ]]; then
    cp -p "${REPO_ROOT}/build.sh" "${REPO_ROOT}/build.sh.before-system-python"
fi
cat > "${REPO_ROOT}/build.sh" <<'BUILD'
#!/usr/bin/env bash
set -Eeuo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${ROOT}/artifact/scripts/rrss_environment.sh"
JOBS="${BUILD_JOBS:-4}"
[[ "${JOBS}" =~ ^[1-9][0-9]*$ ]] || { echo "Invalid BUILD_JOBS" >&2; exit 1; }

# Preserve an incompatible copied/old build instead of reusing its cache.
if [[ -e "${ROOT}/build" || -L "${ROOT}/build" ]]; then
    if [[ ! -f "${ROOT}/build/.rrss-system-build" ]] ||
       [[ "$(cat "${ROOT}/build/.rrss-system-build")" != "${ROOT}" ]]; then
        BACKUP="$(mktemp -d "${ROOT}/build-backup.XXXXXX")"
        mv -- "${ROOT}/build" "${BACKUP}/build"
        echo "Previous build preserved in ${BACKUP}/build"
    fi
fi

cmake -S "${ROOT}" -B "${ROOT}/build" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER=/usr/bin/gcc \
    -DCMAKE_CXX_COMPILER=/usr/bin/g++ \
    -DPython3_EXECUTABLE=/usr/bin/python3 \
    -DPYTHON_EXECUTABLE=/usr/bin/python3
cmake --build "${ROOT}/build" --parallel "${JOBS}"
printf '%s\n' "${ROOT}" > "${ROOT}/build/.rrss-system-build"
/usr/bin/python3 -c "import rrss; print('RRSS binding:', rrss.__file__)"
echo "Build and Python import succeeded."
BUILD
chmod +x "${REPO_ROOT}/build.sh"

# Add the shared environment to existing reproduction scripts without
# replacing their benchmark logic. Repeated installer runs are idempotent.
/usr/bin/python3 - "${REPO_ROOT}" <<'PATCH'
import pathlib
import sys

root = pathlib.Path(sys.argv[1])
scripts = root / "artifact/scripts"
hook = 'source "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/rrss_environment.sh"'
for path in sorted(scripts.glob("reproduce*.sh")):
    text = path.read_text()
    if hook not in text:
        backup = path.with_name(path.name + ".before-system-python")
        if not backup.exists():
            backup.write_text(text)
        lines = text.splitlines(keepends=True)
        index = 1 if lines and lines[0].startswith("#!") else 0
        lines.insert(index, hook + "\n")
        path.write_text("".join(lines))

# Ubuntu packages use the Cryptodome namespace. Update direct imports;
# existing Crypto/Cryptodome fallback blocks remain valid.
for path in (root / "external/fPAKE-benchmark").rglob("*.py"):
    original = path.read_text()
    updated = original.replace("import Crypto.", "import Cryptodome.")
    updated = updated.replace("from Crypto.", "from Cryptodome.")
    if updated != original:
        backup = path.with_name(path.name + ".before-system-python")
        if not backup.exists():
            backup.write_text(original)
        path.write_text(updated)
PATCH

bash -n "${REPO_ROOT}/build.sh"
for script in "${REPO_ROOT}"/artifact/scripts/reproduce*.sh; do
    [[ -f "${script}" ]] && bash -n "${script}"
done

echo "[5/5] Verifying the installation..."
test -f /usr/local/lib/libSafeheronCryptoSuites.so
test -f /usr/local/include/crypto-suites/crypto-bn/bn.h
/usr/bin/python3 -c \
    "import Cryptodome.Util.number, cryptography, numpy, matplotlib; print('System Python dependencies: OK')"

echo
echo "All RRSS dependencies were installed successfully."
echo "Python interpreter: /usr/bin/python3"
echo "No virtual-environment activation or manual PYTHONPATH is required."
echo "Next commands (from the repository root):"
echo "  bash build.sh"
echo "  bash artifact/scripts/reproduce_table1_RSSFPAKE.sh 128"
echo "  bash artifact/scripts/reproduce_table1_RRSSFPAKE.sh 128"
echo "  bash artifact/scripts/reproduce_figure2.sh"
