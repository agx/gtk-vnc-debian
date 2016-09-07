# gtk-vnc autoconf

AC_PREREQ(2.63)

AC_INIT([gtk-vnc],[0.6.0],[https://bugzilla.gnome.org/enter_bug.cgi?product=gtk-vnc],[gtk-vnc],[http://live.gnome.org/gtk-vnc/])
AC_CONFIG_SRCDIR([src/vncconnection.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])

dnl Make automake keep quiet about wildcards & other GNUmake-isms
AM_INIT_AUTOMAKE([1.10 -Wno-portability dist-xz])

LT_PREREQ([2.2.6])
LT_INIT([win32-dll disable-static])

AC_CONFIG_MACRO_DIR([m4])

GNOME_MAINTAINER_MODE_DEFINES
AM_MAINTAINER_MODE([enable])

# Use the silent-rules feature when possible.
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_CANONICAL_HOST

dnl *******************************************************************************
dnl Declare required library versions
dnl *******************************************************************************

GOBJECT_REQUIRED=2.10.0
AC_SUBST(GOBJECT_REQUIRED)
GIO_REQUIRED=2.10.0
AC_SUBST(GIO_REQUIRED)
GDK_PIXBUF_REQUIRED=2.10.0
AC_SUBST(GDK_PIXBUF_REQUIRED)
GCRYPT_REQUIRED=1.4.0
AC_SUBST(GCRYPT_REQUIRED)
GNUTLS_REQUIRED=2.2.0
AC_SUBST(GNUTLS_REQUIRED)

PYGTK_REQUIRED=2.0.0
VIEW_REQUIRED=0.6.0
GTHREAD_REQUIRED=2.0.0
PYTHON_REQUIRED=2.4
CAIRO_REQUIRED=1.2.0

dnl *******************************************************************************
dnl Get the version information at compile-time
dnl *******************************************************************************
VERSION_MAJOR=`echo $VERSION | awk -F. '{print $1}'`
VERSION_MINOR=`echo $VERSION | awk -F. '{print $2}'`
VERSION_MICRO=`echo $VERSION | awk -F. '{print $3}'`

AC_DEFINE_UNQUOTED([VERSION_MAJOR], [$VERSION_MAJOR], [Major version number of package])
AC_DEFINE_UNQUOTED([VERSION_MINOR], [$VERSION_MINOR], [Minor version number of package])
AC_DEFINE_UNQUOTED([VERSION_MICRO], [$VERSION_MICRO], [Micro version number of package])

# Autoconf 2.61a.99 and earlier don't support linking a file only
# in VPATH builds.  But since GNUmakefile is for maintainer use
# only, it does not matter if we skip the link with older autoconf.
# Automake 1.10.1 and earlier try to remove GNUmakefile in non-VPATH
# builds, so use a shell variable to bypass this.
GNUmakefile=GNUmakefile
m4_if(m4_version_compare([2.61a.100],
        m4_defn([m4_PACKAGE_VERSION])), [1], [],
      [AC_CONFIG_LINKS([$GNUmakefile:$GNUmakefile], [],
        [GNUmakefile=$GNUmakefile])])

dnl *******************************************************************************
dnl Setup core compilers / build helpers
dnl *******************************************************************************

AC_PROG_CC_STDC
AM_PROG_CC_C_O

dnl *******************************************************************************
dnl Internationalization
dnl *******************************************************************************
GETTEXT_PACKAGE=gtk-vnc
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],"$GETTEXT_PACKAGE", [GETTEXT package name])

IT_PROG_INTLTOOL([0.40.0])
AM_GLIB_GNU_GETTEXT

dnl *******************************************************************************

################################################################################
# GTK+
################################################################################

AC_MSG_CHECKING([which gtk+ version to compile against])
AC_ARG_WITH([gtk],
  [AS_HELP_STRING([--with-gtk=2.0|3.0],[which gtk+ version to compile against (default: 3.0)])],
  [case "$with_gtk" in
     2.0|3.0) ;;
     *) AC_MSG_ERROR([invalid gtk version specified]) ;;
   esac],
  [with_gtk=3.0])
