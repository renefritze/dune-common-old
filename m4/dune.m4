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

AC_DEFUN([DUNE_MODULE_ADD_SUMMARY_ENTRY],[
  m4_pushdef([_dune_name], [$1])
  m4_pushdef([_dune_module], [m4_translit(_dune_name, [-], [_])])
  m4_pushdef([_DUNE_MODULE], [m4_toupper(_dune_module)])
  result="$with_[]_dune_module"
  if test "x$_DUNE_MODULE[]_ROOT" != "x"; then
	result="$result ($_DUNE_MODULE[]_ROOT)"
  fi
  if test "x$_DUNE_MODULE[]_VERSION" != "x"; then
	result="$result version $_DUNE_MODULE[]_VERSION"
  fi
  DUNE_ADD_SUMMARY_MOD_ENTRY(_dune_name,[$result])
])

AC_DEFUN([DUNE_CHECK_MODULES],[
  AC_REQUIRE([AC_PROG_CXX])
  AC_REQUIRE([AC_PROG_CXXCPP])
  AC_REQUIRE([AC_PROG_LIBTOOL])
  m4_ifdef([LT_OUTPUT],[LT_OUTPUT])
  AC_REQUIRE([PKG_PROG_PKG_CONFIG])
  AC_REQUIRE([DUNE_DISABLE_LIBCHECK])

  # ____DUNE_CHECK_MODULES_____ ($1)

  m4_pushdef([_dune_name], [$1])
  m4_pushdef([_dune_module], [m4_translit(_dune_name, [-], [_])])
  m4_pushdef([_dune_header], [$2])
  m4_pushdef([_dune_ldpath], [lib])
  m4_pushdef([_dune_lib],    [m4_translit(_dune_name, [-], [])])
  m4_pushdef([_dune_symbol], [$3])
  m4_pushdef([_DUNE_MODULE], [m4_toupper(_dune_module)])

  # switch tests to c++
  AC_LANG_PUSH([C++])

  # the usual option...
  AC_ARG_WITH(_dune_name,
    AC_HELP_STRING([--with-_dune_name=PATH],[_dune_module directory]))

  # backup of flags
  ac_save_CPPFLAGS="$CPPFLAGS"
  ac_save_LIBS="$LIBS"
  ac_save_LDFLAGS="$LDFLAGS"
  CPPFLAGS=""
  LIBS=""

  ##
  ## Where is the module $1?
  ##

  AC_MSG_CHECKING([for $1 installation or source tree])

  # is a directory set?
  if test x$with_[]_dune_module = x ; then
    #
    # search module $1 via pkg-config
    #
    with_[]_dune_module="global installation"
    if test -z "$PKG_CONFIG"; then
      AC_MSG_RESULT([failed])
      AC_MSG_NOTICE([could not search for module _dune_name])
      AC_MSG_ERROR([pkg-config is required for using installed modules])
    fi
    if AC_RUN_LOG([$PKG_CONFIG --exists --print-errors "$1"]); then
      _DUNE_MODULE[]_CPPFLAGS="`$PKG_CONFIG --cflags _dune_name`" 2>/dev/null
      _DUNE_MODULE[]_ROOT="`$PKG_CONFIG --variable=prefix _dune_name`" 2>/dev/null 
      _DUNE_MODULE[]_VERSION="`$PKG_CONFIG --modversion _dune_name`" 2>/dev/null
      ifelse(_dune_symbol,,,[
        _DUNE_MODULE[]_LDFLAGS="-L`$PKG_CONFIG --variable=libdir _dune_name`" 2>/dev/null 
        _DUNE_MODULE[]_LIBS="-l[]_dune_lib"
      ])
      HAVE_[]_DUNE_MODULE=1
      AC_MSG_RESULT([
        global installation in $_DUNE_MODULE[]_ROOT])
    else
      HAVE_[]_DUNE_MODULE=0
      AC_MSG_RESULT([not found])
    fi
  else
    #
    # path for module $1 is specified via command line
    #
    if test -d $with_[]_dune_module ; then
      # expand tilde / other stuff
      _DUNE_MODULE[]_ROOT=`cd $with_[]_dune_module && pwd`

      # expand search path (otherwise empty CPPFLAGS)
      if test -d $_DUNE_MODULE[]_ROOT/include/dune; then
        # Dune was installed into directory given by with-dunecommon
        _DUNE_MODULE[]_CPPFLAGS="-I$_DUNE_MODULE[]_ROOT/include"
		_DUNE_MODULE[]_VERSION="`PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$_DUNE_MODULE[]_ROOT/lib/pkgconfig $PKG_CONFIG --modversion _dune_name`" 2>/dev/null
      else
        _DUNE_MODULE[]_CPPFLAGS="-I$_DUNE_MODULE[]_ROOT"
		_DUNE_MODULE[]_VERSION="`grep Version $_DUNE_MODULE[]_ROOT/dune.module | sed -e 's/^Version: *//'`" 2>/dev/null
      fi
      ifelse(_dune_symbol,,,[
        _DUNE_MODULE[]_LDFLAGS="-L$_DUNE_MODULE[]_ROOT/lib"
        _DUNE_MODULE[]_LIBS="-l[]_dune_lib"
      ])
      # set expanded module path
      with_[]_dune_module="$_DUNE_MODULE[]_ROOT"
      HAVE_[]_DUNE_MODULE=1
      AC_MSG_RESULT([
        found in $_DUNE_MODULE[]_ROOT])
    else
      HAVE_[]_DUNE_MODULE=0
      AC_MSG_RESULT([not found])
      AC_MSG_ERROR([_dune_name-directory $with_[]_dune_module does not exist])
    fi
  fi

  DUNE_CPPFLAGS="$DUNE_CPPFLAGS $_DUNE_MODULE[]_CPPFLAGS"
  CPPFLAGS="$DUNE_CPPFLAGS"
  SET_CPPFLAGS="$_DUNE_MODULE[]_CPPFLAGS"

  ##  
  ## check for an arbitrary header
  ##
  AC_CHECK_HEADER([dune/[]_dune_header],
    [HAVE_[]_DUNE_MODULE=1
     _DUNE_MODULE[]_CPPFLAGS="$SET_CPPFLAGS"],
    [HAVE_[]_DUNE_MODULE=0
     _DUNE_MODULE[]_CPPFLAGS=""
     if test x$_DUNE_MODULE[]_ROOT != x; then
       AC_MSG_WARN([$_DUNE_MODULE[]_ROOT does not seem to contain a valid _dune_name (dune/[]_dune_header not found)])]
     fi
  )

  ##
  ## check for lib (if lib name was provided)
  ##
  ifelse(_dune_symbol,,AC_MSG_NOTICE([_dune_name does not provide libs]),[
    if test "x$enable_dunelibcheck" != "xyes"; then
      AC_MSG_WARN([library check for _dune_name is disabled. DANGEROUS!])
    fi
    if test x$HAVE_[]_DUNE_MODULE = x1 -a x$enable_dunelibcheck = "xyes"; then

      # save current LDFLAGS
      ac_save_LDFLAGS="$LDFLAGS"
      ac_save_LIBS="$LIBS"
      ac_save_CXX="$CXX"
      HAVE_[]_DUNE_MODULE=0

      # define LTCXXCOMPILE like it will be defined in the Makefile
      LTCXXLINK="./libtool --tag=CXX --mode=link $CXX $CXXFLAGS $LDFLAGS"
      CXX="$LTCXXLINK"

      # use module LDFLAGS
      LDFLAGS="$LDFLAGS $DUNE_LDFLAGS $_DUNE_MODULE[]_LDFLAGS"
      LIBS="$DUNE_LIBS $_DUNE_MODULE[]_LIBS"

      AC_MSG_CHECKING([for lib[]_dune_lib])
      AC_TRY_LINK(dnl
        [#]include<dune/[]_dune_header>,
        _dune_symbol,
          [AC_MSG_RESULT([yes])
           HAVE_[]_DUNE_MODULE=1
           _DUNE_MODULE[]_LIBS="$LIBS"],
          [AC_MSG_RESULT([no])
           HAVE_[]_DUNE_MODULE=0
		   if test x$_DUNE_MODULE[]_ROOT != x; then
             AC_MSG_WARN([$with_[]_dune_module does not seem to contain a valid _dune_name (failed to link with lib[]_dune_lib[].la)])]
           fi
      )

      # reset variables
      LDFLAGS="$ac_save_LDFLAGS"
      LIBS="$ac_save_LIBS"
      CXX="$ac_save_CXX"
    fi
  ])

  # did we succeed?
  if test x$HAVE_[]_DUNE_MODULE = x1; then
    # set variables for our modules
    AC_SUBST(_DUNE_MODULE[]_CPPFLAGS, "$_DUNE_MODULE[]_CPPFLAGS")
    AC_SUBST(_DUNE_MODULE[]_LDFLAGS, "$_DUNE_MODULE[]_LDFLAGS")
    AC_SUBST(_DUNE_MODULE[]_LIBS, "$_DUNE_MODULE[]_LIBS")
    AC_SUBST(_DUNE_MODULE[]_ROOT, "$_DUNE_MODULE[]_ROOT")
    AC_DEFINE(HAVE_[]_DUNE_MODULE, 1, [Define to 1 if _dune_module was found])
    AC_DEFINE_UNQUOTED(_DUNE_MODULE[]_VERSION, "$_DUNE_MODULE[]_VERSION", [Define to the version of _dune_module.])

    # set DUNE_* variables
    AC_SUBST(DUNE_CPPFLAGS, "$DUNE_CPPFLAGS")
    AC_SUBST(DUNE_LDFLAGS, "$DUNE_LDFLAGS $_DUNE_MODULE[]_LDFLAGS")
    AC_SUBST(DUNE_LIBS, "$DUNE_LIBS $_DUNE_MODULE[]_LIBS")
    
    # add to global list
    # only add my flags other flags are added by other packages 
    DUNE_PKG_CPPFLAGS="$DUNE_PKG_CPPFLAGS $_DUNE_MODULE[]_CPPFLAGS"
    DUNE_PKG_LIBS="$DUNE_PKG_LIBS $LIBS"
    DUNE_PKG_LDFLAGS="$DUNE_PKG_LDFLAGS $_DUNE_MODULE[]_LDFLAGS"

    with_[]_dune_module="yes"
  else
    with_[]_dune_module="no"
  fi

  # reset previous flags
  CPPFLAGS="$ac_save_CPPFLAGS"
  LIBS="$ac_save_LIBS"

  # add this module to DUNE_SUMMARY
  DUNE_MODULE_ADD_SUMMARY_ENTRY(_dune_name)

  # remove local variables
  m4_popdef([_dune_name])
  m4_popdef([_dune_module])
  m4_popdef([_dune_header])
  m4_popdef([_dune_ldpath])
  m4_popdef([_dune_lib])
  m4_popdef([_dune_symbol])
  m4_popdef([_DUNE_MODULE])

  # restore previous language settings (leave C++)
  AC_LANG_POP([C++])
])

AC_DEFUN([DUNE_FEM_CHECK_MODULE],
[
    DUNE_CHECK_MODULES([dune-fem], [fem/space/basefunctions/storageinterface.hh], [] )
])
AC_DEFUN([DUNE_FEM_CHECKS])

AC_DEFUN([DUNE_DISC_CHECK_MODULE],
[
    DUNE_CHECK_MODULES([dune-disc], [disc/shapefunctions/lagrangeshapefunctions.hh], [Dune::LagrangeShapeFunctions<double[,]double[,]3>::general])
])
AC_DEFUN([DUNE_DISC_CHECKS])

AC_DEFUN([DUNE_SUBGRID_CHECK_MODULE],
[
    DUNE_CHECK_MODULES([dune-subgrid], [subgrid/subgrid.hh], [])
])
AC_DEFUN([DUNE_SUBGRID_CHECKS])

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
        AC_MSG_ERROR([Sanity check for symlink failed! Please send a bugreport to dune@dune-project.org])
      fi
    fi 
  else
    # no symlinks possible... check name of directory
    AC_MSG_ERROR([No symlinks supported! You have to install dune. No inplace usage possible!])
  fi
])

