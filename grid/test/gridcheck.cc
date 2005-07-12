// $Id$

/*

  Implements a generic grid check

  TODO:
  - check return types
  
*/

#include "../../common/capabilities.hh"
#include "../../common/helpertemplates.hh"
#include "../../common/exceptions.hh"
#include "../../common/stdstreams.hh"

#include <limits>

// machine epsilon is multiplied by this factor 
static double factorEpsilon = 10.0;

class CheckError : public Dune::Exception {};

// --- compile-time check of element-interface

template <class Geometry, bool doCheck>
struct JacobianInverse
{
  static void check(const Geometry &e) 
    {
      typedef typename Geometry::ctype ctype;
      Dune::FieldVector<ctype, Geometry::mydimension> v;
      e.jacobianInverse(v);
    }
  JacobianInverse() 
    {
      c = check;
    };
  void (*c)(const Geometry&);
};

template <class Geometry>
struct JacobianInverse<Geometry, false>
{
  static void check(const Geometry &e) 
    {
    }
  JacobianInverse() 
    {
      c = check;
    };
  void (*c)(const Geometry&);
};

template <class Geometry, int codim, int dim>
struct GeometryInterface 
{
  static void check(const Geometry &e) 
    {
      IsTrue<dim-codim == Geometry::mydimension>::yes();
      IsTrue<dim == Geometry::dimension>::yes();
      
      typedef typename Geometry::ctype ctype;
      
      e.type();
      e.corners();
      e[0];
      e.refelem();
      
      Dune::FieldVector<ctype, Geometry::mydimension> v;
      e.global(v);
      Dune::FieldVector<ctype, Geometry::coorddimension> g;
      e.local(g);
      e.checkInside(v);
      e.integrationElement(v);
      JacobianInverse<Geometry,
        (int)Geometry::coorddimension == (int)Geometry::mydimension>();
    }
  GeometryInterface() 
    {
      c = check;
    };
  void (*c)(const Geometry&);
};

// reduced test on vertices
template <class Geometry, int dim>
struct GeometryInterface <Geometry, dim, dim>
{
  static void check(const Geometry &e) 
    {
      IsTrue<0 == Geometry::mydimension>::yes();
      IsTrue<dim == Geometry::dimension>::yes();
      
      // vertices have only a subset of functionality
      e.type();
      e.corners();
      e[0];
    }
  GeometryInterface() 
    {
      c = check;
    };
  void (*c)(const Geometry&);
};

// --- compile-time check of entity-interface

// tests that should work on entities of all codimensions
template <class Entity>
void DoEntityInterfaceCheck (Entity &e) 
{
  // exported types
  typedef typename Entity::ctype ctype;
  
  // methods on each entity
  e.level();
  e.index();
  e.partitionType();
  e.geometry();
  
  // check interface of attached element-interface
  GeometryInterface<typename Entity::Geometry, Entity::codimension, Entity::dimension>();
}

// recursive check of codim-0-entity methods count(), entity(), subIndex()
template <class Grid, int cd, bool hasEntity>
struct ZeroEntityMethodCheck 
{
  typedef typename Grid::template Codim<0>::Entity Entity;
  static void check(Entity &e)
    {
      // check types
      typedef typename Entity::IntersectionIterator IntersectionIterator;
      typedef typename Entity::HierarchicIterator HierarchicIterator;
      typedef typename Entity::EntityPointer EntityPointer;

      e.template count<cd>();
      e.template entity<cd>(0);
      e.template subIndex<cd>(0);

      // recursively check on
      ZeroEntityMethodCheck<Grid, cd - 1,
        Dune::Capabilities::hasEntity<Grid, cd - 1>::v >();
    }
  ZeroEntityMethodCheck () 
    {
      c = check;
    }
  void (*c)(Entity &e);
};

// just the recursion if the grid does not know about this codim-entity
template<class Grid, int cd>
struct ZeroEntityMethodCheck<Grid, cd, false>
{
  typedef typename Grid::template Codim<0>::Entity Entity;
  static void check(Entity &e)
    {
      // check types
      typedef typename Entity::IntersectionIterator IntersectionIterator;
      typedef typename Entity::HierarchicIterator HierarchicIterator;
      typedef typename Entity::EntityPointer EntityPointer;
      
      // recursively check on
      ZeroEntityMethodCheck<Grid, cd - 1,
         Dune::Capabilities::hasEntity<Grid, cd - 1>::v >();
    }
  ZeroEntityMethodCheck () 
    {
      c = check;
    }
  void (*c)(Entity &e);
};

