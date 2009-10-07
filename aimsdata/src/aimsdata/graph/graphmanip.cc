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

#include <cartobase/object/object_d.h>
#include <aims/graph/graphmanip_d.h>
#include <aims/resampling/motion.h>
#include <aims/io/aimsGraphR.h>
#include <aims/io/datatypecode.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/surfaceOperation.h>
#include <aims/mesh/texture.h>
#include <graph/graph/graph.h>
#include <cartobase/smart/rcptr.h>
#include <cartobase/type/string_conversion.h>
#include <vector>
#include <map>

using namespace aims;
using namespace carto;
using namespace std;

// GraphElementCode struct

GraphElementCode::GraphElementCode( const GraphElementCode & x )
  : id( x.id ), attribute( x.attribute ), objectType( x.objectType ), 
    dataType( x.dataType ), storageType( x.storageType ), 
    local_file_attribute( x.local_file_attribute ), 
    global_index_attribute( x.global_index_attribute ),
    global_filename( x.global_filename ), 
    global_attribute( x.global_attribute ), syntax( x.syntax )
{
}


GraphElementCode::GraphElementCode( const string & a, const string & ot, 
				    const string & dt, StorageType st ) 
  : id( a ), attribute( a ), objectType( ot ), dataType( dt ), 
    storageType( st )
{
  if( id.length() > 5 && a.substr( 0, 5 ) == "aims_" )
    id.erase( 0, 5 );
  if( id.rfind( string( "_" ) + ot ) != string::npos )
    id.erase( id.length() - id.length() - 1, id.length() + 1 );
}


GraphElementCode & GraphElementCode::operator = ( const GraphElementCode & x )
{
  if( this != &x )
    {
      id = x.id;
      attribute = x.attribute;
      objectType = x.objectType; 
      dataType = x.dataType;
      storageType = x.storageType; 
      local_file_attribute = x.local_file_attribute; 
      global_index_attribute = x.global_index_attribute; 
      global_filename = x.global_filename; 
      global_attribute = x.global_attribute;
      syntax = x.syntax;
    }
  return( *this );
}

// functions

namespace aims
{
  namespace DataManip
  {

    template<> 
    void adjustVoxelSize( BucketMap<Void> & x, const BucketMap<Void> & y )
    {
      x.setSizeXYZT( y.sizeX(), y.sizeY(), y.sizeZ(), y.sizeT() );
    }

    template<> 
    void adjustVoxelSize( Texture1d &, const Texture1d & )
    {
    }

    template<> 
    void adjustVoxelSize( Texture2d &, const Texture2d & )
    {
    }

    template<> 
    void adjustVoxelSize( TimeTexture<short> &, const TimeTexture<short> & )
    {
    }

    template<> 
    void adjustVoxelSize( AimsSurfaceTriangle &, const AimsSurfaceTriangle & )
    {
    }

    template<> 
    void adjustVoxelSize( AimsSurfaceFacet &, const AimsSurfaceFacet & )
    {
    }

    template<> 
    void adjustVoxelSize( AimsTimeSurface<2, Void> &, 
                          const AimsTimeSurface<2, Void> & )
    {
    }

    template<> 
    void adjustVoxelSize( AimsData<short> & x, const AimsData<short> & y )
    {
      x.setSizeXYZT( y.sizeX(), y.sizeY(), y.sizeZ(), y.sizeT() );
    }

    template<typename T> void 
    insertElement( T & dest, int index, const T & src )
    {
      if( src.empty() )
	dest[ index ];
      else
	dest[ index ] = src.begin()->second;
    }

    template<> void 
    insertElement( AimsData<short> & dest, int index, 
		   const AimsData<short> & src )
    {
      if( &dest == &src )
	return;
      short x, y, z;
      ForEach3d( src, x, y, z )
	if( src( x, y, z ) >= 0 )
	  dest( x, y, z ) = index;
    }


    template <> inline PropertySet & getHeader( AimsData<short> & obj )
    {
      return obj.volume()->header();
    }


    template <> inline PropertySet & getHeader( AimsSurfaceTriangle & obj )
    {
      return obj.header().getValue();
    }


    template <> inline PropertySet & getHeader( AimsSurfaceFacet & obj )
    {
      return obj.header().getValue();
    }


    template <> inline PropertySet & getHeader( AimsTimeSurface<2,
        Void> & obj )
    {
      return obj.header().getValue();
    }


    template <> inline PropertySet & getHeader( BucketMap<Void> & obj )
    {
      return obj.header().getValue();
    }


    template <> inline PropertySet & getHeader( Texture1d & obj )
    {
      return obj.header().getValue();
    }


    template <> inline PropertySet & getHeader( Texture2d & obj )
    {
      return obj.header().getValue();
    }


    template <> inline PropertySet & getHeader( TimeTexture<short> & obj )
    {
      return obj.header().getValue();
    }


    template<typename T> void setHeaderInfo( T & dest,
                                             const GenericObject & hdr )
    {
      PropertySet & hdest = getHeader( dest );
      Object x;
      string atts[] = { "referential", "referentials", "transformations" };
      for( unsigned i=0; i<3; ++i )
        try
        {
          x = hdr.getProperty( atts[i] );
          hdest.setProperty( atts[i], x );
        }
        catch( ... )
        {
        }
    }

    // compilation
    template void insertElement( BucketMap<Void> & dest, int index, 
				 const BucketMap<Void> & src );
    template void insertElement( Texture1d & dest, int index, 
				 const Texture1d & src );
    template void insertElement( Texture2d & dest, int index, 
				 const Texture2d & src );
    template void insertElement( TimeTexture<short> & dest, int index, 
				 const TimeTexture<short> & src );
    template void insertElement( AimsSurfaceTriangle & dest, int index, 
				 const AimsSurfaceTriangle & src );
    template void insertElement( AimsTimeSurface<2, Void> & dest, int index, 
				 const AimsTimeSurface<2, Void> & src );
    template void insertElement( AimsSurfaceFacet & dest, int index, 
				 const AimsSurfaceFacet & src );

