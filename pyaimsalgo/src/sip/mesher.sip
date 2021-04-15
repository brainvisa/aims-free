class Mesher
{
%TypeHeaderCode
  #include <aims/mesh/mesher.h>
%End
  public:
    enum SmoothingType
    {
      LAPLACIAN,
      SIMPLESPRING,
      POLYGONSPRING,
      LOWPASS,
    };

%Docstring
Mesh binary objects in a volume and produce surface meshes.
%End

  public:
    Mesher();
    virtual ~Mesher();

    void doit( const AimsData_S16&, const std::string&,
               const std::string& = "binar" ) /ReleaseGIL/;
%Docstring
doit(volume, filename_base, write_mode="binar")

Mesh every interface of objects in the input label volume. Each mesh is written in a separate file. Files are numbered according to objects interfaces (label1_label2) and an interface number for this pair of objects.
write_mode is an old flag to write files in ascii or binary modes. It's obsolete.

%End

    void doit( const BucketMap_VOID&, const std::string&,
               const std::string& = "binar" ) /ReleaseGIL/;
%Docstring
doit(bucket, filename_base, write_mode="binar")

Mesh every interface of objects in the input label volume. Each mesh is written in a separate file. Files are numbered according to objects interfaces (label1_label2) and an interface number for this pair of objects.
write_mode is an old flag to write files in ascii or binary modes. It's obsolete.
%End

    void setSmoothing( SmoothingType smoothType, int nIteration, float factor );
    void setSmoothingLaplacian( float featureAngle );
    void setSmoothingSpring( float smoothForce );
    void unsetSmoothing();
    void smooth( AimsSurfaceTriangle& surface ) /ReleaseGIL/;

    void setDecimation( float, float, float, float );
    void unsetDecimation();

    float decimate( AimsSurfaceTriangle& surface );
    float decimate( AimsSurfaceTriangle& surface,
                    const vector_FLOAT & precthresholds,
                    const TimeTexture_FLOAT & precisionmap );

    void setSplitting();
    void unsetSplitting();

    void setLabelInf( int );
    void setLabelSup( int );

    void setMinSurface( float );

    void setMinFacetNumber( unsigned int );
    bool verbose() const;
    void setVerbose( bool);

%If (SIPTHROW_BUG)
    void getBrain( const AimsData_S16&, AimsSurfaceTriangle&, 
      bool insideinterface=false ) throw ( ) /ReleaseGIL/;
%End
%If (!SIPTHROW_BUG)
    void getBrain( const AimsData_S16&, AimsSurfaceTriangle&, 
      bool insideinterface=false )
      throw ( std::runtime_error, carto::assert_error ) /ReleaseGIL/;
%End

    void getBrain( const BucketMap_VOID&, AimsSurfaceTriangle&,
      bool insideinterface=false )
      throw ( std::runtime_error, carto::assert_error ) /ReleaseGIL/;

    void getWhite( const AimsData_S16& , AimsSurfaceTriangle& ) /ReleaseGIL/;

    void getSingleLabel(const AimsData_S16&, AimsSurfaceTriangle &)
      /ReleaseGIL/;
};