// end recursive checking
template <class Grid>
struct ZeroEntityMethodCheck<Grid, 0, true>
{
  typedef typename Grid::template Codim<0>::Entity Entity;
  static void check(Entity &e)
    {
      // check types
      typedef typename Entity::IntersectionIterator IntersectionIterator;
      typedef typename Entity::HierarchicIterator HierarchicIterator;
      typedef typename Entity::EntityPointer EntityPointer;
      
      e.template count<0>();
      e.template entity<0>(0);
      e.template subIndex<0>(0);
    }
  ZeroEntityMethodCheck () 
    {
      c = check;
    }
  void (*c)(Entity &e);
};

// end recursive checking - same as true
// ... codim 0 is always needed
template <class Grid>
struct ZeroEntityMethodCheck<Grid, 0, false>
{
  typedef typename Grid::template Codim<0>::Entity Entity;
  static void check(Entity &e)
    {
      // check types
      typedef typename Entity::IntersectionIterator IntersectionIterator;
      typedef typename Entity::HierarchicIterator HierarchicIterator;
      typedef typename Entity::EntityPointer EntityPointer;
      
      e.template count<0>();
      e.template entity<0>(0);
      e.template subIndex<0>(0);
    }
  ZeroEntityMethodCheck () 
    {
      c = check;
    }
  void (*c)(Entity &e);
};

// check codim-entity and pass on to codim + 1 
template <class Grid, int codim, int dim, bool hasEntity>
struct EntityInterface
{
  typedef typename Grid::template Codim<codim>::Entity Entity;
  
  static void check (Entity &e)
    {
      // consistent?
      IsTrue<codim == Entity::codimension>::yes();
      IsTrue<dim == Entity::dimension>::yes();      

      // do the checking
      DoEntityInterfaceCheck(e);
      
      // recursively check sub-entities
      EntityInterface<Grid, codim + 1, dim,
        Dune::Capabilities::hasEntity<Grid, codim + 1>::v >();
    }
  EntityInterface ()
    {
      c = check;  
    }
  void (*c)(Entity&);    
};

// just the recursion if the grid does not know about this codim-entity
template <class Grid, int codim, int dim>
struct EntityInterface<Grid, codim, dim, false>
{
  typedef typename Grid::template Codim<codim>::Entity Entity;
  
  static void check (Entity &e)
    {
      // recursively check sub-entities
      EntityInterface<Grid, codim + 1, dim,
        Dune::Capabilities::hasEntity<Grid, codim + 1>::v >();
    }
  EntityInterface ()
    {
      c = check;
    }
  void (*c)(Entity&);    
};

// codim-0 entities have different interface
template <class Grid, int dim>
struct EntityInterface<Grid, 0, dim, true>
{
  typedef typename Grid::template Codim<0>::Entity Entity;
  
  static void check (Entity &e)
    {
      // consistent?
      IsTrue<0 == Entity::codimension>::yes();
      IsTrue<dim == Entity::dimension>::yes();      

      // do the common checking
      DoEntityInterfaceCheck(e);

      // special codim-0-entity methods which are parametrized by a codimension
      ZeroEntityMethodCheck
        <Grid, dim, Dune::Capabilities::hasEntity<Grid, dim>::v >();

      // grid hierarchy
      e.father();
      e.geometryInFather();

      // intersection iterator
      e.ibegin();      
      e.iend();

      // hierarchic iterator
      e.hbegin(0);
      e.hend(0);

      // adaption
      e.state();

      // recursively check sub-entities
      EntityInterface<Grid, 1, dim,
        Dune::Capabilities::hasEntity<Grid, 1>::v >();      
    }
  EntityInterface ()
    {
      c = check;  
    }
  void (*c)(Entity&);    
};

// non existinng codim-0 entity
template <class Grid, int dim>
struct EntityInterface<Grid, 0, dim, false>
{
  typedef typename Grid::template Codim<0>::Entity Entity;

  static void check (Entity &e)
    {
      // recursively check sub-entities
      EntityInterface<Grid, 1, dim,
        Dune::Capabilities::hasEntity<Grid, 1>::v >();      
    }
  EntityInterface ()
    {
      c = check;
    }
  void (*c)(Entity&);    
};

