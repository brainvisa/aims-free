/* Copyright (c) 1995-2005 CEA
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

#include <aims/io/aimsGraphR_d.h>
#include <aims/bucket/bucket.h>
#include <aims/io/finder.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/texture.h>
#include <graph/graph/graph.h>
#include <aims/data/data.h>
#include <cartobase/stream/sstream.h>
#include <cartobase/stream/fileutil.h>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace aims;
using namespace aims::internal;
using namespace carto;
using namespace std;


static void scanObjectAttribute( const Graph & gr, 
				 map<string, list<string> > & locals, 
				 map<string, vector<int> > & colors, 
				 const string & attr );
static void 
scanGlobalObjectAttribute( const Graph & gr, 
			   map<string, map<string, string> > & attlist, 
			   map<string, vector<int> > & colors, 
			   const string & attr );
static void 
buildObjectsAttributes( const Graph & gr, 
			map<string, list<string> > & locals, 
			map<string, map<string, string> > & globals, 
			map<string, vector<int> > & colors );


namespace aims
{
  template<> AimsGraphReader::Wrapper * 
  AimsGraphReader::ObjectWrapper<AimsData<short> >::extract( int )
  {
    return( new ObjectWrapper<AimsData<short> >( 0 ) );
  }
}


#if ( __GNUC__-0 == 2 && __GNUC_MINOR__-0 == 95 )
static void AimsGraphReader_construct( AimsGraphReader & gr )
{
  string	bck = DataTypeCode<BucketMap<Void> >().objectType();
  string	seg = DataTypeCode<AimsTimeSurface<2, Void> >().objectType();
  string	msh = DataTypeCode<AimsSurfaceTriangle>().objectType();
  string	vol = DataTypeCode<AimsData<short> >().objectType();
  string	tex = DataTypeCode<Texture1d>().objectType();
  string	p2f = DataTypeCode<Point2df>::name();
  string	vod = DataTypeCode<Void>::name();
  string	flt = DataTypeCode<float>::name();
  string	s16 = DataTypeCode<int16_t>::name();
  gr.registerProcessType( bck, vod, 
			  &AimsGraphReader::read<BucketMap<Void> > );
  gr.registerProcessType( msh, vod, 
			  &AimsGraphReader::read<AimsSurfaceTriangle> );
  gr.registerProcessType( seg, vod, 
			  &AimsGraphReader::read<AimsTimeSurface<2, Void> > );
  gr.registerProcessType( tex, flt, 
			  &AimsGraphReader::read<Texture1d> );
  gr.registerProcessType( tex, s16, 
			  &AimsGraphReader::read<TimeTexture<short> > );
  gr.registerProcessType( tex, p2f, 
			  &AimsGraphReader::read<Texture2d> );
  gr.registerProcessType( vol, s16, 
			  &AimsGraphReader::read<AimsData<short> > );
}
#endif

AimsGraphReader::AimsGraphReader( const string & fname )
  : Process(), _priv( new AimsGraphReader_Private )
{
  _priv->filename = fname;
  string	bck = DataTypeCode<BucketMap<Void> >().objectType();
  string	seg = DataTypeCode<AimsTimeSurface<2, Void> >().objectType();
  string	msh = DataTypeCode<AimsSurfaceTriangle>().objectType();
  string	vol = DataTypeCode<AimsData<short> >().objectType();
  string	tex = DataTypeCode<Texture1d>().objectType();
  string	p2f = DataTypeCode<Point2df>::name();
  string	vod = DataTypeCode<Void>::name();
  string	flt = DataTypeCode<float>::name();
  string	s16 = DataTypeCode<int16_t>::name();

#if ( __GNUC__-0 == 2 && __GNUC_MINOR__-0 == 95 )
  // workaround bug in gcc-2.95
  AimsGraphReader_construct( *this );
#elif ( __GNUC__-0 == 2 && __GNUC_MINOR__-0 <= 91 )
  // workaround bug in egcs-1.1.2, avoid it in gcc-2.96
  registerProcessType( bck, vod, &read<BucketMap<Void> > );
  registerProcessType( msh, vod, &read<AimsSurfaceTriangle> );
  registerProcessType( seg, vod, &read<AimsTimeSurface<2, Void> > );
  registerProcessType( tex, flt, &read<Texture1d> );
  registerProcessType( tex, s16, &read<TimeTexture<short> > );
  registerProcessType( tex, p2f, &read<Texture2d> );
  registerProcessType( vol, s16, &read<AimsData<short> > );
#else
  registerProcessType( bck, vod, &AimsGraphReader::read<BucketMap<Void> > );
  registerProcessType( msh, vod, &AimsGraphReader::read<AimsSurfaceTriangle> );
  registerProcessType( seg, vod, 
                       &AimsGraphReader::read<AimsTimeSurface<2, Void> > );
  registerProcessType( tex, flt, &AimsGraphReader::read<Texture1d> );
  registerProcessType( tex, s16, &AimsGraphReader::read<TimeTexture<short> > );
  registerProcessType( tex, p2f, &AimsGraphReader::read<Texture2d> );
  registerProcessType( vol, s16, &AimsGraphReader::read<AimsData<short> > );
#endif
}


AimsGraphReader::~AimsGraphReader()
{
  delete _priv;
}


void scanObjectAttribute( const Graph & gr, 
			  map<string, list<string> > & locals, 
			  map<string, vector<int> > & colors, 
			  const string & attr )
{
  string	str, al, syntA;
  istringstream	sst( attr.c_str() ); //, attr.length() );
  vector<int>	col;

  while( !sst.eof() )
    {
      sst >> str;	// attribute describing a syntactic attribute

      if ( gr.getProperty( str, al ) )	// list of attributes
	{
	  istringstream	sa( al.c_str() ); //, al.length() );

	  sa >> syntA;	// first: syntactic att
	  list<string>	& tl = locals[ syntA ];

	  while( !sa.eof() )
	    {
	      sa >> str;
	      tl.push_back( str );
	      // cout << "insert " << str << " in " << syntA << endl;
	      // check for default color
	      if ( gr.getProperty( str, col ) && col.size() >= 3 )
		colors[ str ] = col;
	    }
	}
    }
}


static void 
scanGlobalObjectAttribute( const Graph & gr, 
			   map<string, map<string, string> > & attlist, 
			   map<string, vector<int> > & colors, 
			   const string & attr )
{
  string	str, al, syntA, filen;
  istringstream	sst( attr.c_str() ); //, attr.length() );
  vector<int>	col;

  while( !sst.eof() )
    {
      sst >> str;	// attribute describing a syntactic attribute

      if ( gr.getProperty( str, al ) )	// list of attributes
	{
	  istringstream	sa( al.c_str() ); //, al.length() );

	  sa >> syntA;	// first: syntactic att
	  while( !sa.eof() )
	    {
	      sa >> filen;	// filename for bucket / mesh
	      sa >> str;	// attribute for [time] index in bck/mesh
	      attlist[ syntA ][ filen ] = str;
	      // cout << "global attrib, synt : " << syntA 
	      //    << ", attrib : " << str << ", file : " << filen << endl;
	      if ( gr.getProperty( str, col ) && col.size() >= 3 )
		colors[ str ] = col;
	    }
	}
    }
}


static void 
buildObjectsAttributes( const Graph & gr, 
			map<string, list<string> > & locals, 
			map<string, map<string, string> > & globals, 
			map<string, vector<int> > & colors )
{
  /* locals : objects attribute list: buckets, triangulations... by syntax.
     locals[syntax] = semantic attributes */
  /* globals : global objects attribute list.
     globals[syntax] = list(global_object, index_attrib) */
  /* colors : Default colors of buckets, triangulations etc. 
     (for visualization, not used in Aims, but used in Anatomist)
     colors[object attribute] = color */

  string				typeObj;
  
  locals.clear();
  colors.clear();
  globals.clear();

  if ( gr.getProperty( "type.tri", typeObj ) )
    scanObjectAttribute( gr, locals, colors, typeObj );
  if ( gr.getProperty( "type.bck", typeObj ) )
    scanObjectAttribute( gr, locals, colors, typeObj );
  if ( gr.getProperty( "type.global.bck", typeObj ) )
    scanGlobalObjectAttribute( gr, globals, colors, typeObj );
  if ( gr.getProperty( "type.global.tri", typeObj ) )
    scanGlobalObjectAttribute( gr, globals, colors, typeObj );
  if ( gr.getProperty( "type.global.tex", typeObj ) )
    scanGlobalObjectAttribute( gr, globals, colors, typeObj );
  if ( gr.getProperty( "type.global.vol", typeObj ) )
    scanGlobalObjectAttribute( gr, globals, colors, typeObj );
}


