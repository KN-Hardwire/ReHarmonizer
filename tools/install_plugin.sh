#!/bin/bash
# Installs plugin build artefacts in VST3 and AU formats to system plugin directories
#
# Note: Written for macOS, update this if using on different OS

set -e

# System check - update this when adding support for other systems
if [[ "$(uname)" != "Darwin" ]]; then
    echo "This script only supports macOS. Update it to support different systems."
    exit 1
fi

BUILD_DIR="$(dirname "$0")/../build/ReHarmonizer_artefacts/Debug"
VST3_SRC="$BUILD_DIR/VST3/ReHarmonizer.vst3"
AU_SRC="$BUILD_DIR/AU/ReHarmonizer.component"

VST3_DEST="$HOME/Library/Audio/Plug-Ins/VST3"
AU_DEST="$HOME/Library/Audio/Plug-Ins/Components"

# VST3
mkdir -p "$VST3_DEST"
rm -rf "$VST3_DEST/ReHarmonizer.vst3"
cp -r "$VST3_SRC" "$VST3_DEST"
echo "VST3 installed to $VST3_DEST"

# AU
mkdir -p "$AU_DEST"
rm -rf "$AU_DEST/ReHarmonizer.component"
cp -r "$AU_SRC" "$AU_DEST"
echo "AU installed to $AU_DEST"
