/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info". 
 * 
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability. 
 * 
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or 
 * data to be ensured and,  more generally, to use and operate it in the 
 * same conditions as regards security. 
 * 
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

/*
 *  NIFTI reader class
 */
#ifndef AIMS_IO_NIFTIR_H
#define AIMS_IO_NIFTIR_H

#include <aims/io/niftiheader.h>
#include <aims/data/data.h>
#include <cartobase/allocator/mappingcopy.h>

template <class T> class AimsData;

namespace aims 
{
  template <class T> class NiftiReader;
  
  /* class NiftiHeader; */
  
  template< class T > NiftiReader< T >&
  operator >> ( NiftiReader< T >& reader, AimsData< T >& thing );

  /** The template class for NIFTI read operation.
  */
  template <class T>
  class NiftiReader
  {
  public:
    NiftiReader(const std::string& name) 
      : _name( name ) 
    { }
    virtual ~NiftiReader() { }
  
    /** Read the data with "name" file name from disk
	    @param thing reference to the data to read
    */
    void read(AimsData<T>& thing, const carto::AllocatorContext & context, 
              carto::Object options );

    void readSubImage( NiftiHeader* hdr, int t, AimsData<T> & data );
    /*
    void readFrame( std::ifstream &, AimsData<T> & data, int t, 
                    const std::string & type, NiftiHeader* hdr, bool bswap, 
                    bool radio );

    void readScaledFrame( std::ifstream &, AimsData<T> & data, int t, 
                          NiftiHeader* hdr, bool bswap, bool radio );
    */
    /// Return a name without .hdr or .img or .nii or .nii.gz extension
    std::string removeExtension(const std::string& name);
	
	/// Input stream operator
	friend
    NiftiReader<T>& 
    operator >> <> ( NiftiReader<T>& reader, AimsData<T>& thing );
  
  private:

    std::string	_name;
  };


  template< class T > inline
  NiftiReader< T >&
  operator >> (NiftiReader<T>& reader, AimsData<T>& thing)
  {
    reader.read( thing );
    return( reader );
  }

}

#endif
