#ifndef DUNE_GRID_LEAFITERATOR_HH
#define DUNE_GRID_LEAFITERATOR_HH

namespace Dune
{

template<class GridImp, template<class> class LeafIteratorImp>
class LeafIterator :
    public EntityPointer<GridImp, LeafIteratorImp<GridImp> >
{
public:
  typedef typename GridImp::template codim<0>::Entity Entity;

  /** @brief Preincrement operator. */
  LeafIterator& operator++()
    {
      this->realIterator.increment();
      return *this;
    }
  
  /** @brief Postincrement operator. */
  LeafIterator& operator++(int)
    {
      this->realIterator.operator++();
      return *this;
    }
  
  /** @brief copy constructor from LevelIteratorImp */
  LeafIterator (const LeafIteratorImp<const GridImp> & i) : 
    EntityPointer<GridImp, LeafIteratorImp<GridImp> >(i) {};
};

/*
  LeafIteratorInterface
 */
template<class GridImp, template<class> class LeafIteratorImp>
class LeafIteratorInterface
{
public:
  typedef typename GridImp::template codim<0>::Entity Entity;

  //! increment
  void increment()
    {
      asImp().increment();
    }

private:
  //!  Barton-Nackman trick
  LeafIteratorImp<GridImp>& asImp ()
    {return static_cast<LeafIteratorImp<GridImp>&>(*this);}
  const LeafIteratorImp<GridImp>& asImp () const
    {return static_cast<const LeafIteratorImp<GridImp>&>(*this);}
};
  
template<class GridImp, template<class> class LeafIteratorImp>
class LeafIteratorDefault
: public LeafIteratorInterface <GridImp,LeafIteratorImp>
{
private:
  //!  Barton-Nackman trick
  LeafIteratorImp<GridImp>& asImp ()
    {return static_cast<LeafIteratorImp<GridImp>&>(*this);}
  const LeafIteratorImp<GridImp>& asImp () const
    {return static_cast<const LeafIteratorImp<GridImp>&>(*this);}
};

template<class GridImp>
class GenericLeafIterator :
  public GridImp::template codim<0>::EntityPointer::base,
  public LeafIteratorDefault<GridImp, GenericLeafIterator>
{
  friend class GenericLeafIterator<const GridImp>;
  typedef typename GridImp::template codim<0>::EntityPointer::base EntityPointerImp;
public:
  typedef typename GridImp::template codim<0>::Entity Entity;
  typedef typename GridImp::template codim<0>::LevelIterator LevelIterator;
  typedef typename GridImp::template codim<0>::HierarchicIterator HierarchicIterator;
  //! increment
  void increment()
    {
      while(true)
      {
        ++hit;
        if (hit == hend)
        {
          ++lit;
          if (lit == lend)
          {
            break;
          }
          else
          {
            hit = lit->hbegin(maxlevel);
            hend = lit->hend(maxlevel);
          }
        }
        if(hit->level() == maxlevel) break;
        if(hit->isLeaf()) break;
      }
      static_cast<EntityPointerImp&>(*this) = hit.realIterator;
    }

  GenericLeafIterator(GridImp & g, int maxl, bool end) :
    EntityPointerImp(g.template lbegin<0>(0).realIterator),
    grid(g), maxlevel(maxl),
    lit(grid.template lbegin<0>(0)),
    lend(grid.template lend<0>(0)),
    hit(lit->hbegin(maxlevel)),
    hend(lit->hend(maxlevel))
    {
      if (end)
      {
        lit = grid.template lend<0>(0);
      }
      static_cast<EntityPointerImp&>(*this) = hit.realIterator;
    }

  GenericLeafIterator(const GenericLeafIterator & rhs) :
    EntityPointerImp(rhs.hit.realIterator),
    grid(rhs.grid), maxlevel(rhs.maxlevel),
    lit(rhs.lit), lend(rhs.lend),
    hit(rhs.hit), hend(rhs.hend)
    {}
  
private:
  GridImp & grid;
  const int maxlevel;
  LevelIterator lit;
  LevelIterator lend;
  HierarchicIterator hit;
  HierarchicIterator hend;
};

}

#endif // DUNE_GRID_LEAFITERATOR_HH
