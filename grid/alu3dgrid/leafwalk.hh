#ifndef __BSGRID_LEAFWALK_HH__
#define __BSGRID_LEAFWALK_HH__

//! new rule for Dune LeafIterator 
//! all entities with hav either the level or are leaf entities with one
//! level <= the desired level 
template < class A > class leaf_or_has_level
{
  int lvl_;
  public :
    //! Constructor storing the level 
    leaf_or_has_level (int i = 0) : lvl_ (i) { }
    
    //! check if go next
    int operator () (const A * x) const
    {
      return ((x->level () == lvl_) || (x->leaf () && (x->level () <= lvl_) )) ? 1 : 0 ;
    }

    //! check if go next
    int operator () (const A & x) const
    {
      return ((x.level  () == lvl_) || (x.leaf  () && (x.level  () <= lvl_) )) ? 1 : 0 ;
    }
};

#endif
