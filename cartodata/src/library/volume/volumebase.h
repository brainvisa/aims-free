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

#ifndef CARTODATA_VOLUME_VOLUMEBASE_H
#define CARTODATA_VOLUME_VOLUMEBASE_H
//--- cartodata --------------------------------------------------------------
#include <cartodata/config/config.h>
#include <cartodata/volume/volumeproxy.h>
#include <cartodata/transformation/referential.h>
//--- soma-io ----------------------------------------------------------------
#include <soma-io/utilities/allocatedvector.h>
#include <soma-io/utilities/creator.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/allocator/allocator.h>
#include <cartobase/smart/rcptr.h>
#include <cartobase/type/datatypetraits.h>
//--- blitz ------------------------------------------------------------------
  #ifdef _WIN32
    // disable thread-safe operations in blitz++ on windows since it uses pthread
    #ifdef _REENTRANT
      #define CARTO_REENTRANT_MEMO
      #undef _REENTRANT
    #endif
    #include <blitz/blitz.h>
    #ifdef CARTO_REENTRANT_MEMO
      #define _REENTRANT
      #undef CARTO_REENTRANT_MEMO
    #endif
  #endif
  #include <blitz/array.h>
  
namespace carto {
  typedef ptrdiff_t BlitzStridesType;
}


//--- std --------------------------------------------------------------------
#include <vector>
//--- forward declarations ---------------------------------------------------
namespace carto {
  class AllocatorContext;
  class PropertyFilter;
}
//----------------------------------------------------------------------------

namespace carto
{
//============================================================================
//   V O L U M E
//============================================================================
  /// N-D Volume main class
  ///
  /// Since 2013 release (version 4.4), Volume and VolumeView are merged into
  /// a singleclass. Every Volume can be a view into an other volume. If it is
  /// not, its parent volume is NULL. This allows to deal the same way with
  /// classic volumes, volumes with borders or partially read volumes.
  ///
  /// Volumes should generally be used via reference-counting pointers
  /// (rc_ptr): there is a (slightly) specialized rc_ptr for Volume
  /// subclasses: VolumeRef.
  ///
  /// Since 2016 release (version 4.5), comparison and arithmetic operators
  /// are specialized and shipped in volume.h. They can be used as long as
  /// the same operation between contained types is well defined.
  /// Several helper method (member and non member) are also included.
  ///
  /// In version 4.6 Volume has extended support from 4D to N-D, still limited
  /// to a fixed max number of dimensions at compilation time (normally set
  /// to 8).
  ///
  /// \sa \ref cartovolumes
  ///
  /// Iterating on volumes is described in \ref volume_iteration
  ///
  /// To use Volume classes on "standard" types (numeric types, on which the
  /// Volume class has already been compiled in the libraries), just include
  /// <cartodata/volume/volume.h>. If you need to use them on other
  /// non-standard types (if you get a link error about missing symbols), you
  /// will need to recompile the Volume template class on these types: in this
  /// case you should include <cartodata/volume/volume_d.h> and force the
  /// compilation:
  /// \code
  /// #include <cartodata/volume/volume_d.h>
  ///
  /// template class carto::Volume<MyType>;
  /// \endcode
  /// This should be done only once (in one source file) for each type.
  template < typename T >
  class Volume : public VolumeProxy< T >
  {
  public:
    //========================================================================
    //   TYPES
    //========================================================================
    /// 4D int position / size.
    /// It contains a constructor enabling implicit conversion from any
    /// "vector-like" object, i.e. possessing a size() method and the []
    /// operator
    class Position4Di;
    typedef std::vector<int> Position;

    /// Access to T type
    typedef T datatype;

    /// The most "basic" (and fastest) iterators:
    /// they go from the first voxel linerarly in memory, not taking care of
    /// offsets when in a Volume view. Taking care of splitting loops
    /// line-by-line is the responsability of programmers using such
    // static const int DIM_MAX = 8; leads to build issues on old GCC versions
    // so we need to use enum to be able to declare DIM_MAX in carto::Volume
    enum { DIM_MAX = 8 };
    typedef typename blitz::Array<T,Volume<T>::DIM_MAX>::iterator iterator;
    typedef typename blitz::Array<T,Volume<T>::DIM_MAX>::const_iterator const_iterator;

