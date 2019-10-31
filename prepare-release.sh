#!/bin/sh

set -e
set -v

rm -rf build vroot

INSTALL_ROOT=`pwd`/vroot

meson --prefix="$INSTALL_ROOT" build/native

ninja -C build/native install

ninja -C build/native syntax-check

build-aux/make-dist build/

rpmbuild --nodeps \
   --define "_sourcedir `pwd`/build" \
   -ta --clean build/gtk-vnc*.tar.xz

if test -x /usr/bin/i686-w64-mingw32-gcc && \
   test -r /usr/share/mingw/toolchain-mingw32.meson ; then
  meson build/win32 --prefix="$INSTALL_ROOT/i686-w64-mingw32/sys-root/mingw" \
              --cross-file="/usr/share/mingw/toolchain-mingw32.meson"

  ninja -C build/win32
  ninja -C build/win32 install
fi

if test -x /usr/bin/x86_64-w64-mingw32-gcc && \
   test -r /usr/share/mingw/toolchain-mingw64.meson ; then
  meson build/win64 --prefix="$INSTALL_ROOT/x86_64-w64-mingw32/sys-root/mingw" \
              --cross-file="/usr/share/mingw/toolchain-mingw64.meson"

  ninja -C build/win64
  ninja -C build/win64 install
fi

if test -x /usr/bin/i686-w64-mingw32-gcc && \
   test -r /usr/share/mingw/toolchain-mingw32.meson && \
   test -x /usr/bin/x86_64-w64-mingw32-gcc && \
   test -r /usr/share/mingw/toolchain-mingw64.meson &&
   test -f /usr/bin/rpmbuild; then

   rpmbuild --nodeps \
       --define "_sourcedir `pwd`/build" \
       -ba --clean build/native/mingw-gtk-vnc.spec
fi
