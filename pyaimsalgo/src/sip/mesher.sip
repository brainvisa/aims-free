class Mesher
{
%TypeHeaderCode
  #include <aims/mesh/mesher.h>
%End
  public:
    Mesher();
    virtual ~Mesher();

    void doit( const AimsData_S16&, const std::string&,
               const std::string& = "binar" ) /ReleaseGIL/;

    void setSmoothing( float, int, float, float, float );
    void unsetSmoothing();

    void setDecimation( float, float, float, float );
    void unsetDecimation();

    void setSplitting();
    void unsetSplitting();

    void setLabelInf( int );
    void setLabelSup( int );

    void setMinSurface( float );

    void setMinFacetNumber( unsigned int );
    bool verbose() const;
    void setVerbose( bool);

    void getBrain( const AimsData_S16&, AimsSurfaceTriangle& )
      throw ( std::runtime_error ) /ReleaseGIL/;

    void getWhite( const AimsData_S16& , AimsSurfaceTriangle& ) /ReleaseGIL/;

    void getSingleLabel(const AimsData_S16&, AimsSurfaceTriangle &)
      /ReleaseGIL/;
};
