#!/usr/bin/env bash
# Download and archive Calypsi release binaries and documentation.
# Run from the repo root: bash vendor/calypsi/fetch.sh [VERSION]
#
# Files land in vendor/calypsi/<VERSION>/ — gitignored (proprietary binaries).
# Commit this script and the generated manifest; not the packages themselves.
set -euo pipefail

VERSION="${1:-5.17}"
BASE="https://github.com/hth313/Calypsi-tool-chains/releases/download/${VERSION}"
DEST="$(dirname "$0")/${VERSION}"
mkdir -p "$DEST"

TARGETS=(65816 68000 6502 nut)
DOCS=(
    Calypsi6502Guide.pdf
    Calypsi65816Guide.pdf
    Calypsi68000Guide.pdf
    CalypsiNutDebuggerGuide.pdf
    CalypsiNutGuide.pdf
    ReleaseNotes-6502.md
    ReleaseNotes-65816.md
    ReleaseNotes-68000.md
    ReleaseNotes-Nut.md
)

echo "==> Fetching Calypsi ${VERSION} to ${DEST}/"

for t in "${TARGETS[@]}"; do
    for ext in deb "pkg.tar.zst" rpm zip; do
        case "$ext" in
            "pkg.tar.zst") fname="calypsi-${t}-${VERSION}-1-x86_64.pkg.tar.zst" ;;
            rpm)            fname="calypsi-${t}-${VERSION}-1.x86_64.rpm" ;;
            *)              fname="calypsi-${t}-${VERSION}.${ext}" ;;
        esac
        # nut has no rpm in 5.17; skip gracefully
        out="$DEST/$fname"
        if [ -f "$out" ]; then
            echo "  skip  $fname (already present)"
        else
            echo "  fetch $fname"
            curl -fsSL --retry 3 -o "$out" "${BASE}/${fname}" || {
                echo "  warn  ${fname} not available — skipping"
                rm -f "$out"
            }
        fi
    done
done

for doc in "${DOCS[@]}"; do
    out="$DEST/$doc"
    if [ -f "$out" ]; then
        echo "  skip  $doc (already present)"
    else
        echo "  fetch $doc"
        curl -fsSL --retry 3 -o "$out" "${BASE}/${doc}" || {
            echo "  warn  ${doc} not available — skipping"
            rm -f "$out"
        }
    fi
done

# Generate a SHA-256 manifest
MANIFEST="$DEST/SHA256SUMS"
echo "==> Writing manifest ${MANIFEST}"
(cd "$DEST" && sha256sum -- * | grep -v 'SHA256SUMS' > SHA256SUMS || true)

# Mirror-clone the open-source tool-chains repo (website + release metadata)
MIRROR="$(dirname "$0")/tool-chains.git"
if [ -d "$MIRROR" ]; then
    echo "==> Updating tool-chains mirror"
    git -C "$MIRROR" remote update
else
    echo "==> Cloning tool-chains mirror"
    git clone --mirror https://github.com/hth313/Calypsi-tool-chains.git "$MIRROR"
fi

echo "==> Done. Archive in vendor/calypsi/${VERSION}/"
echo "    Commit the SHA256SUMS manifest and PDFs; binary packages are gitignored."