//  Loads objects and stores them in a list of wrappers
void 
AimsGraphReader_Private::loadGlobalObjects( map<string, 
					    AimsGraphReader::Wrapper*> 
					    & objects, 
					    const map<string, map<string, 
					    string> > & globals, 
					    const string & basepath, 
					    map<string, map<string, 
					    GraphElementCode> > & objmap, 
					    AimsGraphReader &agr )
{
  map<string, map<string, string> >::const_iterator 
    ig, ige = globals.end();
  map<string, string>::const_iterator	il, el;
  string				fname, ffname, attid;
  string::size_type			pos;
  const AimsGraphReader::ElementInfo 
    & info = agr._priv->postproc.elementInfo();
  bool readOK;

  for ( ig=globals.begin(); ig!=ige; ++ig )
  {
    map<string, GraphElementCode>	& om = objmap[ ig->first ];
    for ( il=(*ig).second.begin(), el=(*ig).second.end(); il!=el; ++il )
    {
      fname = (*il).first;
      if ( fname[0] != '/' )	// relative path: append to base path
        ffname = basepath + fname;
      else
        ffname = fname;
      readOK = false;
      if( objects.find( fname ) != objects.end() )
        readOK = true;
      else
        try
        {
          // cout << "load global object " << ffname << endl;
          if ( agr.execute( ffname ) )
          {
            readOK = true;
            // cout << "loaded OK\n";
            objects[ fname ] = info.object;
          }
          else
            cerr << "warning : could not load global object " << ffname
                  << endl;
        }
        catch( exception & )
        {
          cerr << "warning : could not load global object " << ffname
                << endl;
        }
      if( readOK )
      {
        // attid = FileUtil::removeExtension( fname );
        attid = il->second; // index attribute
        pos = attid.rfind( '_' );
        if ( pos != string::npos ) // remove filename/index suffix (type)
        {
          string suffix = attid.substr( pos, attid.length() - pos );
          if( suffix == "_label" || suffix == "_filename" )
            attid.erase( pos, attid.length() - pos );
        }
        // cout << "attid: " << attid << endl;
        GraphElementCode	& ec = om[ attid ];
        ec.id = attid;
        if( ec.attribute.empty() )
          ec.attribute = string( "aims_" ) + attid;
        ec.objectType = info.object->objectType();
        ec.dataType = info.object->dataType();
        if( ec.objectType == "Volume" )
          ec.storageType = GraphElementCode::GlobalPacked;
        else
          ec.storageType = GraphElementCode::Global;
        ec.global_index_attribute = il->second;
        if( ec.global_filename.empty() )
          ec.global_filename = fname;
        if( ec.global_attribute.empty() )
          ec.global_attribute = ec.attribute;
        if( ec.local_file_attribute.empty() )
          ec.local_file_attribute = attid + "_filename";
        ec.syntax = ig->first;
        // cout << "GEC attribute: " << ec.attribute << endl;
      }
    }
  }
}


