
namespace aims
{

class MeshInterpoler
{
%TypeHeaderCode
  #include <aims/mesh/meshinterpoler.h>
%End

  public:
    MeshInterpoler( const AimsSurfaceTriangle & source, const
      AimsSurfaceTriangle & dest );
    ~MeshInterpoler();

    void project();
    AimsSurfaceTriangle *resampleMesh(
      const AimsSurfaceTriangle & sourceshape ) const;
    TimeTexture_FLOAT *resampleTexture( const TimeTexture_FLOAT & ) const;
    Texture_FLOAT *resampleTexture( const Texture_FLOAT &, Texture_FLOAT &,
      int=0 ) const;

    const TimeTexture_U32 & projectedTriangles() const;
    const TimeTexture_FLOAT & projectedTriCoord1() const;
    const TimeTexture_FLOAT & projectedTriCoord2() const;
    const TimeTexture_FLOAT & projectedTriCoord3() const;
};

};