    template PropertySet & getHeader( AimsData<short> & );
    template PropertySet & getHeader( BucketMap<Void> & );
    template PropertySet & getHeader( AimsSurfaceTriangle & );
    template PropertySet & getHeader( AimsTimeSurface<2, Void> & );
    template PropertySet & getHeader( AimsSurfaceFacet & );
    template PropertySet & getHeader( Texture1d & );
    template PropertySet & getHeader( Texture2d & );
    template PropertySet & getHeader( TimeTexture<short> & );

    template void setHeaderInfo( AimsData<short> &, const GenericObject & );
    template void setHeaderInfo( BucketMap<Void> &, const GenericObject & );
    template void setHeaderInfo( AimsSurfaceTriangle &,
                                 const GenericObject & );
    template void setHeaderInfo( AimsTimeSurface<2, Void> &,
                                 const GenericObject & );
    template void setHeaderInfo( AimsSurfaceFacet &, const GenericObject & );
    template void setHeaderInfo( Texture1d &, const GenericObject & );
    template void setHeaderInfo( Texture2d &, const GenericObject & );
    template void setHeaderInfo( TimeTexture<short> &, const GenericObject & );
  }
}

// GraphManip class

Motion GraphManip::talairach( const Graph & g )
{
  Motion		m;

  vector<float>	trans, rot, scl;
  if( g.getProperty( "Talairach_translation", trans ) 
      && g.getProperty( "Talairach_rotation", rot ) 
      && trans.size() == 3 && rot.size() == 9 )
    {
      // scale
      if( g.getProperty( "Talairach_scale", scl ) && scl.size() == 3 )
	{
	  rot[0] *= scl[0];
	  rot[1] *= scl[0];
	  rot[2] *= scl[0];
	  rot[3] *= scl[1];
	  rot[4] *= scl[1];
	  rot[5] *= scl[1];
	  rot[6] *= scl[2];
	  rot[7] *= scl[2];
	  rot[8] *= scl[2];
	}
      Point3df	& t = m.translation();
      t[0] = rot[0] * trans[0] + rot[1] * trans[1] + rot[2] * trans[2];
      t[1] = rot[3] * trans[0] + rot[4] * trans[1] + rot[5] * trans[2];
      t[2] = rot[6] * trans[0] + rot[7] * trans[1] + rot[8] * trans[2];

      AimsData<float>	& r = m.rotation();
      r(0,0) = rot[0];
      r(0,1) = rot[1];
      r(0,2) = rot[2];
      r(1,0) = rot[3];
      r(1,1) = rot[4];
      r(1,2) = rot[5];
      r(2,0) = rot[6];
      r(2,1) = rot[7];
      r(2,2) = rot[8];
    }

  return m;
}


void GraphManip::storeTalairach( Graph & g, const Motion & m )
{
  vector<float>		rot(9), scl(3, 1.), trans(3);
  Motion		minv = m.inverse();
  const AimsData<float>	& r = m.rotation();
  rot[0] = r( 0, 0 );
  rot[1] = r( 0, 1 );
  rot[2] = r( 0, 2 );
  rot[3] = r( 1, 0 );
  rot[4] = r( 1, 1 );
  rot[5] = r( 1, 2 );
  rot[6] = r( 2, 0 );
  rot[7] = r( 2, 1 );
  rot[8] = r( 2, 2 );
  Point3df	t = -minv.translation();
  trans[0] = t[0];
  trans[1] = t[1];
  trans[2] = t[2];
  g.setProperty( "Talairach_rotation", rot );
  g.setProperty( "Talairach_translation", trans );
  g.setProperty( "Talairach_scale", scl );
}


static string volumeProperty( const string &attribute )
{
  string			att = attribute;
  if( att.rfind( "_Bucket" ) == att.length() - 7 )
    att.erase( att.length() - 7, 7 );
  //att += "_Volume";
  return att;
}


static rc_ptr<AimsData<short> >
retreiveVol( Graph & g, const string & attribute, const Point3d & dims, 
	     const vector<float> & vs )
{
  rc_ptr<AimsData<short> >	vol;
  string			att = volumeProperty( attribute );
  if( !g.getProperty( att, vol ) )
    {
      // cout << "creating volume in attrib " << att << endl;
      vol = rc_ptr<AimsData<short> >
	( new AimsData<short>( dims[0], dims[1], dims[2] ) );
      *vol = -1;
      PythonHeader	*ph = new PythonHeader;
      vol->setHeader( ph );
      vol->setSizeXYZT( vs[0], vs[1], vs[2] );
      ph = static_cast<PythonHeader *>( vol->header() );
      vector<float>	org;
      if( g.getProperty( "origin", org ) && org.size() >= 3 )
        ph->setProperty( "origin", org );
      int	norm = 0;
      if( g.getProperty( "spm_normalized", norm ) )
        ph->setProperty( "spm_normalized", (bool) norm );
      g.setProperty( att, vol );
    }
  return vol;
}


static AimsData<short> *
retreiveVol2( Graph & g, const string & attribute )
{
  rc_ptr<AimsData<short> >	vol;
  string			att = attribute;
  /*if( att.rfind( "_Bucket" ) == att.length() - 7 )
    att.erase( att.length() - 7, 7 );
    att += "_Volume";*/
  if( !g.getProperty( att, vol ) )
    return 0;
  return &*vol;
}


namespace
{

  BucketMap<Void> * createBucket( GraphObject* ao, const string & att )
  {
    BucketMap<Void>	*bck = new BucketMap<Void>;
    ao->setProperty( att, rc_ptr<BucketMap<Void> >( bck ) );
    return bck;
  }

}


namespace aims
{
  namespace internal_graphmanip
  {
    struct ConversionStruct
    {
      struct Delay
      {
	Delay() : gec( 0 ) {}
	Delay( const string & s, GraphElementCode & g ) 
	  : syntax( s ), gec( &g )
	{}
	string		syntax;
	GraphElementCode	*gec;
      };

