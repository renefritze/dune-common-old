## -*- autoconf -*-

# DUNE_PATH_GMP()
#
# shell variables
#   with_gmp
#     no or path
#   HAVE_GMP
#     no or yes
#   GMP_CPPFLAGS
#   GMP_LIBS
#
# substitutions
#   GMP_CPPFLAGS
#   GMP_LIBS
#
# defines:
#   HAVE_GMP
#
# conditionals:
#   GMP
AC_DEFUN([DUNE_PATH_GMP],[
  AC_REQUIRE([AC_PROG_CXX])

  AC_ARG_WITH([gmp],
    [AS_HELP_STRING([--with-gmp=PATH],
                   [directory to GMP (GNU MP Bignum Library)])])

  HAVE_GMP=no
  AS_IF([test x"$with_gmp" = x"no"], [
    AC_MSG_NOTICE([skipping check for GMP])
  ], [
    AC_CACHE_CHECK([for GMP], dune_cv_gmp_path, [
      dune_cv_gmp_path=no
      AS_IF([test x"$with_gmp" = x || test x"$with_gmp" = xyes], [
        for d in /usr /usr/local; do
          AC_MSG_NOTICE([searching for GMP in $d...])
          DUNE_CHECK_PATH_GMP($d)
          AS_IF([test $HAVE_GMP = yes],[
            dune_cv_gmp_path=$d
            break
          ])
        done],[
        DUNE_CHECK_PATH_GMP($with_gmp)
        AS_IF([test $HAVE_GMP = yes],[
          dune_cv_gmp_path=$with_gmp
        ])
      ])
    ])

    if test "x$dune_cv_gmp_path" != xno; then
      HAVE_GMP=yes
      GMP_CPPFLAGS="-I$dune_cv_gmp_path/include -DENABLE_GMP=1"
      GMP_LIBS="-L$dune_cv_gmp_path/lib -lgmpxx -lgmp"
    fi
  ])

  AS_IF([test $HAVE_GMP = yes],[
    AC_DEFINE([HAVE_GMP],[ENABLE_GMP],[Was GMP found and GMP_CPPFLAGS used?])
    DUNE_ADD_ALL_PKG([GMP], [\${GMP_CPPFLAGS}],
                     [], [\${GMP_LIBS}])
  ], [
    GMP_CPPFLAGS=
    GMP_LIBS=
  ])

  AC_SUBST([GMP_CPPFLAGS])
  AC_SUBST([GMP_LIBS])

  AM_CONDITIONAL(GMP,[test $HAVE_GMP = yes])
  DUNE_ADD_SUMMARY_ENTRY([GMP],[$HAVE_GMP])
])

AC_DEFUN([DUNE_CHECK_PATH_GMP],[
  GMP_CPPFLAGS="-I$1/include -DENABLE_GMP=1"
  GMP_LIBS="-L$1/lib -lgmpxx -lgmp"

  AC_LANG_PUSH([C++])
  ac_save_CPPFLAGS="$CPPFLAGS"
  ac_save_LIBS="$LIBS"

  CPPFLAGS="$CPPFLAGS $GMP_CPPFLAGS"

  AC_CHECK_HEADER([gmpxx.h], [HAVE_GMP=yes],
    [AC_MSG_WARN([gmpxx.h not found in $1])])

  AS_IF([test $HAVE_GMP = yes],[
    DUNE_CHECK_LIB_EXT([$1/lib], gmp,[__gmpz_abs],[],[
      HAVE_GMP=no
      AC_MSG_WARN(libgmp not found)])
  ])
  
  AS_IF([test $HAVE_GMP = yes],[
    DUNE_CHECK_LIB_EXT([$1/lib], gmpxx,[__gmpz_abs],[],[
      HAVE_GMP=no
      AC_MSG_WARN(libgmpxx not found)])
  ])

  CPPFLAGS="$ac_save_CPPFLAGS"
  LIBS="$ac_save_LIBS"
  AC_LANG_POP([C++])
])

