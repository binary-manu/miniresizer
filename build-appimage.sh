#!/bin/sh

set -ve

BUILDDIR='build-appimage'
PREFIX=/usr
LINUXDEPLOYEXE='https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage'

if [ -e "$BUILDDIR" ]; then
    rm -rf "$BUILDDIR"
fi
mkdir -p "$BUILDDIR"
cd "$BUILDDIR"

LINUXDEPLOY="linuxdeploy"
if ! which $LINUXDEPLOY >/dev/null 2>&1; then
    curl -s -L -o linuxdeploy "$LINUXDEPLOYEXE"
    LINUXDEPLOY="./linuxdeploy"
    chmod +x "$LINUXDEPLOY"
fi

# For CentOS 7
CMAKE=cmake
which cmake3 >/dev/null 2>&1 && CMAKE=cmake3

$CMAKE -G 'Unix Makefiles' -DCMAKE_INSTALL_PREFIX="$PREFIX" ..
make -j$(nproc)
make install DESTDIR=AppDir
strip -s "AppDir/$PREFIX/bin/miniresizer"
ln -s  \
    "$PREFIX/share/icons/hicolor/128x128/apps/miniresizer.png" \
    AppDir/.DirIcon

VERSION=$(git describe --tags) $LINUXDEPLOY --appdir AppDir --output appimage
# Strip version info to make the filename predictable
mv *.AppImage "$(echo *.AppImage | sed -e 's/-.*-/-/' )"