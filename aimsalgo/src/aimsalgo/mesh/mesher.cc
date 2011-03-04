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


#include <aims/mesh/mesher.h>
#include <aims/mesh/surfaceOperation.h> // surfacemanip
#include <aims/io/writer.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/stream/sstream.h>
#include <iomanip>
#include <stdio.h>

using namespace aims;
using namespace carto;
using namespace std;


void Mesher::setSmoothing( float featureAngle, int nIteration,
                           float xFactor, float yFactor, float zFactor )
{
  _smoothFlag = true;
  _featureAngle = featureAngle;
  _nIteration = nIteration;
  _xFactor = xFactor;
  _yFactor = yFactor;
  _zFactor = zFactor;
}


void Mesher::unsetSmoothing()
{
  _smoothFlag = false;
}


void Mesher::setDecimation( float deciReductionRate,
                            float deciMaxClearance,
                            float deciMaxError,
                            float deciFeatureAngle )
{
  _deciFlag = true;
  _deciReductionRate = deciReductionRate;
  _deciMaxClearance = deciMaxClearance;
  _deciMaxError = deciMaxError;
  _deciFeatureAngle = deciFeatureAngle;
}


void Mesher::unsetDecimation()
{
  _deciFlag = false;
}



void Mesher::setSplitting()
{
  _splittingFlag = true;
}


void Mesher::unsetSplitting()
{
  _splittingFlag = false;
}



void Mesher::doit( const AimsData<short>& thing,
                   const string& name, const string& mode )
{
  map<size_t, list< MapOfFacet > > interface;
  string	fname = FileUtil::basename( name );
  string	ext;
  string::size_type	p = fname.rfind( '.' );
  if( p != string::npos )
    {
      string::size_type	n = fname.length() - p;
      ext = fname.substr( p, n );
      fname.erase( p, n );
    }
  fname = FileUtil::dirname( name ) + FileUtil::separator() + fname;

  getInterface( interface, thing );

  if( _verbose )
    cout << "processing mesh    : " << flush;

  list< MapOfFacet >::iterator i;
  for ( int n = _labelInf; n <= _labelSup; n++ )
  {
    int nObject = 0;
    if ( interface[ n ].size() )
      for ( i = interface[ n ].begin(); i != interface[ n ].end(); i++ )
      {

        if( _verbose )
          {
            cout << "[ " << setw( 5 ) << n << ", " 
                 << setw( 5 ) << nObject + 1 << " ] " << flush; 

            cout << "reduced neighborhood " << flush;
          }
        getReducedNeighOutwardCycles( *i );
        if( _verbose )
          {
            cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;

            cout << "extended neighborhood" << flush;
          }
        getExtendedNeighOfFacet( *i );
        if( _verbose )
          cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;

        // creates of vector of pointers to facet
        vector< Facet* > vfac( i->size() );
        MapOfFacet::iterator f;
        int k;
        for ( f = i->begin(), k = 0; f != i->end(); f++, k++ )
        {
          vfac[ k ] = f->second;
          vfac[ k ]->id() = k;
        }

        AimsSurfaceTriangle surface;

        // gets the coordinates of the center of each facet
        if( _verbose )
          cout << "vertices             " << flush;
        getVertices( vfac, surface.vertex(), 
                     thing.sizeX(), thing.sizeY(), thing.sizeZ() );        
        if( _verbose )
          cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;

        // smoothes the vertex coordinates
        if ( _smoothFlag )
          getSmoothedVertices( vfac, surface.vertex(), _featureAngle,
                               _nIteration, _xFactor, _yFactor, _zFactor );

        // decimates the mesh
        if ( _deciFlag && vfac.size() > _minFacetNumber )
          getDecimatedVertices( vfac, surface.vertex(), _deciReductionRate,
                                _deciMaxClearance, _deciMaxError, 
                                _deciFeatureAngle );


        // gets the normals by taking the average of all the 
        // normals of the neighbors of a facet
        if( _verbose )
          cout << "normals              " << flush;
        getNormals( vfac, surface.vertex(), surface.normal() );        
        if( _verbose )
          {
            cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;
            
            // gets the triangles with a SLAVE/MASTER algorithm
            cout << "triangles            " << flush;
          }
        getTriangles( vfac, surface.polygon() );
        if( _verbose )
          {
            cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;

            // gets surface of interface
            cout << "surface              " << flush;
          }
        float surf = surfaceOfInterface( surface );
        if( _verbose )
          cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;

        // splits surface
        if ( _splittingFlag )
        {
          if( _verbose )
            cout << "splitting            " << flush;
          map< short, list< AimsSurfaceTriangle > > splitted;
          splitting( thing, vfac, surface, splitted );

          if( _verbose )
            {
              cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;
              cout << "writing              " << flush;
            }
          map< short, list< AimsSurfaceTriangle > >::iterator l;
          list< AimsSurfaceTriangle >::iterator s;
          for ( l = splitted.begin(); l != splitted.end(); l++ )
          {
            int nSubObject = 0;
            for ( s = l->second.begin(); s != l->second.end(); s++ )
            {
              if ( surfaceOfInterface( *s ) > _minSurface )
              {
                ostringstream	ss;
                ss << fname << '_' << n << '_' << nObject << '_' << l->first 
                   << '_' << nSubObject;

		Writer<AimsSurfaceTriangle> surfaceW( ss.str() );
		static string	tris( "TRI" );
		surfaceW.write( *s, mode == "ascii", 
				_triOutputFlag ? &tris : 0 );
                nSubObject++;
              }
            }
            nObject++;
          }
          if( _verbose )
            cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;
        }
        else
        {
          cout << "writing              " << flush;
          if ( surf > _minSurface )
          {
            ostringstream	ss;
            ss << fname << '_' << n << '_' << nObject << ext;

	    Writer<AimsSurfaceTriangle> surfaceW( ss.str() );
	    static string	tris( "TRI" );
	    surfaceW.write( surface, mode == "ascii", 
			    _triOutputFlag ? &tris : 0 );
            nObject++;
          }
          if( _verbose )
            cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;
        }
        cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush; 
      }
  }

  if( _verbose )
    cout << "done                       " << endl;

  clear( interface );
}



