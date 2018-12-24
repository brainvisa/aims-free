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

#ifndef CARTODATA_VOLUME_VOLUMEREF_H
#define CARTODATA_VOLUME_VOLUMEREF_H

//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volumebase.h>
#include <cartodata/volume/volumebase_d_operators.h>
#include <cartodata/volume/volumebase_d_inline.h>
#include <cartodata/volume/volumebase_d_instantiate.h>
//--- soma-io ----------------------------------------------------------------
#include <soma-io/utilities/creator.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/smart/rcptr.h>
#include <cartobase/type/datatypetraits.h>
//----------------------------------------------------------------------------
#include <iostream>

#define CARTO_VOLUME_AUTO_DEREFERENCE

namespace carto {

//============================================================================
//   V O L U M E   R E F
//============================================================================
  /// Convenient handle for a Volume - this is normally the entry point
  /// for all volumes handling
  ///
  /// See Volume for more documentation
  template <typename T>
  class VolumeRef: public rc_ptr<Volume<T> >
  {
  public:
    //========================================================================
    //   CONSTRUCTORS
    //========================================================================
    VolumeRef();
    VolumeRef( Volume<T>* x );
    VolumeRef( const rc_ptr<Volume<T> > & x );
    virtual ~VolumeRef();

    Volume<T>& operator* ()
    {
      return *(this->get());
    }

    const Volume<T>& operator* () const
    {
      return *(this->get());
    }

    //========================================================================
    //   TYPES
    //========================================================================
    typedef T datatype;

#ifdef CARTO_VOLUME_AUTO_DEREFERENCE

  public:
    //========================================================================
    //   TYPES
    //========================================================================
    typedef typename Volume<T>::Position4Di Position4Di;
    typedef typename Volume<T>::Position Position;
    typedef typename Volume<T>::iterator iterator;
    typedef typename Volume<T>::const_iterator const_iterator;

    //========================================================================
    //   CONSTRUCTORS
    //========================================================================
    explicit VolumeRef( int sizeX, int sizeY = 1, int sizeZ = 1,
                        int sizeT = 1,
                        const AllocatorContext& allocatorContext
                         = AllocatorContext(),
                        bool allocated = true );
    explicit VolumeRef( const Position4Di & size,
                        const AllocatorContext& allocatorContext
                         = AllocatorContext(),
                        bool allocated = true );
    explicit VolumeRef( int sizeX, int sizeY, int sizeZ,
                        int sizeT, int bordersize,
                        const AllocatorContext& allocatorContext
                         = AllocatorContext(),
                        bool allocated = true );
    explicit VolumeRef( const Position4Di & size, int bordersize,
                        const AllocatorContext& allocatorContext
                         = AllocatorContext(),
                        bool allocated = true );
    explicit VolumeRef( int sizeX, int sizeY, int sizeZ, int sizeT,
                        const Position4Di & border,
                        const AllocatorContext& allocatorContext
                         = AllocatorContext(),
                        bool allocated = true );
    explicit VolumeRef( const Position4Di & size,
                        const Position4Di & border,
                        const AllocatorContext& allocatorContext
                         = AllocatorContext(),
                        bool allocated = true );
    explicit VolumeRef( rc_ptr<Volume<T> > other,
                        const Position4Di & pos,
                        const Position4Di & size = Position4Di( -1, -1, -1, -1 ),
                        const AllocatorContext & allocContext = AllocatorContext() );
    explicit VolumeRef( rc_ptr<Volume<T> > other,
                        const Position & pos,
                        const Position & size = Position(),
                        const AllocatorContext & allocContext
                          = AllocatorContext() );

    //========================================================================
    //   VOLUME PROXY
    //========================================================================
    std::vector<int> getSize() const;
    int getSizeX() const;
    int getSizeY() const;
    int getSizeZ() const;
    int getSizeT() const;
    const PropertySet& header() const;
          PropertySet& header();
    const PropertySet& getPropertySet() const
      __attribute__((__deprecated__("use header() instead")));
    PropertySet& getPropertySet()
      __attribute__((__deprecated__("use header() instead")));
    std::vector<float> getVoxelSize() const;

    virtual void copyHeaderFrom( const PropertySet & other );
    virtual void copyHeaderFrom( const Object & other );

    //========================================================================
    //   ITERATORS
    //========================================================================
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    //========================================================================
    //   ACCESSORS
    //========================================================================
    const T& operator()( long x, long y = 0, long z = 0, long t = 0 ) const;
    T& operator() ( long x, long y = 0, long z = 0, long t = 0 );
    const T& at( long x, long y = 0, long z = 0, long t = 0 ) const;
    T& at( long x, long y = 0, long z = 0, long t = 0 );
    const T& operator() ( const Position4Di & position ) const;
    T& operator() ( const Position4Di & position );
    const T& at( const Position4Di & position ) const;
    T& at( const Position4Di & position );
    const T& operator() ( const Position & position ) const;
    T& operator() ( const Position & position );
    const T& at( const Position & position ) const;
    T& at( const Position & position );