    //========================================================================
    //   CONSTRUCTORS
    //========================================================================
    /// Volume construction and allocation
    /// \param sizeX number of voxels
    /// \param sizeY number of voxels
    /// \param sizeZ number of voxels
    /// \param sizeT number of voxels
    /// \param allocatorContext information about how to allocate the volume:
    /// it can be a bit complex to do really optimal things, but the default
    /// value (default constructor of AllocatorContext) is OK in most cases.
    /// \param allocated normally left to \c true, it can exceptionnally be
    /// set to \c false for "virtual" volumes that must not be actually
    /// allocated but are only sources for a view Volume.
    explicit Volume( int sizeX = 1, int sizeY = 1, int sizeZ = 1,
                     int sizeT = 1,
                     const AllocatorContext& allocatorContext
                      = AllocatorContext(),
                     bool allocated = true );
    /// Volume construction and allocation: Position4Di version
    /// \param size number of voxels in each dimension
    /// \param allocatorContext information about how to allocate the volume:
    /// it can be a bit complex to do really optimal things, but the default
    /// value (default constructor of AllocatorContext) is OK in most cases.
    /// \param allocated normally left to \c true, it can exceptionnally be
    /// set to \c false for "virtual" volumes that must not be actually
    /// allocated but are only sources for a view Volume.
    explicit Volume( const Position4Di & size,
                     const AllocatorContext& allocatorContext
                      = AllocatorContext(),
                     bool allocated = true );
    /// Volume construction and allocation
    /// Same as the above constructor, but allows to specify a border size.
    /// Border is handled as a larger volume, with a view in it. The current
    /// volume is this view.
    explicit Volume( int sizeX, int sizeY, int sizeZ,
                     int sizeT, int bordersize,
                     const AllocatorContext& allocatorContext
                      = AllocatorContext(),
                     bool allocated = true );
    /// Volume construction and allocation: Position4Di version
    /// Same as the above constructor, but allows to specify a border size.
    /// Border is handled as a larger volume, with a view in it. The current
    /// volume is this view.
    explicit Volume( const Position4Di & size, int bordersize,
                     const AllocatorContext& allocatorContext
                      = AllocatorContext(),
                     bool allocated = true );
    /// Volume construction and allocation
    /// Same as the above constructor, but allows to specify a border size
    /// in each dimension
    explicit Volume( int sizeX, int sizeY, int sizeZ, int sizeT,
                     const Position4Di & border,
                     const AllocatorContext& allocatorContext
                      = AllocatorContext(),
                     bool allocated = true );
    /// Volume construction and allocation: Position4Di version
    /// Same as the above constructor, but allows to specify a border size
    /// in each dimension
    explicit Volume( const Position4Di & size,
                     const Position4Di & border,
                     const AllocatorContext& allocatorContext
                      = AllocatorContext(),
                     bool allocated = true );
    /// Volume construction and allocation: std::vector version
    /// Same as the above constructor, but allows to specify a border size
    /// in each dimension
    explicit Volume( const std::vector<int> & size,
                     const AllocatorContext& allocatorContext
                      = AllocatorContext(),
                     bool allocated = true );
    /// Volume construction and allocation: std::vector version
    /// Same as the above constructor, but allows to specify a border size
    /// in each dimension
    explicit Volume( const std::vector<int> & size,
                     const std::vector<int> & border,
                     const AllocatorContext& allocatorContext
                      = AllocatorContext(),
                     bool allocated = true );
    /// This constructor builds a Volume on an already allocated buffer.
    /// The Volume is not owner of the underlying data.
    Volume( int sizeX, int sizeY, int sizeZ, int sizeT, T* buffer,
            const std::vector<long> *strides = 0 );
    /// Position4Di version
    /// This constructor builds a Volume on an already allocated buffer.
    /// The Volume is not owner of the underlying data.
    Volume( const Position4Di & size, T* buffer,
            const std::vector<long> *strides = 0 );
    Volume( const std::vector<int> & size, T* buffer,
            const std::vector<long> *strides = 0 );
    /// This is the volume view constructor.
    /// Beware not to mix it up with the copy constructor ( it takes a pointer
    /// to volume instead of a volume )
    /// If parent volume is allocated, view points to its data and doesn't own
    /// it. Else, it allocates \c size and owns it.
    Volume( rc_ptr<Volume<T> > other,
            const Position4Di & pos = Position4Di( 0, 0, 0, 0 ),
            const Position4Di & size = Position4Di( -1, -1, -1, -1 ),
            const AllocatorContext & allocContext = AllocatorContext() );
    /// This is the volume view constructor.
    /// Beware not to mix it up with the copy constructor ( it takes a pointer
    /// to volume instead of a volume )
    /// If parent volume is allocated, view points to its data and doesn't own
    /// it. Else, it allocates \c size and owns it.
    Volume( rc_ptr<Volume<T> > other,
            const Position & pos,
            const Position & size = Position(),
            const AllocatorContext & allocContext = AllocatorContext() );
    /// This "very special" constructor should not be used in regular programs.
    /// It is meant for the IO system to map file views into memory views.
    Volume( rc_ptr<Volume<T> > other,
            const Position & pos,
            const Position & size,
            T* buffer, const std::vector<long> & strides );
    /// Copy constructor
    /// The copy constructors actually duplicates data buffers. In the case
    /// of a volume view, the underlying volume is also duplicated, so the
    /// new volume will be a view in a new duplicate bigger volume.
    Volume( const Volume< T >& other );
    virtual ~Volume();

