
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
    virtual ~MeshInterpoler();

    void project();
    AimsSurfaceTriangle *resampleMesh(
      const AimsSurfaceTriangle & sourceshape ) const /Factory/;
    TimeTexture_FLOAT *resampleTexture( const TimeTexture_FLOAT & ) const
      /Factory/;
    void resampleTexture( const Texture_FLOAT &, Texture_FLOAT &, int=0 ) const;

    const TimeTexture_U32 & projectedTriangles() const /Transfer/;
    const TimeTexture_FLOAT & projectedTriCoord1() const /Transfer/;
    const TimeTexture_FLOAT & projectedTriCoord2() const /Transfer/;
    const TimeTexture_FLOAT & projectedTriCoord3() const /Transfer/;

    void reloadProjectionParams( const TimeTexture_U32 & projTriangles,
      const TimeTexture_FLOAT & projTriCoord1,
      const TimeTexture_FLOAT & projTriCoord2,
      const TimeTexture_FLOAT & projTriCoord3 );
    void setMeshes( const AimsSurfaceTriangle & source,
      const AimsSurfaceTriangle & dest );
};

  class CoordinatesFieldMeshInterpoler : aims::MeshInterpoler
  {
%TypeHeaderCode
  #include <aims/mesh/meshinterpoler.h>
%End

  public:
    CoordinatesFieldMeshInterpoler( const AimsSurfaceTriangle & source,
                                    const AimsSurfaceTriangle & dest,
                                    const TimeTexture_FLOAT & srccoord1,
                                    const TimeTexture_FLOAT & srccoord2,
                                    const TimeTexture_FLOAT & dstcoord1,
                                    const TimeTexture_FLOAT & dstcoord2 );
    virtual ~CoordinatesFieldMeshInterpoler();
  };

};