      Graph			*graph;
      map<GraphObject *, Delay>	delayed;
      map<string, set<int> >	usedatt;
      set<GraphElementCode *>	gecs;
      Point3d			dims;
      Point3d			offset;
      vector<float>		vs;
      GraphElementTable		get;
      map<AimsData<short> *, map<int, BucketMap<Void>::Bucket *> > 
      buckets;
      GraphManip::CreateBucketFunc	createfunc;

      void bucket2Vol( GraphObject* );
      void vol2Bucket( GraphObject* );
    };

  }
}

using namespace aims::internal_graphmanip;

void ConversionStruct::bucket2Vol( GraphObject* ao )
{
  string					synt = ao->getSyntax();
  GraphElementTable::iterator			imgec = get.find( synt );
  map<string, GraphElementCode>::iterator	igec, egec;
  rc_ptr<BucketMap<Void> >			bck;
  BucketMap<Void>::Bucket::iterator		ib, eb;
  int						index;

  if( imgec != get.end() )
    {
      for( igec=imgec->second.begin(), egec=imgec->second.end(); 
	   igec!=egec; ++igec )
	{
	  GraphElementCode			& gec = igec->second;
	  if( ao->getProperty( gec.attribute, bck ) )
	    {
	      if( !ao->getProperty( gec.global_index_attribute, index ) )
		delayed[ ao ] = Delay( synt, gec );
	      else
		{
		  /*cout << "elem index " << index << ", att: " 
		    << gec.attribute << endl;*/
		  rc_ptr<AimsData<short> >	vol 
		    = retreiveVol( *graph, gec.attribute, dims, vs );
		  // print bucket to volume
		  for( ib=bck->begin()->second.begin(), 
			 eb=bck->begin()->second.end(); ib!=eb; ++ib )
		    (*vol)( ib->first - offset ) = (short) index;
		  usedatt[ gec.attribute ].insert( index );
		  ao->removeProperty( gec.attribute );
		}
	      gecs.insert( &gec );
	    }
	}
    }
}


void ConversionStruct::vol2Bucket( GraphObject* ao )
{
  string					synt = ao->getSyntax();
  GraphElementTable::iterator			imgec = get.find( synt );
  map<string, GraphElementCode>::iterator	igec, egec;
  AimsData<short>				*vol;
  int						index;

  if( imgec != get.end() )
    {
      for( igec=imgec->second.begin(), egec=imgec->second.end(); 
	   igec!=egec; ++igec )
	{
	  GraphElementCode			& gec = igec->second;
	  if( ao->getProperty( gec.global_index_attribute, index ) )
	    {
	      vol = retreiveVol2( *graph, gec.global_attribute );
	      if( vol )
		{
		  // print volume to bucket
                  BucketMap<Void>	*bck = createfunc( ao, gec.attribute );
		  bck->setSizeXYZT( vol->sizeX(), vol->sizeY(), 
				    vol->sizeZ(), 1 );
		  BucketMap<Void>::Bucket	& b = (*bck)[0];
		  buckets[vol][index] = &b;
		  usedatt[ gec.attribute ].insert( index );
		  //ao->setProperty( gec.attribute, bck );
		  gecs.insert( &gec );
		}
	      else
		cerr << "Global volume not found: " 
		     << gec.global_attribute << "\n";
	    }
	}
    }
}