    Volume< T >& operator=( const Volume< T >& other );

    //========================================================================
    //   ITERATORS
    //========================================================================

    /// Iterators returned here are the most "basic" (and fastest) iterators:
    /// they go from the first voxel linerarly in memory, not taking care of
    /// offsets when in a Volume view. Taking care of splitting loops
    /// line-by-line is the responsability of programmers using such
    /// iterators.
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    //========================================================================
    //   ACCESSORS
    //========================================================================

    /// Warning: this operator is not virtual, so may not have the expected
    /// result on inherited classes (see old VolumeView)
    const T& operator()( long x, long y = 0, long z = 0, long t = 0 ) const;
//     T& operator() ( long x );
    T& operator() ( long x, long y = 0, long z = 0, long t = 0 );
    const T& at( long x, long y = 0, long z = 0, long t = 0 ) const;
    T& at( long x, long y = 0, long z = 0, long t = 0 );
    const T& operator() ( const Position4Di & position ) const;
    T& operator() ( const Position4Di & position );
    const T& at( const Position4Di & position ) const;
    T& at( const Position4Di & position );
    const T & at( const std::vector<int> & ) const;
    T & at( const std::vector<int> & );
    const T& operator() ( const std::vector<int> & position ) const;
    T& operator() ( const std::vector<int> & position );

