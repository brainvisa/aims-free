
namespace carto
{

  class Referential
  {
%TypeHeaderCode
#include <cartodata/transformation/referential.h>
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

    Referential();
    Referential( const carto::Referential & ref );
    Referential( carto::Object ref );
    ~Referential();

    std::string uuid() const;
    std::string orientationStr() const;
    std::string orientationStr( const std::string & orient ) const;
    vector_S32 axesOrientation() const;
    rc_ptr_Transformation
    toOrientation( const std::string & orient, const vector_FLOAT & transl = vector_FLOAT() ) const;
    rc_ptr_Transformation
    toOrientation( const vector_S32 & orient, const vector_FLOAT & transl = vector_FLOAT() ) const;
    std::string lpiReferentialUuid() const;

    void setUuid( const std::string & uuid );
    void setOrientation( const std::string & orient );
    void setOrientation( const vector_S32 & orient );
    void setLpiReferential( const std::string & lpi_uuid );

    static std::string orientationStr( carto::Referential::Orientation orient );
    static std::string orientationStr( const vector_S32 & orient );
    static carto::Referential::Orientation orientationCode(
      const std::string & orient );
    static vector_S32 orientationVector( const std::string & orient,
                                         unsigned ndim = 0 );
    static void setAxisTransform( soma::AffineTransformationBase & tr,
                                  int src_axis,
                                  int dst_axis,
                                  int inv_mult,
                                  const vector_FLOAT & tsl = vector_FLOAT() );
    carto::Object header() const;
    bool is3DOriented() const;
%Docstring
    tells if the orientation is comatible with a 3D transformation
    (all axes after the 3th are the default ones)
%End

    bool is3DOriented( const std::string & orient ) const;
    static bool is3DOriented( const vector_S32 & orient );
  };

};

