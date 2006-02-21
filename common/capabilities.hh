#ifndef DUNE_CAPABILITIES_HH
#define DUNE_CAPABILITIES_HH

/** \file
    \brief A set of traits classes to store static information about grid implementation
*/

namespace Dune
{

    /** \brief Contains all capabilities classes */
namespace Capabilities
{

    /** \brief Specialize with 'true' if the grid implements a leaf iterator. */
template<class Grid>
struct hasLeafIterator
{
  static const bool v = false;
};

    /** \brief Specialize with 'true' for all codims that a grid implements entities for. */
template<class Grid, int codim>
struct hasEntity
{
  static const bool v = false;
};

    /** \todo Please doc me! */
template<class Grid>
struct isParallel
{
  static const bool v = false;
};

template<class Grid>
struct isLevelwiseConforming
{
  static const bool v = false;
};

template<class Grid>
struct hasHangingNodes
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

template<class Grid>
struct isLevelwiseConforming<const Grid>
{
  static const bool v = Dune::Capabilities::isLevelwiseConforming<Grid>::v;
};

template<class Grid>
struct hasHangingNodes<const Grid>
{
  static const bool v = Dune::Capabilities::hasHangingNodes<Grid>::v;
};

}

}

#endif // DUNE_CAPABILITIES_HH