namespace aims
{
  namespace internal
  {

    struct AimsGraphReader_ElemStruct
    {
      enum FiltMode { None, Exclude, Read };

      AimsGraphReader_ElemStruct();

      rc_ptr<map<string, map<string, GraphElementCode> > >	objTable;
      map<string, list<string> >			locals;
      map<string, map<string, string> >		globals;
      map<string, AimsGraphReader::Wrapper*>	gobjects;
      string					path;
      FiltMode					filter;
      Finder					fdr;
    };

    AimsGraphReader_ElemStruct::AimsGraphReader_ElemStruct()
      : objTable( new map<string, map<string, GraphElementCode> > )
    {
    }

  }
}


void AimsGraphReader::readElement( AttributedObject* v, 
				   AimsGraphReader_ElemStruct & es )
{
  string					filename, name;
  map<string, list<string> >::iterator		itl;
  list<string>::iterator			il, fl;
  map<string, map<string, string> >::iterator	igl, 
    igle = es.globals.end();
  map<string, GraphElementCode>	& om = (*es.objTable)[ v->getSyntax() ];
  set<string>::const_iterator		er = _priv->readfilter.end();
  set<string>::const_iterator		ee = _priv->excludefilter.end();
  string				attid, attrib; //, attid2;
  ElementInfo				& info = _priv->postproc.info;
  string::size_type			pos;
  map<string, string>::const_iterator	ila, ela;
  int					ind = 0;

  //cout << "readElement " << v->getSyntax() << endl;

  // look if there are things to load for this syntax
  itl = es.locals.find( v->getSyntax() );
  if ( itl != es.locals.end() )
    {
      list<string> 			& tl = (*itl).second;

      // look for loadable objects attributes in node
      for ( il=tl.begin(), fl=tl.end(); il!=fl; ++il )
	if ( v->getProperty( *il, filename ) )
	  {
	    name = es.path + filename;
	    pos = il->rfind( "_filename" );
	    if ( pos == il->length() - 9 )
	      attid = il->substr( 0, pos );
	    else
	      attid = *il;
	    if ( es.filter == AimsGraphReader_ElemStruct::None 
		|| ( es.filter == AimsGraphReader_ElemStruct::Exclude 
		     && set<string>::const_iterator
		     ( _priv->excludefilter.find( attid ) ) == ee ) 
		|| ( es.filter == AimsGraphReader_ElemStruct::Read 
		     && set<string>::const_iterator
		     ( _priv->readfilter.find( attid ) ) != er ) )
	      {
		info.element = v;
		info.attribute = string( "aims_" ) + attid;
		if ( execute( name ) )
		  {
		    //attid2 = v->getSyntax() + '_' + attid;
		    GraphElementCode	& ec = om[attid];
		    ec.id = attid;
		    ec.attribute = info.attribute;
		    ec.objectType = info.object->objectType();
		    ec.dataType = info.object->dataType();
		    ec.storageType = GraphElementCode::Local;
		    ec.local_file_attribute = *il;
                    if( ec.global_filename.empty() )
                      ec.global_filename = attid + '_' + ec.objectType;
		    ec.global_index_attribute = attid + "_label";
		    if ( !ec.syntax.empty() && ec.syntax != v->getSyntax() )
		      cerr << "warning: changing syntax of ElementCode, " 
			   << ec.syntax << " -> " << v->getSyntax() << endl;
		    ec.syntax = v->getSyntax();
		    es.fdr.setObjectType( ec.objectType );
		    es.fdr.setDataType( ec.dataType );
		    _priv->postproc.execute( es.fdr, name );
		    delete info.object;
		  }
		else
		  cerr << "loading " << name << " FAILED.\n";
	      }
	  }
    }

  //	global objects
  igl = es.globals.find( v->getSyntax() );
  if ( igl != igle )
    //	for each (filename,attribute)
    for ( ila=(*igl).second.begin(), ela=(*igl).second.end(); ila!=ela; 
	 ++ila )
      if ( v->getProperty( (*ila).second, ind ) )
	{	// corresponding attribute present in node
	  filename = (*ila).first;
	  pos = filename.rfind( '.' );
          attid = ila->second;
	  pos = attid.rfind( '_' );
          if( pos != string::npos )	// remove filename/label suffix
          {
            string suffix = attid.substr( pos, attid.length() - pos );
            if( suffix == "_label" || suffix == "_filename" )
              attid.erase( pos, attid.length() - pos );
          }
	  if ( es.filter == AimsGraphReader_ElemStruct::None 
	      || ( es.filter == AimsGraphReader_ElemStruct::Exclude 
		   && set<string>::const_iterator
		   ( _priv->excludefilter.find( attid ) ) == ee ) 
	      || ( es.filter == AimsGraphReader_ElemStruct::Read 
		   && set<string>::const_iterator
		   ( _priv->readfilter.find( attid ) ) != er ) )
	    {
	      attrib = string( "aims_" ) + attid;
	      info.element = v;
	      info.attribute = attrib;
	      map<string, AimsGraphReader::Wrapper*>::iterator	igo 
		= es.gobjects.find( filename );
	      if ( igo != es.gobjects.end() )
		{
		  if ( igo->second )
		    info.object = igo->second->extract( ind );
		  else cerr << "object " << igo->first << " declared but "
			    << "not found\n";
		  es.fdr.setObjectType( info.object->objectType() );
		  es.fdr.setDataType( info.object->dataType() );
		  _priv->postproc.execute( es.fdr, "dummy" );
		  delete info.object;
		}
	    }
	}
}


