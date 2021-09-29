namespace aims
{

class HarmonicCageMeshResampler
{
%TypeHeaderCode
  #include <aims/resampling/harmonicCageResampler.h>
%End
  public:
    HarmonicCageMeshResampler(const rc_ptr_Volume_U32 &, vector_POINT3DF &controls,
                             short background=0, short border=1, short inside=2);
    virtual ~HarmonicCageMeshResampler();

    Point3df coordinate(const Point3df &p);
    void computeCoordinates(float threshold=0.0001) /ReleaseGIL/;
    AimsSurfaceTriangle *coordinates(const AimsSurfaceTriangle &) /Factory, ReleaseGIL/;
    const Point3df &getControl(int ind);
    void set_keep_image_coords(bool state);
    const rc_ptr_Volume_FLOAT &get_image_coords(int ind);
    void moveControl(int ind, const Point3df &p);
};

};