AC_DEFUN([DUNE_WEB],
[
  # special variable to include the documentation into the website
  AC_ARG_WITH(duneweb,
    AC_HELP_STRING([--with-duneweb=PATH],[Only needed for website-generation, path to checked out version of dune-web]))

  if test x$with_duneweb != x; then
     if test x$with_duneweb != xno; then
       # parameter is set. Check it
       AC_MSG_CHECKING([whether passed Dune-Web ($with_duneweb) directory appears correct])
       WEBTESTFILE="$with_duneweb/layout/default.wml"
       if test -d "$with_duneweb" && test -e "$WEBTESTFILE" ; then
         AC_MSG_RESULT([ok])
         # normalize path
         with_duneweb=`(cd $with_duneweb && pwd)` ;
       else
         if test -d "$with_duneweb" ; then
           AC_MSG_ERROR([$WEBTESTFILE not found in Dune-web dir $with_duneweb!])
         else
           AC_MSG_ERROR([Dune-Web directory $with_duneweb not found!])
         fi
		 with_duneweb=no
	   fi
     fi
	 DUNE_ADD_SUMMARY_ENTRY([dune web],[$with_duneweb])
  else
    with_duneweb=no
  fi
  AC_SUBST(DUNEWEBDIR, $with_duneweb)
  AM_CONDITIONAL(DUNEWEB, test x$with_duneweb != xno)
])
