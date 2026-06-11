#!/bin/bash
# Copy the built external into the package, ad-hoc sign it, zip, and install
# into the user's Max Packages folder(s). Invoked from the Xcode post-build phase.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
DSTROOT="${DSTROOT:-/tmp/max-ble.dst}"
MXO_SRC="${DSTROOT}/max-package/max-ble/externals/max-ble.mxo"
PKG_DIR="${REPO_ROOT}/max-package/max-ble"
MXO_DST="${PKG_DIR}/externals/max-ble.mxo"

if [[ ! -d "${MXO_SRC}" ]]; then
  echo "postbuild-macos: expected build product at ${MXO_SRC}" >&2
  exit 1
fi

mkdir -p "${PKG_DIR}/externals"
rm -rf "${MXO_DST}"
cp -R "${MXO_SRC}" "${MXO_DST}"

# Ad-hoc sign so Max can load the external on Apple Silicon without a Developer ID.
codesign --force --sign - "${MXO_DST}/Contents/MacOS/max-ble"
codesign --force --deep --sign - "${MXO_DST}"

cd "${REPO_ROOT}"
zip -r max-package/max-ble.zip max-package/max-ble -x "*.DS_Store"

install_package() {
  local dest="$1"
  mkdir -p "$(dirname "${dest}")"
  rm -rf "${dest}"
  cp -R "${PKG_DIR}" "${dest}"
  xattr -cr "${dest}" 2>/dev/null || true
}

for max_dir in "${HOME}/Documents/Max 8" "${HOME}/Documents/Max 9"; do
  if [[ -d "${max_dir}" ]]; then
    install_package "${max_dir}/Packages/max-ble"
    echo "postbuild-macos: installed to ${max_dir}/Packages/max-ble"
  fi
done
