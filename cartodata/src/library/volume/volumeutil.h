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

#ifndef CARTODATA_VOLUME_VOLUMEUTIL_H
#define CARTODATA_VOLUME_VOLUMEUTIL_H

//--- cartodata --------------------------------------------------------------
#include <cartodata/volume/volumeref.h>
#include <cartodata/volume/functional.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/type/datatypetraits.h>
#include <cartobase/smart/rcptr.h>
#include <cartobase/containers/nditerator.h>
//--- std --------------------------------------------------------------------
#include <algorithm>
#include <exception>
//----------------------------------------------------------------------------

namespace carto
{

  //==========================================================================
  //
  //                            DECLARATIONS
  //
  //==========================================================================

  namespace volumeutil {

    //========================================================================
    //   Generic operators
    //========================================================================
    /// Used by the actual Volume and VolumeRef operators
    /// It allows to keep the loops in one place and to specialize for non
    /// arithmetic types

    /// test if two volumes sizes vectors actually represent the same
    /// size (extending missing dimensions to 1)
    inline bool sameSize( const std::vector<int> & s1,
                          const std::vector<int> & s2 );
    /// return the largest dimensions between s1 and s2
    inline std::vector<int> maxSize( const std::vector<int> & s1,
                                     const std::vector<int> & s2 );
    /// return the smallest dimensions between s1 and s2
    inline std::vector<int> minSize( const std::vector<int> & s1,
                                     const std::vector<int> & s2 );

    /// Apply a function to all the elements of a volume
    /// (create output version)
    /// @{
    template <typename T, typename UnaryFunction>
    Volume<typename UnaryFunction::result_type>
    apply( const Volume<T> & vol, UnaryFunction func );
    template <typename T, typename U, typename BinaryFunction>
    Volume<typename BinaryFunction::result_type>
    apply( const Volume<T> & vol1, const Volume<U> & vol2, BinaryFunction func );
    template <typename T, typename UnaryFunction>
    rc_ptr<Volume<typename UnaryFunction::result_type> >
    apply( const rc_ptr<Volume<T> > & vol, UnaryFunction func );
    template <typename T, typename U, typename BinaryFunction>
    rc_ptr<Volume<typename BinaryFunction::result_type> >
    apply( const rc_ptr<Volume<T> > & vol1, const Volume<U> & vol2, BinaryFunction func );

    /// Apply a function to all the elements of a volume
    /// (in place version)
    /// @{
    template <typename T, typename UnaryFunction>
    Volume<T> & selfApply( Volume<T> & vol, UnaryFunction func );
    template <typename T, typename U, typename BinaryFunction>
    Volume<T> & selfApply( Volume<T> & vol1, const Volume<U> & vol2, BinaryFunction func );
    template <typename T, typename UnaryFunction>
    rc_ptr<Volume<T> > & selfApply( rc_ptr<Volume<T> > & vol, UnaryFunction func );
    template <typename T, typename U, typename BinaryFunction>
    rc_ptr<Volume<T> > & selfApply( rc_ptr<Volume<T> > & vol1, const Volume<U> & vol2, BinaryFunction func );
    /// @}

    /// Apply a function to all the elements of a volume
    /// (already allocated output version)
    ///
    /// \note Loop usage is restricted to these 2 functions.
    /// Maybe a bit of multithreading could be done here ?
    /// @{
    template <typename T, typename OUTP, typename UnaryFunction>
    Volume<OUTP> &
    applyTowards( const Volume<T> & vol, Volume<OUTP> & dst, UnaryFunction func );
    template <typename T, typename OUTP, typename UnaryFunction>
    Volume<OUTP> &
    applyTowards( const T & cst, Volume<OUTP> & dst, UnaryFunction func );
    template <typename T, typename U, typename OUTP, typename BinaryFunction>
    Volume<OUTP> &
    applyTowards( const Volume<T> & vol1, const Volume<U> & vol2, Volume<OUTP> & dst, BinaryFunction func );
    template <typename T, typename U, typename OUTP, typename BinaryFunction>
    Volume<OUTP> &
    applyTowards( const Volume<T> & vol1, const U & cst2, Volume<OUTP> & dst, BinaryFunction func );
    /// @}

    /// Accumulation over a volume
    ///
    /// The result is equivalent to
    /// \code
    /// for value in vol:
    ///   initial = func( initial, value )
    /// return initial
    /// \endcode
    ///
    /// Examples:
    /// - Calling it with function \c plus performs a sum over the volume
    /// - Calling it with function \c select_min returns its minimum value
    /// @{
    template <typename OUTP, typename T, typename BinaryFunction>
    OUTP accumulate( const Volume<T> & vol, BinaryFunction func, OUTP initial );
    template <typename OUTP, typename T, typename BinaryFunction>
    OUTP accumulate( const rc_ptr<Volume<T> > & vol, BinaryFunction func, OUTP initial );
    /// @}

  } // namespace volumeutil

  //==========================================================================
  // Copy functions
  //==========================================================================

  /// Transfer data from an allocated volume to another
  /// Allocated sizes must be equal (same number of voxels)
  /// @{
  template <typename T>
  void transfer( const Volume<T> & src, Volume<T> & dst );
  template <typename T>
  void transfer( const rc_ptr<Volume<T> > & src, rc_ptr<Volume<T> > & dst );
  template <typename OUTP, typename INP>
  void transfer( const Volume<INP> & src, Volume<OUTP> & dst );
  template <typename OUTP, typename INP>
  void transfer( const rc_ptr<Volume<INP> > & src, rc_ptr<Volume<OUTP> > & dst );
  /// @}

  /// Performs a copy of the data (not only a reference copy)
  /// The whole view hierarchy is fully duplicated
  /// @{
  template <typename T>
  Volume<T> deepcopy( const Volume<T> & src,
                      const std::vector<int> & size = std::vector<int>() );
  template <typename T>
  rc_ptr<Volume<T> > deepcopy(
    const rc_ptr<Volume<T> > & src,
    const std::vector<int> & size = std::vector<int>() );
  template <typename OUTP, typename INP>
  Volume<OUTP> deepcopy( const Volume<INP> & src,
                         const std::vector<int> & size = std::vector<int>() );
  template <typename OUTP, typename INP>
  rc_ptr<Volume<OUTP> > deepcopy(
    const rc_ptr<Volume<INP> > & src,
    const std::vector<int> & size = std::vector<int>() );
  /// @}

  /// Performs a copy of the data (not only a reference copy)
  /// Only the data inside the view is copied. A potential view
  /// hierarchy would be lost.
  /// @{
  template <typename T>
  Volume<T> copy( const Volume<T> & src );
  template <typename T>
  rc_ptr<Volume<T> > copy( const rc_ptr<Volume<T> > & src );
  template <typename OUTP, typename INP>
  Volume<OUTP> copy( const Volume<INP> & src );
  template <typename OUTP, typename INP>
  rc_ptr<Volume<OUTP> > copy( const rc_ptr<Volume<INP> > & src );
  /// @}

  /// Performs a copy of the view structure without transfering the data.
  /// @{
  template <typename T>
  Volume<T> copyStructure( const Volume<T> & src );
  template <typename T>
  rc_ptr<Volume<T> > copyStructure( const rc_ptr<Volume<T> > & src );
  template <typename OUTP, typename INP>
  Volume<OUTP> copyStructure( const Volume<INP> & src );
  template <typename OUTP, typename INP>
  rc_ptr<Volume<OUTP> > copyStructure( const rc_ptr<Volume<INP> > & src );
  /// @}

