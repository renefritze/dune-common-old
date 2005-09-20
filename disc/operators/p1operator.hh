// $Id$

#ifndef __DUNE_P1OPERATOR_HH__
#define __DUNE_P1OPERATOR_HH__

#include<iostream>
#include<vector>
#include"common/fvector.hh"
#include"common/exceptions.hh"
#include"grid/common/grid.hh"
#include"grid/common/mcmgmapper.hh"
#include"istl/bvector.hh"
#include"istl/operators.hh"
#include"istl/bcrsmatrix.hh"

/**
 * @file 
 * @brief  defines a class for piecewise linear finite element functions
 * @author Peter Bastian
 */
namespace Dune
{
  /** @addtogroup DISC
   *
   * @{
   */
  /**
   * @brief defines a class for piecewise linear finite element functions
   *
   */

  // template meta program for inserting indices
  template<int n, int c>
  struct P1FEOperator_meta {
	template<class Entity, class VMapper, class AMapper, class Refelem, class Matrix>
	static void addrowscube (const Entity& e, const VMapper& vertexmapper, const AMapper& allmapper, 
				   const Refelem& refelem, Matrix& A, std::vector<bool>& visited)
	{
	  for (int i=0; i<refelem.size(c); i++) // loop over subentities of codim c of e
		{
		  int index = allmapper.template submap<c>(e,i);
		  if (!visited[index]) 
			{
			  int corners = refelem.size(i,c,n);
			  for (int j=0; j<corners/2; j++) // uses fact that diagonals are (0,corners-1), (1,corners-2) ...
				{
				  int alpha = vertexmapper.template submap<n>(e,refelem.subentity(i,c,j,n));
				  int beta = vertexmapper.template submap<n>(e,refelem.subentity(i,c,corners-1-j,n));
				  A.incrementrowsize(alpha);
				  A.incrementrowsize(beta);
				}
			  visited[index] = true;
			}
		}
	  P1FEOperator_meta<n,c-1>::addrowscube(e,vertexmapper,allmapper,refelem,A,visited);
	  return;
	}
	template<class Entity, class VMapper, class AMapper, class Refelem, class Matrix>
	static void addindicescube (const Entity& e, const VMapper& vertexmapper, const AMapper& allmapper, 
				   const Refelem& refelem, Matrix& A, std::vector<bool>& visited)
	{
	  for (int i=0; i<refelem.size(c); i++)
		{
		  int index = allmapper.template submap<c>(e,i,0);
		  if (!visited[index]) 
			{
			  int corners = refelem.size(i,c,n);
			  for (int j=0; j<corners/2; j++) // uses fact that diagonals are (0,corners-1), (1,corners-2) ...
				{
				  int alpha = vertexmapper.template submap<n>(e,refelem.subentity(i,c,j,n));
				  int beta = vertexmapper.template submap<n>(e,refelem.subentity(i,c,corners-1-j,n));
				  A.addindex(alpha,beta);
				  A.addindex(beta,alpha);
				}
			  visited[index] = true;
			}
		}
	  P1FEOperator_meta<n,c-1>::addindicescube(e,vertexmapper,allmapper,refelem,A,visited);
	  return;
	}
  };
  template<int n>
  struct P1FEOperator_meta<n,0> {
	template<class Entity, class VMapper, class AMapper, class Refelem, class Matrix>
	static void addrowscube (const Entity& e, const VMapper& vertexmapper, const AMapper& allmapper, 
				  const Refelem& refelem, Matrix& A, std::vector<bool>& visited)
	{
	  int corners = refelem.size(n);
	  for (int j=0; j<corners/2; j++) // uses fact that diagonals are (0,corners-1), (1,corners-2) ...
		{
		  int alpha = vertexmapper.template submap<n>(e,refelem.subentity(0,0,j,n));
		  int beta = vertexmapper.template submap<n>(e,refelem.subentity(0,0,corners-1-j,n));
		  A.incrementrowsize(alpha);
		  A.incrementrowsize(beta);
		}
	  return;
	}
	template<class Entity, class VMapper, class AMapper, class Refelem, class Matrix>
	static void addindicescube (const Entity& e, const VMapper& vertexmapper, const AMapper& allmapper, 
				  const Refelem& refelem, Matrix& A, std::vector<bool>& visited)
	{
	  int corners = refelem.size(n);
	  for (int j=0; j<corners/2; j++) // uses fact that diagonals are (0,corners-1), (1,corners-2) ...
		{
		  int alpha = vertexmapper.template submap<n>(e,refelem.subentity(0,0,j,n));
		  int beta = vertexmapper.template submap<n>(e,refelem.subentity(0,0,corners-1-j,n));
		  A.addindex(alpha,beta);
		  A.addindex(beta,alpha);
		}
	  return;
	}
  };


  //! a class for mapping a P1 function to a P1 function
  template<class G, class RT, class IS>
  class AssembledP1FEOperator
  {
	typedef typename G::ctype DT;
	enum {n=G::dimension};
	typedef typename G::Traits::template Codim<0>::Entity Entity;
	typedef typename IS::template Codim<0>::template Partition<All_Partition>::Iterator Iterator;

	// a function to compute the number of nonzeros
	// does not work for prisms and pyramids yet ?!
	int nnz (const G& g)
	{
	  int s = 0;
	  s += g.size(n);   // vertices
	  s += 2*g.size(n-1); // edges
	  for (int c=0; c<n-1; c++)
		s += 2*g.size(c,cube)*(1<<(n-c-1));
	  return s;
	}