bool GraphManip::buckets2Volume( Graph & g )
{
  rc_ptr<GraphElementTable>	mgec;
  if( !g.getProperty( "aims_objects_table", mgec ) )
    return true;	// no elements to convert

  ConversionStruct				cs;
  GraphElementTable::iterator			imgec, emgec = mgec->end();
  map<string, GraphElementCode>::iterator	igec, egec;
  DataTypeCode<BucketMap<Void> >		dtc;

  int   loaded = 0;
  g.getProperty( "aims_reader_loaded_objects", loaded );

  for( imgec=mgec->begin(); imgec!=emgec; ++imgec )
    for( igec=imgec->second.begin(), egec=imgec->second.end(); igec!=egec; 
         ++igec )
    {
      GraphElementCode	& gec = igec->second;
      if( gec.objectType == dtc.objectType()
          && gec.dataType == dtc.dataType() )
        {
          if( loaded && loaded != 3 )
            return false; // all buckets may not have been read from disk yet
          cout << "convert synt: " << imgec->first << ", id: " << igec->first
                << ", attr: " << gec.attribute << endl;
          cs.get[ imgec->first ][ igec->first ] = gec;
        }
    }

  vector<int>				vdims;
  Graph::iterator			ig, eg = g.end();

  cs.graph = &g;

  if( !g.getProperty( "boundingbox_max", vdims ) )
    {
      cerr << "No boundingbox in graph - conversion not possible\n";
      return false;	// no dimensions
    }
  cs.dims = Point3d( vdims[0]+1, vdims[1]+1, vdims[2]+1 );
  if( g.getProperty( "boundingbox_min", vdims ) )
    {
      cs.offset = Point3d( vdims[0], vdims[1], vdims[2] );
      cs.dims -= cs.offset;
    }
  if( !g.getProperty( "voxel_size", cs.vs ) )
    {
      cs.vs.push_back( 1 );
      cs.vs.push_back( 1 );
      cs.vs.push_back( 1 );
    }

  //cout << "starting conversion...\n";
  for( ig=g.begin(); ig!=eg; ++ig )
    cs.bucket2Vol( *ig );

  // relations
  const set<Edge *>		&edg = g.edges();
  set<Edge *>::const_iterator	ie, ee = edg.end();
  for( ie=edg.begin(); ie!=ee; ++ie )
    cs.bucket2Vol( *ie );

  //cout << "DELAYED ELEMENTS...\n";
  // process delayed elements
  map<GraphObject *, ConversionStruct::Delay>::iterator 
    id, ed = cs.delayed.end();
  set<int>::iterator			iu, eu;
  rc_ptr<BucketMap<Void> >		bck;
  BucketMap<Void>::Bucket::iterator	ib, eb;
  int					index;

  for( id=cs.delayed.begin(); id!=ed; ++id )
    {
      GraphElementCode			& gec = *id->second.gec;
      id->first->getProperty( gec.attribute, bck );
      set<int>				& used = cs.usedatt[ gec.attribute ];
      index = 1;
      iu = used.begin();
      eu = used.end();
      if( iu != eu && *iu == 0 )
        index = 0;
      for( ; iu!=eu && index==*iu; ++index, ++iu ) {}
      used.insert( index );

      rc_ptr<AimsData<short> >	vol 
	= retreiveVol( g, gec.attribute, cs.dims, cs.vs );
      // print bucket to volume
      for( ib=bck->begin()->second.begin(), 
	     eb=bck->begin()->second.end(); ib!=eb; ++ib )
	(*vol)( ib->first - cs.offset ) = (short) index;
      id->first->removeProperty( gec.attribute );
      id->first->removeProperty( gec.local_file_attribute );
    }

  // check for min value (0 or -1)
  map<string, set<int> >::const_iterator	iua, eua = cs.usedatt.end();
  for( iua = cs.usedatt.begin(); iua != eua; ++iua )
    if( !iua->second.empty() && *iua->second.begin() > 0 )
      {
        // change background to 0
        rc_ptr<AimsData<short> >	vol;
        string				att = volumeProperty( iua->first );
        int				x, y, z;
        if( g.getProperty( att, vol ) )
          ForEach3d( (*vol), x, y, z )
          {
            short	& val = (*vol)( x, y, z );
            if( val == -1 )
              val = 0;
          }
      }

  //cout << "global attributes...\n";
  // update global attributes
  set<GraphElementCode *>::iterator	igecs, egecs = cs.gecs.end();
  DataTypeCode<AimsData<short> >	dtcv;
  string				att;

  for( igecs=cs.gecs.begin(); igecs!=egecs; ++igecs )
    {
      GraphElementCode	& gec1 = **igecs;
      GraphElementCode	& gec = (*mgec)[ gec1.syntax ][ gec1.id ];
      gec.storageType = GraphElementCode::GlobalPacked;
      gec.objectType = dtcv.objectType();
      gec.dataType = dtcv.dataType();
      gec.global_filename = gec.id + "_Volume.ima";
      gec.global_attribute =  volumeProperty( gec.attribute );
      /*cout << "GEC: id: " << gec.id << endl;
      cout << "     objectType: " << gec.objectType << endl;
      cout << "     global_filename: " << gec.global_filename << endl;
      cout << "     global_attribute: " << gec.global_attribute << endl;
      cout << "     global_index_attribute: " << gec.global_index_attribute 
	   << endl;
	   cout << "     attribute:" << gec.attribute << endl;*/
      att = gec.id + ".global.bck";
      if( g.hasProperty( att ) )
	g.removeProperty( att );
      att = gec.id + ".bck";
      if( g.hasProperty( att ) )
	g.removeProperty( att );
    }

  // empty (unchanged) GECs
  for( imgec=cs.get.begin(), emgec=cs.get.end(); imgec!=emgec; ++imgec )
    for( igec=imgec->second.begin(), egec=imgec->second.end(); igec!=egec; 
	 ++igec )
      {
	GraphElementCode	& geco = igec->second;
	GraphElementCode	& gec = (*mgec)[ geco.syntax ][ geco.id ];
	if( gec.objectType != dtcv.objectType() )
	  {
	    gec.storageType = GraphElementCode::GlobalPacked;
	    gec.objectType = dtcv.objectType();
	    gec.dataType = dtcv.dataType();
	    gec.global_filename = gec.id + "_Volume.ima";
	    gec.global_attribute =  volumeProperty( gec.attribute );
	  }
      }

  if( g.hasProperty( "type.bck" ) )
    g.removeProperty( "type.bck" );
  if( g.hasProperty( "type.global.bck" ) )
    g.removeProperty( "type.global.bck" );
  //cout << "volume attributes...\n";
  // build volume attributes
  string	types;
  for( imgec=mgec->begin(), emgec=mgec->end(); imgec!=emgec; ++imgec )
    for( igec=imgec->second.begin(), egec=imgec->second.end(); igec!=egec; 
	 ++igec )
      {
	GraphElementCode	& gec = igec->second;
	if( gec.objectType == dtcv.objectType() 
	    && gec.dataType == dtcv.dataType() )
	  {
	    if( !types.empty() )
	      types += ' ';
	    types += gec.id + ".global.vol";
	    g.setProperty( gec.id + ".global.vol", 
			    gec.syntax + ' ' + gec.attribute + ' ' 
			    + gec.global_index_attribute );
	  }
      }

  return true;
}


