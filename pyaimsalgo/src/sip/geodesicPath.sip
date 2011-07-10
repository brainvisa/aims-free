
class GeodesicPath
{
%TypeHeaderCode
  #include <aims/geodesicpath/geodesicPath.h>
  #include <pyaims/vector/stdvector.h>
  #include <aims/vector/vector.h>
  #ifndef PYAIMSSIP_VECTOR_U32_DEFINED
  #define PYAIMSSIP_VECTOR_U32_DEFINED
  typedef std::vector<unsigned int> vector_U32;
  #endif
%End

  public :

    GeodesicPath( AimsSurfaceTriangle surface , int method, int strain);
     ~GeodesicPath();

    vector_U32 shortestPath_1_1_ind(unsigned source, unsigned target);

    vector_U32 shortestPath_1_1_1_ind(unsigned source, unsigned middle, unsigned target);
    void shortestPath_1_N_ind(unsigned source, vector_U32 targets, unsigned *target, double *length);

    void shortestPath_1_1_ind_xyz(unsigned source, unsigned target, vector_U32& indice, vector_POINT3DF& coord3D);
    void shortestPath_1_1_tex(unsigned source, unsigned target, float texturevalue, TimeTexture_FLOAT &tex);
    double shortestPath_1_1_len(unsigned source, unsigned target);

    void longestPath_1_N_ind(unsigned source, vector_U32 targets, unsigned *target, double *length, int type_distance);
    vector_U32 longestPath_N_N_ind(vector_U32 points, int* s, int *d, double *length, int type_distance);
    void distanceMap_1_N_ind(unsigned source, vector_FLOAT &distanceMap,double *length, int type_distance);
 };