void Mesher::doit( const AimsData<short>& thing,
                   map< size_t, list< AimsSurfaceTriangle > >& surface )
{
  map< size_t, list< MapOfFacet > > interface;
  getInterface( interface, thing );

  if( _verbose )
    cout << "processing mesh    : " << flush;

  list< MapOfFacet >::iterator i;
  for ( int n = _labelInf; n <= _labelSup; n++ )
  {
    int nObject = 0;
    if ( interface[ n ].size() )
      for ( i = interface[ n ].begin(); i != interface[ n ].end(); i++ )
      {
        if( _verbose )
          {
            cout << "[ " << setw( 5 ) << n << ", " 
                 << setw( 5 ) << nObject + 1 << " ] " << flush; 

            cout << "reduced neighborhood " << flush;
          }
        getReducedNeighOutwardCycles( *i );
        if( _verbose )
          {
            cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;


            cout << "extended neighborhood" << flush;
          }
        getExtendedNeighOfFacet( *i );
        if( _verbose )
          cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;

        // creates of vector of pointers to facet
        vector< Facet* > vfac( i->size() );
        MapOfFacet::iterator f;
        int k;
        for ( f = i->begin(), k = 0; f != i->end(); f++, k++ )
        {
          vfac[ k ] = f->second;
          vfac[ k ]->id() = k;
        }

        AimsSurfaceTriangle theSurface;
        surface[ n ].push_back( theSurface );
        AimsSurfaceTriangle& current = surface[ n ].back();

        // gets the coordinates of the center of each facet
        if( _verbose )
          cout << "vertices             " << flush;
        getVertices( vfac, current.vertex(), 
                     thing.sizeX(), thing.sizeY(), thing.sizeZ() );        
        if( _verbose )
          cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;

        // smoothes the vertex coordinates
        if ( _smoothFlag )
          getSmoothedVertices( vfac, current.vertex(), _featureAngle,
                               _nIteration, _xFactor, _yFactor, _zFactor );

        // decimates the mesh
        if ( _deciFlag && vfac.size() > _minFacetNumber )
          getDecimatedVertices( vfac, current.vertex(), _deciReductionRate,
                                _deciMaxClearance, _deciMaxError, 
                                _deciFeatureAngle );

        // gets the normals by taking the average of all the 
        // normals of the neighbors of a facet
        if( _verbose )
          cout << "normals              " << flush;
        getNormals( vfac, current.vertex(), current.normal() );        
        if( _verbose )
          {
            cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;

            // gets the triangles with a SLAVE/MASTER algorithm
            cout << "triangles            " << flush;
          }
        getTriangles( vfac, current.polygon() );
        if( _verbose )
          {
            cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;
            cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush; 
          }

        nObject++;
      }
  }
  if( _verbose )
    cout << "done                       " << endl;

  clear( interface );
}