bool GraphManip::volume2Buckets( Graph & g, CreateBucketFunc createfunc )
{
  rc_ptr<GraphElementTable>	mgec;
  if( !g.getProperty( "aims_objects_table", mgec ) )
    return true;	// no elements to convert

  ConversionStruct				cs;
  GraphElementTable::iterator			imgec, emgec = mgec->end();
  map<string, GraphElementCode>::iterator	igec, egec;
  DataTypeCode<AimsData<short> >		dtc;

  int   loaded = 0;
  g.getProperty( "aims_reader_loaded_objects", loaded );

  if( createfunc == 0 )
    createfunc = &createBucket;
  cs.createfunc = createfunc;

  for( imgec=mgec->begin(); imgec!=emgec; ++imgec )
    for( igec=imgec->second.begin(), egec=imgec->second.end(); igec!=egec; 
         ++igec )
    {
      GraphElementCode	& gec = igec->second;
      if( gec.objectType == dtc.objectType()
          && gec.dataType == dtc.dataType() )
        {
          if( loaded && loaded != 3 )
            return false; // all volumes may not have been read from disk yet
          cout << "convert synt: " << imgec->first << ", id: " << igec->first
                << ", attr: " << gec.attribute << endl;
          cs.get[ imgec->first ][ igec->first ] = gec;
        }
    }

  vector<int>				vdims;
  Graph::iterator			ig, eg = g.end();

  cs.graph = &g;

  if( g.getProperty( "boundingbox_min", vdims ) )
    cs.offset = Point3d( vdims[0], vdims[1], vdims[2] );

  //cout << "starting conversion...\n";

  if( !loaded || ( loaded & 1 ) )
    for( ig=g.begin(); ig!=eg; ++ig )
      cs.vol2Bucket( *ig );

  // relations
  if( !loaded || ( loaded & 2 ) )
  {
    const set<Edge *>		&edg = g.edges();
    set<Edge *>::const_iterator	ie, ee = edg.end();
    for( ie=edg.begin(); ie!=ee; ++ie )
      cs.vol2Bucket( *ie );
  }

  // fill in buckets, voume by volume
  //cout << "filling buckets...\n";
  map<AimsData<short> *, map<int, BucketMap<Void>::Bucket *> >::iterator 
    ibm, ebm = cs.buckets.end();
  map<int, BucketMap<Void>::Bucket *>::iterator	isbm, esbm;
  short						x, y, z;

  for( ibm=cs.buckets.begin(); ibm!=ebm; ++ibm )
  {
    AimsData<short>	& volu = *ibm->first;
    map<int, BucketMap<Void>::Bucket *>	& bcks = ibm->second;
    esbm = bcks.end();
    ForEach3d( volu, x, y, z )
    {
      isbm = bcks.find( (int) volu( x, y, z ) );
      if( isbm != esbm )
        (*isbm->second)[ cs.offset + Point3d( x, y, z ) ];
    }
  }

  //cout << "\nUpdating global attributes...\n";

  // update global attributes
  set<GraphElementCode *>::iterator	igecs, egecs = cs.gecs.end();
  DataTypeCode<BucketMap<Void> >	dtcv;
  string				att;
  for( igecs=cs.gecs.begin(); igecs!=egecs; ++igecs )
    {
      GraphElementCode	& gec1 = **igecs;
      GraphElementCode	& gec = (*mgec)[ gec1.syntax ][ gec1.id ];
      gec.storageType = GraphElementCode::Global;
      gec.objectType = dtcv.objectType();
      gec.dataType = dtcv.dataType();
      gec.global_filename = gec.id + "_Bucket.bck";
      if( g.hasProperty( gec.id + ".global.vol" ) )
	g.removeProperty( gec.id + ".global.vol" );
      att = gec.global_attribute;
      if( g.hasProperty( att ) )
	g.removeProperty( att );
    }

  // empty (unchanged) GECs
  for( imgec=cs.get.begin(), emgec=cs.get.end(); imgec!=emgec; ++imgec )
    for( igec=imgec->second.begin(), egec=imgec->second.end(); igec!=egec; 
	 ++igec )
      {
	GraphElementCode	& geco = igec->second;
	GraphElementCode	& gec = (*mgec)[ geco.syntax ][ geco.id ];
	if( gec.objectType != dtcv.objectType() )
	  {
	    gec.storageType = GraphElementCode::GlobalPacked;
	    gec.objectType = dtcv.objectType();
	    gec.dataType = dtcv.dataType();
	    gec.global_filename = gec.id + "_Volume.ima";
	    gec.global_attribute =  volumeProperty( gec.attribute );
	  }
      }

  if( g.hasProperty( "type.global.vol" ) )
    g.removeProperty( "type.global.vol" );
  // build bucket attributes
  string	types;
  for( imgec=mgec->begin(), emgec=mgec->end(); imgec!=emgec; ++imgec )
    for( igec=imgec->second.begin(), egec=imgec->second.end(); igec!=egec; 
	 ++igec )
      {
	GraphElementCode	& gec = igec->second;
	if( gec.objectType == dtcv.objectType() 
	    && gec.dataType == dtcv.dataType() )
	  {
	    if( !types.empty() )
	      types += ' ';
	    types += gec.id + ".global.bck";
	    g.setProperty( gec.id + ".global.bck", 
			    gec.syntax + ' ' + gec.global_filename + ' ' 
			    + gec.global_index_attribute );
	  }
      }

  //cout << "conversion done.\n";
  return true;
}


GraphElementCode & 
GraphManip::graphElementCode( Graph & g, const string & syntax, 
			      const string & id )
{
  rc_ptr<GraphElementTable>	get;
  if( !g.getProperty( "aims_objects_table", get ) )
    {
      get = rc_ptr<GraphElementTable>( new GraphElementTable );
      g.setProperty( "aims_objects_table", get );
    }
  return (*get)[ syntax ][ id ];
}


GraphElementCode & 
GraphManip::graphElementCodeByAtt( Graph & g, const string & syntax, 
				   const string & attrib )
{
  rc_ptr<GraphElementTable>	get;
  if( !g.getProperty( "aims_objects_table", get ) )
    {
      get = rc_ptr<GraphElementTable>( new GraphElementTable );
      g.setProperty( "aims_objects_table", get );
    }

  map<string, GraphElementCode>	& mec = (*get)[ syntax ];
  map<string, GraphElementCode>::iterator	ic, ec = mec.end();
  for( ic=mec.begin(); ic!=ec && ic->second.attribute!=attrib; ++ic ) {}
  if( ic != ec )
    return ic->second;

  string	id = attrib;
  if( id.length() > 5 && id.substr( 0, 5 ) == "aims_" )
    id.erase( 0, 5 );
  GraphElementCode	& gec = (*get)[ syntax ][ id ];
  gec.attribute = attrib;
  gec.id = id;
  gec.syntax = syntax;
  gec.storageType = GraphElementCode::Global;
  gec.local_file_attribute = attrib + "_filename";
  gec.global_attribute = attrib + "_label";
  gec.global_filename = attrib;
  gec.global_attribute = attrib;
  return gec;
}


Graph* GraphManip::graphFromVolume( const AimsData<short> & vol,
				    short background  , map<short,string> *trans)
{
  Graph	*g = new Graph( "RoiArg" );
  graphFromVolume( vol, *g, background , trans);
  return g;
}


