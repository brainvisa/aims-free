/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */


#ifndef AIMS_MESH_MESHER_H
#define AIMS_MESH_MESHER_H

#include <aims/config/aimsalgo_config.h>
#include <aims/mesh/facet.h>
#include <aims/mesh/surface.h>
#include <cartodata/volume/volume.h>
#include <aims/bucket/bucket.h>
#include <map>
#include <list>
#include <vector>

typedef std::map< uint, Facet*, std::less< int > > MapOfFacet;

template <typename T> class TimeTexture;


class Mesher
{
  public:
    enum SmoothingType
    {
      LAPLACIAN,
      SIMPLESPRING,
      POLYGONSPRING,
      LOWPASS,
    };

    Mesher() : _smoothFlag( false ),
               _smoothIt( 30 ),
               _smoothRate( 0.4 ),
               _smoothFeatureAngle( 180. ),
               _smoothForce( 0.2 ),
               _smoothType( LOWPASS ),
               _deciFlag( false ),
               _deciReductionRate( 99. ),
               _deciMaxClearance( 3. ),
               _deciMaxError( .2 ),
               _deciFeatureAngle( 180. ),
               _splittingFlag( false ),
               _labelInf( 1 ),
               _labelSup( 32767 ),	// should maybe disapear ?
               _minSurface( 0.0f ),
               _triOutputFlag( false ),
               _minFacetNumber( 50 ), 
               _verbose( true ) { }
    virtual ~Mesher() { }

    // GETTING INTERFACE / MESH
    // =========================
    void getInterface( std::map<size_t, std::list< MapOfFacet > > & interface,
                       const carto::rc_ptr<carto::Volume<short> > & thing );
    void doit( const carto::rc_ptr<carto::Volume<short> > & thing,
               const std::string& name, const std::string& mode = "binar" );
    void doit( const aims::BucketMap<Void>& thing, const std::string& name,
               const std::string& mode = "binar" );
    void doit( const carto::rc_ptr<carto::Volume<short> > & thing,
               std::map<size_t, std::list<AimsSurfaceTriangle> >& surface );
    void doit( const aims::BucketMap<Void>& thing,
               std::map<size_t, std::list<AimsSurfaceTriangle> >& surface );
    void getBrain( const carto::rc_ptr<carto::Volume<short> > & thing,
                   AimsSurfaceTriangle& surface, bool insideinterface=false );
    void getBrain( const aims::BucketMap<Void>& thing,
                   AimsSurfaceTriangle& surface, bool insideinterface=false );
    // like getBrain but dedicated to gray/white interface in 6 connectivity,
    // which solves hole problems. JFM
    void getWhite( const carto::rc_ptr<carto::Volume<short> > & thing,
                   AimsSurfaceTriangle& surface );
    void getWhite( const aims::BucketMap<Void>& thing,
                   AimsSurfaceTriangle& surface );

    // create one mesh for all components of each label
    void getSingleLabel( const carto::rc_ptr<carto::Volume<short> > & thing,
                         AimsSurfaceTriangle& surface );

    // create one smoothed and decimated mesh from a map of facets
    // (used by getBrain and getSingleLabel)
    void getMeshFromMapOfFacet(
      const carto::rc_ptr<carto::Volume<short> > & thing,
      AimsSurfaceTriangle& surface, MapOfFacet &mof);

    /** Smoothing

    default : smoothType = LOWPASS (other values are LAPLACIAN, SIMPLESPRING,
                                    POLYGONSPRING)
              smoothIt = 30
              smoothRate in [0.0;1.0] (instance : 0.4)
              smoothForce in [0.0;1.0] (instance : 0.2)
    */
    void setSmoothing( SmoothingType smoothType, int smoothIt, float smoothRate );
    // smoothFeatureAngle good value: 180. degrees
    void setSmoothingLaplacian( float smoothFeatureAngle );
    void setSmoothingSpring( float smoothForce );
    void unsetSmoothing();
    void smooth( AimsSurfaceTriangle& surface );

    /** Decimation

    default : deciReductionRate = 99.0 %
              deciMaxClearance = 3.0
              deciMaxError = 0.2
              deciFeatureAngle = 180.0 deg
    */
    void setDecimation( float deciReductionRate,
                        float deciMaxClearance,
                        float deciMaxError,
                        float deciFeatureAngle );
    void unsetDecimation();
    float decimate( AimsSurfaceTriangle& surface );
    /** Decimation using a precision texture map.
        In this mode, the texture values are checked according to the precision
        thresholds, and vertices above the threshold are not removed.
        A time texture may be provided if a step-by-step evolution is needed.
        Similarly, the thresholds may evolve with decimation timesteps.
        precisionmap and thresholds may have differing sizes (they are
        clamped if overrun).
    */
    float decimate( AimsSurfaceTriangle& surface,
                    const std::vector<float> & precthresholds,
                    const TimeTexture<float> & precisionmap );

