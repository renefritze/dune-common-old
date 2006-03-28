// $Id$

#ifndef DUNE_VTKWRITER_HH
#define DUNE_VTKWRITER_HH

#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <string.h>
#include <dune/common/exceptions.hh>
#include "dune/grid/common/mcmgmapper.hh"
#include "dune/grid/common/referenceelements.hh"


// namespace base64
// {
// #include"cencode.c"
// }

/** @file
	@author Peter Bastian
	@brief Provides file i/o for the visualization toolkit
*/

/** 
	put vtk io intro here ...
*/


namespace Dune
{
  //! options for VTK output
  struct VTKOptions
  {
	enum Type { 
	  ascii, binary, binaryappended, conforming, nonconforming };
  };


  // map type to name in data array
  template<class T>
  struct VTKTypeNameTraits {
	std::string operator () (){
	  return "";
	}
  };

  template<>
  struct VTKTypeNameTraits<char> {
	std::string operator () () {
	  return "Int8";
	}
	typedef int PrintType;
  };

  template<>
  struct VTKTypeNameTraits<unsigned char> {
	std::string operator () () {
	  return "UInt8";
	}	
	typedef int PrintType;
  };

  template<>
  struct VTKTypeNameTraits<short> {
	std::string operator () () {
	  return "Int16";
	}	
	typedef short PrintType;
  };

  template<>
  struct VTKTypeNameTraits<unsigned short> {
	std::string operator () () {
	  return "UInt16";
	}	
	typedef unsigned short PrintType;
  };

  template<>
  struct VTKTypeNameTraits<int> {
	std::string operator () () {
	  return "Int32";
	}	
	typedef int PrintType;
  };

  template<>
  struct VTKTypeNameTraits<unsigned int> {
	std::string operator () () {
	  return "UInt32";
	}	
	typedef unsigned int PrintType;
  };

  template<>
  struct VTKTypeNameTraits<float> {
	std::string operator () () {
	  return "Float32";
	}	
 	typedef float PrintType;
 };

  template<>
  struct VTKTypeNameTraits<double> {
	std::string operator () () {
	  return "Float64";
	}	
	typedef double PrintType;
  };


  /** @brief A class providing vtk file i/o
  */
  template<class GridImp, class IS=typename GridImp::template Codim<0>::LeafIndexSet>
  class VTKWriter {

	// extract types
	enum {n=GridImp::dimension};
	enum {w=GridImp::dimensionworld};
	typedef typename GridImp::ctype DT;
	typedef typename GridImp::Traits::template Codim<0>::Entity Entity;
	typedef typename GridImp::Traits::template Codim<0>::Entity Cell;
	typedef typename GridImp::Traits::template Codim<n>::Entity Vertex;
    
  public:
	// A base class for grid functions with any return type and dimension
	// Trick : use double as return type
	class VTKFunction
	{
	public:
	  //! return number of components
	  virtual int ncomps () const = 0;

	  //! evaluate single component comp in the entity e at local coordinates xi
	  /*! Evaluate the function in an entity at local coordinates.
		@param[in]  comp   number of component to be evaluated
		@param[in]  e      reference to grid entity of codimension 0
		@param[in]  xi     point in local coordinates of the reference element of e
		\return            value of the component
	  */
	  virtual double evaluate (int comp, const Entity& e, const Dune::FieldVector<DT,n>& xi) const = 0;

	  // get name
	  virtual std::string name () const = 0;

	  // virtual destructor
	  virtual ~VTKFunction () {}
	};

  private:
	typedef typename std::list<VTKFunction*>::iterator functioniterator;

	template<int dim>
	struct P0Layout
	{
	  bool contains (int codim, Dune::GeometryType gt)
	  {
		if (codim==0) return true;
		return false;
	  }
	};
	template<int dim>
	struct P1Layout
	{
	  bool contains (int codim, Dune::GeometryType gt)
	  {
		if (codim==dim) return true;
		return false;
	  }
	};
	typedef IS IndexSet;
	static const PartitionIteratorType vtkPartition = InteriorBorder_Partition;
    typedef typename IS::template Codim<0>::template Partition<vtkPartition>::Iterator CellIterator;
    typedef typename IS::template Codim<n>::template Partition<vtkPartition>::Iterator VertexIterator;
	typedef MultipleCodimMultipleGeomTypeMapper<GridImp,IS,P0Layout> VM0;
	typedef MultipleCodimMultipleGeomTypeMapper<GridImp,IS,P1Layout> VM1;
	

	// take a vector and interpret it as cell data
	template<class V>
	class P0VectorWrapper : public VTKFunction  
	{
	public:
	  //! return number of components
	  virtual int ncomps () const
	  {
		return 1;
	  }

	  //! evaluate
	  virtual double evaluate (int comp, const Entity& e, const Dune::FieldVector<DT,n>& xi) const
	  {
		return v[mapper.map(e)];
	  }
	  
	  //! get name
	  virtual std::string name () const
	  {
		return s;
	  }

