# searches for grid_dim parameter

# asks for grid-dim and pass to ALL_PKG_CPPFLAGS
AC_DEFUN([DUNE_GRID_DIMENSION],[

# default is disabled 
  AC_ARG_WITH(grid_dim,
            AC_HELP_STRING([--with-grid-dim=1|2|3],
          [dimension of grid mainly used to determine grids dimension
          during compilation (default=disabled)]),, with_grid_dim=0)
          
  AC_ARG_WITH(grid_type,
            AC_HELP_STRING([--with-grid-type= ALBERTAGRID | ALUGRID_CUBE | ALUGRID_SIMPLEX | ALUGRID_CONFORM | ONEDGRID | SGRID | UGGRID | YASPGRID],
          [only enabled if grid-dim deliverd, sets type of grid (default=YASPGRID)]),, with_grid_type=YASPGRID)
          
if test x$with_grid_dim != x0 ; then 
  
  echo -n "checking grid-dim and grid-type... "
  # make sure that given grid dim is valid 
  if test x$with_grid_dim != x1 && test x$with_grid_dim != x2 && test x$with_grid_dim != x3 ; then 
    AC_MSG_ERROR([Delivered grid-dim = $with_grid_dim is not valid. Valid are only 1, 2, or 3 .])
  fi

  # make sure that given grid type is valid 
  if test $with_grid_type != "ALBERTAGRID" && 
     test $with_grid_type != "ALUGRID_CUBE" &&
     test $with_grid_type != "ALUGRID_SIMPLEX" &&
     test $with_grid_type != "ALUGRID_CONFORM" &&
     test $with_grid_type != "ONEDGRID" &&
     test $with_grid_type != "SGRID" &&
     test $with_grid_type != "UGGRID" &&
     test $with_grid_type != "YASPGRID" ; then 
    AC_MSG_ERROR([Delivered grid-type = $with_grid_type is not valid. Run ./configure --help for valid values.])
  fi
  
  variablegriddimworld="$``(``GRIDDIM``)``"
  griddim_cppflags="-DGRIDDIM=$``(``GRIDDIM``)`` -D$``(``GRIDTYPE``)``"
  AC_SUBST(GRIDDIM, $with_grid_dim)
  AC_SUBST(GRIDDIMWORLD, $variablegriddimworld )
  AC_SUBST(GRIDTYPE, $with_grid_type )
  AC_SUBST(GRIDDIM_CPPFLAGS, $griddim_cppflags)
  DUNE_PKG_CPPFLAGS="$DUNE_PKG_CPPFLAGS $griddim_cppflags"
  echo "yes (GRIDDIM=$GRIDDIM and GRIDTYPE=$GRIDTYPE)" 
fi 
])
