
#ifndef AIMS_ROI_CLUSTERARG_H
#define AIMS_ROI_CLUSTERARG_H

#include <cartodata/volume/volume.h>
#include <aims/connectivity/connectivity.h>
#include <float.h>

class Graph;

namespace aims
{

  class ClusterArgMaker
  {
  public:
    ClusterArgMaker( const std::string & outfilename, Connectivity::Type conn 
		     = Connectivity::CONNECTIVITY_18_XYZ, 
		     int minsz = 0, const std::string & mtxfile = "", 
		     bool mkmesh = true, float maxclear = 5., 
		     float maxerr = 0.1, uint minfac = 50, bool bin = false, 
		     float low = -FLT_MAX, float up = FLT_MAX );
    ~ClusterArgMaker();

    void setFilename( const std::string & n ) { _fileout = n; }
    std::string filename() const { return( _fileout ); }
    void setConnectivity( Connectivity::Type conn ) { _connectivity = conn; }
    Connectivity::Type connectivity() const { return( _connectivity ); }
    void setMinClusterSize( int sz ) { _minsize = sz; }
    int minClusterSize() const { return( _minsize ); }
    void setTransformationFilename( const std::string & mtxfile )
    { _matrix = mtxfile; }
    std::string transformationFilename() const { return( _matrix); }
    void setMeshFlag( bool x ) { _domesh = x; }
    bool meshFlag() const { return( _domesh ); }
    void setDecimationMaxClearance( float x ) { _deciMaxClearance = x; }
    float decimationMaxClearance() const { return( _deciMaxClearance ); }
    void setDecimationMaxError( float x ) { _deciMaxError = x; }
    float decimationMaxError() const { return( _deciMaxError ); }
    void setMinFacetNumber( uint x ) { _minFacetNumber = x; }
    uint minFacetNumber() const { return( _minFacetNumber ); }
    void setBinarize( bool x ) { _binarize = x; }
    bool binarize() const { return( _binarize ); }
    void setLowThreshold( float x ) { _lowth = x; }
    float lowThreshold() const { return( _lowth ); }
    void setUpperThershold( float x ) { _upth = x; }
    float upperThreshold() const { return( _upth ); }

    template<typename T> void make(
      Graph & gr, const carto::rc_ptr<carto::Volume<T> > & data );

  private:
    std::string		_fileout;
    Connectivity::Type	_connectivity;
    int			_minsize;
    std::string		_matrix;
    bool		_domesh;
    float		_deciMaxClearance;
    float		_deciMaxError;
    uint		_minFacetNumber;
    bool		_binarize;
    float		_lowth;
    float		_upth;
  };

}


#endif
