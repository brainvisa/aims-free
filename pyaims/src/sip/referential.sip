
namespace carto
{

  class Referential
  {
%TypeHeaderCode
#include <cartodata/transformation/referential.h>
%End

%Docstring
      Referential holds information about coordinates system and axes.

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
%End

  public:
    enum Orientation
    {
      Undefined,
      L,
      R,
      A,
      P,
      S,
      I
    };

    Referential( unsigned ndim = 3 );
%Docstring
    ref = Referential(ndim=3)

    Build a referential with the given number of dimensions
%End

    Referential( const carto::Referential & ref );
    Referential( carto::Object ref );
    ~Referential();

    unsigned order() const;
%Docstring
    number of axes (or space dimensions)
%End

    void ensureOrder( unsigned ndim );
%Docstring
    update the order, keeping existing orientation information
%End

    std::string uuid() const;
%Docstring
    a referential has an unique identifier
%End

    std::string orientationStr() const;
%Docstring
    * orient = referential.orientationStr()

    orientation of the referential, as a string of one letter per axis

    * orient = referential.orientationStr('LPI')

    extended orientation information according to the needed number of axes

    * orient = Referential.orientationStr(Referential.L)

    orientation string for a single axis orientation (1 char)

    * orient = Referential.orientationStr([1, 3, 6])

    build the orientation string from a vector of constants
%End

    std::string orientationStr( const std::string & orient ) const;
    vector_S32 axesOrientation() const;
%Docstring
    orientation of the referential, as a vector of constants. Each number
    corresponds to an Orientation enum value, but cast to int
%End

    rc_ptr_Transformation
    toOrientation( const std::string & orient, const vector_FLOAT & transl = vector_FLOAT(), bool allow_resize = false ) const;
%Docstring
    Build a transformation matrix to go from this referential to a given
    orientation.

    Parameters
    ----------
    orient: str or list
        destination orientation, given as a string or a vector of constants.
    allow_resize: bool
        If allow_resize is False, the matrix will have exactly the order of the
        given orientation. If it it True, it will be extended to the order of
        the referential.
    transl: list of float (optional)
        The translation vector is used to build the last column of the
        transformation matrix: it is the origin shift needed when an axis is
        reversed. Typically it is the volume dimension (in mm, or the dimension
        -1 in voxels)
%End

    rc_ptr_Transformation
    toOrientation( const vector_S32 & orient,
                   const vector_FLOAT & transl = vector_FLOAT() ) const;

    std::string lpiReferentialUuid() const;
%Docstring
    LPI oriented referential

    AIMS volumes are initially in LPI orientation. If flipped, the
    referential will change to a diferent one. The LPI referential is the
    original one before any flip is applied.
%End

    void setUuid( const std::string & uuid );
%Docstring
    force a new UUID
%End

    void setOrientation( const std::string & orient,
                         bool allow_resize = false);
%Docstring
    set a new orientation for the referential.

    if allow_resize is false, the given orientation will decide the new
    axes number. If true, it will be updated to keep the older number of
    dimensions.

    No other information will be changed.

    * referential.setOrientation(orient_str, allow_resize=False)
    * referential.setOrientation(orient_vec)

    Parameters
    ----------
    orient_str: str
        orientation string. May be extended to the actual number of dimensions if allow_resize is given.
    orient_vec: list of int
        orientations as a vector of constants
    allow_resize: bool (optional)
        if True, dimensions will be extended to the actual number of dimensions. Otherwise the given orientation will completely override.
%End

    void setOrientation( const vector_S32 & orient );
    void setLpiReferential( const std::string & lpi_uuid );
%Docstring
    set the LPI original referential UUID
%End

    static std::string orientationStr( carto::Referential::Orientation orient );
    static std::string orientationStr( const vector_S32 & orient );
    static carto::Referential::Orientation orientationCode(
      const std::string & orient );
%Docstring
    orientation constant for a single orientation axis character
%End

    vector_S32 orientationFromTransform(
      const soma::AffineTransformationBase & tr ) const;
%Docstring
    target orientation for a given transform matrix.

    The matrix should be a flip matrix, not any affine transform.
%End

    static vector_S32 orientationVector( const std::string & orient,
                                         unsigned ndim = 0 );
%Docstring
    orient = Referential.orientationVector(orient, dim)

    build an orientation vector from a string.

    If dim is given, the vector will be extended to this number of
    dimensions.
%End

    static void setAxisTransform( soma::AffineTransformationBase & tr,
                                  int src_axis,
                                  int dst_axis,
                                  int inv_mult,
                                  const vector_FLOAT & tsl = vector_FLOAT() );
%Docstring
    Build one line of transformation matrix

    This rather internal function is used to build a transformation matrix.
    This function builds one line to get from a source axis to a
    destination one.
%End

    carto::Object header() const;
    void setHeader( const carto::Object header );
    bool isDirect() const;
    bool is3DOriented() const;
%Docstring
    * res = referential.is3DOriented()

    tells if the orientation is comatible with a 3D transformation
    (all axes after the 3th are the default ones)

    * res = referential.is3DOriented(orient)

    tells if the given orientation is 3D compatible. orient may be a string or
    a vector of constants. (in the latter case the method is static)
%End

    bool is3DOriented( const std::string & orient ) const;
    static bool is3DOriented( const vector_S32 & orient );

    bool isLpiOriented() const;
%Docstring
    tells if the orientation is the default LPI one.
%End


    static carto::Referential fromHeader( carto::Object header,
                                          bool except_if_undefined = false );
%Docstring
    referential = Referential.fromHeader(header)

    Build a Referential object from a header dict.
    This is basically the same as the constructor Referential( Object )
    except that if except_if_undefined is set, and if the header does
    not specify an orientation, then an exception is thrown.
%End
  };

};

