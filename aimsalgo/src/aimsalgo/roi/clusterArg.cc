
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/roi/clusterArg_d.h>
#include <aims/def/path.h>

#include <graph/graph/gwriter.h>
#include <aims/io/datatypecode.h>
#include <stdio.h>
#include <float.h>

using namespace aims;
using namespace carto;
using namespace std;


ClusterArgMaker::ClusterArgMaker( const string & out, Connectivity::Type conn, 
                                  int minsz, const string & mtxfile,
                                  bool mkmesh, float maxclear,
                                  float maxerr, uint minfac, bool bin,
                                  float low, float up )
  : _fileout( out ), _connectivity( conn ), _minsize( minsz ), 
    _matrix( mtxfile ), _domesh( mkmesh ), _deciMaxClearance( maxclear ), 
    _deciMaxError( maxerr ), _minFacetNumber( minfac ), _binarize( bin ), 
    _lowth( low ), _upth( up )
{
}


ClusterArgMaker::~ClusterArgMaker()
{
}


template
void ClusterArgMaker::make( Graph & gr,
                            const rc_ptr<Volume<uint8_t> > & data );
template
void ClusterArgMaker::make( Graph & gr,
                            const rc_ptr<Volume<int16_t> > & data );
template
void ClusterArgMaker::make( Graph & gr,
                            const rc_ptr<Volume<float> > & data );