AC_MSG_RESULT([$with_gtk])

case "$with_gtk" in
  2.0) GTK_API_VERSION=2.0
       GTK_REQUIRED=2.18.0
       GTK_VNC_API_VERSION=1.0
       ;;
  3.0) GTK_API_VERSION=3.0
       GTK_REQUIRED=3.0.0
       GTK_VNC_API_VERSION=2.0
       ;;
esac

AC_SUBST([GTK_API_VERSION])
AC_SUBST([GTK_REQUIRED])
AC_SUBST([GTK_VNC_API_VERSION])
AM_CONDITIONAL([HAVE_GTK_2],[test "$with_gtk" = "2.0"])
AM_CONDITIONAL([HAVE_GTK_3],[test "$with_gtk" = "3.0"])

AC_CHECK_HEADERS([pwd.h termios.h])

AC_ARG_WITH(python,
[  --with-python           build python bindings],
[case "${withval}" in
   yes|no) ;;
   *)      AC_MSG_ERROR([bad value ${withval} for python option]) ;;
 esac],[withval=yes])

if test "$with_gtk" = "3.0"
then
  WITH_PYTHON=no
else
  WITH_PYTHON=$withval
fi

GTK_VNC_COMPILE_WARNINGS

AC_ARG_WITH(examples,
[  --with-examples         install example programs],
[case "${withval}" in
   yes|no) ;;
   *)      AC_MSG_ERROR([bad value ${withval} for examples option]) ;;
 esac],[withval=no])
WITH_EXAMPLES=$withval


NO_UNDEFINED_FLAGS=
VERSION_SCRIPT_FLAGS=
USE_VERSION_DEFS=0
case "$host" in
  *-*-mingw*)
    VERSION_SCRIPT_FLAGS="-Wl,"
    USE_VERSION_DEFS=1
    NO_UNDEFINED_FLAGS="-no-undefined"
    ;;

  *-*-cygwin*)
    NO_UNDEFINED_FLAGS="-no-undefined"
    ;;

  *)
    `ld --help 2>&1 | grep -- --no-undefined >/dev/null` && \
      NO_UNDEFINED_FLAGS="-Wl,--no-undefined"
    VERSION_SCRIPT_FLAGS=-Wl,--version-script=
    `ld --help 2>&1 | grep -- --version-script >/dev/null` || \
      VERSION_SCRIPT_FLAGS="-Wl,-M -Wl,"
    ;;
esac
AC_SUBST([NO_UNDEFINED_FLAGS])
AC_SUBST([VERSION_SCRIPT_FLAGS])
AM_CONDITIONAL([USE_VERSION_DEFS], [test "$USE_VERSION_DEFS" = "1"])

PKG_CHECK_MODULES(GOBJECT, gobject-2.0 >= $GOBJECT_REQUIRED)
AC_SUBST(GOBJECT_CFLAGS)
AC_SUBST(GOBJECT_LIBS)

PKG_CHECK_MODULES(GIO, gio-2.0 >= $GIO_REQUIRED)
AC_SUBST(GIO_CFLAGS)
AC_SUBST(GIO_LIBS)

PKG_CHECK_MODULES(GIOUNIX, gio-unix-2.0 >= $GIO_REQUIRED, [have_giounix=yes], [have_giounix=no])
AC_SUBST(GIOUNIX_CFLAGS)
AC_SUBST(GIOUNIX_LIBS)
if test $have_giounix = "yes" ; then
  AC_DEFINE_UNQUOTED([HAVE_GIOUNIX],[1], [Whether GIO UNIX is available])
fi


PKG_CHECK_MODULES(GDK_PIXBUF, gdk-pixbuf-2.0 >= $GDK_PIXBUF_REQUIRED)
AC_SUBST(GDK_PIXBUF_CFLAGS)
AC_SUBST(GDK_PIXBUF_LIBS)

PKG_CHECK_MODULES(GTK, gtk+-$GTK_API_VERSION >= $GTK_REQUIRED)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