// end the recursion over entity-codimensions
template <class Grid, int dim>
struct EntityInterface<Grid, dim, dim, true>
{
  typedef typename Grid::template Codim<dim>::Entity Entity;
  
  // end recursion
  static void check (Entity &e)
    {
      // consistent?
      IsTrue<dim == Entity::codimension>::yes();
      IsTrue<dim == Entity::dimension>::yes();
      
      // run common test
      DoEntityInterfaceCheck(e);      

      // grid hierarchy
      e.ownersFather();
      e.positionInOwnersFather();
    }
  
  EntityInterface()
    {
      c = check;  
    }
  void (*c)(Entity&);    
};

// end the recursion over entity-codimensions
// ... codim dim entity does not exist
template <class Grid, int dim>
struct EntityInterface<Grid, dim, dim, false>
{
  typedef typename Grid::template Codim<dim>::Entity Entity;
  
  // end recursion
  static void check (Entity &e)
    {
    }
  
  EntityInterface()
    {
      c = check;  
    }
  void (*c)(Entity&);    
};

template<class Grid, bool hasLeaf>
struct LeafInterface
{
  static void check(Grid &g)
    {
      g.leafbegin(0);
      g.leafend(0);
    }
  LeafInterface()
    {
      c = check;  
    }
  void (*c)(Grid&);  
};
template<class Grid>
struct LeafInterface<Grid, false>
{
  static void check(Grid &g) {}
  LeafInterface()
    {
      c = check;  
    }
  void (*c)(Grid&);  
};

template <class Grid>
struct GridInterface
{
  static void check (Grid &g)
    {
      // check for exported types
      typedef typename Grid::ctype ctype;
      typedef typename Grid::template Codim<0>::LevelIterator LevelIterator;
      typedef typename Grid::template Codim<0>::EntityPointer EntityPointer;
      typedef typename Grid::LeafIterator LeafIterator;
      
      // check for member functions
      g.maxlevel();
      g.size(0,0);
      g.overlapSize(0,0);
      g.ghostSize(0,0);

      // adaption
      EntityPointer ept = g.template lbegin<0>(0);
      g.mark(100, ept);
      g.adapt();
      g.preAdapt();
      g.postAdapt();
      
      // check for iterator functions
      g.template lbegin<0>(0);
      g.template lend<0>(0);

      LeafInterface< Grid, Dune::Capabilities::hasLeafIterator<Grid>::v >();
      
      // recursively check entity-interface
      // ... we only allow grids with codim 0 zero entites
      IsTrue<Dune::Capabilities::hasEntity<Grid, 0>::v>::yes();
      IsTrue<Dune::Capabilities::hasEntity<const Grid, 0>::v>::yes();
      
      EntityInterface<Grid, 0, Grid::dimension,
        Dune::Capabilities::hasEntity<Grid, 0>::v >();

      // !!! check for parallel grid?
      /*
      g.template lbegin<0, Dune::Ghost_Partition>(0);
      g.template lend<0, Dune::Ghost_Partition>(0);
      */
    }
  GridInterface()
    {
      c = check;
    }
  // member just to avoid "unused variable"-warning in constructor
  void (*c)(Grid&);
};

// check Entity::geometry()[c] == Entity::entity<dim>.geometry()[0] for codim=cd
template <int cd, class Grid, class Entity, bool doCheck>
struct subIndexCheck
{
  subIndexCheck (const Entity & e)
    {
      const int imax = e.template count<cd>();
      for (int i=0; i<imax; ++i)
      {
       if( (e.template entity<cd>(i)->index() != e.template subIndex<cd>(i)      ) &&
           (e.template entity<cd>(i)->globalIndex() != e.template subIndex<cd>(i)) )
          DUNE_THROW(CheckError, "e.template entity<cd>(i)->index() == e.template subIndex<cd>(i) && "<<
                 "e.template entity<cd>(i)->globalIndex() != e.template subIndex<cd>(i) faild!");
      }
      subIndexCheck<cd-1,Grid,Entity,
        Dune::Capabilities::hasEntity<Grid,cd-1>::v> sick(e);
    }
};
// end recursion of subIndexCheck
template <class Grid, class Entity, bool doCheck>
struct subIndexCheck<-1, Grid, Entity, doCheck>
{
  subIndexCheck (const Entity & e)
    {
      return;
    }
};
// do nothing if doCheck==false
template <int cd, class Grid, class Entity>
struct subIndexCheck<cd, Grid, Entity, false>
{
  subIndexCheck (const Entity & e)
    {
      subIndexCheck<cd-1,Grid,Entity,
        Dune::Capabilities::hasEntity<Grid,cd-1>::v> sick(e);
    }
};
template <class Grid, class Entity>
struct subIndexCheck<-1, Grid, Entity, false>
{
  subIndexCheck (const Entity & e)
    {
      return;
    }
};

