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

#ifndef CARTODATA_TRANSFORMATION_REFERENTIAL_H
#define CARTODATA_TRANSFORMATION_REFERENTIAL_H

#include <soma-io/transformation/affinetransformation3d_base.h>
#include <cartobase/smart/rcobject.h>


namespace carto
{

  /** Referential holds information about coordinates system and axes.

      It also provides utility methods to transform to other coordinates
      orientations.

      Orientation describes the coordinates system axes, what they represent
      and in which direction they go.

      The orientation description is either a vector of constants (Orientaton
      enum), or a string containing one letter per axis. Each letter or
      Orientation enum value describes the axis and its direction. It is based
      on the Left/Right, Anterior/Posterior, Superior/Inferior convention, see:

      http://www.grahamwideman.com/gw/brain/orientation/orientterms.htm

      For brain images, axes orientations are described relatively to the head:
      AP (anterior to posterior) or PA, LR (left to right) or RL, SI (superior
      to inferior) or IS. To summarize the 3 axes we sometimes use the 3
      initials of the directions axes are pointing to: RAS (X: to Right, Y: to
      Anterior, Z: to Superior), LAS, LPI, etc.

      This convention used in the neuroimaging community is valid for 3D
      images. Here we extend it to up to 10 dimensions, using letters T to Z,
      with uppercase for increasing order, and lowercase for decreasing order.
      Thus, T can hopefully correspond to "time". But be careful that letters
      X, Y, Z here do not correspond to the common "1st, 2nd and 3rd axes":
      here it is the meaning of axes.

      Orientation strings may be given as "incomplete" strings, and will in
      most cases be extended to the actual number of dimensions, using default
      axes to complement. Thus, for a 5D referential, "LPI" is the same sas
      "LPITU".
  */
  class Referential: RCObject
  {
  public:
    /** Axis orientation enum
    */
    enum Orientation
    {
      Undefined,
      L,
      R,
      A,
      P,
      S,
      I,
      T,
      t,
      U,
      u,
      V,
      v,
      W,
      w,
      X,
      x,
      Y,
      y,
      Z,
      z
    };

    /// Build a referential with the given number of dimensions
    Referential( unsigned ndim = 3 );
    Referential( const Referential & ref );
    Referential( Object ref );
    ~Referential();

    Referential & operator = ( const Referential & ref );

    /// number of axes (or space dimensions)
    unsigned order() const { return _orientation.size(); }
    /// update the order, keeping existing orientation information
    void ensureOrder( unsigned ndim );
    /// a referential has an unique identifier
    const std::string & uuid() const { return _uuid; }
    std::string & uuid() { return _uuid; }
    /// orientation of the referential, as a string of one letter per axis
    std::string orientationStr() const;
    /// extended orientation information according to the needed number of axes
    std::string orientationStr( const std::string & orient ) const;
    /// orientation of the referential, as a vector of constants. Each number
    /// corresponds to an Orientation enum value, but cast to int
    std::vector<int> axesOrientation() const { return _orientation; }
    bool isDirect() const;
    /** Build a transformation matrix to go from this referential to a given
        orientation.

        If allow_resize is false, the matrix will have exactly the order of the
        given orientation. If it it true, it will be extended to the order of
        the referential.

        The translation vector is used to build the last column of the
        transformation matrix: it is the origin shift needed when an axis is
        reversed. Typically it is the volume dimension (in mm, or the dimension
        -1 in voxels)
    */
    rc_ptr<Transformation>
    toOrientation(
      const std::string & orient,
      const std::vector<float> & transl = std::vector<float>(),
      bool allow_resize = false ) const;
    rc_ptr<Transformation>
    /// same as above but translation is given as ints
    toOrientation(
      const std::string & orient,
      const std::vector<int> & transl,
      bool allow_resize = false ) const
      {
        return toOrientation( orient,
                              std::vector<float>( transl.begin(),
                                                  transl.end() ),
                              allow_resize );
      }
    /** Build a transformation matrix to go from this referential to a given
        orientation.

        Same as above, but orientation is given as a vector of constants.
    */
    rc_ptr<Transformation>
    toOrientation(
      const std::vector<int> & orient,
      const std::vector<float> & transl = std::vector<float>() ) const;
    /** LPI oriented referential

        AIMS volumes are initially in LPI orientation. If flipped, the
        referential will change to a diferent one. The LPI referential is the
        original one before any flip is applied.
    */
    std::string lpiReferentialUuid() const
    {
      if( isLpiOriented() && _uuid != _lpi_uuid )
        _lpi_uuid = _uuid;
      return _lpi_uuid;
    }

    /// force a new UUID
    void setUuid( const std::string & uuid );
    /** set a new orientation for the referential.

        if allow_resize is false, the given orientation will decide the new
        axes number. If true, it will be updated to keep the older number of
        dimensions.

        No other information will be changed.
    */
    void setOrientation( const std::string & orient,
                         bool allow_resize = false );
    /// set a new orientation for the referential.
    void setOrientation( const std::vector<int> & orient );
    /// set the LPI original referential UUID
    void setLpiReferential( const std::string & lpi_uuid );

    /// referential header, may contain anything useful
    Object header() const { return _header; }
    void setHeader( const Object header );
    /// tells if the orientation is comatible with a 3D transformation
    /// (all axes after the 3th are the default ones)
    bool is3DOriented() const;
    /// tells if the given orientation is 3D compatible
    bool is3DOriented( const std::string & orient ) const;
    /// tells if the orientation is the default LPI one.
    bool isLpiOriented() const;
    /** target orientation for a given transform matrix.
        The matrix should be a flip matrix, not any affine transform.
    */
    std::vector<int> orientationFromTransform(
      const AffineTransformationBase & tr ) const;

    /// orientation string for a single axis orientation (1 char)
    static std::string orientationStr( Orientation orient );
    /// build the orientation string from a vector of constants
    static std::string orientationStr( const std::vector<int> & orient );
    /// orientation constant for a single orientation axis character
    static Orientation orientationCode( const std::string & orient );
    /** build an orientation vector from a string.

        If dim is given, the vector will be extended to this number of
        dimensions.
    */
    static std::vector<int> orientationVector( const std::string & orient,
                                               unsigned dim = 0 );
    /** Build one line of transformation matrix

        This rather internal function is used to build a transformation matrix.
        This function builds one line to get from a source axis to a
        destination one.
    */
    static void setAxisTransform( AffineTransformationBase & tr,
                                  int src_axis, int dst_axis, int inv_mult,
                                  const std::vector<float> & transl
                                  = std::vector<float>() );
    /// tells if the given orientation is 3D compatible
    static bool is3DOriented( const std::vector<int> & orient );
    /** Build a Referential object from a header.
        This is basically the same as the constructor Referential( Object )
        except that if except_if_undefined is set, and if the header does
        not specify an orientation, then an exception is thrown.
    */
    static Referential fromHeader( const Object header,
                                   bool except_if_undefined = false );

  private:
    std::string _uuid;
    mutable std::string _lpi_uuid;
    std::vector<int> _orientation;
    Object _header;
  };

}

#endif

