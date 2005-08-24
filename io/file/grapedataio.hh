#ifndef __DUNE_GRAPEDATAIO_HH__
#define __DUNE_GRAPEDATAIO_HH__

#include <dune/common/misc.hh>
#include <dune/grid/common/grid.hh>
#include <dune/io/file/asciiparser.hh>

namespace Dune {

  /*! 
    Specify the format to store grid and vector data
  */
  enum GrapeIOFileFormatType 
      { ascii , //!< store data in a human readable form
        xdr ,   //!< store data in SUN's library routines
                //!< for external data representation (xdr)
        pgm };  //!< store data in portable graymap file format 



typedef std::string GrapeIOStringType;

/** \brief convert type to string 
 */
template <typename T>
inline GrapeIOStringType typeIdentifier ()
{
  GrapeIOStringType tmp = "unknown";
  return tmp;
};

template <>
inline GrapeIOStringType typeIdentifier<float> ()
{
  GrapeIOStringType tmp = "float";
  return tmp;
};

template <>
inline GrapeIOStringType typeIdentifier<int> ()
{
  GrapeIOStringType tmp = "int";
  return tmp;
};

template <>
inline GrapeIOStringType typeIdentifier<double> ()
{
  GrapeIOStringType tmp = "double";
  return tmp;
};



template <class GridType>
class GrapeDataIO 
{
public:

   GrapeDataIO () {};

   /** Write Grid with GridType file filename and time 
   *
   * This method uses the Grid Interface Method writeGrid 
   * to actually write the grid, within this method the real file name is
   * generated out of filename and timestep 
   */
  inline bool writeGrid (const GridType & grid, 
    const GrapeIOFileFormatType ftype, const GrapeIOStringType fnprefix 
      , double time=0.0, int timestep=0, int precision = 6);

  //! get Grid from file with time and timestep , return true if ok 
  inline bool readGrid (GridType & grid, 
      const GrapeIOStringType fnprefix , double & time , int timestep);


  /**
    Write DiscreteFunctions  
  */ 
  //! write disc func information file and write dofs to file+timestep
  //! this method use the write method of the implementation of the
  //! discrete function
  template <class DiscreteFunctionType>
  inline bool writeData(DiscreteFunctionType & df,
     const GrapeIOFileFormatType ftype, const GrapeIOStringType filename, 
      int timestep, int precision = 6);

