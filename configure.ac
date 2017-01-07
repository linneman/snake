#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.53])
AC_INIT([snake], [0.1], [linneman@gmx.de])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([snake.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([stdio.h stdlib.h string.h stdint.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_MEMCMP
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_PROG_LIBTOOL
AC_CHECK_FUNCS([memset])

dnl target architecture (QDOS or Unix)
AC_ARG_ENABLE([qdos],
[  --enable-qdos to cross compile for the Sinclair QL (needs qdos-gcc).],
[enable_qdos=${enableval}], [enable_qdos=no])

if test "x${enable_qdos}" = xyes; then
   AC_DEFINE([QDOS], 1, [QDOS])
   AC_MSG_NOTICE([cross compile for qdos])
else
   AC_MSG_NOTICE([compile for general Unix with ncurse lib])
   AC_CHECK_LIB(ncurses, initscr, [], [AC_MSG_ERROR([snake needs ncurses on Unix platforms])])
fi


AC_CONFIG_FILES([Makefile])
AC_OUTPUT
