%ModuleHeaderCode
#include <aims/mesh/meshwatershed.h>
%End

/* namespace aims
{
  void distancesFromMesh( const AimsSurfaceTriangle & mesh,
                          vector_AimsVector_DOUBLE_2 & edges,
                          vector_DOUBLE & weights );
};
*/

int meshWatershed( const AimsSurfaceTriangle & mesh,
                    const vector_DOUBLE & field, vector_S32 & idx,
                    vector_S32 & depth, vector_S32 & major,
                    vector_S32 & label, double threshold );
%MethodCode
  sipRes = aims::meshWatershed( *a0, *a1, *a2, *a3, *a4, *a5, a6 );
%End


int meshWatershed( const AimsSurfaceTriangle & mesh,
                    const TimeTexture_DOUBLE & field,
                    double threshold,
                    TimeTexture_S32 **idx /TransferBack/,
                    TimeTexture_S32 **depth /TransferBack/,
                    TimeTexture_S32 **major /TransferBack/,
                    TimeTexture_S32 **label /TransferBack/ );
%MethodCode
  a3 = new TimeTexture_S32;
  a4 = new TimeTexture_S32;
  a5 = new TimeTexture_S32;
  a6 = new TimeTexture_S32;
  sipRes = aims::meshWatershed( *a0, *a1, *a3, *a4, *a5, *a6, a2 );
%End

void meshWatershed( const AimsSurfaceTriangle & mesh,
                    const TimeTexture_FLOAT & field,
                    double threshold,
                    TimeTexture_S32 **idx /TransferBack/,
                    TimeTexture_S32 **depth /TransferBack/,
                    TimeTexture_S32 **major /TransferBack/,
                    TimeTexture_S32 **label /TransferBack/ );
%MethodCode
  TimeTexture_DOUBLE field;
  std::vector<double> & f = field[0].data();
  const std::vector<float> & f0 = a1->begin()->second.data();
  f.reserve( f0.size() );
  size_t i, n = f0.size();
  for( i=0; i<n; ++i )
    f.push_back( f0[i] );
  a3 = new TimeTexture_S32;
  a4 = new TimeTexture_S32;
  a5 = new TimeTexture_S32;
  a6 = new TimeTexture_S32;
  aims::meshWatershed( *a0, field, *a3, *a4, *a5, *a6, a2 );
%End


int meshBlobsBifurcation( const AimsSurfaceTriangle & mesh,
                          const vector_DOUBLE & field,
                          vector_S32 & idx,
                          vector_DOUBLE & height,
                          vector_S32 & father,
                          vector_S32 & label, double th );
%MethodCode
  sipRes = aims::meshBlobsBifurcation( *a0, *a1, *a2, *a3, *a4, *a5, a6 );
%End


void meshBlobsBifurcation( const AimsSurfaceTriangle & mesh,
                          const TimeTexture_DOUBLE & field, double th,
                          TimeTexture_S32 **idx,
                          TimeTexture_DOUBLE **height,
                          TimeTexture_S32 **father,
                          TimeTexture_S32 **label );
%MethodCode
  a3 = new TimeTexture_S32;
  a4 = new TimeTexture_DOUBLE;
  a5 = new TimeTexture_S32;
  a6 = new TimeTexture_S32;
  aims::meshBlobsBifurcation( *a0, *a1, *a3, *a4, *a5, *a6, a2 );
%End


void meshBlobsBifurcation( const AimsSurfaceTriangle & mesh,
                          const TimeTexture_FLOAT & field, double th,
                          TimeTexture_S32 **idx,
                          TimeTexture_DOUBLE **height,
                          TimeTexture_S32 **father,
                          TimeTexture_S32 **label );
%MethodCode
  TimeTexture_DOUBLE field;
  std::vector<double> & f = field[0].data();
  const std::vector<float> & f0 = a1->begin()->second.data();
  f.reserve( f0.size() );
  size_t i, n = f0.size();
  for( i=0; i<n; ++i )
    f.push_back( f0[i] );
  a3 = new TimeTexture_S32;
  a4 = new TimeTexture_DOUBLE;
  a5 = new TimeTexture_S32;
  a6 = new TimeTexture_S32;
  aims::meshBlobsBifurcation( *a0, field, *a3, *a4, *a5, *a6, a2 );
%End


  void blobsHeights( const AimsSurfaceTriangle & mesh,
                     const vector_DOUBLE & field,
                     const vector_S32 & watershedlabels,
                     vector_S32 **junctions );
%MethodCode
  a3 = new vector_S32;
  aims::blobsHeights( *a0, *a1, *a2, *a3 );
%End

