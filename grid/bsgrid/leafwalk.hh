#ifndef __BSGRID_LEAFWALK_HH__
#define __BSGRID_LEAFWALK_HH__

// new rule for Dune LeafIterator 
// all entities with hav either the level or are leaf entities with one
// level <= the desired level 
template < class A > class leaf_or_has_level
{
  int lvl;
  public :
    // Constructor storing the level 
    leaf_or_has_level (int i = 0) : lvl (i) { }

    int operator () (const A * x) const
    {
      return ((x->level () == lvl) || (x->leaf () && (x->level () <= lvl) )) ? 1 : 0 ;
    }
    int operator () (const A & x) const
    {
      return ((x.level  () == lvl) || (x.leaf  () && (x.level  () <= lvl) )) ? 1 : 0 ;
    }
};

#endif