// name says all
template <class Grid>
void zeroEntityConsistency (Grid &g)
{
  typedef typename Grid::template Codim<0>::LevelIterator LevelIterator;
  typedef typename Grid::template Codim<0>::Geometry Geometry;
  typedef typename Grid::template Codim<0>::Entity Entity;
  LevelIterator it = g.template lbegin<0>(g.maxlevel());
  const LevelIterator endit = g.template lend<0>(g.maxlevel());
  
  for (; it!=endit; ++it)
  {
    // Entity::entity<0>(0) == Entity
//    assert( it->template entity<0>(0)->index() == it->index() );
//    assert( it->template entity<0>(0)->level() == it->level() );
    // Entity::count<dim>() == Entity::geometry().corners();
    assert( it->template count<Grid::dimension>() == it->geometry().corners() );
    // Entity::geometry()[c] == Entity::entity<dim>.geometry()[0];
    const int cmax = it->template count<Grid::dimension>();
    for (int c=0; c<cmax; ++c)
    {
      Dune::FieldVector<typename Grid::ctype, Grid::dimensionworld> c1(it->geometry()[c]);
      Dune::FieldVector<typename Grid::ctype, Grid::dimensionworld> c2(it->template entity<Grid::dimension>(c)->geometry()[0]);
      if( (c2-c1).two_norm() > 10 * std::numeric_limits<typename Grid::ctype>::epsilon() )
      {
        DUNE_THROW(CheckError, "geometry[i] == entity<dim>(i) failed: || c1-c2 || = || " <<
                   c1 << " - " << c2 << " || = " << (c2-c1).two_norm() << " [ with i = " << c << " ]");
      }
    }
    // Entity::entity<cd>(i).index() == Entity::subIndex(i)
    subIndexCheck<Grid::dimension, Grid, Entity, true> sick(*it);
  }
}

/*
 * search the LevelIterator for each IntersectionIterator
 */
template <class Grid>
void assertNeighbor (Grid &g)
{
  typedef typename Grid::template Codim<0>::LevelIterator LevelIterator;
  typedef typename Grid::template Codim<0>::IntersectionIterator IntersectionIterator;
  LevelIterator e = g.template lbegin<0>(0);
  const LevelIterator eend = g.template lend<0>(0);
  LevelIterator next = e; ++next;
  if (next != eend)
  {
    for (;e != eend; ++e)
    {
      IntersectionIterator endit = e->iend();
      IntersectionIterator it = e->ibegin();
      assert(e->index() >= 0);
      assert(it != endit);
      for(; it != endit; ++it)
      {
        if (it.neighbor())
        {
          assert(it.outside()->index() >= 0);
          assert(it.outside()->index() != e->index());
          LevelIterator n = g.template lbegin<0>(it.level());
          LevelIterator nend = g.template lend<0>(it.level());
          while (n != it.outside() && n != nend) {
            assert(it.outside()->index() != n->index());
            ++n;
          }
        }
      }
    }
  }
}

/*
 * Iterate over the grid und do some runtime checks
 */
template <class Grid, class It>
struct _callMark {
  static void mark (Grid & g, It it) { g.mark(1,it); };
};
template <class Grid, class It>
struct _callMark<const Grid, It> {
  static void mark (const Grid & g, It it) { };
};
template <class Grid, class It>
void callMark(Grid & g, It it)
{
  _callMark<Grid,It>::mark(g,it);
}