	  // construct from a vector and a name
	  P0VectorWrapper (const GridImp& g_, const IS& is_, const V& v_, std::string s_) 
		: g(g_), is(is_), v(v_), s(s_), mapper(g_,is_)
	  {
		if (v.size()!=(unsigned int)mapper.size())
		  DUNE_THROW(IOError,"VTKWriter::P0VectorWrapper: size mismatch");
	  }

      virtual ~P0VectorWrapper() {}
      
	private:
	  const GridImp& g;
	  const IS& is;
	  const V& v;
	  std::string s;
	  VM0 mapper;
	};

	// take a vector and interpret it as vertex data
	template<class V>
	class P1VectorWrapper : public VTKFunction  
	{
	public:
	  //! return number of components
	  virtual int ncomps () const
	  {
		return 1;
	  }

	  //! evaluate
	  virtual double evaluate (int comp, const Entity& e, const Dune::FieldVector<DT,n>& xi) const
	  {
		double min=1E100;
		int imin=-1;
		Dune::GeometryType gt = e.geometry().type();
		for (int i=0; i<e.template count<n>(); ++i)
		  {
			Dune::FieldVector<DT,n> 
			  local = Dune::ReferenceElements<DT,n>::general(gt).position(i,n);
			local -= xi;
			if (local.infinity_norm()<min)
			  {
				min = local.infinity_norm();
				imin = i;
			  }
		  }
		return v[mapper.template map<n>(e,imin)];
	  }
	  
	  //! get name
	  virtual std::string name () const
	  {
		return s;
	  }

	  // construct from a vector and a name
	  P1VectorWrapper (const GridImp& g_, const IS& is_, const V& v_, std::string s_) 
		: g(g_), is(is_), v(v_), s(s_), mapper(g_,is_)
	  {
		if (v.size()!=(unsigned int)mapper.size())
		  DUNE_THROW(IOError,"VTKWriter::P1VectorWrapper: size mismatch");
	  }

      virtual ~P1VectorWrapper() {}
      
	private:
	  const GridImp& g;
	  const IS& is;
	  const V& v;
	  std::string s;
	  VM1 mapper;
	};

  public:
	//! constructor from a grid (uses a leaf indexset)
	VTKWriter (const GridImp& g) : grid(g), is(grid.leafIndexSet())
	{
	  indentCount = 0;
	  numPerLine = 4*3; //should be a multiple of 3 !
	}

	//! constructor from a grid and an indexset
	VTKWriter (const GridImp& g, const IndexSet& i) : grid(g), is(i)
	{
	  indentCount = 0;
	  numPerLine = 4*3; //should be a multiple of 3 !
	}

	//! add a grid function for output
	void addCellData (VTKFunction* p)
	{
	  celldata.push_back(p);
	}

	//! add a grid function for output
	template<class V>
	void addCellData (const V& v, std::string name)
	{
	  VTKFunction* p = new P0VectorWrapper<V>(grid,is,v,name);
	  celldata.push_back(p);
	}

	//! add a grid function for output
	void addVertexData (VTKFunction* p)
	{
	  vertexdata.push_back(p);
	}

	//! add a grid function for output
	template<class V>
	void addVertexData (const V& v, std::string name)
	{
	  VTKFunction* p = new P1VectorWrapper<V>(grid,is,v,name);
	  vertexdata.push_back(p);
	}

	//! clear list of registered functions
	void clear ()
	{
	  typedef typename std::list<VTKFunction*>::iterator iterator;
	  for (iterator it=celldata.begin(); it!=celldata.end(); ++it)
		delete *it;
	  celldata.clear();
	  for (iterator it=vertexdata.begin(); it!=vertexdata.end(); ++it)
		delete *it;
	  vertexdata.clear();
	}

	//! destructor
	~VTKWriter ()
	{
	  this->clear();
	}

	//! write output; interface might change later
	void write (const char* name, VTKOptions::Type dm = VTKOptions::ascii)
	{
	  // make data mode visible to private functions
	  datamode=dm;

	  // reset byte counter for binary appended output
	  bytecount = 0;

	  if (grid.comm().size()==1)
		{
		  std::ofstream file;
		  char fullname[128];
		  sprintf(fullname,"%s.vtu",name);
		  if (datamode==VTKOptions::binaryappended)
			file.open(fullname,std::ios::binary);
		  else
			file.open(fullname);
		  writeDataFile(file);
		  file.close();
		}
	  else
		{
		  std::ofstream file;
		  char fullname[128];
		  sprintf(fullname,"%s-%04d-%04d.vtu",name,grid.comm().size(),grid.comm().rank());
		  if (datamode==VTKOptions::binaryappended)
			file.open(fullname,std::ios::binary);
		  else
			file.open(fullname);
		  writeDataFile(file);
		  file.close();
		  grid.comm().barrier();
		  if (grid.comm().rank()==0)
			{
			  sprintf(fullname,"%s-%04d.pvtu",name,grid.comm().size());
			  file.open(fullname);
			  writeParallelHeader(file,name,"");
			  file.close();
			}
		  grid.comm().barrier();
		}
	}

