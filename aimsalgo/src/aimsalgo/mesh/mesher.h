/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


#ifndef AIMS_MESH_MESHER_H
#define AIMS_MESH_MESHER_H

#include <aims/config/aimsalgo_config.h>
#include <aims/mesh/facet.h>
#include <aims/mesh/surface.h>
#include <aims/data/data.h>
#include <map>
#include <list>
#include <vector>

typedef std::map< uint, Facet*, std::less< int > > MapOfFacet;


class AIMSALGO_API Mesher
{
  public:

    Mesher() : _smoothFlag( false ),
               _deciFlag( false ),
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
    void getInterface( std::map<size_t, std::list< MapOfFacet > >& interface,
                       const AimsData<short>& thing );
    void doit( const AimsData<short>& thing, const std::string& name,
               const std::string& mode = "binar" );
    void doit( const AimsData<short>& thing,
               std::map<size_t, std::list<AimsSurfaceTriangle> >& surface );
    void getBrain( const AimsData<short>& thing, 
		   AimsSurfaceTriangle& surface );
  // like getBrain but dedicated to gray/white interface in 6 connectivity,
  // which solves hole problems. JFM
    void getWhite( const AimsData<short>& thing, 
		   AimsSurfaceTriangle& surface );

    // create one mesh for all components of each label
    void getSingleLabel( const AimsData<short>& thing,
                       AimsSurfaceTriangle& surface );

    // create one smoothed and decimated mesh from a map of facets
    // (used by getBrain and getSingleLabel)
    void getMeshFromMapOfFacet(const AimsData<short>& thing,
             AimsSurfaceTriangle& surface, MapOfFacet &mof);

    // SMOOTHING
    // =========
    // default : featureAngle = 180.0 deg
    //           nIteration = 5
    //           (x/y/z)Factor in [0.0;1.0] (instance : 0.2)
    void setSmoothing( float featureAngle, int nIteration,
                       float xFactor, float yFactor, float zFactor );
    void unsetSmoothing();
    void smooth( AimsSurfaceTriangle& surface );

    // DECIMATION
    // ==========
    // default : deciReductionRate = 100.0 %
    //           deciMaxClearance = 5.0
    //           deciMaxError = 0.003
    //           deciFeatureAngle = 120.0
    void setDecimation( float deciReductionRate,
                        float deciMaxClearance,
                        float deciMaxError,
                        float deciFeatureAngle );
    void unsetDecimation();
    float decimate( AimsSurfaceTriangle& surface );

    // SPLITTING SURFACE OF CONNECTED COMP.
    // ====================================
    void doit( const AimsData<short>& thing,
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
    void setTriOutputFile() { _triOutputFlag = true; }
    void setMeshOutputFile() { _triOutputFlag = false; }

    // MINIMUM NUMBER OF FACETS TO ALLOW DECIMATION
    // ============================================
    void setMinFacetNumber( uint m ) { _minFacetNumber = m; }

    bool verbose() const { return _verbose; }
    void setVerbose( bool x ) { _verbose = x; }

  protected:

    bool _smoothFlag;
    float _featureAngle;
    int _nIteration;
    float _xFactor;
    float _yFactor;
    float _zFactor;

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
                              std::vector< Point3df >& vertex,
                              float featureAngle, int nIteration,
                              float xDeplFactor,
                              float yDeplFactor,
                              float zDeplFactor );
    void getDecimatedVertices( std::vector< Facet* >& vfac,
                               std::vector< Point3df >& vertex,
                               float reductionRatePercent,
                               float maxClearanceMm,
                               float maxErrorMm,
                               float minFeatureEdgeAngleDegree );

    void getNormals( const std::vector< Facet* >& vfac,
                     const std::vector< Point3df >& vertex,
                     std::vector< Point3df >& normal );
    void getTriangles( const std::vector< Facet* >& vfac,
                       std::vector< AimsVector< uint, 3 > >& triangle );

    void splitting( const AimsData< short >& thing,
                    const std::vector< Facet* >& vfac,
                    const AimsSurfaceTriangle& surface,
                    std::map<short,std::list< AimsSurfaceTriangle> >& 
		    splitted );

    void getFacet( const AimsSurfaceTriangle& surface, 
		   std::vector< Facet* >& vfac );

    void clear( std::map< size_t, std::list< MapOfFacet > >& interface );
};


#endif