  //! same as write only read
  template <class DiscreteFunctionType>
  inline bool readData(DiscreteFunctionType & df,
        const GrapeIOStringType filename, int timestep);
};


template <class GridType>
inline bool GrapeDataIO<GridType> :: writeGrid 
(const GridType & grid,
  const GrapeIOFileFormatType ftype, const GrapeIOStringType fnprefix , 
  double time, int timestep, int precision )
{
  const char *path = "";
  std::fstream file (fnprefix.c_str(),std::ios::out);
  file << "Grid: "   << transformToGridName(grid.type()) << std::endl;
  file << "Format: " << ftype <<  std::endl;
  file << "Precision: " << precision << std::endl;

  GrapeIOStringType fnstr = genFilename(path,fnprefix,timestep,precision);
  file.close();
  switch (ftype)
  {
    case xdr  :   return grid.template writeGrid<xdr>  (fnstr,time);
    case ascii:   return grid.template writeGrid<ascii>(fnstr,time);
    default:
        {
          std::cerr << ftype << " GrapeIOFileFormatType not supported at the moment! " << __FILE__ << __LINE__ << "\n";
          assert(false);
          abort();
          return false;
        }
  }
  return false;
}


template <class GridType>
inline bool GrapeDataIO<GridType> :: readGrid 
(GridType & grid, const GrapeIOStringType fnprefix , double & time , int timestep)
{
  int helpType;

  char gridname [1024];
  readParameter(fnprefix,"Grid",gridname);
  std::string gname (gridname);

  if(transformToGridName(grid.type()) != gname)
  {
    std::cerr << "\nERROR: " << transformToGridName(grid.type()) << " tries to read " << gname << " file. \n";
    abort();
  }

  readParameter(fnprefix,"Format",helpType);
  GrapeIOFileFormatType ftype = (GrapeIOFileFormatType) helpType;

  int precision = 6;
  readParameter(fnprefix,"Precision",precision);

  const char *path = "";
  GrapeIOStringType fn = genFilename(path,fnprefix,timestep,precision);
  std::cout << "Read file: fnprefix = `" << fn << "' \n";

  switch (ftype)
  {
    case xdr  :   return grid.template readGrid<xdr>  (fn,time);
    case ascii:   return grid.template readGrid<ascii>(fn,time);
    default:
        {
          std::cerr << ftype << " GrapeIOFileFormatType not supported at the moment! \n";
          assert(false);
          abort();
          return false;
        }
  }
  return false;
}

template <class GridType>
template <class DiscreteFunctionType> 
inline bool GrapeDataIO<GridType> :: writeData(DiscreteFunctionType & df, 
const GrapeIOFileFormatType ftype, const GrapeIOStringType filename, int timestep, int  precision )
{
  {
    typedef typename DiscreteFunctionType::FunctionSpaceType DiscreteFunctionSpaceType;
    typedef typename DiscreteFunctionSpaceType::DomainFieldType DomainFieldType;
    typedef typename DiscreteFunctionSpaceType::RangeFieldType RangeFieldType;

    enum { n = DiscreteFunctionSpaceType::DimDomain };
    enum { m = DiscreteFunctionSpaceType::DimRange };

    std::fstream file( filename.c_str() , std::ios::out );
    GrapeIOStringType d = typeIdentifier<DomainFieldType>();
    GrapeIOStringType r = typeIdentifier<RangeFieldType>();

    file << "DomainField: " << d << std::endl;
    file << "RangeField: " << r << std::endl;
    file << "Dim_Domain: " << n << std::endl;
    file << "Dim_Range: " << m << std::endl;
    file << "Space: " << df.getFunctionSpace().type() << std::endl;
    file << "Format: " << ftype << std::endl;
    file << "Precision: " << precision << std::endl;
    file << "Polynom_order: " << df.getFunctionSpace().polynomOrder() << std::endl;
    file.close();
  }

  const char * path = "";
  GrapeIOStringType fn = genFilename(path,filename,timestep,precision);

  if(ftype == xdr)
    return df.write_xdr(fn);
  if(ftype == ascii)
    return df.write_ascii(fn);
  if(ftype == pgm)
    return df.write_pgm(fn);

  return false;
}

template <class GridType>
template <class DiscreteFunctionType> 
inline bool GrapeDataIO<GridType> :: 
readData(DiscreteFunctionType & df, const GrapeIOStringType filename, int timestep)
{
    typedef typename DiscreteFunctionType::FunctionSpaceType DiscreteFunctionSpaceType;
    typedef typename DiscreteFunctionSpaceType::DomainFieldType DomainFieldType;
    typedef typename DiscreteFunctionSpaceType::RangeFieldType RangeFieldType;

    enum { tn = DiscreteFunctionSpaceType::DimDomain };
    enum { tm = DiscreteFunctionSpaceType::DimRange };

    int n,m;
    GrapeIOStringType r,d;
    GrapeIOStringType tr (typeIdentifier<RangeFieldType>());
    GrapeIOStringType td (typeIdentifier<DomainFieldType>());

    readParameter(filename,"DomainField",d,false);
    readParameter(filename,"RangeField",r,false);
    readParameter(filename,"Dim_Domain",n,false);
    readParameter(filename,"Dim_Range",m,false);
    int space;
    readParameter(filename,"Space",space,false);
    int filetype;
    readParameter(filename,"Format",filetype,false);
    GrapeIOFileFormatType ftype = static_cast<GrapeIOFileFormatType> (filetype);
    int precision;
    readParameter(filename,"Precision",precision,false);

    if((d != td) || (r != tr) || (n != tn) || (m != tm) )
    {
      std::cerr << d << " | " << td << " DomainField in read!\n";
      std::cerr << r << " | " << tr << " RangeField  in read!\n";
      std::cerr << n << " | " << tn << " in read!\n";
      std::cerr << m << " | " << tm << " in read!\n";
      std::cerr << "Can not initialize DiscreteFunction with wrong FunctionSpace! \n";
      abort();
    }

  const char * path = "";
  GrapeIOStringType fn = genFilename(path,filename,timestep,precision);

  if(ftype == xdr)
    return df.read_xdr(fn);
  if(ftype == ascii)
    return df.read_ascii(fn);
  if(ftype == pgm)
    return df.read_pgm(fn);

  std::cerr << ftype << " GrapeIOFileFormatType not supported at the moment! in file " << __FILE__ << " line " << __LINE__ << "\n"; 
  abort();

  return false;
}


} // end namespace

#endif