    const T & at( const blitz::TinyVector<int,1> & ) const;
    T & at( const blitz::TinyVector<int,1> & );
    const T & at( const blitz::TinyVector<int,2> & ) const;
    T & at( const blitz::TinyVector<int,2> & );
    const T & at( const blitz::TinyVector<int,3> & ) const;
    T & at( const blitz::TinyVector<int,3> & );
    const T & at( const blitz::TinyVector<int,4> & ) const;
    T & at( const blitz::TinyVector<int,4> & );
    const T & at( const blitz::TinyVector<int,Volume<T>::DIM_MAX> & ) const;
    T & at( const blitz::TinyVector<int,Volume<T>::DIM_MAX> & );
    blitz::Array<T,Volume<T>::DIM_MAX> at( const blitz::RectDomain<Volume<T>::DIM_MAX>
      & subdomain ) const;
    blitz::Array<T,Volume<T>::DIM_MAX> at( const blitz::StridedDomain<Volume<T>::DIM_MAX>
      & subdomain ) const;
    blitz::Array<T,Volume<T>::DIM_MAX> at( const blitz::Range & r0 ) const;
    blitz::Array<T,Volume<T>::DIM_MAX> at( const blitz::Range & r0,
                          const blitz::Range & r1 ) const;
    blitz::Array<T,Volume<T>::DIM_MAX> at( const blitz::Range & r0,
                                const blitz::Range & r1,
                                const blitz::Range & r2 ) const;
    blitz::Array<T,Volume<T>::DIM_MAX> at( const blitz::Range & r0,
                                const blitz::Range & r1,
                                const blitz::Range & r2,
                                const blitz::Range & r3 ) const;
    // to get beyond 4 dimensions
    const T & at( long x1, long x2, long x3, long x4, long x5, long x6=0,
                  long x7=0, long x8=0 ) const;
    T & at( long x1, long x2, long x3, long x4, long x5, long x6=0,
            long x7=0, long x8=0 );
    const T& operator()( long x1, long x2, long x3, long x4, long x5,
                         long x6=0, long x7=0, long x8=0 ) const;
    T& operator() ( long x1, long x2, long x3, long x4, long x5, long x6=0,
                    long x7=0, long x8=0 );

    //========================================================================
    //   INIT / ALLOCATION
    //========================================================================

    /// Initializes header info.
    virtual void initialize();
    /// returns volume's AllocatorContext
    const AllocatorContext & allocatorContext() const;
    /// This function is only useful in the particular context of an
    /// unallocated Volume, when the constructor has been used with the
    /// \c allocated flag to \c false.
    /// Calling allocate() afterwards will actually allocate the memory.
    /// Otherwise it will do nothing.
    void allocate();
    /// allows resizing and changing allocator
    virtual void reallocate( int sizeX = 1, int sizeY = 1, int sizeZ = 1,
                             int sizeT = 1, bool keepcontents = false,
                             const AllocatorContext& allocatorContext
                             = AllocatorContext(), bool allocate = true,
                             const std::vector<long> *strides = 0 );
    virtual void reallocate( const Position4Di & size,
                             bool keepcontents = false,
                             const AllocatorContext& allocatorContext
                             = AllocatorContext(), bool allocate = true,
                             const std::vector<long> *strides = 0 );
    virtual void reallocate( const std::vector<int> & size,
                             bool keepcontents = false,
                             const AllocatorContext& allocatorContext
                             = AllocatorContext(), bool allocate = true,
                             const std::vector<long> *strides = 0 );
    /// reallocate the volume with given borders, keep (copy) the contents.
    void allocateBorders( int bsx, int bsy = -1, int bsz = -1 );
    /// reallocate the volume with given borders, keep (copy) the contents.
    /// only 1 value is mandatory.
    void allocateBorders( const std::vector<int> & border );

    //========================================================================
    //   COPY / VIEW
    //========================================================================

    /// Create a volume of same dimension and copy the data. This method
    /// does not deal with the underlying view structures, so borders or
    /// parent volumes are not copied or transfered.
    Volume<T> copy() const;
    template <typename OUTP>
    Volume<OUTP> copy() const;
    /// Copy the full data structure. This is similar to what the copy
    /// constructor does.
    Volume<T> deepcopy() const;
    template <typename OUTP>
    Volume<OUTP> deepcopy() const;

    /// Copy the full data structure without copying the actual data.
    Volume<T> copyStructure() const;
    /// Copy the full data structure without copying the actual data.
    template <typename OUTP>
    Volume<OUTP> copyStructure() const;

    /// Copy voxels values from another volume.
    /// pos is the position into the destination (this) volume, and defaults
    /// to 0.
    /// Contrarily to the = operator, the destination (this) volume is not
    /// reallocated or reshaped, and its header is left unchanged.
    void copySubVolume( const Volume<T> & source,
                        const std::vector<int> & pos = std::vector<int>() );
    /// Copy voxels values from another volume
    /// pos is the position into the destination (this) volume, and defaults
    /// to 0.
    /// Contrarily to the = operator, the destination (this) volume is not
    /// reallocated or reshaped, and its header is left unchanged.
    void copySubVolume( const rc_ptr<Volume<T> > & source,
                        const std::vector<int> & pos = std::vector<int>() );