	//! write output; interface might change later
	void pwrite (const char* name,  const char* path, const char* extendpath, 
				 VTKOptions::Type dm = VTKOptions::ascii)
	{
	  // make data mode visible to private functions
	  datamode=dm;

	  // reset byte counter for binary appended output
	  bytecount = 0;

	  // do some magic because paraview can only cope with relative pathes to piece files
	  std::ofstream file;
	  char piecepath[256];
	  char relpiecepath[256];
	  int n=strlen(path);
	  int m=strlen(extendpath);
	  if (n>0 && path[0]=='/' && path[n-1]=='/')
		{
		  // 1) path is an absolute path to the directory where the pvtu file will be placed
		  // 2) extendpath is an absolute path from "/" where the pieces are placed
		  // 3) pieces are addressed relative in the pvtu files
		  if (m==0)
			{
			  // write pieces to root :-)
			  piecepath[0] = '/';
			  piecepath[1] = '\0';
			}
		  else
			{
			  // make piecepath absolute with trailing "/"
			  char *p=piecepath;
			  if (extendpath[0]!='/')
				{
				  *p = '/';
				  p++;
				}
			  for (int i=0; i<m; i++)
				{
				  *p = extendpath[i];
				  p++;
				}
			  if (*(p-1)!='/')
				{
				  *p = '/';
				  p++;
				}
			  *p = '\0';
			}
		  // path and piecepath are either "/" or have leading and trailing /
		  // count slashes in path 
		  int k=0;
		  const char *p=path;
		  while (*p!='\0')
			{
			  if (*p=='/') k++;
			  p++;
			}
		  char *pp = relpiecepath;
		  if (k>1)
			{
			  for (int i=0; i<k; i++)
				{
				  *pp='.'; pp++; *pp='.'; pp++; *pp='/'; pp++; 
				}
			}
		  // now copy the extendpath
		  for (int i=0; i<m; i++)
			{
			  if (i==0 && extendpath[i]=='/') continue;
			  *pp = extendpath[i];
			  pp++;
			}
		  if ( pp!=relpiecepath && (*(pp-1)!='/') )
			{
			  *pp = '/';
			  pp++;
			}
		  *pp = '\0';
		}
	  else
		{
		  // 1) path is a relative path to the directory where pvtu files are placed
		  // 2) extendpath is relative to where the pvtu files are and there the pieces are placed
		  if (n==0 || m==0)
			sprintf(piecepath,"%s%s",path,extendpath);
		  else
			{
			  // both are non-zero
			  if (path[n-1]!='/' && extendpath[0]!='/')
				sprintf(piecepath,"%s/%s",path,extendpath);
			  else
				sprintf(piecepath,"%s%s",path,extendpath);
			}
		  // the pieces are relative to the pvtu files
		  sprintf(relpiecepath,"%s",extendpath);
		}
	  char fullname[256];
	  sprintf(fullname,"%s%s-%04d-%04d.vtu",piecepath,name,grid.comm().size(),grid.comm().rank());
	  if (datamode==VTKOptions::binaryappended)
		file.open(fullname,std::ios::binary);
	  else
		file.open(fullname);
	  writeDataFile(file);
	  file.close();
	  grid.comm().barrier();
	  if (grid.comm().rank()==0)
		{
		  sprintf(fullname,"%s%s-%04d.pvtu",path,name,grid.comm().size());
		  file.open(fullname);
		  writeParallelHeader(file,name,relpiecepath);
		  file.close();
		}
	  grid.comm().barrier();
	}

  private:

    enum VTKGeometryType
    {
      vtkLine = 3,
      vtkTriangle = 5,
      vtkQuadrilateral = 9,
      vtkTetrahedron = 10,
      vtkHexahedron = 12,
      vtkPrism = 13,
      vtkPyramid = 14
    };
    
    //! mapping from GeometryType to VTKGeometryType
    VTKGeometryType vtkType(const Dune::GeometryType & t) const
      {
        if (t.isLine())
          return vtkLine;
        if (t.isTriangle())
          return vtkTriangle;
        if (t.isQuadrilateral())
          return vtkQuadrilateral;
        if (t.isTetrahedron())
          return vtkTetrahedron;
        if (t.isPyramid())
          return vtkPyramid;
        if (t.isPrism())
          return vtkPrism;
        if (t.isHexahedron())
          return vtkHexahedron;
        DUNE_THROW(IOError,"VTKWriter: unsupported GeometryType "
                   << t <<std::endl);
      }

