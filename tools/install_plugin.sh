#!/bin/bash
# Installs plugin build artefacts in VST3 and AU formats to system plugin directories
#
# usage:
# `root-dir`/tools/install_plugin.sh [COMPILE_MODE]
#	eg. ./tools/install_plugin.sh release
#				- installs from Release build
#		./tools/install_plugin.sh debug
#				- installs from Debug build
#		./tools/install_plugin.sh
#				- installs default build
#
# Note: Written for macOS, update this if using on different OS

set -e

# System check - update this when adding support for other systems
if [[ "$(uname)" != "Darwin" ]]; then
    echo "This script only supports macOS. Update it to support different systems."
    exit 1
fi

COMPILE_MODE="${1:-}"

BASE_BUILD_DIR="$(dirname "$0")/../build/ReHarmonizer_artefacts"

case "$COMPILE_MODE" in
    "")
        BUILD_DIR="$BASE_BUILD_DIR"
        ;;
    release|Release)
        BUILD_DIR="$BASE_BUILD_DIR/Release"
        ;;
    debug|Debug)
        BUILD_DIR="$BASE_BUILD_DIR/Debug"
        ;;
    *)
        echo "Usage: $0 [release|debug]"
        exit 1
        ;;
esac

VST3_SRC="$BUILD_DIR/VST3/ReHarmonizer.vst3"
AU_SRC="$BUILD_DIR/AU/ReHarmonizer.component"

VST3_DEST="$HOME/Library/Audio/Plug-Ins/VST3"
AU_DEST="$HOME/Library/Audio/Plug-Ins/Components"

# Check if extracted paths are valid
if [[ ! -d "$BUILD_DIR" ]]; then
    echo "Build directory not found: $BUILD_DIR"
    exit 1
fi

if [[ ! -e "$VST3_SRC" ]]; then
    echo "VST3 not found: $VST3_SRC"
    exit 1
fi

if [[ ! -e "$AU_SRC" ]]; then
    echo "AU not found: $AU_SRC"
    exit 1
fi

# Install
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