  //==========================================================================
  // Accumulated values: Min/Max/Sum...
  //==========================================================================

  /// Returns the minimum value of the volume
  ///
  /// vol must be of size > 0
  /// @{
  template <typename T>
  T min( const Volume<T> & vol );
  template <typename T>
  T min( const rc_ptr<Volume<T> > & vol );
  /// @}

  /// Returns the maximum value of the volume
  ///
  /// vol must be of size > 0
  /// @{
  template <typename T>
  T max( const Volume<T> & vol );
  template <typename T>
  T max( const rc_ptr<Volume<T> > & vol );
  /// @}

  /// Returns the sum of the volume values
  /// @{
  template <typename T>
  typename DataTypeTraits<T>::LongType sum( const Volume<T> & vol );
  template <typename OUTP, typename T>
  OUTP sum( const Volume<T> & vol );
  template <typename T>
  typename DataTypeTraits<T>::LongType sum( const rc_ptr<Volume<T> > & vol );
  template <typename OUTP, typename T>
  OUTP sum( const rc_ptr<Volume<T> > & vol );
  /// @}

  /// Returns true if all values compare to true
  /// @{
  template <typename T>
  bool all( const Volume<T> & vol );
  template <typename T>
  bool all( const rc_ptr<Volume<T> > & vol );
  /// @}

  /// Returns true if at least one value compares to true
  /// @{
  template <typename T>
  bool any( const Volume<T> & vol );
  template <typename T>
  bool any( const rc_ptr<Volume<T> > & vol );
  /// @}

  //==========================================================================
  // Special operators
  //==========================================================================

  /// Find values contained in a set
  ///
  /// For each voxel, output value is true iff the value is contained in \c set
  ///
  /// \param set An iterable object with type const_iterator and methods
  ///            begin() and end()
  /// @{
  template <typename T, typename U>
  Volume<bool> valuesIn( const Volume<T> & volume, const U & set );
  template <typename T, typename U>
  rc_ptr<Volume<bool> > valuesIn( const rc_ptr<Volume<T> > & volume, const U & set );
  /// @}

  /// Find values not contained in a set
  ///
  /// For each voxel, output value is true iff the value is not contained in \c set
  ///
  /// \param set An iterable object with type const_iterator and methods
  ///            begin() and end()
  /// @{
  template <typename T, typename U>
  Volume<bool> valuesNotIn( const Volume<T> & volume, const U & set );
  template <typename T, typename U>
  rc_ptr<Volume<bool> > valuesNotIn( const rc_ptr<Volume<T> > & volume, const U & set );
  /// @}

  /// Conditionally fill a volume
  ///
  /// At each position \c pos of \c volume , sets the value \c value iff
  /// condition(pos) is true.
  /// @{
  template <typename T, typename U>
  void conditionalSet( Volume<T> & volume, const Volume<U> & condition, const T & value );
  template <typename T, typename U>
  void conditionalSet( rc_ptr<Volume<T> > & volume, const rc_ptr<Volume<U> > & condition, const T & value );
  /// @}

  /// Invert a volume between its min and max values.
  ///
  /// Each output value is equal to max - value + min
  /// @{
  template <typename T>
  Volume<T> invertMinMax( const Volume<T> & volume );
  template <typename T>
  rc_ptr<Volume<T> > invertMinMax( const rc_ptr<Volume<T> > & volume );
  /// @}

  /// matrix product
  template <typename T>
  VolumeRef<T> matrix_product( const Volume<T> & v1, const Volume<T> & v2 );
  /// matrix product
  template <typename T>
  VolumeRef<T> matrix_product( const rc_ptr<Volume<T> > & v1,
                               const rc_ptr<Volume<T> > & v2 );
  /// transpose
  template <typename T>
  VolumeRef<T> transpose( const Volume<T> & v );
  /** transpose.
  If copy is false (the default), we can return a shared volume with adapted
  strides.
  */
  template <typename T>
  VolumeRef<T> transpose( const rc_ptr<Volume<T> > & v, bool copy=false );


  /** Build a diagonal matrix from a line (1D) vector */
  template <typename T>
  inline
  rc_ptr<Volume<T> > diag( const rc_ptr<Volume<T> > & thing )
  {
    if( thing->getSizeT() != 1 || thing->getSizeZ() != 1
        || thing->getSizeY() != 1 )
      throw std::runtime_error("diag expects a 1D (Nx1x1x1) vector as input");

    int n = thing->getSizeX();
    // allocate the new thing
    rc_ptr<Volume<T> > m( new Volume<T>( n, n ) );
    // do the operations
    for( int x = 0; x < n; x++ )
      m->at( x, x ) = thing->at( x );

    return m;
  }


  template <typename T>
  inline
  rc_ptr<Volume<T> > undiag( const rc_ptr<Volume<T> > & thing )
  {
    if( thing->getSizeT() != 1 || thing->getSizeZ() != 1 )
      throw std::runtime_error(
        "undiag expects a 2D (NxMx1x1) matrix as input");

    int n = std::min( thing->getSizeX(), thing->getSizeY() );
    // allocate the new thing
    rc_ptr<Volume<T> > m( new Volume<T>( n, 1, 1, 1,
                                         AllocatorContext::fast() ) );
    // do the operations
    for( int x = 0; x < n; x++ )
      m->at( x, 0 ) = thing->at( x, x );

    return m;
  }

  //==========================================================================
  // Borders
  //==========================================================================

  /// Set border width
  ///
  /// If the border differs from top and bottom, new borders are allocated.
  ///
  /// \warning This might cause a lot of data copy, even if borders were
  /// existing and sufficient. To be more conservative, use setMinBorders
  /// (or create a view in the volume).
  /// @{
  template <typename T>
  void setBorders( Volume<T> & volume,
                   const typename Volume<T>::Position4Di & top,
                   const typename Volume<T>::Position4Di & bottom
                     = typename Volume<T>::Position4Di(-1, -1, -1, -1) );
  /// The actual pointed volume will see its structure change. Changes are
  /// thus shared with all the references to this volume.
  template <typename T>
  void setBorders( rc_ptr<Volume<T> > & volume,
                   const typename Volume<T>::Position4Di & top,
                   const typename Volume<T>::Position4Di & bottom
                     = typename Volume<T>::Position4Di(-1, -1, -1, -1) );
  /// @}

  /// Set border width
  ///
  /// If the border is less than top and bottom, new borders are allocated.
  ///
  /// \warning This might cause a lot of data copy. To be even more
  /// conservative, create a view in the volume.
  /// @{
  template <typename T>
  void setMinBorders( Volume<T> & volume,
                      const typename Volume<T>::Position4Di & top,
                      const typename Volume<T>::Position4Di & bottom
                        = typename Volume<T>::Position4Di(-1, -1, -1, -1) );
  /// The actual pointed volume will see its structure change. Changes are
  /// thus shared with all the references to this volume.
  template <typename T>
  void setMinBorders( rc_ptr<Volume<T> > & volume,
                      const typename Volume<T>::Position4Di & top,
                      const typename Volume<T>::Position4Di & bottom
                        = typename Volume<T>::Position4Di(-1, -1, -1, -1) );
  /// @}