void Mesher::doit( const AimsData<short>& thing,
                   map< size_t, list< map< short, 
		   list < AimsSurfaceTriangle > > > >& surface )
{
  map< size_t, list< MapOfFacet > > interface;
  getInterface( interface, thing );

  if( _verbose )
    cout << "processing mesh    : " << flush;

  list< MapOfFacet >::iterator i;
  for ( int n = _labelInf; n <= _labelSup; n++ )
  {
    int nObject = 0;
    if ( interface[ n ].size() )
      for ( i = interface[ n ].begin(); i != interface[ n ].end(); i++ )
      {
        if( _verbose )
          {
            cout << "[ " << setw( 5 ) << n << ", " 
                 << setw( 5 ) << nObject + 1 << " ] " << flush; 

            cout << "reduced neighborhood " << flush;
          }
        getReducedNeighOutwardCycles( *i );
        if( _verbose )
          {
            cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;


            cout << "extended neighborhood" << flush;
          }
        getExtendedNeighOfFacet( *i );
        if( _verbose )
          cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;

        // creates of vector of pointers to facet
        vector< Facet* > vfac( i->size() );
        MapOfFacet::iterator f;
        int k;
        for ( f = i->begin(), k = 0; f != i->end(); f++, k++ )
        {
          vfac[ k ] = f->second;
          vfac[ k ]->id() = k;
        }

        AimsSurfaceTriangle theSurface;

        // gets the coordinates of the center of each facet
        if( _verbose )
          cout << "vertices             " << flush;
        getVertices( vfac, theSurface.vertex(), 
                     thing.sizeX(), thing.sizeY(), thing.sizeZ() );        
        if( _verbose )
          cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;

        // smoothes the vertex coordinates
        if ( _smoothFlag )
          getSmoothedVertices( vfac, theSurface.vertex(), _featureAngle,
                               _nIteration, _xFactor, _yFactor, _zFactor );

        // decimates the mesh
        if ( _deciFlag && vfac.size() > _minFacetNumber )
          getDecimatedVertices( vfac, theSurface.vertex(), _deciReductionRate,
                                _deciMaxClearance, _deciMaxError, 
                                _deciFeatureAngle );

        // gets the normals by taking the average of all the 
        // normals of the neighbors of a facet
        if( _verbose )
          cout << "normals              " << flush;
        getNormals( vfac, theSurface.vertex(), theSurface.normal() );        
        if( _verbose )
          {
            cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;

            // gets the triangles with a SLAVE/MASTER algorithm
            cout << "triangles            " << flush;
          }
        getTriangles( vfac, theSurface.polygon() );
        if( _verbose )
          {
            cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;

            // splitting surfaces
            cout << "splitting            " << flush;
          }
        map< short, list< AimsSurfaceTriangle > > splitted;
        surface[ n ].push_back( splitted );
        map< short, list< AimsSurfaceTriangle > >& 
          current = surface[ n ].back();        
        splitting( thing, vfac, theSurface, current );
        if( _verbose )
          {
            cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;

            cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush; 
          }
        nObject++;
      }
  }
  if( _verbose )
    cout << "done                       " << endl;

  clear( interface );
}

void	Mesher::getMeshFromMapOfFacet(const AimsData<short>& thing,
		AimsSurfaceTriangle& surface, MapOfFacet &mof)
{
      if( _verbose )
      {
        cout << "processing mesh    : " << flush;
        cout << "reduced neighborhood " << flush;
      }

      getReducedNeighOutwardCycles( mof );
      if( _verbose )
      {
        cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\v" << flush;
        cout << "extended neighborhood" << flush;
      }
      getExtendedNeighOfFacet( mof );
      if( _verbose )
        cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\v" << flush;

      // creates of vector of pointers to facet
      vector< Facet* > vfac( mof.size() );
      MapOfFacet::iterator f;
      int k;
      for ( f = mof.begin(), k = 0; f != mof.end(); f++, k++ )
      {
        vfac[ k ] = f->second;
        vfac[ k ]->id() = k;
      }

      // gets the coordinates of the center of each facet
      if( _verbose )
        cout << "vertices             " << flush;
      getVertices( vfac, surface.vertex(), 
                   thing.sizeX(), thing.sizeY(), thing.sizeZ() );        
      if( _verbose )
        cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\v" << flush;

      // smoothes the vertex coordinates
      getSmoothedVertices( vfac, surface.vertex(), 180.0, 5, 0.4, 0.4, 0.4 );

      // decimates the mesh
      float initialMeshSize = (float)vfac.size();
      float currentError = _deciMaxError / 1000.0;
      float currentAngle = 120.0;
      for ( int n = 0; n < 3; n++ )
      {
        if( _verbose ) cout << "pass " << n+1 << "/3 : " << flush;
        if ( vfac.size() < _minFacetNumber ) break; 
        getDecimatedVertices( vfac, surface.vertex(), 100.0,
                              _deciMaxClearance, currentError, currentAngle );
        if( _verbose ) cout << "\b\b\b\b\b\b\b\b\b\b\b\v" << flush;
        currentError *= 10.0;
        currentAngle -= 60.0;
      }
      float finalMeshSize = (float)vfac.size();

      // smoothes the vertex coordinates
      getSmoothedVertices( vfac, surface.vertex(), 180.0, 5, 0.2, 0.2, 0.2 );

      // gets the normals by taking the average of all the 
      // normals of the neighbors of a facet
      if( _verbose ) cout << "normals              " << flush;
      getNormals( vfac, surface.vertex(), surface.normal() );        
      if( _verbose )
      {
        cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\v" << flush;

        // gets the triangles with a SLAVE/MASTER algorithm
        cout << "triangles            " << flush;
      }
      getTriangles( vfac, surface.polygon() );
      if( _verbose )
      {
        cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\v" << flush;

        cout << "done ( " 
             << ( 1.0 - finalMeshSize / initialMeshSize ) * 100.0
             << "% reduction )" << endl;
      }
}