	//! write header file in parallel case to stream
	void writeParallelHeader (std::ostream& s, const char* piecename, const char* piecepath)
	{
	  // xml header
	  s << "<?xml version=\"1.0\"?>" << std::endl;

	  // VTKFile
	  s << "<VTKFile type=\"PUnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">" << std::endl;
	  indentUp();

	  // PUnstructuredGrid
	  indent(s); s << "<PUnstructuredGrid GhostLevel=\"0\">" << std::endl;
	  indentUp();

	  // PPointData
	  indent(s); s << "<PPointData ";
	  for (functioniterator it=vertexdata.begin(); it!=vertexdata.end(); ++it)
		if ((*it)->ncomps()==1)
		  {
			s << "Scalars=\"" << (*it)->name() << "\"" ;
			break;
		  }
	  for (functioniterator it=vertexdata.begin(); it!=vertexdata.end(); ++it)
		if ((*it)->ncomps()>1)
		  {
			s << "Vectors=\"" << (*it)->name() << "\"" ;
			break;
		  }
	  s << ">" << std::endl;
	  indentUp();
	  for (functioniterator it=vertexdata.begin(); it!=vertexdata.end(); ++it)
		{
		  indent(s); s << "<PDataArray type=\"Float32\" Name=\"" << (*it)->name() << "\" ";
		  if ((*it)->ncomps()>1)
			s << "NumberOfComponents=\"" << (*it)->ncomps() << "\" ";
		  if (datamode==VTKOptions::ascii)
			s << "format=\"ascii\"/>" << std::endl;
		  if (datamode==VTKOptions::binary)
			s << "format=\"binary\"/>" << std::endl;
		  if (datamode==VTKOptions::binaryappended)
			s << "format=\"appended\"/>" << std::endl;
		}
	  indentDown();
	  indent(s); s << "</PPointData>" << std::endl;

	  // PCellData
	  indent(s); s << "<PCellData ";
	  for (functioniterator it=celldata.begin(); it!=celldata.end(); ++it)
		if ((*it)->ncomps()==1)
		  {
			s << "Scalars=\"" << (*it)->name() << "\"" ;
			break;
		  }
	  for (functioniterator it=celldata.begin(); it!=celldata.end(); ++it)
		if ((*it)->ncomps()>1)
		  {
			s << "Vectors=\"" << (*it)->name() << "\"" ;
			break;
		  }
	  s << ">" << std::endl;
	  indentUp();
	  for (functioniterator it=celldata.begin(); it!=celldata.end(); ++it)
		{
		  indent(s); s << "<PDataArray type=\"Float32\" Name=\"" << (*it)->name() << "\" ";
		  if ((*it)->ncomps()>1)
			s << "NumberOfComponents=\"" << (*it)->ncomps() << "\" ";
		  if (datamode==VTKOptions::ascii)
			s << "format=\"ascii\"/>" << std::endl;
		  if (datamode==VTKOptions::binary)
			s << "format=\"binary\"/>" << std::endl;
		  if (datamode==VTKOptions::binaryappended)
			s << "format=\"appended\"/>" << std::endl;
		}
	  indentDown();
	  indent(s); s << "</PCellData>" << std::endl;

	  // PPoints
	  indent(s); s << "<PPoints>" << std::endl;
	  indentUp();
	  indent(s); s << "<PDataArray type=\"Float32\" Name=\"Coordinates\" NumberOfComponents=\"" << "3" << "\" ";
	  if (datamode==VTKOptions::ascii)
		s << "format=\"ascii\"/>" << std::endl;
	  if (datamode==VTKOptions::binary)
		s << "format=\"binary\"/>" << std::endl;
	  if (datamode==VTKOptions::binaryappended)
		s << "format=\"appended\"/>" << std::endl;
	  indentDown();
	  indent(s); s << "</PPoints>" << std::endl;

	  // Pieces
	  for (int i=0; i<grid.comm().size(); i++)
		{
		  char fullname[128];
		  sprintf(fullname,"%s%s-%04d-%04d.vtu",piecepath,piecename,grid.comm().size(),i);
		  indent(s); s << "<Piece Source=\"" << fullname << "\"/>" << std::endl;
		}

	  // /PUnstructuredGrid
	  indentDown();
	  indent(s); s << "</PUnstructuredGrid>" << std::endl;

	  // /VTKFile
	  indentDown();
	  s << "</VTKFile>" << std::endl;	  
	}

	//! write data file to stream
	void writeDataFile (std::ostream& s)
	{
	  // xml header
	  s << "<?xml version=\"1.0\"?>" << std::endl;

	  // VTKFile
	  s << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">" << std::endl;
	  indentUp();

	  // UnstructuredGrid
	  indent(s); s << "<UnstructuredGrid>" << std::endl;
	  indentUp();

	  // Piece
	  vertexmapper = new VM1(grid,is);
	  number.resize(vertexmapper->size());
	  for (std::vector<int>::size_type i=0; i<number.size(); i++) number[i] = -1;
	  nvertices = 0;
	  ncells = 0;
	  ncorners = 0;
	  for (CellIterator it=is.template begin<0,vtkPartition>(); it!=is.template end<0,vtkPartition>(); ++it)
		if (it->partitionType()==InteriorEntity)
		  {
			ncells++;
			for (int i=0; i<it->template count<n>(); ++i)
			  {
				ncorners++;
				int alpha = vertexmapper->template map<n>(*it,i);
				if (number[alpha]<0)
				  number[alpha] = nvertices++;
			  }
		  }
	  indent(s); s << "<Piece NumberOfPoints=\"" << nvertices << "\" NumberOfCells=\"" << ncells << "\">" << std::endl;
	  indentUp();

	  // PointData
	  writeVertexDataConforming(s);

	  // CellData
	  writeCellData(s);

	  // Points
	  writePointsConforming(s);

	  // Cells
	  writeCellsConforming(s);

	  // /Piece
	  indentDown();
	  indent(s); s << "</Piece>" << std::endl;

	  // /UnstructuredGrid
	  indentDown();
	  indent(s); s << "</UnstructuredGrid>" << std::endl;

	  // write appended binary dat section
	  if (datamode==VTKOptions::binaryappended)
		writeAppendedData(s);

	  // /VTKFile
	  indentDown();
	  s << "</VTKFile>" << std::endl;

	  delete vertexmapper; number.clear();
	}