  template<typename T>
  inline
  bool isSameVolumeSize( const Volume<T> & v1, const Volume<T> & v2 )
  {
    std::vector<int> dim1 = v1.getSize();
    std::vector<int> dim2 = v2.getSize();
    int i, n1 = dim1.size(), n2 = dim2.size(), n = std::min( n1, n2 );
    for( i=0; i<n; ++i )
      if( dim1[i] != dim2[i] )
        return false;
    for( ; i<n1; ++i )
      if( dim1[i] != 1 )
        return false;
    for( ; i<n2; ++i )
      if( dim2[i] != 1 )
        return false;

    return true;
  }


  template<typename T>
  inline
  bool isSameVolumeSize( const rc_ptr<Volume<T> > & v1,
                         const rc_ptr<Volume<T> > & v2 )
  {
    return isSameVolumeSize( *v1, *v2 );
  }


  //==========================================================================
  //
  //                            DEFINITIONS
  //
  //==========================================================================

  namespace volumeutil {

    //========================================================================
    //   Generic operators
    //========================================================================

    bool sameSize( const std::vector<int> & s1, const std::vector<int> & s2 )
    {
      int s0 = s1.size();
      int i;
      if( s2.size() < s0 )
        s0 = s2.size();
      for( i=0; i<s0; ++i )
        if( s1[i] != s2[i] )
          return false;
      for( ; i<s1.size(); ++i )
        if( s1[i] != 1 )
          return false;
      for( ; i<s2.size(); ++i )
        if( s2[i] != 1 )
          return false;
      return true;
    }


    std::vector<int> maxSize( const std::vector<int> & s1,
                              const std::vector<int> & s2 )
    {
      std::vector<int> dim( std::max( s1.size(), s2.size() ), 1 );
      int s0 = s1.size();
      int i;
      if( s2.size() < s0 )
        s0 = s2.size();
      for( i=0; i<s0; ++i )
        dim[i] = std::max( s1[i], s2[i] );
      for( ; i<s1.size(); ++i )
        dim[i] = s1[i];
      for( ; i<s2.size(); ++i )
        dim[i] = s2[i];
      return dim;
    }


    std::vector<int> minSize( const std::vector<int> & s1,
                              const std::vector<int> & s2 )
    {
      std::vector<int> dim( std::min( s1.size(), s2.size() ), 1 );
      int s0 = s1.size();
      int i;
      if( s2.size() < s0 )
        s0 = s2.size();
      for( i=0; i<s0; ++i )
        dim[i] = std::min( s1[i], s2[i] );
      for( ; i<dim.size(); ++i )
        dim[i] = 1;
      return dim;
    }


    //--- Volume [op] other --------------------------------------------------

    template <typename T, typename UnaryFunction>
    inline
    Volume<typename UnaryFunction::result_type>
    apply( const Volume<T> & vol, UnaryFunction func )
    {
      typedef typename UnaryFunction::result_type OUTP;
      Volume<OUTP> output = deepcopy<OUTP,T>(vol);
      applyTowards( vol, output, func );
      return output;
    }

    template <typename T, typename U, typename BinaryFunction>
    inline
    Volume<typename BinaryFunction::result_type>
    apply( const Volume<T> & vol1, const Volume<U> & vol2, BinaryFunction func )
    {
      typedef typename BinaryFunction::result_type OUTP;
      Volume<OUTP> output = deepcopy<OUTP,T>(
        vol1, maxSize( vol1.getSize(), vol2.getSize() ) );
      applyTowards( vol1, vol2, output, func );
      return output;
    }

    //--- VolumeRef [op]= other ----------------------------------------------

    template <typename T, typename UnaryFunction>
    inline
    rc_ptr<Volume<typename UnaryFunction::result_type> >
    apply( const rc_ptr<Volume<T> > & vol, UnaryFunction func )
    {
      typedef typename UnaryFunction::result_type OUTP;
      rc_ptr<Volume<OUTP> > output = deepcopy<OUTP,T>(vol);
      applyTowards( *vol, *output, func );
      return output;
    }

    template <typename T, typename U, typename BinaryFunction>
    inline
    rc_ptr<Volume<typename BinaryFunction::result_type> >
    apply( const rc_ptr<Volume<T> > & vol1, const Volume<U> & vol2, BinaryFunction func )
    {
      typedef typename BinaryFunction::result_type OUTP;
      rc_ptr<Volume<OUTP> > output = deepcopy<OUTP,T>(
        vol1, maxSize( vol1->getSize(), vol2.getSize()) );
      applyTowards( *vol1, vol2, *output, func );
      return output;
    }

    //--- Volume [op]= other -------------------------------------------------

    template <typename T, typename UnaryFunction>
    inline
    Volume<T> & selfApply( Volume<T> & vol, UnaryFunction func )
    {
      applyTowards( vol, vol, func );
      return vol;
    }

    template <typename T, typename U, typename BinaryFunction>
    inline
    Volume<T> & selfApply( Volume<T> & vol1, const Volume<U> & vol2, BinaryFunction func )
    {
      applyTowards( vol1, vol2, vol1, func );
      return vol1;
    }

    //--- VolumeRef [op]= other ----------------------------------------------

    template <typename T, typename UnaryFunction>
    inline
    rc_ptr<Volume<T> > & selfApply( rc_ptr<Volume<T> > & vol, UnaryFunction func )
    {
      selfApply( *vol, func );
      return vol;
    }

    template <typename T, typename U, typename BinaryFunction>
    inline
    rc_ptr<Volume<T> > & selfApply( rc_ptr<Volume<T> > & vol1, const Volume<U> & vol2, BinaryFunction func )
    {
      selfApply( *vol1, vol2, func );
      return vol1;
    }

    //--- Volume [op] other -> Output  ---------------------------------------
    template <typename T, typename OUTP, typename UnaryFunction>
    inline
    Volume<OUTP> &
    applyTowards( const Volume<T> & vol, Volume<OUTP> & dst,
                  UnaryFunction func )
    {
      std::vector<int> size = minSize( vol.getSize(), dst.getSize() );
      if( !sameSize( size, vol.getSize() )
          || !sameSize( size, dst.getSize() ) )
      {
        // build views
        carto::rc_ptr<Volume<T> > invvol(
          const_cast<carto::Volume<T> *>( &vol ) );
        std::vector<int> pos( size.size(), 0 );
        carto::VolumeRef<T> invol( invvol, pos, size );
        carto::rc_ptr<Volume<OUTP> > outvvol( &dst );
        carto::VolumeRef<OUTP> outvol( outvvol, pos, size );
        // applyTowards on views of the same size
        applyTowards( *invol, *outvol, func );
        // release views and restore refcount on initial volumes
        invol.reset( 0 );
        invvol.release();
        outvol.reset( 0 );
        outvvol.release();
        // FIXME TODO: handle parts outside of the intersection
        return dst;
      }
      carto::const_line_NDIterator<T> it( &vol.at( 0 ), vol.getSize(),
                                          vol.getStrides() );
      carto::line_NDIterator<OUTP> dit( &dst.at( 0 ), dst.getSize(),
                                        dst.getStrides() );
      const T *p, *pp;
      OUTP *d;
      for( ; !it.ended(); ++it, ++dit )
      {
        p = &*it;
        d = &*dit;
        for( pp=p + it.line_length(); p!=pp;
             it.inc_line_ptr( p ), dit.inc_line_ptr( d ) )
          *d = func( *p );
      }
      return dst;
    }

