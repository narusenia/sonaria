#!/usr/bin/env bash
# Builds an unsigned macOS installer (.pkg) and a zip from a Release build.
#
# Usage: packaging/macos/build_pkg.sh <version> [build-dir] [out-dir]
#
# The installer lets the user pick the standalone app, VST3 and AU. Models are
# always installed to /Library/Application Support/NXE/Sonaria/models, which is
# where Release builds look first (Source/Utils/PlatformPaths.h).
set -euo pipefail

VERSION="$1"
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="$(cd "${2:-$ROOT_DIR/build}" && pwd)"
OUT_DIR="${3:-$ROOT_DIR/dist}"
ID_PREFIX="one.nxeu.sonaria.pkg"

APP="$BUILD_DIR/PitchNet_artefacts/Release/Sonaria.app"
VST3="$BUILD_DIR/PitchNetPlugin_artefacts/Release/VST3/Sonaria.vst3"
AU="$BUILD_DIR/PitchNetPlugin_artefacts/Release/AU/Sonaria.component"
MODELS="$ROOT_DIR/Resources/models"
for f in "$APP" "$VST3" "$AU" "$MODELS/pc_nsf_hifigan.mlmodelc" "$MODELS/GAME"; do
    [ -e "$f" ] || { echo "Missing: $f" >&2; exit 1; }
done

STAGE="$(mktemp -d)"
trap 'rm -rf "$STAGE"' EXIT
mkdir -p "$STAGE"/{app,vst3,au,resources,pkgs,res} "$OUT_DIR"

ditto --norsrc --noextattr --noacl "$APP" "$STAGE/app/Sonaria.app"
ditto --norsrc --noextattr --noacl "$VST3" "$STAGE/vst3/Sonaria.vst3"
ditto --norsrc --noextattr --noacl "$AU" "$STAGE/au/Sonaria.component"
ditto --norsrc --noextattr --noacl "$MODELS" "$STAGE/resources/models"
cp "$ROOT_DIR/LICENSE" "$ROOT_DIR/THIRD_PARTY_NOTICES.txt" "$STAGE/resources/"
find "$STAGE" -name .DS_Store -delete
xattr -cr "$STAGE"  # extended attributes would become ._ files in the payload

# component <name> <install-location>
component() {
    local plist="$STAGE/$1.plist"
    pkgbuild --analyze --root "$STAGE/$1" "$plist"
    # Install where the user picked, never into a copy found elsewhere on disk.
    if plutil -extract 0 xml1 -o /dev/null "$plist" >/dev/null 2>&1; then
        plutil -replace 0.BundleIsRelocatable -bool NO "$plist"
    fi
    pkgbuild --root "$STAGE/$1" --component-plist "$plist" \
        --identifier "$ID_PREFIX.$1" --version "$VERSION" \
        --install-location "$2" "$STAGE/pkgs/$1.pkg"
}
component app "/Applications"
component vst3 "/Library/Audio/Plug-Ins/VST3"
component au "/Library/Audio/Plug-Ins/Components"
component resources "/Library/Application Support/NXE/Sonaria"

cp "$ROOT_DIR/LICENSE" "$STAGE/res/LICENSE.txt"
cat > "$STAGE/distribution.xml" <<EOF
<?xml version="1.0" encoding="utf-8"?>
<installer-gui-script minSpecVersion="2">
    <title>Sonaria $VERSION</title>
    <license file="LICENSE.txt"/>
    <options customize="always" require-scripts="false" hostArchitectures="arm64,x86_64"/>
    <domains enable_localSystem="true"/>
    <choices-outline>
        <line choice="app"/>
        <line choice="vst3"/>
        <line choice="au"/>
        <line choice="resources"/>
    </choices-outline>
    <choice id="app" title="Standalone app" description="Sonaria.app in /Applications">
        <pkg-ref id="$ID_PREFIX.app"/>
    </choice>
    <choice id="vst3" title="VST3 plug-in" description="Sonaria.vst3 in /Library/Audio/Plug-Ins/VST3">
        <pkg-ref id="$ID_PREFIX.vst3"/>
    </choice>
    <choice id="au" title="Audio Unit plug-in" description="Sonaria.component in /Library/Audio/Plug-Ins/Components">
        <pkg-ref id="$ID_PREFIX.au"/>
    </choice>
    <choice id="resources" title="Models (required)" enabled="false" selected="true"
            description="Neural models, license and notices in /Library/Application Support/NXE/Sonaria">
        <pkg-ref id="$ID_PREFIX.resources"/>
    </choice>
    <pkg-ref id="$ID_PREFIX.app">app.pkg</pkg-ref>
    <pkg-ref id="$ID_PREFIX.vst3">vst3.pkg</pkg-ref>
    <pkg-ref id="$ID_PREFIX.au">au.pkg</pkg-ref>
    <pkg-ref id="$ID_PREFIX.resources">resources.pkg</pkg-ref>
</installer-gui-script>
EOF

productbuild --distribution "$STAGE/distribution.xml" --package-path "$STAGE/pkgs" \
    --resources "$STAGE/res" "$OUT_DIR/Sonaria-$VERSION-macOS.pkg"

# Zip: bundles with models beside them. Release builds also find a "models"
# folder next to the app or plug-in, so this works without the installer.
ZIP_ROOT="$STAGE/zip/Sonaria-$VERSION-macOS"
mkdir -p "$ZIP_ROOT"
ditto --norsrc --noextattr --noacl "$STAGE/app" "$ZIP_ROOT"
ditto --norsrc --noextattr --noacl "$STAGE/vst3" "$ZIP_ROOT"
ditto --norsrc --noextattr --noacl "$STAGE/au" "$ZIP_ROOT"
ditto --norsrc --noextattr --noacl "$STAGE/resources" "$ZIP_ROOT"
(cd "$STAGE/zip" && ditto -c -k --norsrc --noextattr --keepParent "Sonaria-$VERSION-macOS" "$OUT_DIR/Sonaria-$VERSION-macOS.zip")

ls -l "$OUT_DIR"