void GraphManip::graphFromVolume( const AimsData<short> & vol, Graph & g, 
				  short background,  map<short,string> *trans , bool automaticBackgroundSearch )
{
  g.setSyntax( "RoiArg" );
  // global attributes
  vector<float>	vs(3);
  vs[0] = vol.sizeX();
  vs[1] = vol.sizeY();
  vs[2] = vol.sizeZ();
  g.setProperty( "voxel_size", vs );
  vector<int>	dims(3);
  dims[0] = 0;
  dims[1] = 0;
  dims[2] = 0;
  g.setProperty( "boundingbox_min", dims );
  dims[0] = vol.dimX() - 1;
  dims[1] = vol.dimY() - 1;
  dims[2] = vol.dimZ() - 1;
  g.setProperty( "boundingbox_max", dims );
  
  rc_ptr<GraphElementTable>	mgec( new GraphElementTable );
  GraphElementCode	& gec = (*mgec)[ "roi" ][ "roi" ];
  g.setProperty( "aims_objects_table", mgec );
  DataTypeCode<AimsData<short> >	dtcv;
  gec.id = "roi";
  gec.attribute = "aims_roi";
  gec.objectType = dtcv.objectType();
  gec.dataType = dtcv.dataType();
  gec.storageType = GraphElementCode::GlobalPacked;
  gec.global_filename = gec.id + "_Volume.ima";
  gec.global_attribute =  volumeProperty( gec.attribute );
  gec.global_index_attribute = "roi_label";
  gec.syntax = "roi";

  rc_ptr<AimsData<short> >	volume( new AimsData<short>( vol ) );
  if( vol.header() )
    volume->setHeader( vol.header()->cloneHeader() );
  g.setProperty( gec.global_attribute, volume );
  map<short,string>::iterator im, em ;
  
  if (trans != NULL)
    em = (*trans).end(); 
  
  // build nodes
  int			x, y, z;
  map<short, Vertex *>	nodes;
  short			label;
  if( automaticBackgroundSearch )
     background = vol.minimum() ;
  ForEach3d( vol, x, y, z )
    {
      label = vol( x, y, z );
      if( label != background )
	{
	  Vertex	*& v = nodes[ label ];
	  if( !v )
	    {
	      v = g.addVertex( "roi" );
	      if (trans == NULL)
		v->setProperty( "name", string( "label_" ) + 
				 toString( label ) );      
	      else
		{
		  im = (*trans).find(label);
		  if (im == em)
		    {
		      cout << "The label " << label << 
			" does not have a corresponding (string) name \n";
		      v->setProperty( "name", string( "label_" ) +
				       toString( label ) ); 
		    }
		  else
		      v->setProperty( "name", im->second );
		   
		}
	      v->setProperty( "roi_label", (int) label );
	    }
	}
    }
}


namespace
{

  void getArea( const AttributedObject* ao, rc_ptr<GraphElementTable> mgec, 
                float & area, float & narea, const Motion* mtal )
  {
    GraphElementTable::iterator			imgec, emgec = mgec->end();
    map<string, GraphElementCode>::iterator	igec, egec;
    DataTypeCode<AimsSurfaceTriangle>		mdtc;
    rc_ptr<AimsSurfaceTriangle>			mesh;

    area = 0;
    narea = 0;
    imgec = mgec->find( ao->getSyntax() );
    if( imgec != emgec )
      for( igec=imgec->second.begin(), egec=imgec->second.end(); 
           igec!=egec; ++igec )
        {
          // we could pre-filter GECs to keep only those of type mesh
          // this would need fewer tests for each node
          GraphElementCode	& gec = igec->second;
          if( gec.objectType == mdtc.objectType() 
              && gec.dataType == mdtc.dataType() 
              && ao->getProperty( gec.attribute, mesh ) 
              && mesh.get() )
            {
              area += SurfaceManip::meshArea( *mesh );
              if( mtal )
                {
                  AimsSurfaceTriangle	mcopy = *mesh;
                  SurfaceManip::meshTransform( mcopy, *mtal );
                  narea += SurfaceManip::meshArea( mcopy );
                }
            }
        }
  }


 void getSize( const AttributedObject* ao, rc_ptr<GraphElementTable> mgec, 
               float & size, float & nsize, const float vvol,const float nvvol )
  {
    GraphElementTable::iterator			imgec, emgec = mgec->end();
    map<string, GraphElementCode>::iterator	igec, egec;
    DataTypeCode<BucketMap<Void> >		dtc;
    rc_ptr<BucketMap<Void > > bucket(new BucketMap<Void >);

    size = 0;
    nsize = 0;
    imgec = mgec->find( ao->getSyntax() );

   
    if( imgec != emgec )
      for( igec=imgec->second.begin(), egec=imgec->second.end(); 
           igec!=egec; ++igec )
        {
          // we could pre-filter GECs to keep only those of type mesh
          // this would need fewer tests for each node
          GraphElementCode	& gec = igec->second;
          if( gec.objectType == dtc.objectType() 
              && gec.dataType == dtc.dataType() 
              && ao->getProperty( gec.attribute, bucket ) 
              && bucket.get() )
            {
              //size += bucket->sizeX() * bucket->sizeY() * bucket->sizeZ() * ((bucket->begin())->second).size() ;
              size += vvol * ((bucket->begin())->second).size() ;
              if( nvvol != 0 )
                  nsize = nvvol * ((bucket->begin())->second).size() ;
              
            }
        }
  }

}