    /// Cast to Volume of different datatype
    template <typename OUTP>
    operator Volume<OUTP>() const;
    
    /// Get levels count in volume hierarchy from the current volume to the 
    /// topmost volume.
    int getLevelsCount() const;
    
    /// Transform a level index to a valid level index in the volume hierarchy.
    /// The current volume has level 0, parent volume as level 1, ... 
    /// Level can also be a negative index from topmost volume. 
    /// -1 is topmost volume, -2 is the the child volume below topmost volume.
    int refLevel(const int level) const;

    /// Get parent volume
    ///
    /// \note refVolume() returns a rc_ptr<Volume> and not a VolumeRef.
    /// Because arithmetic operators are only defined for Volume and VolumeRef
    /// objects, this result must be converted to a VolumeRef to be used in
    /// arithmetic operations.
    rc_ptr<Volume<T> > refVolume() const;
    
    /// Get parent volume at a specified level in volume hierarchy
    ///
    /// \note refVolumeAtLevel() returns a rc_ptr<Volume> and not a VolumeRef.
    /// Because arithmetic operators are only defined for Volume and VolumeRef
    /// objects, this result must be converted to a VolumeRef to be used in
    /// arithmetic operations.    
    rc_ptr<Volume<T> > refVolumeAtLevel(const int level) const;
    
    /// Set parent volume
    void setRefVolume(const rc_ptr<Volume<T> > & refvol);
    /// Get position in parent volume
    const Position & posInRefVolume() const;
    /// Get position relatively to parent volume at specified level
    Position posInRefVolumeAtLevel(const int level) const;

    /// Set position in parent volume
    void setPosInRefVolume(const Position4Di & pos);
    void setPosInRefVolume(const Position & pos);
    

    /// Get borders for the volume. A volume that can have borders is a volume \n
    /// that references another volume. It can be understood as a view in the reference volume.
    /// \return std::vector<uint16_t> that contains the borders availables for the volume.
    ///         vector[0]: low border in x direction
    ///         vector[1]: high border in x direction
    ///         vector[2]: low border in y direction
    ///         vector[3]: high border in y direction
    ///         vector[4]: low border in z direction
    ///         vector[5]: high border in z direction
    ///         vector[6]: low border in t direction
    ///         vector[7]: high border in t direction
    std::vector<int> getBorders() const;

    /// Get strides for the volume. Strides contain the number of voxels for \n
    /// each dimension including.
    std::vector<long> getStrides() const;

    //========================================================================
    //   BOOLEANS / ACCUMULATED VALUES
    //========================================================================
    /// Implicit cast to bool
    /// returns all()
    operator bool() const;
    /// True if all values compare to true
    bool all() const;
    /// True if at least one value compares to true
    bool any() const;
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
    /** Fill border with a constant value

        More precisely, fill the surrounding of the volume view in the
        reference volume (if any) using the given value.
    */
    void fillBorder( const T & value );

    /// Referential and orientation information
    const Referential & referential() const;
    /// Referential and orientation information
    Referential & referential();

    /** Copy operator.
        Care should be taken regarding the behavior of the copy operator:
        depending on the allocation mode of the copied volume, and whether it
        is a view into another volume, different behaviors will be achieved:

        - if "value" is a "regular" volume, then a full copy is performed: if
          "this" is modified later, "value" will not be changed.
        - if "value" is a view into a larger volume (its volumeRef() is not
          null, and its own data buffer is not actually allocated), then the
          copied object, "this", will be another view into the same volume,
          thus resulting in a shallow copy. If "this" is modified later, both
          "value " and the reference volume from which the view is taken, will
          be modified.
    */
    Volume<T> & operator= ( const T & value );

