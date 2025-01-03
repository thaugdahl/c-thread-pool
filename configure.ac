#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([linux-wait-test], [0.0.1], [thaugdahl@gmail.com])

# Checks for programs.

AC_CONFIG_FILES([Makefile src/Makefile])

AC_PROG_CC([clang gcc icc])
AC_PROG_CXX([clang++ g++])

AM_INIT_AUTOMAKE([foreign])

# Checks for libraries.
AC_SEARCH_LIBS([pthread_create], [pthread])

# Checks for header files.
AC_CHECK_HEADERS([pthread.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_OUTPUT
