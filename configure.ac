#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([MiniResizer], [0.0.4])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([foreign -Wportability])

AC_CONFIG_SRCDIR([src/miniresizer_core.cpp])
AC_CONFIG_HEADERS([config.h])

AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CC
if test "x$CC" == "x"; then
	echo "Cannot find usable a C compiler"
	(exit 1); exit 1
fi

AC_PROG_CXX
if test "x$CXX" == "x"; then
	echo "Cannot find usable a C++ compiler"
	(exit 1); exit 1
fi

AC_CHECK_PROGS([FLUID], [fluid])
if test "x$FLUID" == "x"; then
	echo "The fluid program (part of FLTK) was not found on this system."
	echo "Please check that FLTK is installed. On some systems, fluid is"
	echo "packaged separately from the main libraries"
	(exit 1); exit 1
fi

case "$host" in
*-*-*mingw*)
	AC_CHECK_TOOL([WINDRES], [windres])
	if test "x$WINDRES" = "x"; then
		AC_MSG_FAILURE([Cannot find a suitable windres executable])
	fi
	type_of_host=win32
	AC_DEFINE([WIN32], [1], [Defined when building for MS Windows])
;;
*) ;;
esac
AM_CONDITIONAL([WIN32], [test "x$type_of_host" = "xwin32"])

has_ffmpeg=yes
has_fltk=yes

# Checks for libraries.
AC_SEARCH_LIBS([floor], [m], , [AC_MSG_FAILURE([Math library missing])])

# Check for FLTK
AC_SEARCH_LIBS([fl_open], [fltk], ,[has_fltk=no])
# Check for ffmpeg
AC_SEARCH_LIBS([avformat_license], [avformat], ,[has_ffmpeg=no])
AC_SEARCH_LIBS([avcodec_license], [avcodec], ,[has_ffmpeg=no])
AC_SEARCH_LIBS([avutil_license], [avutil], ,[has_ffmpeg=no])
AC_SEARCH_LIBS([swscale_license], [swscale], ,[has_ffmpeg=no])

# Checks for header files.
AC_CHECK_HEADERS([stdint.h], ,[AC_MSG_FAILURE([Header file 'stdint.h' is missing])])

if test "x$has_fltk" == "xyes"; then
	# Here I just check for one FLTK header; if that one is present, then
	# I'll assume the entire distribution is present
	AC_LANG_PUSH([C++])
	AC_CHECK_HEADERS([FL/Fl.H], ,[has_fltk=no])
	AC_LANG_POP([C++])
fi

if test "x$has_ffmpeg" == "xyes"; then
	AC_CHECK_HEADERS([libavformat/avformat.h libavcodec/avcodec.h libavutil/avutil.h libavutil/imgutils.h libswscale/swscale.h], ,[has_ffmpeg=no])
fi

if test "x$has_fltk" == "xno"; then
	echo "The Fast Light Toolkit (FLTK) libraries were not found on this system."
	echo "Please check that FLTK is installed."
	(exit 1); exit 1
fi

if test "x$has_ffmpeg" == "xno"; then
	echo "FFmpeg libraries were not found on this system."
	echo "Please check that the ffmpeg package is installed."
	(exit 1); exit 1
fi

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

echo
echo "Build configuration------------------------------------------------"
echo

echo -n "Target system: "
if test "x$type_of_host" == "xwin32"; then
	echo "Win32"
else
	echo "Posix"
fi

echo
echo "-------------------------------------------------------------------"
