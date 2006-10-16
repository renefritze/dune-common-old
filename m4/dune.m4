#! /bin/sh
# $Id$
# checks for dune-headers and everything they need

# TODO
#
# - use pkg-config if --enable-localdune is not provided

#   #export PKG_CONFIG_LIBDIR=$with_dune/dune
#  #PKG_CHECK_MODULES(DUNE, dune)  

AC_DEFUN([DUNE_DISABLE_LIBCHECK],[
  # hidden feature... --enable-dunelibcheck tells the dune module check to
  # check only for the headers and not for the libs. This feature is used
  # when building the web page, because we don't need the libs in this case

  AC_ARG_ENABLE(dunelibcheck,[],,enable_dunelibcheck=yes)
])

AC_DEFUN([DUNE_CHECK_MODULES],[
  AC_REQUIRE([AC_PROG_CXX])
  AC_REQUIRE([AC_PROG_CXXCPP])
  AC_REQUIRE([DUNE_DISABLE_LIBCHECK])

  # ____DUNE_CHECK_MODULES_____ ($1)

  echo checking for $1

  m4_pushdef([_dune_module_plain], [$1])
  m4_pushdef([_dune_module], [m4_translit(_dune_module_plain, [-], [_])])
  m4_pushdef([_dune_header], [$2])
  m4_pushdef([_dune_ldpath], [$3])
  m4_pushdef([_dune_lib],    [$4])
  m4_pushdef([_dune_symbol], [$5])
  m4_pushdef([_DUNE_MODULE], [m4_toupper(_dune_module)])

  # switch tests to c++
  AC_LANG_PUSH([C++])

  # the usual option...
  withval=""
  AC_ARG_WITH(_dune_module_plain,
    AC_HELP_STRING([--with-_dune_module_plain=PATH],[_dune_module directory]))
  withval=$with_[]_dune_module

  # backup of flags
  ac_save_CPPFLAGS="$CPPFLAGS"
  ac_save_LIBS="$LIBS"
  ac_save_LDFLAGS="$LDFLAGS"
  CPPFLAGS=""
  LIBS=""

  # is a directory set?
  if test x$withval != x ; then
    # expand tilde
    if test -d $withval ; then
      # expand tilde / other stuff
      _DUNE_MODULE[]_ROOT=`cd $withval && pwd`

      # expand search path (otherwise empty CPPFLAGS)
      if test -d $_DUNE_MODULE[]_ROOT/include/dune; then
    # Dune was installed into directory given by with-dunecommon
    LINSTALL=1
    _DUNE_MODULE[]_CPPFLAGS="-I$_DUNE_MODULE[]_ROOT/include"
    LDFLAGS="$LDFLAGS -L$_DUNE_MODULE[]_ROOT/lib"
      else
        _DUNE_MODULE[]_CPPFLAGS="-I$_DUNE_MODULE[]_ROOT"
      fi
    else
      AC_MSG_ERROR([_dune_module_plain-directory $withval does not exist])
    fi
    # set correct dune path
    with_[]_dune_module="$withval"
  else
    # set correct dune path
    with_[]_dune_module="global installation"
  fi

  DUNE_CPPFLAGS="$DUNE_CPPFLAGS $_DUNE_MODULE[]_CPPFLAGS"
  CPPFLAGS="$DUNE_CPPFLAGS"
  SET_CPPFLAGS="$_DUNE_MODULE[]_CPPFLAGS"
  
  # test for an arbitrary header
  AC_CHECK_HEADER([dune/[]_dune_header],
    [HAVE_[]_DUNE_MODULE=1
     _DUNE_MODULE[]_CPPFLAGS="$SET_CPPFLAGS"],
    [HAVE_[]_DUNE_MODULE=0
     _DUNE_MODULE[]_CPPFLAGS=""
     AC_MSG_ERROR([$with_[]_dune_module does not seem to contain a valid _dune_module_plain (dune/[]_dune_header not found)])]
  )

  ## check for lib (if lib name was provided)
  ifelse(_dune_lib,,AC_MSG_NOTICE([_dune_module_plain does not provide libs]),[
    if test "x$enable_dunelibcheck" != "xyes"; then
      AC_MSG_WARN([library check for _dune_module_plain is disabled. DANGEROUS!])    
    fi
    # did we find the headers?
    if test x$HAVE_[]_DUNE_MODULE = x1 &&
       test "x$enable_dunelibcheck" = "xyes"; then
      ac_save_LDFLAGS="$LDFLAGS"
      ac_save_LIBS="$LIBS"
      HAVE_[]_DUNE_MODULE=0

      ## special test for a local installation
      if test x$_DUNE_MODULE[]_ROOT != x && test ! $LINSTALL; then
        # have a look into the dune module directory
        LDFLAGS="$LDFLAGS -L$_DUNE_MODULE[]_ROOT/dune/_dune_ldpath"

        # only check for a .la-file
        if test -s $_DUNE_MODULE[]_ROOT/_dune_ldpath/lib[]_dune_lib[].la ; then
          _DUNE_MODULE[]_LDFLAGS="-L$_DUNE_MODULE[]_ROOT/_dune_ldpath"
          echo found lib[]_dune_lib.la, setting LDFLAGS to $_DUNE_MODULE[]_LDFLAGS

          # provide arguments like normal lib-check
          _DUNE_MODULE[]_LIBS="-l[]_dune_lib"
          HAVE_[]_DUNE_MODULE=1
        else 
          AC_MSG_ERROR([$with_[]_dune_module does not seem to contain a valid _dune_module_plain (lib[]_dune_lib[].la not found)])
        fi
      fi

      ## normal test for a systemwide install
      if test x$HAVE_[]_DUNE_MODULE = x0 ; then
        # !!! should be pkg-config later (which would save the special
        # header-check as well)

        # Beware! Untested!!!
        LIBS="-ldune[]_dune_lib"
        AC_MSG_CHECKING([for dune[]_dune_lib library])
        AC_TRY_LINK(dnl
        [#]include<dune/[]_dune_header>,
        _dune_symbol,
            [AC_MSG_RESULT([yes])
             HAVE_[]_DUNE_MODULE=1
             _DUNE_MODULE[]_LIBS="$LIBS"],
            [AC_MSG_RESULT([no])
             HAVE_[]_DUNE_MODULE=0
             AC_MSG_ERROR([failed to link with libdune[]_dune_lib[].la])]
        )
      fi

      if test x$_DUNE_MODULE[]_LIBS = x; then
        AC_MSG_ERROR([failed to find lib[]_dune_lib[] needed of module _dune_module_plain])
      fi

      # reset variables
      LDFLAGS="$ac_save_LDFLAGS"
      LIBS="$ac_save_LIBS"
    fi
  ])

  # did we succeed?
  if test x$HAVE_[]_DUNE_MODULE = x1 ; then
    # set variables for our modules
    AC_SUBST(_DUNE_MODULE[]_CPPFLAGS, "$_DUNE_MODULE[]_CPPFLAGS")
    AC_SUBST(_DUNE_MODULE[]_LDFLAGS, "$_DUNE_MODULE[]_LDFLAGS")
    AC_SUBST(_DUNE_MODULE[]_LIBS, "$_DUNE_MODULE[]_LIBS")
    AC_SUBST(_DUNE_MODULE[]_ROOT, "$_DUNE_MODULE[]_ROOT")
    AC_DEFINE(HAVE_[]_DUNE_MODULE, 1, [Define to 1 if _dune_module was found])

    # set DUNE_* variables
    AC_SUBST(DUNE_CPPFLAGS, "$DUNE_CPPFLAGS")
    AC_SUBST(DUNE_LDFLAGS, "$DUNE_LDFLAGS $_DUNE_MODULE[]_LDFLAGS")
    AC_SUBST(DUNE_LIBS, "$DUNE_LIBS $_DUNE_MODULE[]_LIBS")
    
    # add to global list
    # only add my flags other flags are added by other packages 
    DUNE_PKG_CPPFLAGS="$DUNE_PKG_CPPFLAGS $_DUNE_MODULE[]_CPPFLAGS"
    DUNE_PKG_LIBS="$DUNE_PKG_LIBS $LIBS"
    #DUNE_PKG_LDFLAGS="$DUNE_PKG_LDFLAGS $DUNE_LDFLAGS"
    DUNE_PKG_LDFLAGS="$DUNE_PKG_LDFLAGS $_DUNE_MODULE[]_LDFLAGS"

    with_[]_dune_module="yes"
  else
    AC_MSG_ERROR([could not find required module _dune_module_plain])
  fi

  # reset previous flags
  CPPFLAGS="$ac_save_CPPFLAGS"
  LIBS="$ac_save_LIBS"

  # remove local variables
  m4_popdef([_dune_module_plain])
  m4_popdef([_dune_module])
  m4_popdef([_dune_header])
  m4_popdef([_dune_ldpath])
  m4_popdef([_dune_lib])
  m4_popdef([_dune_symbol])
  m4_popdef([_DUNE_MODULE])

  # restore previous language settings (leave C++)
  AC_LANG_POP([C++])
])

AC_DEFUN([DUNE_CHECK_DISPATCH],[
  ifelse([$1], [], [],
         [$1], [dune-common],[
           DUNE_CHECK_MODULES([dune-common], [common/stdstreams.hh], [common], [common], 
	[#define DUNE_MINIMAL_DEBUG_LEVEL 1
	Dune::derr.active();])],
         [$1], [dune-grid],[
           DUNE_CHECK_MODULES([dune-grid], [grid/common/grid.hh], [grid], [grid], [Dune::PartitionName])],
         [$1], [dune-fem],[
           DUNE_CHECK_MODULES([dune-fem], [fem/basefunctions/common/storageinterface.hh], [], [], [])],
         [$1], [dune-istl],[
           DUNE_CHECK_MODULES([dune-istl], [istl/allocator.hh],,,)],
         [$1], [dune-disc],[
           DUNE_CHECK_MODULES([dune-disc], [disc/shapefunctions/lagrangeshapefunctions.hh], [disc], [disc], [Dune::LagrangeShapeFunctions<double[,]double[,]3>::general])],
         [AC_MSG_ERROR([Unknown module $1])])
])

AC_DEFUN([DUNE_MODULE_DEPENDENCIES],[
  ifelse($#, 0, , $#, 1, [DUNE_CHECK_DISPATCH($1)], [DUNE_CHECK_DISPATCH($1) DUNE_MODULE_DEPENDENCIES(m4_shift($@))])
])

AC_DEFUN([DUNE_DEV_MODE],[
  AC_ARG_ENABLE(dunedevel,
    AC_HELP_STRING([--enable-dunedevel],[activate Dune-Developer-mode]))

  if test x"$enable_dunedevel" = xyes ; then
    AC_DEFINE(DUNE_DEVEL_MODE, 1, [Activates developer output])
  fi
])

AC_DEFUN([DUNE_SYMLINK],[
  # create symlink for consistent paths even when $(top_srcdir) is not
  # called dune/ (if filesystem/OS supports symlinks)
  AC_PROG_LN_S
  if test x"$LN_S" = x"ln -s" ; then
    # Symlinks possible!
 
    # Note: we are currently in the build directory which may be != the
    # source directory
 
    # does a file already exist?
    if test -e dune ; then
      # exists: is a symlink?
      if test -L dune ; then
        if ! test -r dune/$ac_unique_file ; then
          AC_MSG_ERROR([Symlink 'dune' exists but appears to be wrong! Please remove it manually])
        fi
      fi
    else
      echo Creating dune-symlink...
      # set symlink in build directory to sources
      ln -s $srcdir dune

      # sanity check
      if ! test -r dune/$ac_unique_file ; then
        AC_MSG_ERROR([Sanity check for symlink failed! Please send a bugreport to dune@hal.iwr.uni-heidelberg.de])
      fi
    fi 
  else
    # no symlinks possible... check name of directory
    AC_MSG_ERROR([No symlinks supported! You have to install dune. No inplace usage possible!])
  fi
])
