#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"

# Build first if needed
if [ ! -f "${BUILD_DIR}/LegalCrimeTests" ]; then
    echo "Tests not built yet, building..."
    "${SCRIPT_DIR}/build.sh" Debug
fi

echo "=== Running LegalCrime Tests ==="
"${BUILD_DIR}/LegalCrimeTests"
EXIT_CODE=$?

if [ ${EXIT_CODE} -eq 0 ]; then
    echo "=== All tests passed ==="
else
    echo "=== Some tests failed ==="
fi

exit ${EXIT_CODE}