    // SPLITTING SURFACE OF CONNECTED COMP.
    // ====================================
    void doit( const carto::rc_ptr<carto::Volume<short> > & thing,
               std::map< size_t, std::list< std::map<short,
                  std::list<AimsSurfaceTriangle > > > >& surface );
    void doit( const aims::BucketMap<Void>& thing,
               std::map< size_t, std::list< std::map<short,
                  std::list<AimsSurfaceTriangle > > > >& surface );
    void setSplitting();
    void unsetSplitting();

    // LABELS
    // ======
    void setLabelInf( int labelInf ) { _labelInf = labelInf; }
    void setLabelSup( int labelSup ) { _labelSup = labelSup; }

    // SURFACE VALUE
    // =============
    void setMinSurface( float minSurface ) { _minSurface = minSurface; }
    float surfaceOfInterface( const AimsSurfaceTriangle& surface );

    // OUTPUT FORMAT
    // =============
    /** OBSOLETE - specify extension in the output filename of 
	setMeshOutputFile() or doit() */
    void setTriOutputFile()
      __attribute__((__deprecated__("specify extension in the output filename "
                                    "of setMeshOutputFile() or doit()")))
    { _triOutputFlag = true; }
    void setMeshOutputFile() { _triOutputFlag = false; }

    // MINIMUM NUMBER OF FACETS TO ALLOW DECIMATION
    // ============================================
    void setMinFacetNumber( uint m ) { _minFacetNumber = m; }

    bool verbose() const { return _verbose; }
    void setVerbose( bool x ) { _verbose = x; }

  protected:

    bool _smoothFlag;
    int _smoothIt;
    float _smoothRate;
    float _smoothFeatureAngle;
    float _smoothForce;
    SmoothingType _smoothType;

    bool _deciFlag;
    float _deciReductionRate;
    float _deciMaxClearance;
    float _deciMaxError;
    float _deciFeatureAngle;

    bool _splittingFlag;

    int _labelInf;
    int _labelSup;

    float _minSurface;

    bool _triOutputFlag;

    uint _minFacetNumber;

    bool _verbose;

    void getReducedNeighOutwardCycles( MapOfFacet& thing );
    void getExtendedNeighOfFacet( MapOfFacet& thing );

    void getVertices( const std::vector< Facet* >& vfac,
                      std::vector< Point3df >& vertex,
                      float sizeX, float sizeY, float sizeZ );
    void getSmoothedVertices( const std::vector< Facet* >& vfac,
                              AimsSurfaceTriangle& surface,
                              float rate );
    void getSmoothedLaplacian( const std::vector< Facet* >& vfac,
                               AimsSurfaceTriangle& surface,
                               float featureAngle,
                               int nIteration,
                               float rate );
    void getSmoothedSimpleSpring( const std::vector< Facet* >& vfac,
                                  AimsSurfaceTriangle& surface,
                                  float force,
                                  int nIteration,
                                  float rate );
    void getSmoothedPolygonSpring( const std::vector< Facet* >& vfac,
                                   AimsSurfaceTriangle& surface,
                                   float force,
                                   int nIteration,
                                   float rate );
    void getSmoothedLowPassFilter( const std::vector< Facet* >& vfac,
                                   AimsSurfaceTriangle& surface,
                                   int nIteration,
                                   float rate );
    void getDecimatedVertices( std::vector< Facet* >& vfac,
                               std::vector< Point3df >& vertex,
                               float reductionRatePercent,
                               float maxClearanceMm,
                               float maxErrorMm,
                               float minFeatureEdgeAngleDegree );
    void getDecimatedVertices( std::vector< Facet* >& vfac,
                               std::vector< Point3df >& vertex,
                               float reductionRatePercent,
                               float maxClearanceMm,
                               float maxErrorMm,
                               float minFeatureEdgeAngleDegree,
                               const std::vector<float> & thresholds,
                               const TimeTexture<float> *precisionmap = 0
                             );
    float decimate( AimsSurfaceTriangle& surface,
                    const std::vector<float> & thresholds,
                    const TimeTexture<float> *precisionmap );

    void getNormals( const std::vector< Facet* >& vfac,
                     const std::vector< Point3df >& vertex,
                     std::vector< Point3df >& normal );
    void getTriangles( const std::vector< Facet* >& vfac,
                       std::vector< AimsVector< uint, 3 > >& triangle );

    void splitting( const carto::rc_ptr<carto::Volume< short > > & thing,
                    const std::vector< Facet* >& vfac,
                    const AimsSurfaceTriangle& surface,
                    std::map<short,std::list< AimsSurfaceTriangle> >&
                      splitted );

    void getFacet( const AimsSurfaceTriangle& surface, 
                   std::vector< Facet* >& vfac );

    void clear( std::map< size_t, std::list< MapOfFacet > >& interface );
    /// reshape (if needed) the input volume to add a border filled with -1
    static carto::VolumeRef<int16_t> reshapedVolume(
      const carto::VolumeRef<int16_t> in_vol );
};


#endif