	// mapper: one data element per vertex
	template<int dim>
	struct P1Layout
	{
	  bool contains (int codim, Dune::GeometryType gt)
	  {
		if (codim==dim) return true;
		return false;
	  }
	}; 

	// mapper: one data element in every entity
	template<int dim>
	struct AllLayout
	{
	  bool contains (int codim, Dune::GeometryType gt)
	  {
		return true;
	  }
	}; 


  public:
	// export type used to store the matrix
	typedef BCRSMatrix<FieldMatrix<RT,1,1> > RepresentationType;

	AssembledP1FEOperator (const G& g, const IS& indexset) : grid(g),is(indexset),A(g.size(n),g.size(n),nnz(g),RepresentationType::random),
									vertexmapper(g,indexset),allmapper(g,indexset)
	{
	  // set size of all rows to zero
	  for (int i=0; i<g.size(n); i++)
		A.setrowsize(i,0); 

	  // build needs a flag for all entities of all codims
	  std::vector<bool> visited(allmapper.size());
	  for (int i=0; i<allmapper.size(); i++) visited[i] = false;

	  // handle each element and compute row sizes
	  Iterator eendit = is.template end<0,All_Partition>();
	  for (Iterator it = is.template begin<0,All_Partition>(); it!=eendit; ++it)
		{
		  Dune::GeometryType gt = it->geometry().type();
		  const typename Dune::ReferenceElementContainer<DT,n>::value_type& 
			refelem = ReferenceElements<DT,n>::general(gt);

		  // vertices, c=n
		  for (int i=0; i<refelem.size(n); i++)
			{
			  int index = allmapper.template submap<n>(*it,i);
			  int alpha = vertexmapper.template submap<n>(*it,i);
			  if (!visited[index]) 
				{
				  A.incrementrowsize(alpha);
				  visited[index] = true;
				}
			}

		  // edges for all element types, c=n-1
		  for (int i=0; i<refelem.size(n-1); i++)
			{
			  int index = allmapper.template submap<n-1>(*it,i);
			  int alpha = vertexmapper.template submap<n>(*it,refelem.subentity(i,n-1,0,n));
			  int beta = vertexmapper.template submap<n>(*it,refelem.subentity(i,n-1,1,n));
			  if (!visited[index]) 
				{
				  A.incrementrowsize(alpha);
				  A.incrementrowsize(beta);
				  visited[index] = true;
				}
			}

		  // for codim n-2 to 0 we need a template metaprogram
		  if (gt==Dune::cube)
			P1FEOperator_meta<n,n-2>::addrowscube(*it,vertexmapper,allmapper,refelem,A,visited);
		}

	  // now the row sizes have been set
	  A.endrowsizes();

	  // clear the flags for the next round 
	  for (int i=0; i<allmapper.size(); i++) visited[i] = false;

	  // handle each leaf element and insert the nonzeros
	  eendit = is.template end<0,All_Partition>();
	  for (Iterator it = is.template begin<0,All_Partition>(); it!=eendit; ++it)
		{
		  Dune::GeometryType gt = it->geometry().type();
		  const typename Dune::ReferenceElementContainer<DT,n>::value_type&
			refelem = ReferenceElements<DT,n>::general(gt);

		  // vertices, c=n
		  for (int i=0; i<refelem.size(n); i++)
			{
			  int index = allmapper.template submap<n>(*it,i);
			  if (!visited[index]) 
				{
				  int alpha = vertexmapper.template submap<n>(*it,i);
				  A.addindex(alpha,alpha);
				  visited[index] = true;
				}
			}

		  // edges for all element types, c=n-1
		  for (int i=0; i<refelem.size(n-1); i++)
			{
			  int index = allmapper.template submap<n-1>(*it,i);
			  if (!visited[index]) 
				{
				  int alpha = vertexmapper.template submap<n>(*it,refelem.subentity(i,n-1,0,n));
				  int beta = vertexmapper.template submap<n>(*it,refelem.subentity(i,n-1,1,n));
				  A.addindex(alpha,beta);
				  A.addindex(beta,alpha);
				  visited[index] = true;
				}
			}

		  // for codim n-2 to 0 we need a template metaprogram
		  if (gt==Dune::cube)
			P1FEOperator_meta<n,n-2>::addindicescube(*it,vertexmapper,allmapper,refelem,A,visited);
		}

	  // now the matrix is ready for use
	  A.endindices();
	}

	//! return const reference to coefficient vector
	const RepresentationType& operator* () const
	{
	  return A;
	}

	//! return reference to coefficient vector
	RepresentationType& operator* ()
	{
	  return A;
	}

  protected:
	const G& grid;	
	const IS& is;
	RepresentationType A;
	MultipleCodimMultipleGeomTypeMapper<G,IS,P1Layout> vertexmapper;
	MultipleCodimMultipleGeomTypeMapper<G,IS,AllLayout> allmapper;
  };


  template<class G, class RT>
  class LeafAssembledP1FEOperator : public AssembledP1FEOperator<G,RT,typename G::Traits::LeafIndexSet>
  {
  public:
	LeafAssembledP1FEOperator (const G& grid) 
	  : AssembledP1FEOperator<G,RT,typename G::Traits::LeafIndexSet>(grid,grid.leafIndexSet())
	{}
  };


  template<class G, class RT>
  class LevelAssembledP1FEOperator : public AssembledP1FEOperator<G,RT,typename G::Traits::LevelIndexSet>
  {
  public:
	LevelAssembledP1FEOperator (const G& grid, int level) 
	  : AssembledP1FEOperator<G,RT,typename G::Traits::LevelIndexSet>(grid,grid.levelIndexSet(level))
	{}
  };


  /** @} */

}
#endif