	void writeCellData (std::ostream& s)
	{
	  indent(s); s << "<CellData ";
	  for (functioniterator it=celldata.begin(); it!=celldata.end(); ++it)
		if ((*it)->ncomps()==1)
		  {
			s << "Scalars=\"" << (*it)->name() << "\"" ;
			break;
		  }
	  for (functioniterator it=celldata.begin(); it!=celldata.end(); ++it)
		if ((*it)->ncomps()>1)
		  {
			s << "Vectors=\"" << (*it)->name() << "\"" ;
			break;
		  }
	  s << ">" << std::endl;
	  indentUp();
	  for (functioniterator it=celldata.begin(); it!=celldata.end(); ++it)
		{
		  VTKDataArrayWriter<float> *p=0;
		  if (datamode==VTKOptions::ascii)
			p = new VTKAsciiDataArrayWriter<float>(s,(*it)->name(),(*it)->ncomps());
		  if (datamode==VTKOptions::binary)		  
			p = new VTKBinaryDataArrayWriter<float>(s,(*it)->name(),(*it)->ncomps(),(*it)->ncomps()*ncells); 
		  if (datamode==VTKOptions::binaryappended)		  
			p = new VTKBinaryAppendedDataArrayWriter<float>(s,(*it)->name(),(*it)->ncomps(),bytecount); 
		  for (CellIterator i=is.template begin<0,vtkPartition>(); i!=is.template end<0,vtkPartition>(); ++i)
			if (i->partitionType()==InteriorEntity)
			  for (int j=0; j<(*it)->ncomps(); j++)
				p->write((*it)->evaluate(j,*i,ReferenceElements<DT,n>::general(i->geometry().type()).position(0,0)));
		  delete p;
		}
	  indentDown();
	  indent(s); s << "</CellData>" << std::endl;
	}

	void writeVertexDataConforming (std::ostream& s)
	{
	  indent(s); s << "<PointData ";
	  for (functioniterator it=vertexdata.begin(); it!=vertexdata.end(); ++it)
		if ((*it)->ncomps()==1)
		  {
			s << "Scalars=\"" << (*it)->name() << "\"" ;
			break;
		  }
	  for (functioniterator it=vertexdata.begin(); it!=vertexdata.end(); ++it)
		if ((*it)->ncomps()>1)
		  {
			s << "Vectors=\"" << (*it)->name() << "\"" ;
			break;
		  }
	  s << ">" << std::endl;
	  indentUp();
	  for (functioniterator it=vertexdata.begin(); it!=vertexdata.end(); ++it)
		{
		  VTKDataArrayWriter<float> *p=0;
		  if (datamode==VTKOptions::ascii)
			p = new VTKAsciiDataArrayWriter<float>(s,(*it)->name(),(*it)->ncomps()); 
		  if (datamode==VTKOptions::binary)
			p = new VTKBinaryDataArrayWriter<float>(s,(*it)->name(),(*it)->ncomps(),(*it)->ncomps()*nvertices); 
		  if (datamode==VTKOptions::binaryappended)
			p = new VTKBinaryAppendedDataArrayWriter<float>(s,(*it)->name(),(*it)->ncomps(),bytecount); 
		  std::vector<bool> visited(vertexmapper->size(), false);
		  for (CellIterator eit=is.template begin<0,vtkPartition>(); eit!=is.template end<0,vtkPartition>(); ++eit)
			if (eit->partitionType()==InteriorEntity)
			  for (int i=0; i<eit->template count<n>(); ++i)
				{
				  int alpha = vertexmapper->template map<n>(*eit,i);
				  if (!visited[alpha])
					{
					  for (int j=0; j<(*it)->ncomps(); j++)
						p->write((*it)->evaluate(j,*eit,ReferenceElements<DT,n>::general(eit->geometry().type()).position(i,n)));
					  visited[alpha] = true;
					}
				}
		  delete p;
		}
	  indentDown();
	  indent(s); s << "</PointData>" << std::endl;
	}