    template <typename T, typename OUTP, typename UnaryFunction>
    inline
    Volume<OUTP> &
    applyTowards( const T & cst, Volume<OUTP> & dst,
                  UnaryFunction func )
    {
      carto::line_NDIterator<OUTP> dit( &dst.at( 0 ), dst.getSize(),
                                        dst.getStrides() );
      OUTP *d, *dd;
      OUTP value = func( cst );
      for( ; !dit.ended(); ++dit )
      {
        d = &*dit;
        for( dd=d + dit.line_length(); d!=dd; dit.inc_line_ptr( d ) )
          *d = value;
      }
      return dst;
    }

    template <typename T, typename U, typename OUTP, typename BinaryFunction>
    inline
    Volume<OUTP> &
    applyTowards( const Volume<T> & vol1, const Volume<U> & vol2,
                  Volume<OUTP> & dst, BinaryFunction func )
    {
      std::vector<int> sizei = minSize( vol1.getSize(), vol2.getSize() );
      std::vector<int> size = minSize( sizei, dst.getSize() );
      if( !sameSize( size, vol1.getSize() )
          || !sameSize( size, vol2.getSize() )
          || !sameSize( size, dst.getSize() ) )
      {
        // build views
        carto::rc_ptr<Volume<T> > invvol1(
          const_cast<carto::Volume<T> *>( &vol1 ) );
        std::vector<int> pos( size.size(), 0 );
        carto::VolumeRef<T> invol1( invvol1, pos, size );
        carto::rc_ptr<Volume<T> > invvol2(
          const_cast<carto::Volume<T> *>( &vol2 ) );
        carto::VolumeRef<T> invol2( invvol2, pos, size );
        carto::rc_ptr<Volume<OUTP> > outvvol( &dst );
        carto::VolumeRef<OUTP> outvol( outvvol, pos, size );
        // applyTowards on views of the same size
        applyTowards( *invol1, *invol2, *outvol, func );
        // release views and restore refcount on initial volumes
        invol1.reset( 0 );
        invvol1.release();
        invol2.reset( 0 );
        invvol2.release();
        outvol.reset( 0 );
        outvvol.release();
        // FIXME TODO: handle parts outside of the intersection
        return dst;
      }

      carto::const_line_NDIterator<T> it1( &vol1.at( 0 ), vol1.getSize(),
                                           vol1.getStrides() );
      carto::const_line_NDIterator<U> it2( &vol2.at( 0 ), vol2.getSize(),
                                           vol2.getStrides() );
      carto::line_NDIterator<OUTP> dit( &dst.at( 0 ), dst.getSize(),
                                        dst.getStrides() );
      const T *p, *pp;
      const U *p2;
      OUTP *d;
      for( ; !it1.ended(); ++it1, ++it2, ++dit )
      {
        p = &*it1;
        p2 = &*it2;
        d = &*dit;
        for( pp=p + it1.line_length(); p!=pp;
             it1.inc_line_ptr( p ), it2.inc_line_ptr( p2 ),
             dit.inc_line_ptr( d ) )
          *d = func( *p, *p2 );
      }
      return dst;
    }

    template <typename T, typename U, typename OUTP, typename BinaryFunction>
    inline
    Volume<OUTP> &
    applyTowards( const T & cst1, const Volume<U> & vol2,
                  Volume<OUTP> & dst, BinaryFunction func )
    {
      std::vector<int> size = minSize( vol2.getSize(), dst.getSize() );
      if( !sameSize( size, vol2.getSize() )
          || !sameSize( size, dst.getSize() ) )
      {
        // build views
        std::vector<int> pos( size.size(), 0 );
        carto::rc_ptr<Volume<T> > invvol2(
          const_cast<carto::Volume<T> *>( &vol2 ) );
        carto::VolumeRef<T> invol2( invvol2, pos, size );
        carto::rc_ptr<Volume<OUTP> > outvvol( &dst );
        carto::VolumeRef<OUTP> outvol( outvvol, pos, size );
        // applyTowards on views of the same size
        applyTowards( cst1, *invol2, *outvol, func );
        // release views and restore refcount on initial volumes
        invol2.reset( 0 );
        invvol2.release();
        outvol.reset( 0 );
        outvvol.release();
        // FIXME TODO: handle parts outside of the intersection
        return dst;
      }

      carto::const_line_NDIterator<U> it2( &vol2.at( 0 ), vol2.getSize(),
                                           vol2.getStrides() );
      carto::line_NDIterator<OUTP> dit( &dst.at( 0 ), dst.getSize(),
                                        dst.getStrides() );
      const U *p2, *pp;
      OUTP *d;
      for( ; !it2.ended(); ++it2, ++dit )
      {
        p2 = &*it2;
        d = &*dit;
        for( pp=p2+ it2.line_length(); p2!=pp;
             it2.inc_line_ptr( p2 ), dit.inc_line_ptr( d ) )
          *d = func( cst1, *p2 );
      }
      return dst;
    }

    template <typename T, typename U, typename OUTP, typename BinaryFunction>
    inline
    Volume<OUTP> &
    applyTowards( const Volume<T> & vol1, const U & cst2,
                  Volume<OUTP> & dst, BinaryFunction func )
    {
      std::vector<int> size = minSize( vol1.getSize(), dst.getSize() );
      if( !sameSize( size, vol1.getSize() )
          || !sameSize( size, dst.getSize() ) )
      {
        // build views
        carto::rc_ptr<Volume<T> > invvol1(
          const_cast<carto::Volume<T> *>( &vol1 ) );
        std::vector<int> pos( size.size(), 0 );
        carto::VolumeRef<T> invol1( invvol1, pos, size );
        carto::rc_ptr<Volume<OUTP> > outvvol( &dst );
        carto::VolumeRef<OUTP> outvol( outvvol, pos, size );
        // applyTowards on views of the same size
        applyTowards( *invol1, cst2, *outvol, func );
        // release views and restore refcount on initial volumes
        invol1.reset( 0 );
        invvol1.release();
        outvol.reset( 0 );
        outvvol.release();
        // FIXME TODO: handle parts outside of the intersection
        return dst;
      }

      carto::const_line_NDIterator<T> it1( &vol1.at( 0 ), vol1.getSize(),
                                           vol1.getStrides() );
      carto::line_NDIterator<OUTP> dit( &dst.at( 0 ), dst.getSize(),
                                        dst.getStrides() );
      const T *p, *pp;
      OUTP *d;
      for( ; !it1.ended(); ++it1, ++dit )
      {
        p = &*it1;
        d = &*dit;
        for( pp=p + it1.line_length(); p!=pp;
             it1.inc_line_ptr( p ), dit.inc_line_ptr( d ) )
          *d = func( *p, cst2 );
      }
      return dst;
    }

    //--- Volume: accumulate -------------------------------------------------

    template <typename OUTP, typename T, typename BinaryFunction>
    inline
    OUTP accumulate( const Volume<T> & vol, BinaryFunction func, OUTP initial )
    {
      carto::const_line_NDIterator<T> it( &vol.at( 0 ), vol.getSize(),
                                          vol.getStrides() );
      const T *p, *pp;
      for( ; !it.ended(); ++it )
      {
        p = &*it;
        for( pp=p + it.line_length(); p!=pp; it.inc_line_ptr( p ) )
          initial = func( initial, *p );
      }
      return initial;
    }

    //--- VolumeRef: accumulate ----------------------------------------------

    template <typename OUTP, typename T, typename BinaryFunction>
    inline
    OUTP accumulate( const rc_ptr<Volume<T> > & vol, BinaryFunction func, OUTP initial )
    {
      return accumulate<OUTP, T, BinaryFunction>( *vol, func, initial );
    }

  } // namespace volumeutil

