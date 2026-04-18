#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
BUILD_TYPE="${1:-Debug}"

echo "=== Building LegalCrime (${BUILD_TYPE}) ==="

# Configure
cmake -S "${SCRIPT_DIR}" -B "${BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DCMAKE_TOOLCHAIN_FILE="${VCPKG_ROOT:-${HOME}/vcpkg}/scripts/buildsystems/vcpkg.cmake" \
    2>&1

# Build
cmake --build "${BUILD_DIR}" --config "${BUILD_TYPE}" -j "$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)"

echo "=== Build complete ==="
echo "Executable: ${BUILD_DIR}/LegalCrime"