	void writePointsConforming (std::ostream& s)
	{
	  indent(s); s << "<Points>" << std::endl;
	  indentUp();

	  VTKDataArrayWriter<float> *p=0;
	  if (datamode==VTKOptions::ascii)
		p = new VTKAsciiDataArrayWriter<float>(s,"Coordinates",3);
	  if (datamode==VTKOptions::binary)
		p = new VTKBinaryDataArrayWriter<float>(s,"Coordinates",3,3*nvertices);
	  if (datamode==VTKOptions::binaryappended)
		p = new VTKBinaryAppendedDataArrayWriter<float>(s,"Coordinates",3,bytecount);
	  std::vector<bool> visited(vertexmapper->size(), false);
	  for (CellIterator it=is.template begin<0,vtkPartition>(); it!=is.template end<0,vtkPartition>(); ++it)
		if (it->partitionType()==InteriorEntity)
		  for (int i=0; i<it->template count<n>(); ++i)
			{
			  int alpha = vertexmapper->template map<n>(*it,i);
			  if (!visited[alpha])
				{
				  int dimw=w;
				  for (int j=0; j<std::min(dimw,3); j++)
					p->write(it->geometry()[i][j]);
				  for (int j=std::min(dimw,3); j<3; j++)
					p->write(0.0);
				  visited[alpha] = true;
				}
			}
	  delete p;
	  
	  indentDown();
	  indent(s); s << "</Points>" << std::endl;
	}

	void writeCellsConforming (std::ostream& s)
	{
	  indent(s); s << "<Cells>" << std::endl;
	  indentUp();

	  // connectivity
	  VTKDataArrayWriter<int> *p1=0;
	  if (datamode==VTKOptions::ascii)
		p1 = new VTKAsciiDataArrayWriter<int>(s,"connectivity",1); 
	  if (datamode==VTKOptions::binary)
		p1 = new VTKBinaryDataArrayWriter<int>(s,"connectivity",1,ncorners); 
	  if (datamode==VTKOptions::binaryappended)
		p1 = new VTKBinaryAppendedDataArrayWriter<int>(s,"connectivity",1,bytecount); 
	  for (CellIterator it=is.template begin<0,vtkPartition>(); it!=is.template end<0,vtkPartition>(); ++it)
		if (it->partitionType()==InteriorEntity)
		  for (int i=0; i<it->template count<n>(); ++i)
			p1->write(number[vertexmapper->template map<n>(*it,renumber(*it,i))]);
	  delete p1;

	  // offsets
	  VTKDataArrayWriter<int> *p2=0;
	  if (datamode==VTKOptions::ascii)
		p2 = new VTKAsciiDataArrayWriter<int>(s,"offsets",1);
	  if (datamode==VTKOptions::binary)
		p2 = new VTKBinaryDataArrayWriter<int>(s,"offsets",1,ncells); 
	  if (datamode==VTKOptions::binaryappended)
		p2 = new VTKBinaryAppendedDataArrayWriter<int>(s,"offsets",1,bytecount); 
	  int offset = 0;
	  for (CellIterator it=is.template begin<0,vtkPartition>(); it!=is.template end<0,vtkPartition>(); ++it)
		if (it->partitionType()==InteriorEntity)
		  {
			offset += it->template count<n>();
			p2->write(offset);
		  }
	  delete p2;

	  // types
	  VTKDataArrayWriter<unsigned char> *p3=0; 
	  if (datamode==VTKOptions::ascii)
		p3 = new VTKAsciiDataArrayWriter<unsigned char>(s,"types",1);
	  if (datamode==VTKOptions::binary)
		p3 = new VTKBinaryDataArrayWriter<unsigned char>(s,"types",1,ncells); 
	  if (datamode==VTKOptions::binaryappended)
		p3 = new VTKBinaryAppendedDataArrayWriter<unsigned char>(s,"types",1,bytecount); 
	  for (CellIterator it=is.template begin<0,vtkPartition>(); it!=is.template end<0,vtkPartition>(); ++it)
		if (it->partitionType()==InteriorEntity)
		  {
            int vtktype = vtkType(it->geometry().type());
			p3->write(vtktype);
		  }
	  delete p3;

	  indentDown();
	  indent(s); s << "</Cells>" << std::endl;
	}