PKG_CHECK_MODULES(X11, x11,,AC_MSG_NOTICE([Not building against X11]))
AC_SUBST(X11_CFLAGS)
AC_SUBST(X11_LIBS)

PKG_CHECK_MODULES(CAIRO, cairo >= $CAIRO_REQUIRED)
AC_SUBST(CAIRO_CFLAGS)
AC_SUBST(CAIRO_LIBS)


AC_ARG_WITH(libview,
[  --with-libview          enable libview support in gvncviewer],
[case "${withval}" in
   yes|no) ;;
   *)	   AC_MSG_ERROR([bad value ${withval} for libview option]) ;;
 esac],[withval=yes])

WITH_LIBVIEW=0
if test "${withval}" = "yes"; then
  PKG_CHECK_MODULES(VIEW, libview >= $VIEW_REQUIRED,
		   [WITH_LIBVIEW=1], [WITH_LIBVIEW=0])
fi
AC_DEFINE_UNQUOTED([WITH_LIBVIEW],[$WITH_LIBVIEW], [Whether to use libview])
AC_SUBST(VIEW_CFLAGS)
AC_SUBST(VIEW_LIBS)


AC_ARG_WITH(pulseaudio,
  [AS_HELP_STRING([--with-pulseaudio],
    [use PulseAudio for audio playback @<:@default=check@:>@])],
  [],
  [with_pulseaudio=check])

HAVE_PULSEAUDIO=no
if test "x$with_pulseaudio" != "xno"; then
  PULSEAUDIO_MODULES=libpulse-simple
  if test "x$with_pulseaudio" == "xyes"; then
    PKG_CHECK_MODULES(PULSEAUDIO, $PULSEAUDIO_MODULES, [HAVE_PULSEAUDIO=yes])
  else
    PKG_CHECK_MODULES(PULSEAUDIO, $PULSEAUDIO_MODULES, [HAVE_PULSEAUDIO=yes],
      [AC_MSG_NOTICE([libpulse-simple not found, disabling PulseAudio support])])
  fi
fi
if test "x$HAVE_PULSEAUDIO" = "xyes"; then
    AC_DEFINE_UNQUOTED([HAVE_PULSEAUDIO], 1,
      [whether pulseaudio is available for audio streaming])
fi
AM_CONDITIONAL([HAVE_PULSEAUDIO], [test "x$HAVE_PULSEAUDIO" = "xyes"])
AC_SUBST(PULSEAUDIO_CFLAGS)
AC_SUBST(PULSEAUDIO_LIBS)


AM_PATH_LIBGCRYPT($GCRYPT_REQUIRED)


AC_ARG_WITH([tls-priority],
  [AS_HELP_STRING([--with-tls-priority],
    [set the default TLS session priority string @<:@default=NORMAL@:>@])],
  [],
  [with_tls_priority=NORMAL])

AC_DEFINE_UNQUOTED([TLS_PRIORITY], ["$with_tls_priority"],
                  [TLS default priority string])


PKG_CHECK_MODULES(GNUTLS, gnutls >= $GNUTLS_REQUIRED)
AC_SUBST(GNUTLS_CFLAGS)
AC_SUBST(GNUTLS_LIBS)

if $PKG_CONFIG --exists 'gnutls >= 3.0'; then
    GNUTLS_GCRYPT=no
elif $PKG_CONFIG --exists 'gnutls >= 2.12'; then
    case `$PKG_CONFIG --libs --static gnutls` in
      *gcrypt*) GNUTLS_GCRYPT=yes     ;;
      *nettle*) GNUTLS_GCRYPT=no      ;;
      *)        GNUTLS_GCRYPT=yes     ;;
    esac
else
    GNUTLS_GCRYPT=yes
fi
if test "x$GNUTLS_GCRYPT" = "xyes"; then
  AC_DEFINE_UNQUOTED([HAVE_GNUTLS_GCRYPT], [1], [whether gnutls uses gcrypt])
fi


dnl Cyrus SASL
AC_ARG_WITH([sasl],
  [AS_HELP_STRING([--with-sasl],
    [use cyrus SASL for authentication @<:@default=check@:>@])],
  [],
  [with_sasl=check])