void GraphManip::completeGraph( Graph & g )
{
  rc_ptr<GraphElementTable>	mgec;
  if( !g.getProperty( "aims_objects_table", mgec ) )
    return;

  float						area, narea;
  float						size, nsize;
  float						vvol,nvvol = 0;
  Graph::iterator				ig, eg = g.end();
  Motion					mtal = talairach( g );
  bool	normalized = !mtal.isIdentity();
  vector<float>	vs(3), nvs(3);

  g.getProperty( "voxel_size", vs );
  vvol = vs[0]*vs[1]*vs[2];
  if (normalized)
    {
      Point3df	p0( 0, 0, 0 );
      p0 = mtal.transform( p0 );
      nvs[0] = ( mtal.transform( Point3df( vs[0], 0, 0 ) ) - p0 ).norm();
      nvs[1] = ( mtal.transform( Point3df( 0, vs[1], 0 ) ) - p0 ).norm();
      nvs[2] = ( mtal.transform( Point3df( 0, 0, vs[2] ) ) - p0 ).norm();
      nvvol = nvs[0] * nvs[1] * nvs[2]; 
    }

  for( ig=g.begin(); ig!=eg; ++ig )
    {
      getArea( *ig, mgec, area, narea, normalized ? &mtal : 0 );
      if( area > 0 )
	(*ig)->setProperty( "surface_area", area );
      if( narea > 0 )
	(*ig)->setProperty( "refsurface_area", narea );

      getSize( *ig, mgec, size, nsize, vvol, normalized ? nvvol : 0 );
      if( size > 0 )
	(*ig)->setProperty( "size", size );
      if( nsize > 0 )
	(*ig)->setProperty( "refsize", nsize );
    }
}



//TODO! attention les maillages d'un de graphes est modifie !!
Graph*  GraphManip::mergeGraph( const string & key,  Graph & h,  Graph &g, 
                                bool Merge, bool invNormal )
{
  string syntax;
  syntax = h.getSyntax();
  if (syntax.empty() )
    cerr << "Missing syntax \n";

  //m = h
  Graph *m = new Graph(syntax);
  h.extract(*m,h.begin(),h.end());
  m->copyProperties( Object::reference( 
    h.value<AttributedObject::ContentType>() ) );

  m->copyProperties( Object::reference( 
    g.value<AttributedObject::ContentType>() ) );

  //Merging GraphElementTable
  cout << "Merging the two GraphElementTable..." << std::flush;
  rc_ptr<GraphElementTable>	g_get,h_get;
  if( !g.getProperty( "aims_objects_table", g_get ) )
    cerr << "Missing GraphElementTable \n";
  if( !h.getProperty( "aims_objects_table", h_get ) )
    cerr << "Missing GraphElementTable \n";

  GraphElementTable::iterator			ig_get, eg_get = g_get->end();
  map<string, GraphElementCode>::iterator	iget, eget;

  rc_ptr<GraphElementTable> m_get(new GraphElementTable);
  *m_get = *h_get;

  for (ig_get = g_get->begin(); ig_get != eg_get; ++ig_get )
    for( iget = (ig_get->second).begin(), eget = (ig_get->second).end(); iget != eget; ++iget )
      (*m_get)[ig_get->first][iget->first] = iget->second;

  cout << "done\n";

  DataTypeCode<AimsSurfaceTriangle>		mdtc;
  DataTypeCode<BucketMap<Void> >		bdtc;
  GraphElementTable::iterator			imgec, emgec = m_get->end();
  map<string, GraphElementCode>::iterator	igec, egec;


  //Copying the vertex
  cout << "Copying the vertices..." << endl;
  Graph::iterator ig,eg = g.end(); 
  Graph::iterator im,em = m->end(); 
  map<string,Vertex *> nodem;
  map<string,Vertex *>::iterator in, en = nodem.end();
  Vertex *vm = 0;
  Vertex *vg;
  Object X;
  string keyValue;
 
  m->setProperty("aims_objects_table", m_get);

  for ( im = m->begin(); im != em; ++im )
    {
      vm = *im;
      try
	{      
	  X = vm->getProperty( key );
	  keyValue = X->getString();
	  nodem[keyValue] = vm;
	}
      catch( exception & e )
	{
	  cerr << e.what() << endl;
	  throw( e );
	}
    }
  
  bool matching;

  for ( ig = g.begin(); ig != eg; ++ig )
    {
      matching = true;
      vg = *ig;
      
      //vg->getProperty(key,keyValue);
      try
	{ 
          X = vm->getProperty( key );
          keyValue = X->getString();
        }
     

      catch( exception & e )
	{
	  cerr << "Can find the value of " << key << " in the current vertex." << endl;
          matching = false;	}

      if (matching)
        in = nodem.find(keyValue);
      else
        in = en;
     

      rc_ptr<AimsSurfaceTriangle>	mesh1(new AimsSurfaceTriangle), 
	mesh2(new AimsSurfaceTriangle),merge(new AimsSurfaceTriangle);
      rc_ptr<BucketMap<Void > > bucket1(new BucketMap<Void >), 
	bucket2(new BucketMap<Void >), bucketM(new BucketMap<Void >);  
      BucketMap<Void>::Bucket::iterator ib,eb; 
      if (in != en)
	{
	  vm = in->second;
	  vm->copyProperties( Object::reference(
	    vg->value<AttributedObject::ContentType>() ) );

	  if (Merge)
	    {
	      imgec = m_get->find( (*ig)->getSyntax() );
	      if( imgec != emgec )
		for( igec=imgec->second.begin(), egec=imgec->second.end(); 
		     igec!=egec; ++igec )
		  {
		    // we could pre-filter GECs to keep only those of type mesh
		    // this would need fewer tests for each node
		    GraphElementCode	& gec = igec->second;
		    //Mesh
		    if( gec.objectType == mdtc.objectType() 
			&& gec.dataType == mdtc.dataType() 
			&& vg->getProperty( gec.attribute, mesh1 ) 
			&& mesh1.get() 
			&& vm->getProperty( gec.attribute, mesh2 )
			&& mesh2.get() )
		    
		      {
			mesh1 = rc_ptr<AimsSurfaceTriangle >(new AimsSurfaceTriangle(*mesh1));
			vector<Point3df>		& normal = mesh2->normal();
			vector<Point3df>::iterator in,en = normal.end();
			if (invNormal)
			  for (in = normal.begin(); in != en; ++in)
			    {
			      Point3df & norm = *in;
			      norm[0] *= -1;
			      norm[1] *= -1;
			      norm[2] *= -1;  
			    }
 
			SurfaceManip::meshMerge(*mesh1,*mesh2);
			vm->setProperty( gec.attribute, mesh1 ); 

		      }
		    //Bucket
		    else
		      if( gec.objectType == bdtc.objectType() 
			  && gec.dataType == bdtc.dataType() 
			  && vg->getProperty( gec.attribute, bucket1 ) 
			  && bucket1.get() 
			  && vm->getProperty( gec.attribute, bucket2 )
			  && bucket2.get() )
			{
			  
			  bucket1 = rc_ptr<BucketMap<Void > >
			    (new BucketMap<Void >(*bucket1));
			  for (ib = (*bucket2)[0].begin(), 
			       eb = (*bucket2)[0].end(); ib != eb; ++ib)
			    (*bucket1)[0][ib->first] = ib->second;
			  
			  vm->setProperty( gec.attribute, bucket1 ); 
			} 
			
		  }
	    }
	      
	}
      else
	{
	  cout << "Cannot translate the (" << key << ") whose value is" << keyValue << endl;
	  syntax = vg->getSyntax();
	  vm = m->addVertex(syntax);
	  vm->copyProperties( Object::reference
	    ( vg->value<AttributedObject::ContentType>() ) );
	}
   
    }
   
      
    
    cout << "done\n";

    //TODO : Copy of the node relations...

    return(m);
}


