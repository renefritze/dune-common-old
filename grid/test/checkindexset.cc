#ifndef DUNE_CHECKINDEXSET_CC
#define DUNE_CHECKINDEXSET_CC

#include <iostream>
#include <algorithm>

#include <dune/common/fvector.hh>
#include <dune/common/capabilities.hh>
#include <dune/grid/common/referenceelements.hh>


#include <map>
#include <vector>
#include <limits>

namespace Dune {

// compare 2 FieldVectors 
template <typename ctype, int dim> 
bool compareVec(const FieldVector<ctype,dim> & vx1 , const FieldVector<ctype,dim> & vx2 ) 
{
  const ctype eps = 1e2 * std::numeric_limits<ctype>::epsilon();
  bool comp = true;
  for(int i=0;i<dim; i++)
  {
    if(std::abs( vx1[i] - vx2[i] ) > eps ) comp = false;
  }
  return comp; 
}

// check some functionality of grid 
template <int codim, class GridType,
          class IndexSetType, class OutputStreamImp >
void checkIndexSetForCodim ( const GridType &grid , const IndexSetType & lset,
    OutputStreamImp & sout )
{
  enum { dim = GridType :: dimension };
  
  sout <<"\n\nStart consistency check of index set \n\n";
  typedef typename IndexSetType :: template Codim<0>::template Partition<All_Partition> :: Iterator Iterator;
  typedef typename GridType :: ctype coordType;

  typedef std::map < int , std::pair<int,int> > edgemapType;
  std::map < int , std::vector<int> > edges;
  std::map < std::vector<int> , int > vertices;
  
  std::map < int , FieldVector<coordType,dim> > vertexCoordsMap;
  // setup vertex map , store vertex coords for vertex number 
  {
    unsigned int count = 0;
    typedef typename IndexSetType :: template Codim<dim>::template Partition<All_Partition> :: Iterator VxIterator;
    VxIterator end = lset.template end <dim,All_Partition>();
    for(VxIterator it = lset.template begin <dim,All_Partition>(); it != end; ++it )
    {
      count ++ ;
      // get coordinates of vertex 
      FieldVector<coordType,dim> vx ( it->geometry()[0] );    

      // get index of vertex 
      int idx = lset.index( *it );
      
      sout << "Vertex " << idx << " = [" << vx << "]\n";
      
      // if vertex not in map insert it 
      if( vertexCoordsMap.find(idx) == vertexCoordsMap.end())
        vertexCoordsMap[idx] = vx;
    }
    sout << "Found " << vertexCoordsMap.size() << " vertices for that index set!\n\n";

    // check whether size of map equals all found vertices 
    assert( vertexCoordsMap.size() == count );
    
    // check whether size of vertices of set equals all found vertices 
    assert( count == (unsigned int)lset.size(dim,vertex) );
  }

  ReferenceElementWrapper< ReferenceSimplex< coordType, dim > > refSimplex; 
  ReferenceElementWrapper< ReferenceCube   < coordType, dim > > refCube; 

  {
    // choose the right reference element 
    Iterator refend = lset.template end  <0,All_Partition>();
    Iterator refit  = lset.template begin<0,All_Partition>();
    assert( refit != refend );
      
    GeometryType type = refit->geometry().type();    
    
    {
      ReferenceElement< coordType, dim > * refEl = 0;
      if(type == simplex) refEl = &refSimplex;
      if(type == cube)    refEl = &refCube;
      // if this assertion is thrown then geometry type is wrong
      assert( refEl );

      const ReferenceElement< coordType, dim > & refElem = *refEl; 

      // print dune reference element 
      sout << "Dune reference element provides: \n";
      for(int i=0; i<refElem.size(codim); i++)
      {
        sout << i << " subEntity [";
        int s = refElem.size(i,codim,dim);
        for(int j=0; j<s; j++)
        {
          sout << refElem.subEntity(i , codim , j , dim );
          if(j == s-1) sout << "]\n";
          else sout << ",";
        }
      }
    }
  }

  {
    Iterator endit  = lset.template end  <0,All_Partition>();
    for(Iterator it = lset.template begin<0,All_Partition>();
        it != endit; ++it)
    {
      GeometryType type = it->geometry().type();    
      ReferenceElement< coordType, dim > * refEl = 0;
      if(type == simplex) refEl = &refSimplex;
      if(type == cube)    refEl = &refCube;
      // if this assertion is thrown then geometry type is wrong
      assert( refEl );

      const ReferenceElement< coordType, dim > & refElem = *refEl; 
      sout << "****************************************\n";
      sout << "Element = " << lset.index(*it) << " on level " << it->level () << "\n";
      sout << "Vertices      = [";
      int svx = it->template count<dim>();

      // print all vertex numbers 
      for(int i=0; i<svx; i++)
      {
        int idx = lset.template subIndex<dim> (*it,i);
        if(i == svx-1) sout << idx << "]\n"; 
        else sout << idx << ", "; 
      }

      // print all vertex coordinates 
      sout << "Vertex Coords = ["; 
      for(int i=0; i<svx; i++)
      {
        // get entity pointer of sub entity codim=dim (Vertex)
        typedef typename GridType :: template Codim<dim> :: EntityPointer VertexPointerType;
        VertexPointerType vxp = it->template entity<dim> (i);
       
        // get coordinates of entity pointer 
        FieldVector<coordType,dim> vx (vxp->geometry()[0]);

        // output vertex coordinates 
        if(i<svx-1) sout << vx << " , "; 
        else sout << vx << "]\n";
        
        int vxidx = lset.template subIndex<dim> (*it,i); 
        int realidx = lset.index( *vxp ); 
        
        // the subIndex and the index for subEntity must be the same 
        assert( vxidx == realidx );
          
        // check whether the coordinates are the same 
        FieldVector<coordType,dim> vxcheck ( vertexCoordsMap[vxidx] );
        if( ! compareVec( vxcheck, vx ) ) 
        {
          sout << "map global vertex " << vxidx << " vx " << vxcheck << " is not " << vx << "\n";
          assert( compareVec( vxcheck, vx ) );
        }
      }

      // check all subEntities of codimension  codim 
      for(int edge = 0; edge< it->template count<codim>(); edge++)
      {
        {
          int numSubEntities = refElem.size(edge,codim,dim);
          // create vectors of number of vertices on sub entity  
          std::vector<int> local (numSubEntities,-1);
          std::vector<int> global(numSubEntities,-1);

          for(int j=0 ;j<numSubEntities; j++ )
            local[j] = refElem.subEntity(edge , codim , j , dim );

          sout << "check suben [";
          for(int j=0 ;j<numSubEntities-1; j++ )
            sout << local[j] <<  ", "; 
          sout << local[numSubEntities-1] << "]\n";
         
          for(int j=0 ;j<numSubEntities; j++ )
          {
            global[j] = lset.template subIndex<dim> (*it, local[j]);
          }
          
          int globedge = lset.template subIndex<codim>(*it,edge);
          assert( globedge >= 0 );
          sout << "local subentity " << edge << " consider subentity with global index " << globedge << " on en = " << lset.index(*it) << "\n";
          if(numSubEntities > 0)
          {
            sout << "Found global numbers of entity [ "; 
            for(int j=0 ;j<numSubEntities; j++ )
            {
              sout << global[j] << " "; 
            }
            sout << "]\n";
          }

          for(int j=0; j<numSubEntities; j++)
          {
            {
              // get entity pointer of sub entity codim=dim (Vertex)
              typedef typename GridType :: template Codim<dim> :: EntityPointer VertexPointerType;
              VertexPointerType vxp = it->template entity<dim> (local[j]);

              FieldVector<coordType,dim> vx ( vxp->geometry()[0]);
              if(vertexCoordsMap.find(global[j]) != vertexCoordsMap.end())
              {
                FieldVector<coordType,dim> vxcheck ( vertexCoordsMap[global[j]] );
                if( ! compareVec( vxcheck, vx ) ) 
                {
                  sout << "map global vertex [" << global[j] << "] vx " << vxcheck << " is not " << vx << "\n";
                  assert( compareVec( vxcheck, vx ) );
                }
              }
            }
            
            typedef typename GridType :: template Codim<codim> :: EntityPointer SubEnPointerType;
            SubEnPointerType subenp = it->template entity<codim> (edge);
       
            FieldVector<coordType,dim> vx ( subenp->geometry()[j]);
            if(vertexCoordsMap.find(global[j]) != vertexCoordsMap.end())
            {
              FieldVector<coordType,dim> vxcheck ( vertexCoordsMap[global[j]] );
              if( ! compareVec( vxcheck, vx ) ) 
              {
                sout << "map global vertex [" << global[j] << "] vx " << vxcheck << " is not " << vx << "\n";
                assert( compareVec( vxcheck, vx ) );
              }
            }
            sout << "vx[" << global[j] << "] = "  << vx << "\n";
          }
         
          // sort vector of global vertex number for storage in map 
          // the smallest entry is the first entry 
          sort( global.begin(), global.end() );

          if(numSubEntities > 0)
          {
            // check whether vertex key is already stored in map
            if(vertices.find(global) == vertices.end())
            {
              vertices[global] = globedge;
            }
            else 
            {
              int otheredge = vertices[global];
              assert( globedge == otheredge );
            }

            // check whether edge is already stored in map 
            if(edges.find(globedge) == edges.end() )
            {
              edges[globedge] = global;
            }
            else 
            {
              std::vector<int> globalcheck = edges[globedge];
              if(! (global == globalcheck ))
              {
                sout << "For edge " << globedge << "\n";
                sout << "Got ";
                for(int j=0 ;j<numSubEntities; j++ )
                {
                  sout << global[j] << " "; 
                }
                sout << "\n";
                sout << "Found ";
                for(int j=0 ;j<numSubEntities; j++ )
                {
                  sout << globalcheck [j] << " "; 
                }
                sout << "\n";
                assert( global == globalcheck );
              }
            }
          }
        }
      }
    }
  }
}


template <class GridType, class IndexSetType, class OutputStreamImp,
          int codim, bool hasCodim>
struct CheckIndexSet 
{
  static void checkIndexSet( const GridType &grid , 
        const IndexSetType & iset, OutputStreamImp & sout )
  {
    checkIndexSetForCodim<codim> (grid,iset,sout);
    CheckIndexSet<GridType,IndexSetType,OutputStreamImp,
      codim-1, Dune::Capabilities::hasEntity<GridType, codim-1>::v > :: 
      checkIndexSet( grid, iset, sout );
  }
};

template <class GridType, class IndexSetType, class OutputStreamImp,
          int codim>
struct CheckIndexSet<GridType,IndexSetType,OutputStreamImp,codim,false>
{
  static void checkIndexSet( const GridType &grid , 
        const IndexSetType & iset, OutputStreamImp & sout )
  {
    CheckIndexSet<GridType,IndexSetType,OutputStreamImp,
      codim-1, Dune::Capabilities::hasEntity<GridType, codim-1>::v > :: 
      checkIndexSet( grid, iset, sout );
  }
};

// end loop over codim by specialisation 
template <class GridType, class IndexSetType, class OutputStreamImp>
struct CheckIndexSet<GridType,IndexSetType,OutputStreamImp,0,true>
{
  static void checkIndexSet( const GridType &grid , 
        const IndexSetType & iset, OutputStreamImp & sout )
  {
    checkIndexSetForCodim<0> (grid,iset,sout);
  }
};

template <class GridType, class IndexSetType, class OutputStreamImp>
void checkIndexSet( const GridType &grid , const IndexSetType & iset,
    OutputStreamImp & sout )
{
  CheckIndexSet<GridType,IndexSetType,OutputStreamImp,
    GridType::dimension, true> :: 
    checkIndexSet (grid,iset,sout);
}

} // end namespace Dune 
#endif