void	Mesher::getSingleLabel( const AimsData<short>& thing,
			AimsSurfaceTriangle &mesh)
{
  map< size_t, list< MapOfFacet > > interface;
  map< size_t, list< MapOfFacet > >::iterator	ii, ei = interface.end();

  getInterface( interface, thing );

  list< MapOfFacet >::iterator i;
  for ( ii=interface.begin(); ii!=ei; ++ii )
  {
    if ( !ii->second.size() ) continue;
    for ( i = ii->second.begin(); i != ii->second.end(); ++i )
    {
      AimsSurfaceTriangle surface;
      getMeshFromMapOfFacet(thing, surface, *i);      
      aims::SurfaceManip::meshMerge(mesh, surface);
    }
  }
  clear( interface ); 
}


void Mesher::smooth( AimsSurfaceTriangle& surface )
{
  vector< Facet* > vfac;
  getFacet( surface, vfac );

  surface.normal().clear();
  surface.polygon().clear();

  getSmoothedVertices( vfac, surface.vertex(), _featureAngle,
                       _nIteration, _xFactor, _yFactor, _zFactor );

  if( _verbose )
    cout << "normals              " << flush;
  getNormals( vfac, surface.vertex(), surface.normal() );        
  if( _verbose )
    {
      cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;

      cout << "triangles            " << flush;
    }
  getTriangles( vfac, surface.polygon() );
  if( _verbose )
    cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;

  for ( int n = 0; n < (int)vfac.size(); n++ )
    if ( vfac[ n ] )
      delete vfac[ n ];

  if( _verbose )
    cout << "         \b\b\b\b\b\b\b\b\b" << flush;
}


float Mesher::decimate( AimsSurfaceTriangle& surface )
{
  return decimate( surface, vector<float>(), 0 );
}


float Mesher::decimate( AimsSurfaceTriangle& surface,
                        const std::vector<float> & precthresholds,
                        const TimeTexture<float> & precisionmap )
{
  return decimate( surface, precthresholds, &precisionmap );
}


float Mesher::decimate( AimsSurfaceTriangle& surface,
                        const std::vector<float> & precthresholds,
                        const TimeTexture<float> *precisionmap )
{
  vector< Facet* > vfac;
  getFacet( surface, vfac );

  float initMeshSize = (float)vfac.size();

  surface.normal().clear();
  surface.polygon().clear();

  if ( _smoothFlag )
    getSmoothedVertices( vfac, surface.vertex(), _featureAngle,
                         _nIteration, _xFactor, _yFactor, _zFactor );

  if ( vfac.size() > _minFacetNumber ) 
    getDecimatedVertices( vfac, surface.vertex(), _deciReductionRate,
                          _deciMaxClearance, _deciMaxError, 
                          _deciFeatureAngle, precthresholds, precisionmap );

  float endMeshSize = (float)vfac.size();

  if( _verbose )
    cout << "normals              " << flush;
  getNormals( vfac, surface.vertex(), surface.normal() );        
  if( _verbose )
    {
      cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;

      cout << "triangles            " << flush;
    }
  getTriangles( vfac, surface.polygon() );
  if( _verbose )
    {
      cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;
      cout << "         \b\b\b\b\b\b\b\b\b" << flush;
    }

  for ( int n = 0; n < (int)vfac.size(); n++ )
    if ( vfac[ n ] )
      delete vfac[ n ];

  return ( 1.0 - endMeshSize / initMeshSize ) * 100.0;
}