  //==========================================================================
  // Copy functions
  //==========================================================================

  //--- transfer -------------------------------------------------------------

  template <typename T>
  inline
  void transfer( const Volume<T> & src, Volume<T> & dst )
  {
    transfer<T,T>( src, dst );
  }

  template <typename T>
  inline
  void transfer( const rc_ptr<Volume<T> > & src, rc_ptr<Volume<T> > & dst )
  {
    transfer<T,T>( src, dst );
  }

  template <typename OUTP, typename INP>
  inline
  void transfer( const Volume<INP> & src, Volume<OUTP> & dst )
  {
    volumeutil::applyTowards( src, dst, volumeutil::identity<INP>() );
  }

  template <typename OUTP, typename INP>
  inline
  void transfer( const rc_ptr<Volume<INP> > & src, rc_ptr<Volume<OUTP> > & dst )
  {
    transfer( *src, *dst );
  }

  //--- deepcopy -------------------------------------------------------------

  template <typename T>
  inline
  Volume<T> deepcopy( const Volume<T> & src, const std::vector<int> & size )
  {
    return deepcopy<T,T>( src, size );
  }

  template <typename T>
  inline
  rc_ptr<Volume<T> > deepcopy( const rc_ptr<Volume<T> > & src,
                               const std::vector<int> & size )
  {
    return deepcopy<T,T>( src, size );
  }

  template <typename OUTP, typename INP>
  inline
  Volume<OUTP> deepcopy( const Volume<INP> & src,
                         const std::vector<int> & size )
  {
    std::vector<int> nsize = size;
    if( nsize.empty() )
      nsize = src.getSize();
    Volume<OUTP> dst( nsize,
                      src.allocatorContext(),
                      src.allocatorContext().isAllocated() );
    dst.copyHeaderFrom( src.header() );
    if( src.allocatorContext().isAllocated()
        && src.allocatorContext().allocatorType()
          != AllocatorStrategy::Unallocated )
      transfer( src, dst );

    if( src.refVolume() )
    {
      rc_ptr<Volume<INP> > srcparent = src.refVolume();
      rc_ptr<Volume<OUTP> > dstparent( new Volume<OUTP>(
          srcparent->getSize(),
          srcparent->allocatorContext(),
          srcparent->allocatorContext().isAllocated() ) );
      if( srcparent->allocatorContext().isAllocated()
          && srcparent->allocatorContext().allocatorType()
          != AllocatorStrategy::Unallocated )
        transfer( srcparent, dstparent );
      dst.setRefVolume( dstparent );
      dst.setPosInRefVolume( src.posInRefVolume() );
    }

    rc_ptr<Volume<INP> > srcchild = src.refVolume();
    rc_ptr<Volume<OUTP> > dstchild = dst.refVolume();

    while( srcchild->refVolume().get() )
    {
      rc_ptr<Volume<INP> > srcparent = srcchild->refVolume();
      rc_ptr<Volume<OUTP> > dstparent( new Volume<OUTP>(
          srcparent->getSize(),
          srcparent->allocatorContext(),
          srcparent->allocatorContext().isAllocated() ) );
      dstparent->copyHeaderFrom( srcparent->header() );
      if( srcparent->allocatorContext().isAllocated()
          && srcparent->allocatorContext().allocatorType()
            != AllocatorStrategy::Unallocated )
        transfer( srcparent, dstparent );
      dstchild->setRefVolume( dstparent );
      dstchild->setPosInRefVolume( srcchild->posInRefVolume() );
      srcchild = srcparent;
      dstchild = dstparent;
    }

    return dst;
  }

  template <typename OUTP, typename INP>
  inline
  rc_ptr<Volume<OUTP> > deepcopy( const rc_ptr<Volume<INP> > & src,
                                  const std::vector<int> & size )
  {
    std::vector<int> nsize = size;
    if( nsize.empty() )
      nsize = src->getSize();
    rc_ptr<Volume<OUTP> > dst( new Volume<OUTP>(
        nsize,
        src->allocatorContext(),
        src->allocatorContext().isAllocated() ) );
    dst->copyHeaderFrom( src->header() );
    if( src->allocatorContext().isAllocated()
        && src->allocatorContext().allocatorType()
          != AllocatorStrategy::Unallocated )
      transfer( src, dst );

    rc_ptr<Volume<INP> > srcchild = src;
    rc_ptr<Volume<OUTP> > dstchild = dst;

    while( srcchild->refVolume().get() )
    {
      rc_ptr<Volume<INP> > srcparent = srcchild->refVolume();
      rc_ptr<Volume<OUTP> > dstparent( new Volume<OUTP>(
          srcparent->getSize(),
          srcparent->allocatorContext(),
          srcparent->allocatorContext().isAllocated() ) );
      dstparent->copyHeaderFrom( srcparent->header() );
      if( srcparent->allocatorContext().isAllocated()
          && srcparent->allocatorContext().allocatorType()
          != AllocatorStrategy::Unallocated )
        transfer( srcparent, dstparent );
      dstchild->setRefVolume( dstparent );
      dstchild->setPosInRefVolume( srcchild->posInRefVolume() );
      srcchild = srcparent;
      dstchild = dstparent;
    }

    return dst;
  }


  //--- copy -----------------------------------------------------------------

  template <typename T>
  inline
  Volume<T> copy( const Volume<T> & src )
  {
    return copy<T,T>( src );
  }

  template <typename T>
  inline
  rc_ptr<Volume<T> > copy( const rc_ptr<Volume<T> > & src )
  {
    return copy<T,T>( src );
  }

  template <typename OUTP, typename INP>
  inline
  Volume<OUTP> copy( const Volume<INP> & src )
  {
    Volume<OUTP> dst( src.getSize() );
    dst.copyHeaderFrom( src.header() );
    transfer( src, dst );
    return dst;
  }

  template <typename OUTP, typename INP>
  inline
  rc_ptr<Volume<OUTP> > copy( const rc_ptr<Volume<INP> > & src )
  {
    rc_ptr<Volume<OUTP> > dst( new Volume<OUTP>( src->getSize() ) );
    dst->copyHeaderFrom( src->header() );
    transfer( src, dst );
    return dst;
  }


  //--- copyStructure --------------------------------------------------------

  template <typename T>
  inline
  Volume<T> copyStructure( const Volume<T> & src )
  {
    return copyStructure<T,T>( src );
  }

  template <typename T>
  inline
  rc_ptr<Volume<T> > copyStructure( const rc_ptr<Volume<T> > & src )
  {
    return copyStructure<T,T>( src );
  }

  template <typename OUTP, typename INP>
  inline
  Volume<OUTP> copyStructure( const Volume<INP> & src )
  {
    Volume<OUTP> dst( src.getSize(),
                      src.allocatorContext(),
                      src.allocatorContext().isAllocated() );
    dst.copyHeaderFrom( src.header() );
    // if( src.allocatorContext().isAllocated() )
    //   dst.fill(0);

    if( src.refVolume() )
    {
      rc_ptr<Volume<INP> > srcparent = src.refVolume();
      rc_ptr<Volume<OUTP> > dstparent( new Volume<OUTP>(
          srcparent->getSize(),
          srcparent->allocatorContext(),
          srcparent->allocatorContext().isAllocated() ) );
      // if( srcparent->allocatorContext().isAllocated() )
      //   dstparent->fill(0);
      dst.setRefVolume( dstparent );
      dst.setPosInRefVolume( src.posInRefVolume() );

      rc_ptr<Volume<INP> > srcchild = src.refVolume();
      rc_ptr<Volume<OUTP> > dstchild = dst.refVolume();

      while( srcchild->refVolume().get() )
      {
        rc_ptr<Volume<INP> > srcparent = srcchild->refVolume();
        rc_ptr<Volume<OUTP> > dstparent( new Volume<OUTP>(
            srcparent->getSize(),
            srcparent->allocatorContext(),
            srcparent->allocatorContext().isAllocated() ) );
        dstparent->copyHeaderFrom( srcparent->header() );
        // if( srcparent->allocatorContext().isAllocated() )
        //   dstparent->fill(0);
        dstchild->setRefVolume( dstparent );
        dstchild->setPosInRefVolume( srcchild->posInRefVolume() );
        srcchild = srcparent;
        dstchild = dstparent;
      }

    }

    return dst;
  }