void AimsGraphReader::readElements( Graph & g, int mask )
{
  // cout << "AimsGraphReader::readElements\n";

  AimsGraphReader_ElemStruct	es;
  ElementInfo			& info = _priv->postproc.info;
  Graph::iterator		iv, fv = g.end();
  string			base;
  rc_ptr<map<string, vector<int> > > 
    objAttColors( new map<string, vector<int> > );

  //if( g.hasProperty( "aims_objects_table" ) )
  //  return;	// already done
  g.getProperty( "aims_objects_table", es.objTable );

  info.graph = &g;
  es.path = FileUtil::dirname( _priv->filename );
  if( !es.path.empty() )
    es.path += FileUtil::separator();

  if ( g.getProperty( "filename_base", base ) )
    {
      // check if base is a directory
      bool		ok = false;
      if ( base != "*" )
	{
	  struct stat	stbuf;
	  if ( stat( (es.path + base).c_str(), &stbuf ) == 0 )
	    if ( S_ISDIR( stbuf.st_mode ) )
	      ok = true;
	}
      if ( !ok )
	{
	  bool	setbase = false;
	  if ( base != "*" )
	    {
	      cerr << "Warning: wrong filename_base in graph, trying to fix " 
		   << "it\n";
	      //g.setProperty( "filename_base", string( "*" ) );
	      setbase = true;
	    }
	  base = FileUtil::basename( _priv->filename );
	  string::size_type pos = base.rfind( '.' );
	  if ( pos != string::npos )
	    base.erase( pos, base.length() - pos );
	  base += ".data";
	  if ( setbase )
	    g.setProperty( "filename_base", base );
	}
      es.path += base + FileUtil::separator();
    }

  //	Get attributes containing objects to load
  buildObjectsAttributes( g, es.locals, es.globals, *objAttColors );

  /*
  cout << "buildObjectsAttributes done\n";
  cout << "locals : " << es.locals.size() 
       << "\nglobals : " << es.globals.size() 
       << "\ncolors : " << objAttColors->size() << endl;
  */

  g.setProperty( "object_attributes_colors", objAttColors );

  AimsGraphReader_Private::loadGlobalObjects( es.gobjects, es.globals, 
					      es.path, *es.objTable, *this );
  // cout << "loadGlobalObjects done\n";
  // cout << "gobjects : " << es.gobjects.size() << endl;

  string				attid, attrib;
  es.filter = AimsGraphReader_ElemStruct::None;
  if ( !_priv->readfilter.empty() )
    es.filter = AimsGraphReader_ElemStruct::Read;
  else if ( !_priv->excludefilter.empty() )
    es.filter = AimsGraphReader_ElemStruct::Exclude;

  if ( mask & 1 )
    {
      // cout << "graph : " << g.order() << " nodes\n";
      for ( iv=g.begin(); iv!=fv; ++iv )
        readElement( *iv, es );
    }

  if ( mask & 2 )
    {
      const set<Edge *>			&edg = g.edges();
      set<Edge *>::const_iterator	ie, ee = edg.end();

      for ( ie=edg.begin(); ie!=ee; ++ie )
	readElement( *ie, es );
    }

  g.setProperty( "aims_objects_table", es.objTable );

  // cout << "deleting globals...\n";
  //	destroy temp objects (globals)
  map<string, AimsGraphReader::Wrapper*>::iterator ib, ibe = es.gobjects.end();
  map<string, map<string, string> >::const_iterator 
    ig, ige = es.globals.end();
  map<string, GraphElementCode>::const_iterator	iot, eot;

  for ( ig=es.globals.begin(); ig!=ige; ++ig )
    {
      map<string, GraphElementCode>	& om = (*es.objTable)[ ig->first ];
      for ( iot=om.begin(), eot=om.end(); iot!=eot; ++iot )
	{
	  ib = es.gobjects.find( iot->second.global_filename );
	  if ( ib != ibe )
	    {
              /*
	      cout << "cleanup - " << iot->second.attribute << ", file "
		   << iot->second.global_filename << ", id: " 
		   << iot->first << endl;
              */
	      // keep volumes in GlobalPacked mode
	      if ( ib->second->objectType() == "Volume" )
		{
		  //cout << "Volume: keep GlobalPacked\n";
		  //cout << iot->second.global_attribute << endl;
		  es.fdr.setObjectType( ib->second->objectType() );
		  es.fdr.setDataType( ib->second->dataType() );
		  ElementInfo & ei = _priv->postproc.elementInfo();
		  ei.graph = &g;
		  ei.element = &g;
		  ei.attribute = iot->second.global_attribute;
		  ei.object = ib->second;
		  _priv->postproc.execute( es.fdr, 
					   iot->second.global_filename );
		}
	      else
		{
		  // cout << "destroy " << iot->second.attribute << endl;
		  ib->second->destroy();
		}
	      delete ib->second;
	      es.gobjects.erase( ib );
	    }
	}
    }

  // debug
  /*
  cout << "aims_objects_table:" << es.objTable->size() << endl;
  {
    map<string, map<string, GraphElementCode> >::iterator 
      i, e = es.objTable->end();
    map<string, GraphElementCode>::iterator	j, f;
    for( i=es.objTable->begin(); i!=e; ++i )
      {
        cout << "  " << i->first << " : " << i->second.size() << endl;
        for( j=i->second.begin(), f=i->second.end(); j!=f; ++j )
          cout << "    id: " << j->second.id << ", att: " 
               << j->second.attribute << ", otype: " << j->second.objectType 
               << ", dtype: " << j->second.dataType << ", gfile: " 
               << j->second.global_filename << ", storage: " 
               << j->second.storageType << endl;
      }
  }
  cout << "aims_reader_filename: " << _priv->filename << endl;
  cout << "aims_reader_loaded_objects: " << mask << endl;
  */
  // end debug

  g.setProperty( "aims_reader_filename", _priv->filename );
  int	mask2 = 0;
  g.getProperty( "aims_reader_loaded_objects", mask2 );
  mask2 |= mask;
  g.setProperty( "aims_reader_loaded_objects", mask2 );

  //cout << "readElements done\n";
}