void GraphManip::printGraphElementTable( const Graph & g, ostream & out )
{
  rc_ptr<GraphElementTable> get;
  if( !g.getProperty( "aims_objects_table", get ) )
    cout << "no aims_objects_table\n";
  else
  {
    GraphElementTable::const_iterator iget, eget = get->end();
    map< string, GraphElementCode >::const_iterator igec, egec;
    for( iget=get->begin(); iget!=eget; ++iget )
    {
      out << iget->first << endl;
      for( igec=iget->second.begin(), egec=iget->second.end(); igec!=egec;
           ++igec )
      {
        out << "    " << igec->first << endl;
        out << "        id: " << igec->second.id << endl;
        out << "        attribute: " << igec->second.attribute << endl;
        out << "        objectType: " << igec->second.objectType << endl;
        out << "        dataType: " << igec->second.dataType << endl;
        out << "        storageType: ";
        switch( igec->second.storageType )
        {
          case GraphElementCode::Local:
            out << "Local" << endl;
            break;
          case GraphElementCode::Global:
            out << "Global" << endl;
            break;
          case GraphElementCode::GlobalPacked:
            out << "GlobalPacked" << endl;
            break;
        }
        out << "        local_file_attribute: "
            << igec->second.local_file_attribute << endl;
        out << "        global_index_attribute: "
            << igec->second.global_index_attribute << endl;
        out << "        global_filename: " << igec->second.global_filename
            << endl;
        out << "        global_attribute: " << igec->second.global_attribute
            << endl;
        out << "        syntax: " << igec->second.syntax << endl;
      }
    }
  }
}


void GraphManip::setAttributeColor( Graph & graph, const string & att,
                                    const AimsRGB & col )
{
  vector<int> c( 3 );
  c[0] = col[0];
  c[1] = col[1];
  c[2] = col[2];
  setAttributeColor( graph, att, c );
}


void GraphManip::setAttributeColor( Graph & graph, const string & att,
                                    const AimsRGBA & col )
{
  vector<int> c( 4 );
  c[0] = col[0];
  c[1] = col[1];
  c[2] = col[2];
  c[3] = col[3];
  setAttributeColor( graph, att, c );
}


void GraphManip::setAttributeColor( Graph & graph, const string & att,
                                    const vector<int> & col )
{
  string attname = att;
  if( attname.length() >= 6
      && attname.substr( attname.length()-6, 6 ) == "_label" )
    attname = attname.substr( 0, attname.length()-6 );
  else if( attname.length() >= 9
           && attname.substr( attname.length()-9, 9 ) == "_filename" )
    attname = attname.substr( 0, attname.length()-9 );
  string attfname = attname + "_filename";
  attname += "_label";
  rc_ptr<map<string, vector<int> > > cols;
  if( !graph.getProperty( "object_attributes_colors", cols ) )
    cols = rc_ptr<map<string, vector<int> > >( new map<string, vector<int> > );
  (*cols)[attname] = col;
  (*cols)[attfname] = col;
  graph.setProperty( "object_attributes_colors", cols );
  graph.setProperty( attname, col );
  graph.setProperty( attfname, col );
}


vector<int> GraphManip::attributeColor( const Graph & graph,
                                        const string & att )
{
  string attname = att;
  if( attname.length() >= 6
      && attname.substr( attname.length()-6, 6 ) == "_label" )
    attname = attname.substr( 0, attname.length()-6 );
  else if( attname.length() >= 9
           && attname.substr( attname.length()-9, 9 ) == "_filename" )
    attname = attname.substr( 0, attname.length()-9 );
  vector<int> col;
  rc_ptr<map<string, vector<int> > > cols;
  if( !graph.getProperty( "object_attributes_colors", cols ) )
    return col;
  map<string, vector<int> >::const_iterator ia = cols->find( attname
      + "_label" );
  if( ia != cols->end() )
    return ia->second;
  ia = cols->find( attname + "_filename" );
  if( ia != cols->end() )
    return ia->second;
  return col;
}


template void GraphManip::storeAims( Graph &, GraphObject*, const string &, 
                                     rc_ptr<AimsData<short> > );
template void GraphManip::storeAims( Graph &, GraphObject*, const string &, 
                                     rc_ptr<AimsSurfaceTriangle> );
template void GraphManip::storeAims( Graph &, GraphObject*, const string &, 
                                     rc_ptr<AimsTimeSurface<2, Void> > );
template void GraphManip::storeAims( Graph &, GraphObject*, const string &, 
                                     rc_ptr<AimsSurfaceFacet> );
template void GraphManip::storeAims( Graph &, GraphObject*, const string &, 
                                     rc_ptr<BucketMap<Void> > );


  #define _TMP_ rc_ptr<std::map<std::string,std::map<std::string,aims::GraphElementCode> > > 
INSTANTIATE_GENERIC_OBJECT_TYPE( _TMP_ )
#undef _TMP_