  template <typename OUTP, typename INP>
  inline
  rc_ptr<Volume<OUTP> > copyStructure( const rc_ptr<Volume<INP> > & src )
  {
    if( !src.get() )
      return rc_ptr<Volume<OUTP> >( (Volume<OUTP>*)0 );

    rc_ptr<Volume<OUTP> > dst( new Volume<OUTP>(
        src->getSize(),
        src->allocatorContext(),
        src->allocatorContext().isAllocated() ) );
    dst->copyHeaderFrom( src->header() );
    // if( src->allocatorContext().isAllocated() )
    //   dst->fill(0);

    rc_ptr<Volume<INP> > srcchild = src;
    rc_ptr<Volume<OUTP> > dstchild = dst;

    while( srcchild->refVolume().get() )
    {
      rc_ptr<Volume<INP> > srcparent = srcchild->refVolume();
      rc_ptr<Volume<OUTP> > dstparent( new Volume<OUTP>(
          srcparent->getSize(),
          srcparent->allocatorContext(),
          srcparent->allocatorContext().isAllocated() ) );
      dstparent->copyHeaderFrom( srcparent->header() );
      // if( srcparent->allocatorContext().isAllocated() )
      //   dstparent->fill(0);
      dstchild->setRefVolume( dstparent );
      dstchild->setPosInRefVolume( srcchild->posInRefVolume() );
      srcchild = srcparent;
      dstchild = dstparent;
    }

    return dst;
  }


  //==========================================================================
  // Min/Max/Sum/Any/All
  //==========================================================================

  template <typename T>
  inline
  T min( const Volume<T> & vol )
  {
    if( vol.getSizeX() == 0 && vol.getSizeY() == 0 &&
        vol.getSizeZ() == 0 && vol.getSizeT() == 0 )
      throw std::runtime_error("Cannot compute min of an empty volume");
    return accumulate( vol, volumeutil::select_min<T>(), vol.at(0, 0, 0, 0) );
  }

  template <typename T>
  inline
  T min( const rc_ptr<Volume<T> > & vol )
  {
    if( !vol.get() || ( vol->getSizeX() == 0 && vol->getSizeY() == 0 &&
                        vol->getSizeZ() == 0 && vol->getSizeT() == 0 ) )
      throw std::runtime_error("Cannot compute min of an empty volume");
    return accumulate( vol, volumeutil::select_min<T>(), vol->at(0, 0, 0, 0) );
  }

  template <typename T>
  inline
  T max( const Volume<T> & vol )
  {
    if( vol.getSizeX() == 0 && vol.getSizeY() == 0 &&
        vol.getSizeZ() == 0 && vol.getSizeT() == 0 )
      throw std::runtime_error("Cannot compute max of an empty volume");
    return accumulate( vol, volumeutil::select_max<T>(), vol.at(0, 0, 0, 0) );
  }

  template <typename T>
  inline
  T max( const rc_ptr<Volume<T> > & vol )
  {
    if( !vol.get() || ( vol->getSizeX() == 0 && vol->getSizeY() == 0 &&
                        vol->getSizeZ() == 0 && vol->getSizeT() == 0 ) )
      throw std::runtime_error("Cannot compute max of an empty volume");
    return accumulate( vol, volumeutil::select_max<T>(), vol->at(0, 0, 0, 0) );
  }

  template <typename T>
  inline
  typename DataTypeTraits<T>::LongType sum( const Volume<T> & vol )
  {
    return sum<typename DataTypeTraits<T>::LongType,T>( vol );
  }


  template <typename OUTP, typename T>
  inline
  OUTP sum( const Volume<T> & vol )
  {
    return accumulate( vol, volumeutil::plus<OUTP,T>(), static_cast<OUTP>(0) );
  }

  template <typename T>
  inline
  typename DataTypeTraits<T>::LongType sum( const rc_ptr<Volume<T> > & vol )
  {
    return sum<typename DataTypeTraits<T>::LongType,T>( vol );
  }

  template <typename OUTP, typename T>
  inline
  OUTP sum( const rc_ptr<Volume<T> > & vol )
  {
    if( !vol.get() )
      return static_cast<OUTP>(0);
    return accumulate( vol, volumeutil::plus<OUTP,T>(), static_cast<OUTP>(0) );
  }

  template <typename T>
  inline
  bool all( const Volume<T> & vol )
  {
    return volumeutil::accumulate( vol, volumeutil::logical_and<bool,T>(), true );
  }

  template <typename T>
  inline
  bool all( const rc_ptr<Volume<T> > & vol )
  {
    return volumeutil::accumulate( *vol, volumeutil::logical_and<bool,T>(), true );
  }

  template <typename T>
  inline
  bool any( const Volume<T> & vol )
  {
    return volumeutil::accumulate( vol, volumeutil::logical_or<bool,T>(), false );
  }

  template <typename T>
  inline
  bool any( const rc_ptr<Volume<T> > & vol )
  {
    return volumeutil::accumulate( *vol, volumeutil::logical_or<bool,T>(), false );
  }

  //==========================================================================
  // Special operators
  //==========================================================================

  namespace internal {
    template <typename T, typename U>
    struct inSet: public std::binary_function<T, U, bool>
    {
      bool operator() ( const T & x, const U & y )
      {
        for( typename U::const_iterator k = y.begin(); k != y.end(); ++k )
          if( x == *k )
            return true;
        return false;
      }
    };
  } // namespace internal

  template <typename T, typename U>
  inline
  Volume<bool> valuesIn( const Volume<T> & volume, const U & set )
  {
    Volume<bool> output = copyStructure<bool, T>( volume );
    volumeutil::applyTowards( volume, output, std::bind2nd( internal::inSet<T,U>(), set ) );
    return output;
  }

  template <typename T, typename U>
  inline
  rc_ptr<Volume<bool> > valuesIn( const rc_ptr<Volume<T> > & volume, const U & set )
  {
    rc_ptr<Volume<bool> > output = copyStructure<bool, T>( volume );
    volumeutil::applyTowards( *volume, *output, std::bind2nd( internal::inSet<T,U>(), set ) );
    return output;
  }

  namespace internal {
    template <typename T, typename U>
    struct notInSet: public std::binary_function<T, U, bool>
    {
      bool operator() ( const T & x, const U & y )
      {
        for( typename U::const_iterator k = y.begin(); k != y.end(); ++k )
          if( x == *k )
            return false;
        return true;
      }
    };
  } // namespace internal

  template <typename T, typename U>
  inline
  Volume<bool> valuesNotIn( const Volume<T> & volume, const U & set )
  {
    Volume<bool> output = copyStructure<bool, T>( volume );
    volumeutil::applyTowards( volume, output, std::bind2nd( internal::notInSet<T,U>(), set ) );
    return output;
  }