    /** Flip the volume to a given orientation

        The volume voxels will be reordered to match the given orientation.
        Using this method, only the strides will be changed, and the data block
        will remain preserverd. This is different with the other
        flipToOrientation() method and the additional force_memory_layout
        argument.

        Orientation is given as a 3 char string: "LPI" (the default orientation
        in AIMS), "RAS", and combinations of these 6 letters. See
        https://brainvisa.info/aimsdata/user_doc/coordinates_systems.html and
        http://www.grahamwideman.com/gw/brain/orientation/orientterms.htm.
    */
    void flipToOrientation( const std::string & orient );
    /** Flip the volume to a given orientation

        The volume voxels will be reordered to match the given orientation.
        Contrarily to the other flipToOrientation method, the voxels data block
        will be reallocated and flipped to match the given orientation. It is
        also given as a 3 char string, thus it may specify a different memory
        layout from the one used for indices.
    */
    void flipToOrientation( const std::string & orient,
                            const std::string & force_memory_layout );
    /// used by flipToOrientation(), reorient header information
    Object reorientedHeader( const std::string & orient ) const;
    /** determine the memory layout orientation from strides and current
        indices orientation.

        Use
        volume.referential().orientationStr(volume.memoryLayoutOrientation())
        to get a more readable string description.
    */
    std::vector<int> memoryLayoutOrientation() const;
    /** determine the storage (disk) layout orientation.

        The storage orientation is optionnally stored int the header
        "storage_to_memory" matrix. If not, the storage orientation is
        undefined.

        Use
        volume.referential().orientationStr(volume.storageLayoutOrientation())
        to get a more readable string description.
    */
    std::vector<int> storageLayoutOrientation() const;

  protected:
    //========================================================================
    //   PRIVATE UTILS
    //========================================================================
    void allocate( int oldSizeX, int oldSizeY, int oldSizeZ, int oldSizeT,
                   bool allocate, const AllocatorContext& allocatorContext,
                   const std::vector<long> *strides = 0 );
    void allocate( const std::vector<int> & oldSize,
                   bool allocate, const AllocatorContext& allocatorContext,
                   const std::vector<long> *strides = 0 );
    void slotSizeChanged( const PropertyFilter& propertyFilter );
    void updateItemsBuffer();

    void constructBorders( const Position & bordersize,
                           const AllocatorContext& allocatorContext,
                           bool allocated );

    AllocatedVector<T> _items;
    // in case of negative strides, _start adds an offset to _items 1st element
    T *_start;
    blitz::Array<T, Volume<T>::DIM_MAX>  _blitz;
    rc_ptr<Volume<T> >  _refvol;
    std::vector<int>    _pos;
    Referential _referential;
  };

//============================================================================
//   POSITION VECTOR
//============================================================================
  template <typename T>
  class Volume<T>::Position4Di
  {
  public:
    explicit Position4Di( int x = 0, int y = 0, int z = 0, int t = 0 )
      : _coords( 4 )
    {
      _coords[0] = x;
      _coords[1] = y;
      _coords[2] = z;
      _coords[3] = t;
    }

    /// Generic constructor from any "vector-like" object, i.e. that has
    /// a size() method and the [] accessor.
    ///
    /// It is not explicit to allow implicit conversion.
    template <typename U>
    Position4Di( const U & other ):
      _coords(4)
    {
      for( size_t i = 0; i < size_t( other.size() ) && i < 4; ++i )
        _coords[i] = other[i];
      for( size_t i = size_t( other.size() ); i < 4; ++i )
        _coords[i] = 0;
    }

    Position4Di( const Position4Di & pos ) : _coords( pos._coords )
    {
    }

    ~Position4Di() {}

          int & operator [] ( int coord )       { return _coords[ coord ]; }
    const int & operator [] ( int coord ) const { return _coords[ coord ]; }

    bool operator==(const Position4Di& p) const
    {
      return ( _coords == p._coords );
    }
    bool operator!=(const Position4Di& p) const
    { return !(this->operator ==(p)); }

    unsigned size() const { return _coords.size(); }
    const std::vector<int> & toVector() const
    { return _coords; }

