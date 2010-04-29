/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
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
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

/*
 *  SPM reader class
 */
#ifndef AIMS_IO_SPMR_H
#define AIMS_IO_SPMR_H

#include <aims/memmap/allocator.h>
#include <cartobase/object/object.h>
#include <fstream>
#include <string>


template <typename T> class AimsData;

namespace aims
{
class AffineTransformation3d;
}
typedef aims::AffineTransformation3d Motion;

namespace aims 
{

  template <typename T> class SpmReader;
  class SpmHeader;


  template <class T>
  SpmReader<T>&
  operator >> (SpmReader<T>& reader,
	       AimsData<T>& thing);


  /** The template class for SPM read operation.
      The template argument gives the type of the elements stored
      in the container.
  */
  template <typename T>
  class SpmReader
  {
  public:
    SpmReader(const std::string& name) 
      : _name( name ) { }
    virtual ~SpmReader() { }
  
    /** Read the data with "name" file name from disk
	@param thing reference to the data to read
	@param border border width (useful for image processing)
    */
    void read(AimsData<T>& thing, const carto::AllocatorContext & context, 
              carto::Object options );
  
    /// Return a name without .hdr or .img extension
    std::string removeExtension(const std::string& name);
  
    /// Input stream operator
    friend
    SpmReader<T>& 
    operator >> <> (SpmReader<T>& reader,
		    AimsData<T>& thing);
  
  private:
    void readFrame( std::ifstream &, AimsData<T> & data, int t, 
                    const std::string & type, SpmHeader* hdr, bool bswap, 
                    const Motion & storageToMemory );
    template<typename U> 
    void readScaledFrame( std::ifstream &, AimsData<T> & data, int t, 
                          SpmHeader* hdr, bool bswap,
                          const Motion & storageToMemory );
    std::string	_name;
  };


  template <typename T>
  inline
  SpmReader<T>&
  operator >> (SpmReader<T>& reader,
	       AimsData<T>& thing)
  {
    reader.read( thing );
    return reader;
  }

}

#endif
