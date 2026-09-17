/*
 *  Data reader class
 */
#ifndef AIMS_IO_BASEFORMATS_SPARSEMATRIX_H
#define AIMS_IO_BASEFORMATS_SPARSEMATRIX_H

#include <aims/config/aimsdata_config.h>
#include <aims/io/fileFormat.h>
#include <aims/sparsematrix/sparseordensematrix.h>


namespace aims
{

  class ImasFormat : public FileFormat<SparseMatrix>
  {
    virtual bool read( const std::string & filename, SparseMatrix & obj,
                       const carto::AllocatorContext & context,
                       carto::Object options );
    virtual bool write( const std::string & filename,
                        const SparseMatrix & obj,
                        carto::Object options = carto::none() );
  };


  class ImasSorDFormat : public FileFormat<SparseOrDenseMatrix>
  {
    virtual bool read( const std::string & filename, 
                       SparseOrDenseMatrix & obj,
                       const carto::AllocatorContext & context,
                       carto::Object options );
    virtual bool write( const std::string & filename,
                        const SparseOrDenseMatrix & obj,
                        carto::Object options = carto::none() );
  };

}


#endif
