#ifndef DUNE_CAPABILITIES_HH
#define DUNE_CAPABILITIES_HH

namespace Dune
{

namespace Capabilities
{

template<class Grid>
struct hasLeafIterator
{
  static const bool v = false;
};

template<class Grid, int codim>
struct hasEntity
{
  static const bool v = false;
};

template<class Grid>
struct isParallel
{
  static const bool v = false;
};

/*
  forward
  Capabilities::Something<const Grid>
  to
  Capabilities::Something<Grid>
*/

template<class Grid>
struct hasLeafIterator<const Grid>
{
  static const bool v = Dune::Capabilities::hasLeafIterator<Grid>::v;
};

template<class Grid, int codim>
struct hasEntity<const Grid, codim>
{
  static const bool v = Dune::Capabilities::hasEntity<Grid,codim>::v;
};

template<class Grid>
struct isParallel<const Grid>
{
  static const bool v = Dune::Capabilities::isParallel<Grid>::v;
};

}

}

#endif // DUNE_CAPABILITIES_HH