  template <typename T, typename U>
  inline
  rc_ptr<Volume<bool> > valuesNotIn( const rc_ptr<Volume<T> > & volume, const U & set )
  {
    rc_ptr<Volume<bool> > output = copyStructure<bool, T>( volume );
    volumeutil::applyTowards( *volume, *output, std::bind2nd( internal::notInSet<T,U>(), set ) );
    return output;
  }

  namespace internal {
    template <typename T, typename U>
    struct changeIf: public std::binary_function<T, U, T>
    {
      changeIf( const T & value ): _value(value) {}
      bool operator() ( const T & x, const U & y )
      {
        return y ? _value : x;
      }
    private:
      T _value;
    };
  } // namespace internal

  template <typename T, typename U>
  inline
  void conditionalSet( Volume<T> & volume, const Volume<U> & condition, const T & value )
  {
    volumeutil::selfApply( volume, condition, internal::changeIf<T,U>(value) );
  }

  template <typename T, typename U>
  inline
  void conditionalSet( rc_ptr<Volume<T> > & volume, const rc_ptr<Volume<U> > & condition, const T & value )
  {
    volumeutil::selfApply( volume, *condition, internal::changeIf<T,U>(value) );
  }

  namespace internal {
    template <typename T>
    struct invMinMax: public std::unary_function<T, T>
    {
      invMinMax( const T & min, const T & max ):
        _min(min),
        _max(max)
      {}

      T operator() ( const T & x )
      {
        return max - x + min;
      }
    private:
      T _min;
      T _max;
    };
  } // namespace internal

  template <typename T>
  inline
  Volume<T> invertMinMax( const Volume<T> & volume )
  {
    T min = min( volume );
    T max = max( volume );
    volumeutil::apply( volume, internal::invMinMax<T>(min, max) );
  }

  template <typename T>
  inline
  rc_ptr<Volume<T> > invertMinMax( const rc_ptr<Volume<T> > & volume )
  {
    T min = min( volume );
    T max = max( volume );
    volumeutil::apply( volume, internal::invMinMax<T>(min, max) );
  }

  //==========================================================================
  // Borders
  //==========================================================================

  template <typename T>
  inline
  void setBorders( Volume<T> & volume,
                   const typename Volume<T>::Position4Di & top,
                   const typename Volume<T>::Position4Di & bottom )
  {
    const typename Volume<T>::Position4Di & bot =
      ( bottom == typename Volume<T>::Position4Di(-1, -1, -1, -1) ?
        top : bottom );
    std::vector<int> b = volume.getBorders();
    if( b[0] != top[0] || b[1] != bot[0] ||
        b[2] != top[1] || b[3] != bot[1] ||
        b[4] != top[2] || b[5] != bot[2] ||
        b[6] != top[3] || b[7] != bot[3] )
    {
      rc_ptr<Volume<T> > parent( new Volume<T>(
          volume.getSizeX() + top[0] + bot[0],
          volume.getSizeY() + top[1] + bot[1],
          volume.getSizeZ() + top[2] + bot[2],
          volume.getSizeT() + top[3] + bot[3] ) );
      parent->copyHeaderFrom( volume.header() );
      typename Volume<T>::Position4Di size( volume.getSizeX(),
                                            volume.getSizeY(),
                                            volume.getSizeZ(),
                                            volume.getSizeT() );
      Volume<T> view( parent, top, size );
      transfer( volume, view );
      volume.reallocate( volume.getSizeX(), volume.getSizeY(),
                         volume.getSizeZ(), volume.getSizeT(),
                         true, volume.allocatorContext(), false );
      volume.setRefVolume( parent );
      volume.setPosInRefVolume( top );
    }
  }

  template <typename T>
  inline
  void setBorders( rc_ptr<Volume<T> > & volume,
                   const typename Volume<T>::Position4Di & top,
                   const typename Volume<T>::Position4Di & bottom )
  {
    const typename Volume<T>::Position4Di & bot =
      ( bottom == typename Volume<T>::Position4Di(-1, -1, -1, -1) ?
        top : bottom );
    std::vector<int> b = volume->getBorders();
    if( b[0] != top[0] || b[1] != bot[0] ||
        b[2] != top[1] || b[3] != bot[1] ||
        b[4] != top[2] || b[5] != bot[2] ||
        b[6] != top[3] || b[7] != bot[3] )
    {
      rc_ptr<Volume<T> > parent( new Volume<T>(
          volume->getSizeX() + top[0] + bot[0],
          volume->getSizeY() + top[1] + bot[1],
          volume->getSizeZ() + top[2] + bot[2],
          volume->getSizeT() + top[3] + bot[3] ) );
      parent->copyHeaderFrom( volume->header() );
      typename Volume<T>::Position4Di size( volume->getSizeX(),
                                            volume->getSizeY(),
                                            volume->getSizeZ(),
                                            volume->getSizeT() );
      rc_ptr<Volume<T> > view( new Volume<T>( parent, top, size ) );
      transfer( volume, view );
      volume->reallocate( volume->getSizeX(), volume->getSizeY(),
                          volume->getSizeZ(), volume->getSizeT(),
                          true, volume->allocatorContext(), false );
      volume->setRefVolume( parent );
      volume->setPosInRefVolume( top );
    }
  }

  template <typename T>
  inline
  void setMinBorders( Volume<T> & volume,
                      const typename Volume<T>::Position4Di & top,
                      const typename Volume<T>::Position4Di & bottom )
  {
    const typename Volume<T>::Position4Di & bot =
      ( bottom == typename Volume<T>::Position4Di(-1, -1, -1, -1) ?
        top : bottom );
    std::vector<int> b = volume.getBorders();
    if( b[0] < top[0] || b[1] < bot[0] ||
        b[2] < top[1] || b[3] < bot[1] ||
        b[4] < top[2] || b[5] < bot[2] ||
        b[6] < top[3] || b[7] < bot[3] )
    {
      rc_ptr<Volume<T> > parent( new Volume<T>(
          volume.getSizeX() + top[0] + bot[0],
          volume.getSizeY() + top[1] + bot[1],
          volume.getSizeZ() + top[2] + bot[2],
          volume.getSizeT() + top[3] + bot[3] ) );
      parent->copyHeaderFrom( volume.header() );
      typename Volume<T>::Position4Di size( volume.getSizeX(),
                                            volume.getSizeY(),
                                            volume.getSizeZ(),
                                            volume.getSizeT() );
      Volume<T> view( parent, top, size );
      transfer( volume, view );
      volume.reallocate( volume.getSizeX(), volume.getSizeY(),
                         volume.getSizeZ(), volume.getSizeT(),
                         true, volume.allocatorContext(), false );
      volume.setRefVolume( parent );
      volume.setPosInRefVolume( top );
    }
  }