SASL_CFLAGS=
SASL_LIBS=
enable_sasl=no
if test "x$with_sasl" != "xno"; then
  if test "x$with_sasl" != "xyes" && test "x$with_sasl" != "xcheck"; then
    SASL_CFLAGS="-I$with_sasl"
    SASL_LIBS="-L$with_sasl"
  fi
  fail=0
  old_cflags="$CFLAGS"
  old_libs="$LIBS"
  CFLAGS="$CFLAGS $SASL_CFLAGS"
  LIBS="$LIBS $SASL_LIBS"
  AC_CHECK_HEADER([sasl/sasl.h],[],[
    if test "x$with_sasl" != "xcheck" ; then
        with_sasl=no
    else
        fail=1
    fi])
  if test "x$with_sasl" != "xno" ; then
    AC_CHECK_LIB([sasl2], [sasl_client_init],[with_sasl2=yes],[with_sasl2=no])
  fi
  if test "x$with_sasl2" = "xno" && test "x$with_sasl" != "xno" ; then
    AC_CHECK_LIB([sasl], [sasl_client_init],[with_sasl=yes],[with_sasl=no])
  fi
  if test "x$with_sasl2" = "xyes"; then
    SASL_LIBS="$SASL_LIBS -lsasl2"
  elif test "x$with_sasl" = "xyes"; then
    SASL_LIBS="$SASL_LIBS -lsasl"
  else
    AC_MSG_ERROR([You must install the Cyrus SASL development package in order to compile GTK-VNC])
  fi
  CFLAGS="$old_cflags"
  LIBS="$old_libs"
  if test "x$with_sasl2" = "xyes" || test "x$with_sasl" = "xyes" ; then
    AC_DEFINE_UNQUOTED([HAVE_SASL], 1,
      [whether Cyrus SASL is available for authentication])
    enable_sasl=yes
  fi
fi
AM_CONDITIONAL([HAVE_SASL], [test "x$with_sasl2" = "xyes" || test "x$with_sasl" = "xyes"])
AC_SUBST([SASL_CFLAGS])
AC_SUBST([SASL_LIBS])


GTHREAD_CFLAGS=
GTHREAD_LIBS=

AC_CHECK_LIB(z, inflate, [], [AC_MSG_ERROR([zlib not found])])

WITH_UCONTEXT=1

AC_ARG_WITH(coroutine,
[  --with-coroutine=ucontext/gthread  use ucontext or GThread for coroutines],
[],[with_coroutine=ucontext])

case $with_coroutine in
  ucontext)
    ;;
  gthread)
    ;;
  *)
    AC_MSG_ERROR(Unsupported coroutine type)
esac

if test "$with_coroutine" = "ucontext"; then
  AC_CHECK_FUNC(makecontext, [],[with_coroutine=gthread])
  AC_CHECK_FUNC(swapcontext, [],[with_coroutine=gthread])
  AC_CHECK_FUNC(getcontext, [],[with_coroutine=gthread])
fi

if test "$with_coroutine" = "gthread"; then
  PKG_CHECK_MODULES(GTHREAD, gthread-2.0 > $GTHREAD_REQUIRED)
  WITH_UCONTEXT=0
fi
AC_SUBST(GTHREAD_CFLAGS)
AC_SUBST(GTHREAD_LIBS)
AC_DEFINE_UNQUOTED([WITH_UCONTEXT],[$WITH_UCONTEXT], [Whether to use ucontext coroutine impl])
AM_CONDITIONAL(WITH_UCONTEXT, [test "$WITH_UCONTEXT" != "0"])