	void writeAppendedData (std::ostream& s)
	{
	  indent(s); s << "<AppendedData encoding=\"raw\">" << std::endl;
	  indentUp();
	  indent(s); s << "_"; // indicates start of binary data

	  SimpleStream stream(s);

	  // write length before each data block
	  unsigned long blocklength;

	  // point data	  
	  for (functioniterator it=vertexdata.begin(); it!=vertexdata.end(); ++it)
		{
		  blocklength = nvertices * (*it)->ncomps() * sizeof(float);
		  stream.write(blocklength);
		  std::vector<bool> visited(vertexmapper->size(), false);
		  for (CellIterator eit=is.template begin<0,vtkPartition>(); eit!=is.template end<0,vtkPartition>(); ++eit)
			if (eit->partitionType()==InteriorEntity)
			  for (int i=0; i<eit->template count<n>(); ++i)
				{
				  int alpha = vertexmapper->template map<n>(*eit,i);
				  if (!visited[alpha])
					{
					  for (int j=0; j<(*it)->ncomps(); j++)
						{
						  float data = (*it)->evaluate(j,*eit,ReferenceElements<DT,n>::general(eit->geometry().type()).position(i,n));
						  stream.write(data);
						}
					  visited[alpha] = true;
					}
				}
		}

	  // cell data
	  for (functioniterator it=celldata.begin(); it!=celldata.end(); ++it)
		{
		  blocklength = ncells * (*it)->ncomps() * sizeof(float);
		  stream.write(blocklength);
		  for (CellIterator i=is.template begin<0,vtkPartition>(); i!=is.template end<0,vtkPartition>(); ++i)
			if (i->partitionType()==InteriorEntity)
			  for (int j=0; j<(*it)->ncomps(); j++)
				{
				  float data = (*it)->evaluate(j,*i,ReferenceElements<DT,n>::general(i->geometry().type()).position(0,0));
				  stream.write(data);
				}
		}
	  
	  // point coordinates
	  blocklength = nvertices * 3 * sizeof(float);
	  stream.write(blocklength);
	  std::vector<bool> visited(vertexmapper->size(), false);
	  for (CellIterator it=is.template begin<0,vtkPartition>(); it!=is.template end<0,vtkPartition>(); ++it)
		if (it->partitionType()==InteriorEntity)
		  for (int i=0; i<it->template count<n>(); ++i)
			{
			  int alpha = vertexmapper->template map<n>(*it,i);
			  if (!visited[alpha])
				{
				  int dimw=w;
				  float data;
				  for (int j=0; j<std::min(dimw,3); j++)
					{
						data = it->geometry()[i][j];
						stream.write(data);
					}
				  data = 0;
				  for (int j=std::min(dimw,3); j<3; j++)
					stream.write(data);
				  visited[alpha] = true;
				}
			}
	  
	  // connectivity
	  blocklength = ncorners * sizeof(unsigned int);
	  stream.write(blocklength);
	  for (CellIterator it=is.template begin<0,vtkPartition>(); it!=is.template end<0,vtkPartition>(); ++it)
		if (it->partitionType()==InteriorEntity)
		  for (int i=0; i<it->template count<n>(); ++i)
			{
			  int data = number[vertexmapper->template map<n>(*it,renumber(*it,i))];
			  stream.write(data);
			}

	  // offsets
	  blocklength = ncells * sizeof(unsigned int);
	  stream.write(blocklength);
	  int offset = 0;
	  for (CellIterator it=is.template begin<0,vtkPartition>(); it!=is.template end<0,vtkPartition>(); ++it)
		if (it->partitionType()==InteriorEntity)
		  {
			offset += it->template count<n>();
			stream.write(offset);
		  }

	  // cell types
	  blocklength = ncells * sizeof(unsigned char);
	  stream.write(blocklength);
	  for (CellIterator it=is.template begin<0,vtkPartition>(); it!=is.template end<0,vtkPartition>(); ++it)
		if (it->partitionType()==InteriorEntity)
		  {
            unsigned char vtktype = vtkType(it->geometry().type());
			stream.write(vtktype);
		  }

	  s << std::endl;
	  indentDown();
	  indent(s); s << "</AppendedData>" << std::endl;
	}
	
	// base class for data array writers
	template<class T>
	class VTKDataArrayWriter
	{
	public:
	  virtual void write (T data) = 0;
	  virtual ~VTKDataArrayWriter () {}
	};

	// a streaming writer for data array tags
	template<class T>
	class VTKAsciiDataArrayWriter : public VTKDataArrayWriter<T>
	{
	public:
	  //! make a new data array writer
	  VTKAsciiDataArrayWriter (std::ostream& theStream, std::string name, int ncomps) 
		: s(theStream), counter(0), numPerLine(12)
	  {
		VTKTypeNameTraits<T> tn;
		s << "<DataArray type=\"" << tn() << "\" Name=\"" << name << "\" ";
		if (ncomps>1)
		  s << "NumberOfComponents=\"" << ncomps << "\" ";
		s << "format=\"ascii\">" << std::endl;
	  }

	  //! write one data element to output stream
	  void write (T data)
	  {
		typedef typename VTKTypeNameTraits<T>::PrintType PT;
		s << (PT) data << " ";
		counter++;
		if (counter%numPerLine==0) s << std::endl;
	  }

	  //! finish output; writes end tag
	  ~VTKAsciiDataArrayWriter ()
	  {
		if (counter%numPerLine!=0) s << std::endl;
		s << "</DataArray>" << std::endl;	  
	  }

	private:
	  std::ostream& s;
	  int counter;
	  int numPerLine;
	};

	// a streaming writer for data array tags
	template<class T>
	class VTKBinaryDataArrayWriter : public VTKDataArrayWriter<T>
	{
	public:
	  //! make a new data array writer
	  VTKBinaryDataArrayWriter (std::ostream& theStream, std::string name, int ncomps, int nitems) 
		: s(theStream),bufsize(4096),n(0)
	  {
		DUNE_THROW(IOError, "binary does not work yet, use binaryappended!");
		VTKTypeNameTraits<T> tn;
		s << "<DataArray type=\"" << tn() << "\" Name=\"" << name << "\" ";
		if (ncomps>1)
		  s << "NumberOfComponents=\"" << ncomps << "\" ";
		s << "format=\"binary\">" << std::endl;
		buffer = new char[bufsize*sizeof(T)];
		code = new char[2*bufsize*sizeof(T)];
		unsigned int size = nitems*sizeof(T);
		char* p = reinterpret_cast<char*>(&size);
		memcpy(buffer+n,p,sizeof(int));
		n += sizeof(int);
		//		base64::base64_init_encodestate(&_state);
	  }