  template <typename T>
  inline
  void setMinBorders( rc_ptr<Volume<T> > & volume,
                      const typename Volume<T>::Position4Di & top,
                      const typename Volume<T>::Position4Di & bottom )
  {
    const typename Volume<T>::Position4Di & bot =
      ( bottom == typename Volume<T>::Position4Di(-1, -1, -1, -1) ?
        top : bottom );
    std::vector<int> b = volume->getBorders();
    if( b[0] < top[0] || b[1] < bot[0] ||
        b[2] < top[1] || b[3] < bot[1] ||
        b[4] < top[2] || b[5] < bot[2] ||
        b[6] < top[3] || b[7] < bot[3] )
    {
      rc_ptr<Volume<T> > parent( new Volume<T>(
          volume->getSizeX() + top[0] + bot[0],
          volume->getSizeY() + top[1] + bot[1],
          volume->getSizeZ() + top[2] + bot[2],
          volume->getSizeT() + top[3] + bot[3] ) );
      parent->copyHeaderFrom( volume->header() );
      typename Volume<T>::Position4Di size( volume->getSizeX(),
                                            volume->getSizeY(),
                                            volume->getSizeZ(),
                                            volume->getSizeT() );
      rc_ptr<Volume<T> > view( new Volume<T>( parent, top, size ) );
      transfer( volume, view );
      volume->reallocate( volume->getSizeX(), volume->getSizeY(),
                          volume->getSizeZ(), volume->getSizeT(),
                          true, volume->allocatorContext(), false );
      volume->setRefVolume( parent );
      volume->setPosInRefVolume( top );
    }
  }

  //==========================================================================
  //
  //                        OLD CLASSES / METHODS
  //
  //==========================================================================
  template <typename T> class VolumeRef;


  /** Utility functor.
      Converts a regular binary functor to a unary functor using a constant
      value as second argument.
      This class may move in another location later.
  */
  template <typename T, class BinaryFunction>
  class UnaryFromConstantBinaryFunctor
  {
  public:
    inline UnaryFromConstantBinaryFunctor( const T & x, BinaryFunction func )
      : value( x ), f( func ) {}
    inline T operator () ( const T & y ) const
    { return f( y, value ); }
    T     value;
    BinaryFunction  f;
  };


  /** Utility functor.
      Same as the fist form except that the constant term is the first
      argument or the binary functor.
  */
  template <typename T, class BinaryFunction>
  class UnaryFromConstantBinaryFunctor2
  {
  public:
    inline UnaryFromConstantBinaryFunctor2( const T & x, BinaryFunction func )
      : value( x ), f( func ) {}
    inline T operator () ( const T & y ) const
    { return f( value, y ); }
    T     value;
    BinaryFunction  f;
  };
  /** Scaler functor.
      Multiplies the argument with a constant (double) value.
      This class may move in another location later.
  */
  template <typename T, typename U>
  class Scaler
  {
  public:
    inline Scaler( U x ) : scale( x ) {}
    inline T operator () ( const T & x ) const
    { return (T) ( x * scale ); }
    U scale;
  };

  template<> inline cfloat
  Scaler<cfloat, double>::operator () ( const cfloat & x ) const
  {
    return x * (float) scale;
  }

  template<> inline cfloat
  Scaler<cfloat, long>::operator () ( const cfloat & x ) const
  {
    return x * (float) scale;
  }

  template<> inline cdouble
  Scaler<cdouble, float>::operator () ( const cdouble & x ) const
  {
    return x * (double) scale;
  }

  template<> inline cdouble
  Scaler<cdouble, long>::operator () ( const cdouble & x ) const
  {
    return x * (double) scale;
  }

  /** Divider functor.
      Divides the argument with a constant value.
      This class may move in another location later.
  */
  template <typename T, typename U>
  class Divider
  {
  public:
    inline Divider( U x ) : divisor( x ) {}
    inline T operator () ( const T & x ) const
    { return (T) ( x / divisor ); }
    U      divisor;
  };

  template<> inline cfloat
  Divider<cfloat, double>::operator () ( const cfloat & x ) const
  {
    return x * (float) ( 1. / divisor );
  }

  template<> inline cfloat
  Divider<cfloat, long>::operator () ( const cfloat & x ) const
  {
    return x * (float) ( 1. / (double) divisor );
  }

  template<> inline cdouble
  Divider<cdouble, double>::operator () ( const cdouble & x ) const
  {
    return x * ( 1. / divisor );
  }

  template<> inline cdouble
  Divider<cdouble, float>::operator () ( const cdouble & x ) const
  {
    return x * (double) ( 1. / divisor );
  }

  template<> inline cdouble
  Divider<cdouble, long>::operator () ( const cdouble & x ) const
  {
    return x * (double) ( 1. / divisor );
  }

  template <typename T, bool Scalar>
  class VolumeUtilBase {
  };

  /// Volume utilities base class for scalar types
  template <typename T>
  class VolumeUtilBase<T, true> {
    public :
      /** Get the minimum voxel value of a volume.
      */
      static T min( const Volume<T> & o );

      /** Get the maximum voxel value of a volume.
      */
      static T max( const Volume<T> & o );
  };

  /// Volume utilities base class for non scalar types
  template <typename T>
  class VolumeUtilBase<T, false> {
  };

  /// Volume utilities classes
  template <typename T>
  class VolumeUtil: public carto::VolumeUtilBase<T, carto::DataTypeTraits<T>::is_scalar>
  {
  public:
    /// applies a unary function to each voxel of a volume
    template <class UnaryFunction> static
    VolumeRef<T> apply( UnaryFunction f, const VolumeRef<T> & o );
    /// applies a binary function to each voxel of a pair of volumes
//     template <class BinaryFunction> static
//     VolumeRef<T> apply( BinaryFunction f, const VolumeRef<T> & o1,
//                         const VolumeRef<T> & o2 );
    /** same as apply() except that the input volume is used to store the
        result */
    template <class UnaryFunction> static
    void selfApply( UnaryFunction f, VolumeRef<T> & o );
    /** same as apply() except that the first input volume is used to store
        the result */
    template <class BinaryFunction> static
    void selfApply( BinaryFunction f, VolumeRef<T> & o1,
                    const VolumeRef<T> & o2 );
    /** Apply a binary function to each voxel of the volume, with a "current"
        value as second argument. The voxel result is used for other voxels
        function calls.
        Useful for min/max for instance:
        min = VolumeUtil<T>::accumulate( std::min<T>, volume,
          std::numeric_limits<T>::max() );
        max = VolumeUtil::accumulate( std::max<T>, volume,
          -std::numeric_limits<T>::max() );
        May be used also for sum etc.
    */
    template <class BinaryFunction> static
    T accumulate( BinaryFunction f, const VolumeRef<T> & o2, T initial )
    { return accumulate( f, *o2, initial ); }
    template <class BinaryFunction> static
    T accumulate( BinaryFunction f, const Volume<T> & o2, T initial );
  };


  template <typename T>
  inline
  void sort( Volume<T>& thing, bool ascending = true )
  {
    int i, j, n = thing.getSizeX();
    T tmp;

    if( ascending )
    {
      for( j=1; j<n; ++j )
      {
        tmp = thing.at( j );
        i = j - 1;
        while( i >= 0L && thing.at( i ) > tmp )
        {
          thing.at( i + 1 ) = thing.at( i );
          i--;
        }
        thing.at( i + 1 ) = tmp;
      }
    }
    else
    {
      for( j=1; j<n; ++j )
      {
        tmp = thing.at( j );
        i = j - 1;
        while( i >= 0L && thing.at( i ) < tmp )
        {
          thing.at( i + 1 ) = thing.at( i );
          i--;
        }
        thing.at( i + 1 ) = tmp;
      }
    }
  }


  template <typename T>
  inline
  void sort( rc_ptr<Volume<T> >& thing, bool ascending = true )
  {
    sort( *thing, ascending );
  }


} // namespace carto

#endif // CARTODATA_VOLUME_VOLUMEUTIL_H