if test "$WITH_PYTHON" = "yes"; then
  PKG_CHECK_MODULES(PYGTK, pygtk-2.0 >= $PYGTK_REQUIRED)
  AC_SUBST(PYGTK_CFLAGS)
  AC_SUBST(PYGTK_LIBS)

  AM_PATH_PYTHON

  AC_MSG_CHECKING([whether $PYTHON version >= $PYTHON_REQUIRED])
  HAVE_PYTHON_REQUIRED=no
  AM_PYTHON_CHECK_VERSION([$PYTHON], [$PYTHON_REQUIRED],
      [HAVE_PYTHON_REQUIRED="yes"],
      [HAVE_PYTHON_REQUIRED="no"])

  AC_MSG_RESULT($HAVE_PYTHON_REQUIRED)

  if test "x$HAVE_PYTHON_REQUIRED" != "xyes"
  then
    AC_MSG_ERROR("No suitable python found")
  fi

  AM_CHECK_PYTHON_HEADERS(have_python_headers=yes,have_python_headers=no)

  if test "x$have_python_headers" != "xyes"
  then
    AC_MSG_ERROR("No python development headers found")
  fi
fi

dnl define a MAINT-like variable REBUILD which is set
dnl if Perl, so autogenerated sources can be rebuilt
AC_PATH_PROGS(PERL, perl5 perl)
AC_ARG_ENABLE(rebuilds, [  --disable-rebuilds      disable all source autogeneration rules],,enable_rebuilds=yes)
REBUILD=\#
if test "x$enable_rebuilds" = "xyes" && \
     test -n "$PERL" && \
     $PERL -e 'exit !($] >= 5.002)' > /dev/null 2>&1 ; then
  REBUILD=
fi
AC_SUBST(REBUILD)


AM_CONDITIONAL(WITH_PYTHON, [test "$WITH_PYTHON" = "yes"])

AM_CONDITIONAL(WITH_EXAMPLES, [test "$WITH_EXAMPLES" = "yes"])

GOBJECT_INTROSPECTION_CHECK([0.9.4])

if test "x$with_gtk" = "x3.0"; then
        default_vala=check
else
        default_vala=no
fi

AC_ARG_ENABLE([vala],
              AS_HELP_STRING([--enable-vala], [enable Vala binding generation]),
              [], [enable_vala="$default_vala"])
if test "x$found_introspection" = "xyes" ; then
        if test "x$enable_vala" != "xno" ; then
                AC_PATH_PROG(VAPIGEN, vapigen, no)
                if test "x$VAPIGEN" != "xno"; then
                        if test "x$enable_vala" == "xcheck" ; then
                                enable_vala=yes
                                AC_SUBST(VAPIGEN)
                        fi
                else
                        AC_MSG_ERROR([Cannot find the "vapigen" binary in your PATH])
                fi
        fi
fi
AM_CONDITIONAL([WITH_VALA], [test "x$enable_vala" = "xyes"])

AC_MSG_CHECKING([for Text::CSV Perl module])
perl -MText::CSV -e "" >/dev/null 2>&1
if test $? -ne 0 ; then
  AC_MSG_RESULT([not found])
  if test ! -e "$srcdir/src/vncdisplaykeymap_osx2rfb.c"; then
    AC_MSG_ERROR([Text::CSV Perl module is required to compile this package])
  fi
else
  AC_MSG_RESULT([found])
fi

AC_CONFIG_FILES(
  Makefile
  src/Makefile
  tools/Makefile
  examples/Makefile
  po/Makefile.in
  vapi/Makefile
  gvnc-1.0.pc
  gvncpulse-1.0.pc
  gtk-vnc-1.0.pc
  gtk-vnc-2.0.pc
  gtk-vnc.spec
  mingw-gtk-vnc.spec
  )

AC_OUTPUT

echo "
Configure summary:

	Compiler ...................:  ${CC}
	Compiler Flags .............:  ${CFLAGS}
	Extra Link Flags ...........:  ${VERSION_SCRIPT_FLAGS} ${NO_UNDEFINED_FLAGS}
	Prefix......................:  ${prefix}

	Python binding .............:  ${WITH_PYTHON}
	Install example programs ...:  ${WITH_EXAMPLES}
	SASL support................:  ${enable_sasl}
	PulseAudio support..........:  ${HAVE_PULSEAUDIO}
	GTK+ version................:  ${GTK_API_VERSION}
	TLS priority................:  ${with_tls_priority}
"
