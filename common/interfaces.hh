#ifndef DUNE_INTERFACES_HH
#define DUNE_INTERFACES_HH

namespace Dune {

  //! An interface class for cloneable objects
  struct Cloneable {

      /** \brief ???
       \todo Please doc me!!!
      */
    virtual Cloneable* clone() const = 0;
  };

} // end namespace Dune

#endif