    //========================================================================
    //   INIT / ALLOCATION
    //========================================================================
    virtual void initialize();
    const AllocatorContext & allocatorContext() const;
    void allocate();
    virtual void reallocate( int sizeX = 1, int sizeY = 1, int sizeZ = 1,
                             int sizeT = 1, bool keepcontents = false,
                             const AllocatorContext& allocatorContext
                             = AllocatorContext(), bool allocate = true );
#if 0
    virtual void reallocate( const Position4Di & size,
                             bool keepcontents = false,
                             const AllocatorContext& allocatorContext
                             = AllocatorContext(), bool allocate = true );
#endif

    //========================================================================
    //   COPY / VIEW
    //========================================================================
    VolumeRef<T> copy() const;
    template <typename OUTP>
    VolumeRef<OUTP> copy() const;

    VolumeRef<T> deepcopy() const;
    template <typename OUTP>
    VolumeRef<OUTP> deepcopy() const;

    VolumeRef<T> copyStructure() const;
    template <typename OUTP>
    VolumeRef<OUTP> copyStructure() const;

    template <typename OUTP>
    operator VolumeRef<OUTP>() const;

    int getLevelsCount() const;
    int refLevel(const int level) const;
    rc_ptr<Volume<T> > refVolume() const;
    rc_ptr<Volume<T> > refVolumeAtLevel(const int level) const;
    void setRefVolume(const rc_ptr<Volume<T> > & refvol);
    const Position4Di posInRefVolume() const;
    typename Volume<T>::Position posInRefVolumeAtLevel(const int level) const;
    
    void setPosInRefVolume(const Position4Di & pos);
    void setPosInRefVolume(const typename Volume<T>::Position & pos);
    
    std::vector<int> getBorders() const;
    std::vector<size_t> getStrides() const;

    //========================================================================
    //   BOOL
    //========================================================================
    /// True if all values compare to true
    bool all() const;
    /// True if at least one value compares to true
    bool any() const;
    // /// Implicit cast to bool
    // /// returns all()
    // operator bool() const;
    T min() const;
    T max() const;
    /// To avoid overflow, the biggest possible type (intmax_t, uintmax_t,
    /// double...) is used for computation and returned.
    typename DataTypeTraits<T>::LongType sum() const;

    //========================================================================
    //   FILL / REPLACE
    //========================================================================
    /// Fills the volume with a given value.
    void fill( const T & value );
    VolumeRef<T> & operator= ( const T & value );

#endif // CARTO_VOLUME_AUTO_DEREFERENCE

    //========================================================================
    //   SPECIAL VOLUMEREF METHODS
    //========================================================================
    VolumeRef<T> view( const Position4Di & pos, const Position4Di & size );
    const VolumeRef<T> view( const Position4Di & pos, const Position4Di & size ) const;
  }; // class VolumeRef

//============================================================================
//   UTILITIES
//============================================================================
#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

  template <typename T>
  class DataTypeCode<VolumeRef<T> >
  {
  public:
    static std::string objectType()
    { return "VolumeRef"; }
    static std::string dataType()
    { return DataTypeCode<T>::dataType(); }
    static std::string name()
    {
      return std::string("VolumeRef of ") + DataTypeCode< T >::name();
    }
  };

  template <typename T>
  class Creator<VolumeRef<T> >
  {
  public:
    static VolumeRef<T>* create( Object, const AllocatorContext &, Object );
    static void setup( VolumeRef<T> &, Object, const AllocatorContext &, Object );
  };

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES

//============================================================================
//   STREAM
//============================================================================

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

  template <typename T>
  std::ostream & operator<< ( const carto::VolumeOStream & out,
                              const carto::rc_ptr<carto::VolumeRef<T> > & volume );

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES

  /// Volumes are printable to standard output streams.
  /// They are shown as an array, eventually cropped at a given size.
  /// The default crop value is 5 in each dimension.
  /// The crop value can be set with setMaxDim() the following way:
  /// \code
  /// #include <cartodata/volume/volume.h>
  /// #include <iostream>
  /// using namespace carto;
  /// using namespace std;
  ///
  /// carto::VolumeRef<int16_t> vol( 10, 10, 10 );
  /// cout << setMaxDim( 10 ) << vol << endl;
  /// \endcode
  template <typename T>
  std::ostream & operator<< ( std::ostream & out,
                              const carto::rc_ptr<carto::Volume<T> > & volume );

  /// Display information about volume hierarchy
  template <typename T>
  void displayRefVolumes(const carto::VolumeRef<T> & vol);
} // namespace carto


#endif // CARTODATA_VOLUME_VOLUMEREF_H
