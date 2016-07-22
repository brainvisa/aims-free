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

#ifndef CARTODATA_VOLUME_VOLUMEPROXY_H
#define CARTODATA_VOLUME_VOLUMEPROXY_H


#include <cartobase/object/headered.h>


namespace carto
{


  /** VolumeProxy is the base class for volumes. It handles the management of
      the volume size, but not its contents (it has no data block).
  */
  template < typename T >
  class VolumeProxy : public Headered
  {

  public:

    explicit VolumeProxy( int sizeX = 1, int sizeY = 1, int sizeZ = 1,
                          int sizeT = 1 );
    explicit VolumeProxy( const std::vector<int> & size );
    VolumeProxy( const VolumeProxy< T >& other );
    virtual ~VolumeProxy();

    int getSizeX() const;
    int getSizeY() const;
    int getSizeZ() const;
    int getSizeT() const;
    /// get the 4 dimensions in a vector
    std::vector<int> getSize() const;
    /** get the voxel size from the header,
       with 4 values defaulting to 1.mm if not present */
    std::vector<float> getVoxelSize() const;

    VolumeProxy< T >& operator=( const VolumeProxy< T >& other );

    /** copy properties from other to this, avoiding forbidden
        properties like size.
    */
    virtual void copyHeaderFrom( const PropertySet & other );
    virtual void copyHeaderFrom( const Object & other );

  protected:

    std::vector<int> _size;

  };


  template < typename T >
  inline
  int VolumeProxy< T >::getSizeX() const
  {

    return _size[0];

  }


  template < typename T >
  inline
  int VolumeProxy< T >::getSizeY() const
  {

    return _size[1];

  }


  template < typename T >
  inline
  int VolumeProxy< T >::getSizeZ() const
  {

    return _size[2];

  }


  template < typename T >
  inline
  int VolumeProxy< T >::getSizeT() const
  {

    return _size[3];

  }


  template < typename T >
  inline
  std::vector<int> VolumeProxy< T >::getSize() const
  {

    return _size;

  }


  template < typename T >
  inline
  void VolumeProxy< T >::copyHeaderFrom( const PropertySet & other )
  {
    copyHeaderFrom( Object::reference( other ) );
  }


 //  instanciations

  extern template class VolumeProxy<int8_t>;
  extern template class VolumeProxy<uint8_t>;
  // ### remove after everything has been moved to intN_t/uintN_t
#if !defined(__sun__) || !defined(_CHAR_IS_SIGNED)
  extern template class VolumeProxy<char>;
#endif
  extern template class VolumeProxy<int16_t>;
  extern template class VolumeProxy<uint16_t>;
  extern template class VolumeProxy<int32_t>;
  extern template class VolumeProxy<uint32_t>;
  extern template class VolumeProxy<long>;
  extern template class VolumeProxy<unsigned long>;
  extern template class VolumeProxy<float>;
  extern template class VolumeProxy<double>;
  extern template class VolumeProxy<cfloat>;
  extern template class VolumeProxy<cdouble>;
}


#endif