	  //! write one data element to output stream
	  void write (T data)
	  {
		if (n+sizeof(T)>bufsize)
		  {
			// flush buffer
			//			int codelength = base64::base64_encode_block(buffer,n,code,&_state);
			//			s.write(code,codelength);
  			n=0;
		  }
		char* p = reinterpret_cast<char*>(&data);
		memcpy(buffer+n,p,sizeof(T));
		n += sizeof(T);
	  }

	  //! finish output; writes end tag
	  ~VTKBinaryDataArrayWriter ()
	  {
// 		int codelength;
		if (n>0)
		  {
			//			codelength = base64::base64_encode_block(buffer,n,code,&_state);
			//			s.write(code,codelength);
		  }
		//		codelength = base64::base64_encode_blockend(code,&_state);
// 		s.write(code,codelength);
		//		base64::base64_init_encodestate(&_state);
		s << std::endl;
		s << "</DataArray>" << std::endl;
		delete [] code;
		delete [] buffer;
	  }

	private:
	  std::ostream& s;
	  //	  base64::base64_encodestate _state;
	  size_t bufsize;
	  char* buffer;
	  char* code;
	  int n;
	};

	// a streaming writer for data array tags
	template<class T>
	class VTKBinaryAppendedDataArrayWriter : public VTKDataArrayWriter<T>
	{
	public:
	  //! make a new data array writer
	  VTKBinaryAppendedDataArrayWriter (std::ostream& theStream, std::string name, int ncomps, unsigned int& bc) 
		: s(theStream),bytecount(bc)
	  {
		VTKTypeNameTraits<T> tn;
		s << "<DataArray type=\"" << tn() << "\" Name=\"" << name << "\" ";
		if (ncomps>1)
		  s << "NumberOfComponents=\"" << ncomps << "\" ";
		s << "format=\"appended\" offset=\""<< bytecount << "\" />" << std::endl;
		bytecount += 4; // header
	  }

	  //! write one data element to output stream
	  void write (T data)
	  {
		bytecount += sizeof(T);
	  }

	private:
	  std::ostream& s;
	  unsigned int& bytecount;
	};


	// write out data in binary
	class SimpleStream
	{
	public:
	  SimpleStream (std::ostream& theStream)
		: s(theStream)
	  {}
	  template<class T>
	  void write (T data)
	  {
		char* p = reinterpret_cast<char*>(&data);
		s.write(p,sizeof(T));
	  }
	private:
	  std::ostream& s;
	};

	void indentUp ()
	{
	  indentCount++;
	}

	void indentDown ()
	{
	  indentCount--;
	}

	void indent (std::ostream& s)
	{
	  for (int i=0; i<indentCount; i++) 
		s << "  ";
	}

	// renumber VTK -> Dune
	int renumber (const Entity& e, int i)
	{
      static const int quadRenumbering[4] = {0,1,3,2};
      static const int cubeRenumbering[8] = {0,1,3,2,4,5,7,6};
      static const int prismRenumbering[6] = {0,2,1,3,5,4};
      switch (vtkType(e.geometry().type()))
      {
      case vtkQuadrilateral:
        return quadRenumbering[i];
      case vtkHexahedron:
        return cubeRenumbering[i];
      case vtkPrism:
        return prismRenumbering[i];
      default:
        return i;
      }
	}

	// the list of registered functions
	std::list<VTKFunction*> celldata;
	std::list<VTKFunction*> vertexdata;

	// the grid
	const GridImp& grid;

	// the indexset
	const IndexSet& is;

	// intend counter
	int indentCount;
	int numPerLine;

	// temporary grid information
	int ncells;
	int nvertices;
	int ncorners;
	VM1* vertexmapper;
	std::vector<int> number;
	VTKOptions::Type datamode;
	unsigned int bytecount;
  };

  /** \brief VTKWriter on the leaf grid
   */
  template<class G>
  class LeafVTKWriter : public VTKWriter<G,typename G::template Codim<0>::LeafIndexSet>
  {
  public:
	LeafVTKWriter (const G& grid) 
	  : VTKWriter<G,typename G::template Codim<0>::LeafIndexSet>(grid,grid.leafIndexSet())
	{}
  };

  /** \brief VTKWriter on a given level grid
   */
  template<class G>
  class LevelVTKWriter : public VTKWriter<G, typename G::template Codim<0>::LevelIndexSet>
  {
  public:
	LevelVTKWriter (const G& grid, int level) 
	  : VTKWriter<G,typename G::template Codim<0>::LevelIndexSet>(grid,grid.levelIndexSet(level))
	{}
  };
}
#endif