void AimsGraphReader::setReadFilter( const set<string> & filt )
{
  _priv->readfilter = filt;
  _priv->excludefilter.clear();
}


void AimsGraphReader::setExcludeFilter( const set<string> & filt )
{
  _priv->readfilter.clear();
  _priv->excludefilter = filt;
}


void AimsGraphReader::setInsertionFunction( const std::string & objType, 
					    const std::string & dataType, 
					    ProcFunc procFunc )
{
  _priv->postproc.registerProcessType( objType, dataType, procFunc );
}


AimsGraphReader::PostProcessor::PostProcessor() : Process()
{
}


AimsGraphReader::PostProcessor::~PostProcessor()
{
}


void AimsGraphReader::loadAllMissingElements( Graph & g )
{
  string        reader;
  int           loaded = 0;
  if( g.getProperty( "aims_reader_filename", reader ) )
  {
    g.getProperty( "aims_reader_loaded_objects", loaded );
    if( loaded != 3 )
    {
      cout << "Graph " << reader << " has not been loaded entirely. "
          << "Loading missing elements...\n";
      try
      {
        AimsGraphReader   gr( reader );
        gr.readElements( g, (~loaded) & 3 );
        g.setProperty( "aims_reader_loaded_objects", int(3) );
      }
      catch( exception & e )
      {
        cerr << e.what() << endl;
      }
    }
  }
}