template <class Grid>
void iterate(Grid &g)
{
  typedef typename Grid::template Codim<0>::LevelIterator LevelIterator;
  typedef typename Grid::template Codim<0>::EntityPointer EntityPointer;
  typedef typename Grid::template Codim<0>::HierarchicIterator HierarchicIterator;
  typedef typename Grid::template Codim<0>::Geometry Geometry;
  LevelIterator it = g.template lbegin<0>(0);
  const LevelIterator endit = g.template lend<0>(0);

  Dune::FieldVector<typename Grid::ctype, Grid::dimension> origin(1);
  Dune::FieldVector<typename Grid::ctype, Grid::dimension> result;

  for (;it != endit; ++it)
  {
    LevelIterator l1 = it;
    LevelIterator l2 = l1++;
    assert(l2 == it);
    assert(l1 != it);
    l2++;
    assert(l1 == l2);
    
    result = it->geometry().local(it->geometry().global(origin));
    typename Grid::ctype error = (result-origin).two_norm();
    if(error >= factorEpsilon * std::numeric_limits<typename Grid::ctype>::epsilon())
      {
        DUNE_THROW(CheckError, "|| geom.local(geom.global(" << origin
                   << ")) - origin || != 0 ( || " << result << " - origin || ) = " << error);
      };
    it->geometry().integrationElement(origin);
    if((int)Geometry::coorddimension == (int)Geometry::mydimension)
      it->geometry().jacobianInverse(origin);

    it->geometry().type();
    it->geometry().corners();
    it->geometry()[0];

    callMark(g, it);
    EntityPointer ept = it;
    callMark(g, ept);
    HierarchicIterator hit = ept->hbegin(99);
    HierarchicIterator hend = ept->hend(99);
    if (hit != hend) callMark(g, hit);
  }
  
  typedef typename Grid::template Codim<0>::LeafIterator LeafIterator;
  LeafIterator lit = g.leafbegin(g.maxlevel());
  const LeafIterator lend = g.leafend(g.maxlevel());
  if(lit == lend)
    DUNE_THROW(CheckError, "leafbegin() == leafend()");
  for (;lit != lend; ++lit)
  {
    LeafIterator l1 = lit;
    LeafIterator l2 = l1++;
    assert(l2 == lit);
    assert(l1 != lit);
    l2++;
    assert(l1 == l2);
    
    result = lit->geometry().local(lit->geometry().global(origin));
    typename Grid::ctype error = (result-origin).two_norm();
    if(error >= factorEpsilon * std::numeric_limits<typename Grid::ctype>::epsilon())
      {
        DUNE_THROW(CheckError, "|| geom.local(geom.global(" << origin
                   << ")) - origin || != 0 ( || " << result << " - origin || ) = " << error);
      };
    lit->geometry().integrationElement(origin);
    if((int)Geometry::coorddimension == (int)Geometry::mydimension)
      lit->geometry().jacobianInverse(origin);

    lit->geometry().type();
    lit->geometry().corners();
    lit->geometry()[0];
  }

};

template <class Grid>
void iteratorEquals (Grid &g)
{
  typedef typename Grid::template Codim<0>::LevelIterator LevelIterator;
  typedef typename Grid::template Codim<0>::LeafIterator LeafIterator;
  typedef typename Grid::template Codim<0>::HierarchicIterator HierarchicIterator;
  typedef typename Grid::template Codim<0>::IntersectionIterator IntersectionIterator;
  typedef typename Grid::template Codim<0>::EntityPointer EntityPointer;

  LevelIterator l1 = g.template lbegin<0>(0);
  LevelIterator l2 = g.template lbegin<0>(0);
  LeafIterator L1 = g.leafbegin(99);
  LeafIterator L2 = g.leafbegin(99);
  HierarchicIterator h1 = l1->hbegin(99);
  HierarchicIterator h2 = l2->hbegin(99);
  IntersectionIterator i1 = l1->ibegin();
  IntersectionIterator i2 = l2->ibegin();
  EntityPointer e1 = l1;
  EntityPointer e2 = h2;

  l1 == l2;
  i1 == i2;
  L1 == L2;
  h1 == h2;
  e1 == e2;
  e1 == l2;
  e2 == h2;
  e1 == L1;
  l2 == e1;
  l2 == L2;
  i1.inside() == h2;
  i1.outside() == h2;
}

template <class Grid>
void gridcheck (Grid &g)
{
  /*
   * first do the compile-test: this will not produce any code but
   * fails if an interface-component is missing
   */
  GridInterface<Grid>();

  /*
   * now the runtime-tests
   */
  const Grid & cg = g;
  iteratorEquals(g);
  iteratorEquals(cg);
  iterate(g);
  iterate(cg);
  zeroEntityConsistency(g);
  zeroEntityConsistency(cg);
  assertNeighbor(g);
  assertNeighbor(cg);

};