    /* utility functions, not really related, but which we don't really know
       where to put them */
    static std::vector<int> fixed_position( const std::vector<int> & vec )
    {
      if( vec.size() >= 4 )
        return vec;
      std::vector<int> fixed( vec );
      fixed.resize( 4 );
      for( int i=vec.size(); i<4; ++i )
        fixed[i] = 0;
      return fixed;
    }

    static std::vector<int> fixed_size( const std::vector<int> & vec )
    {
      if( vec.size() >= 4 )
        return vec;
      std::vector<int> fixed( vec );
      fixed.resize( 4 );
      for( int i=vec.size(); i<4; ++i )
        fixed[i] = 1;
      return fixed;
    }

    static unsigned long long size_num_elements( const std::vector<int> & vec )
    {
      int i, n = vec.size();
      unsigned long long num = 1;
      for( i=0; i<n; ++i )
        num *= vec[i];
      return num;
    }

  private:
    std::vector<int>  _coords;
  };

//============================================================================
//   UTILITIES
//============================================================================
#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

  template <typename T>
  class DataTypeCode<Volume<T> >
  {
  public:
    static std::string objectType()
    { return "CartoVolume"; }
    static std::string dataType()
    { return DataTypeCode<T>::dataType(); }
    static std::string name()
    {
      return std::string("carto_volume of ") + DataTypeCode< T >::name();
    }
  };

  template <typename T>
  class Creator<Volume<T> >
  {
  public:
    static Volume<T>* create( Object, const AllocatorContext &, Object );
    static void setup( Volume<T> &, Object, const AllocatorContext &, Object );
  };

#endif

//============================================================================
// STREAM
//============================================================================

#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

  // Warper for output streams that integrates parameters for volume
  // printing
  class VolumeOStream // : public std::ostream // is not really a stream
  {
  public:
    VolumeOStream( std::ostream & ostream );
    VolumeOStream( const VolumeOStream & other );
    ~VolumeOStream();

    std::ostream & ostream() const;

    const size_t & maxT() const;
    const size_t & maxZ() const;
    const size_t & maxY() const;
    const size_t & maxX() const;

    size_t & maxT();
    size_t & maxZ();
    size_t & maxY();
    size_t & maxX();

  private:
    size_t _maxT;
    size_t _maxZ;
    size_t _maxY;
    size_t _maxX;
    std::ostream & _ostream;
  };

  // Object returned by setMaxDim used to set Volume printing parameters
  class VolumeOStreamSetter
  {
  public:
    VolumeOStreamSetter();

    const size_t & maxT() const;
    const size_t & maxZ() const;
    const size_t & maxY() const;
    const size_t & maxX() const;

    size_t & maxT();
    size_t & maxZ();
    size_t & maxY();
    size_t & maxX();

  private:
    size_t _maxT;
    size_t _maxZ;
    size_t _maxY;
    size_t _maxX;
  };

  VolumeOStream operator<< ( std::ostream & out,
                             const VolumeOStreamSetter & setter );

  template <typename T>
  std::ostream & operator<< ( const VolumeOStream & out,
                              const Volume<T> & volume );

  // Method used to set Volume printing parameters
  VolumeOStreamSetter setMaxDim( size_t m );
  VolumeOStreamSetter setMaxDim( size_t mx, size_t my, size_t mz, size_t mt );

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
  /// carto::Volume<int16_t> vol( 10, 10, 10 );
  /// cout << setMaxDim( 10 ) << vol << endl;
  /// cout << setMaxDim( 5, 10, 1, 1 ) << vol << endl;
  /// \endcode
  /// setMaxDim can either receive 4 value (used in all directions) or
  /// 4 values (directions x y, z, t).
  template <typename T>
  std::ostream & operator<< ( std::ostream & out,
                              const Volume<T> & volume );


  /// Display information about volumes hierarchy
  template <typename T>
  void displayRefVolumes(const Volume<T> & vol);

} // namespace carto:


// the CT macro defined in blitz++ interferes with QColor::CT in Qt
#    ifdef CT
#      undef CT
#    endif

#endif // CARTODATA_VOLUME_VOLUMEBASE_H
