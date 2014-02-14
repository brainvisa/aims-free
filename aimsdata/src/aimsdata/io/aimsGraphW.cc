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

#include <aims/io/aimsGraphW_d.h>
#include <aims/bucket/bucket.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/texture.h>
#include <aims/data/data.h>
#include <aims/io/finder.h>
#include <aims/io/aimsGraphR.h>
#include <graph/graph/graph.h>
#include <cartobase/stream/directory.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/exception/file.h>
#include <stdio.h>
#include <time.h>

using namespace aims;
using namespace carto;
using namespace std;


namespace aims
{
  namespace internal
  {

    struct AimsGraphWriter_ElemStruct
    {
      AimsGraphWriter_ElemStruct();

      rc_ptr<map<string, map<string, GraphElementCode> > >	objTable;
      Finder							fdr;
      map<AttributedObject *, 
	  map<string, pair<AimsGraphWriter::ElementInfo,GraphElementCode> > > 
      delayed;
    };

    AimsGraphWriter_ElemStruct::AimsGraphWriter_ElemStruct()
      : objTable( new map<string, map<string, GraphElementCode> > )
    {
    }

  }

  template<> int 
  AimsGraphWriter::ObjectWrapper<AimsData<short> >::freeindex() const
  {
    // extremely not optimal...
    short	x, y, z;
    set<short>	values;
    ForEach3d( (*data), x, y, z )
      values.insert( (*data)( x, y, z ) );

    set<short>::const_iterator	i = values.begin(), e = values.end();
    short	n = 1;
    if( i != e && *i == 0 )
      n = 0;
    for( ; i!=e && *i==n; ++i, ++n ) {}
    return( (int) n );
  }


  template<> int
  AimsGraphWriter::ObjectWrapper<AimsData<int32_t> >::freeindex() const
  {
    // extremely not optimal...
    short       x, y, z;
    set<int32_t>  values;
    ForEach3d( (*data), x, y, z )
      values.insert( (*data)( x, y, z ) );

    set<int32_t>::const_iterator  i = values.begin(), e = values.end();
    short       n = 1;
    if( i != e && *i == 0 )
      n = 0;
    for( ; i!=e && *i==n; ++i, ++n ) {}
    return( (int) n );
  }

}

#if ( __GNUC__-0 >= 3 ) || ( __GNUC__-0 == 2 && __GNUC_MINOR__-0 >= 95 )
static void AimsGraphWriter_construct( AimsGraphWriter & gw )
{
  gw.registerProcessType( "Bucket", "VOID",
                          &AimsGraphWriter::defaultTakeObject<BucketMap<Void> >
                          );
  gw.registerProcessType( "Mesh", "VOID",
                          &AimsGraphWriter::defaultTakeObject
                          <AimsSurfaceTriangle> );
  gw.registerProcessType( "Segments", "VOID",
                          &AimsGraphWriter::defaultTakeObject
                          <AimsTimeSurface<2, Void> > );
  gw.registerProcessType( "Texture", "FLOAT",
                          &AimsGraphWriter::defaultTakeObject<Texture1d> );
  gw.registerProcessType( "Texture", "S16",
                          &AimsGraphWriter::defaultTakeObject
                          <TimeTexture<short> > );
  gw.registerProcessType( "Texture", "POINT2DF",
                          &AimsGraphWriter::defaultTakeObject<Texture2d> );
  gw.registerProcessType( "Volume", "S16",
                          &AimsGraphWriter::defaultTakeObject<AimsData<short> >
                          );
  gw.registerProcessType( "Volume", "S32",
      &AimsGraphWriter::defaultTakeObject<AimsData<int32_t> >
    );
}
#endif


AimsGraphWriter::AimsGraphWriter( const string & fname )
  : Process(), d( new AimsGraphWriter_Private )
{
  d->filename = fname;
#if ( __GNUC__-0 >= 3 ) || ( __GNUC__-0 == 2 && __GNUC_MINOR__-0 >= 95 )
  // workaround bug in gcc-2.95
  AimsGraphWriter_construct( *this );
#else	// ( __GNUC__-0 == 2 && __GNUC_MINOR__-0 <= 91 )
  registerProcessType( string( "Bucket" ), string( "VOID" ), 
                      &defaultTakeObject<BucketMap<Void> > );
  registerProcessType( string( "Mesh" ), string( "VOID" ),
                      &defaultTakeObject<AimsSurfaceTriangle> );
  registerProcessType( string( "Segments" ), string( "VOID" ),
                      &defaultTakeObject<AimsTimeSurface<2, Void> > );
  registerProcessType( "Texture", "FLOAT",
                      &defaultTakeObject<Texture1d> );
  registerProcessType( "Texture", "S16",
                      &defaultTakeObject<TimeTexture<short> > );
  registerProcessType( "Texture", "POINT2DF",
                      &defaultTakeObject<Texture2d> );
  registerProcessType( "Volume", "S16",
                      &defaultTakeObject<AimsData<short> > );
  registerProcessType( "Volume", "S32",
                      &defaultTakeObject<AimsData<int32_t> > );
#endif
}


AimsGraphWriter::~AimsGraphWriter()
{
  delete d;
}


static void findGlobal( AttributedObject *, AimsGraphWriter_Private & d, 
			int index )
{
  d.mode = AimsGraphWriter::Global;
  d.elemcode.storageType = GraphElementCode::Global;
  if( index >= 0 )
    d.elemindex = index;
  //cout << "findGlobal, index : " << index << endl;

  if( d.elemcode.global_filename.empty() )
    {
      if( d.elemcode.local_file_attribute.empty() )
	d.elemcode.global_filename = d.elemcode.id + '_' 
	  + d.elemcode.objectType
	  + GraphManip::defaultExtensionForObjectType( d.elemcode.objectType,
                                                       d.elemcode.dataType );
      else
	d.elemcode.global_filename = d.elemcode.id + '_' 
	  + d.elemcode.local_file_attribute + '_' + d.elemcode.objectType
	  + GraphManip::defaultExtensionForObjectType( d.elemcode.objectType,
                                                       d.elemcode.dataType );
    }
  if( d.modified || index < 0 )
    d.globalmodified[ d.elemcode.global_filename ] = true;
  if( index < 0 )
    {
      //cout << "looking for object " << d.elemcode.global_filename << endl;
      d.delayindex = true;	// wait before assigning a new index
      // (to avoid taking one already used by another node)
    }
  //cout << "global_filename: " << d.elemcode.global_filename << endl;
}


static void findLocal( AttributedObject *ao, AimsGraphWriter_Private & d )
{
  d.mode = AimsGraphWriter::Local;
  d.elemcode.storageType = GraphElementCode::Local;
  string num;
  {
    ostringstream snum;
    time_t t = time( 0 );
    snum << t;
    num = snum.str();
  }
  ostringstream snum;
  snum << d.genindexcount++;
  string	fname = d.elemcode.id + "_" + d.elemcode.objectType
    + "_" + num + "_" + snum.str()
    + GraphManip::defaultExtensionForObjectType( d.elemcode.objectType,
                                                 d.elemcode.dataType );
  d.elemfname = d.directory + fname;
  ao->setProperty( d.elemcode.local_file_attribute, fname );
}


static 
void findObjectAndFilename( AttributedObject * ao, const string & attid, 
			    AimsGraphWriter_Private & d, 
			    AimsGraphWriter::ElementInfo & ei )
{
  //string	attidloc;
  string	locatt = d.elemcode.local_file_attribute;
  string	globatt = d.elemcode.global_index_attribute;
  string	fname;
  int		index;
  bool		haslocal, hasglobal;
  /*  if( attid.find( d.elemcode.syntax + '_' ) == 0 )
    {
      unsigned	x = d.elemcode.syntax.length();
      attidloc = attid.substr( x + 1, attid.length() - x );
    }
  else
  attidloc = attid;*/
  if( locatt.empty() )
    {
      locatt = attid + "_filename";
      d.elemcode.local_file_attribute = locatt;
    }
  if( globatt.empty() )
    {
      globatt = attid + "_label";
      d.elemcode.global_index_attribute = globatt;
    }
  if( ( d.modified = ao->hasProperty( "modified" ) ) )
    ao->removeProperty( "modified" );
  haslocal = ao->getProperty( locatt, fname );
  hasglobal = ao->getProperty( globatt, index );
  if( haslocal )
    {
      if( d.oldmode == AimsGraphWriter::Global )
	{
	  // convert to global
	  ao->removeProperty( locatt );
	  // find global object / index...
	  findGlobal( ao, d, -1 );
	  ao->setProperty( globatt, d.elemindex );
	  /*cout << "convert to global, id: " << d.elemcode.id << endl;
	  cout << "attrib: " << d.elemcode.attribute << endl;
	  cout << "index_att: " << d.elemcode.global_index_attribute << endl;
	  cout << "fname : " << d.elemcode.global_filename << endl;
	  if( d.delayindex )
	    cout << "(delayed)\n";
	  else
	  cout << "index : " << d.elemindex << endl;*/

	  // move global color attribute (if any) in graph
	  vector<int>	color;
	  if( ei.graph->getProperty( d.elemcode.local_file_attribute, 
				      color ) )
	    {
	      ei.graph->removeProperty( d.elemcode.local_file_attribute );
	      ei.graph->setProperty( d.elemcode.global_index_attribute, 
				      color );
	      // modify graph attribute objects_attribute_color
	      map<string, vector<int> >::iterator	ic 
		= d.colors->find( d.elemcode.local_file_attribute );
	      if( ic != d.colors->end() )
		{
		  (*d.colors)[ d.elemcode.global_index_attribute ] 
		    = ic->second;
		  d.colors->erase( ic );
		}
	    }
	}
      else
	{
	  //cout << "keep local\n";
	  d.mode = AimsGraphWriter::Local;
	  d.elemfname = d.directory + fname;
	}
    }
  else if( hasglobal )
    {
      if( d.oldmode == AimsGraphWriter::Local )
	{
	  //cout << "convert to local: " << attid << endl;
	  // convert to local
	  d.mode = AimsGraphWriter::Local;
	  ao->removeProperty( globatt );
	  // generate new filename
	  findLocal( ao, d );
	  //cout << "filename: " << d.elemfname << endl;

	  // move global color attribute (if any) in graph
	  vector<int>	color;
	  if( ei.graph->getProperty( d.elemcode.global_index_attribute, 
				      color ) )
	    {
	      ei.graph->removeProperty( d.elemcode.global_index_attribute );
	      ei.graph->setProperty( d.elemcode.local_file_attribute, 
				      color );
	      // modify graph attribute objects_attribute_color
	      map<string, vector<int> >::iterator	ic 
		= d.colors->find( d.elemcode.global_index_attribute );
	      if( ic != d.colors->end() )
		{
		  (*d.colors)[ d.elemcode.local_file_attribute ] = ic->second;
		  d.colors->erase( ic );
		}
	    }
	}
      else
	{
	  //cout << "keep global: " << attid << endl;
	  index = -1;
	  ao->getProperty( globatt, index );
	  //cout << "index : " << index << endl;
	  findGlobal( ao, d, index );
	}
    }
  else	// new object
    {
      //cout << "new object: " << attid << endl;
      AimsGraphWriter::SavingMode	mode = d.newmode;
      if( mode == AimsGraphWriter::Keep )
	mode = d.prevmode;

      if( mode == AimsGraphWriter::Global )
	{
	  findGlobal( ao, d, -1 );
	  if( !d.delayindex )
	    ao->setProperty( globatt, d.elemindex );
	}
      else
	findLocal( ao, d );
    }
}


void AimsGraphWriter::writeElement( AttributedObject *ao, 
				    internal::AimsGraphWriter_ElemStruct & es )
{
  // cout << "writeElement " << es.objTable->size() << endl;
  typedef pair<ElementInfo,GraphElementCode> DelayStruct;

  map<string, GraphElementCode>::iterator		ige, ege;
  map<string, map<string, GraphElementCode> >::iterator 
    iot, eot = es.objTable->end();

  iot = es.objTable->find( ao->getSyntax() );
  if( iot != eot )
    // { cout << "  " << ao->getSyntax() << " found" << endl;
    for( ige=iot->second.begin(), ege=iot->second.end(); ige!=ege; ++ige )
      {
	d->elemcode = ige->second;
        // cout << "try " << d->elemcode.attribute << endl;
	if( ao->hasProperty( d->elemcode.attribute ) )
	  {
            /*
            cout << "fill element table: " << d->elemcode.id << ", att: " 
               << d->elemcode.attribute << ", otype: " 
                 << d->elemcode.objectType 
               << ", dtype: " << d->elemcode.dataType << ", gfile: " 
               << d->elemcode.global_filename << ", storage: " 
               << d->elemcode.storageType << endl;
            */
	    // sub-optimal: will search twice for attribute in element
	    es.fdr.setObjectType( d->elemcode.objectType );
	    es.fdr.setDataType( d->elemcode.dataType );
	    info.element = ao;
	    info.attribute = d->elemcode.attribute;
	    d->delayindex = false;
	    d->modified = false;
	    findObjectAndFilename( ao, ige->first, *d, info );
	    ige->second = d->elemcode;	// back to objTable
	    if( d->delayindex )
	      {
		// cout << "element " << d->elemcode.attribute << " delayed\n";
		DelayStruct & ds = es.delayed[ ao ][ d->elemcode.attribute ];
		ds.first = info;
		ds.second = d->elemcode;
	      }
	    if( !d->delayindex 
		|| !d->globalobjects[ d->elemcode.global_filename ] )
	      try
		{
		  if( execute( es.fdr, "dummy" ) )
		    {
		      info.object->write( *this );
		      delete info.object;
		    }
		  else
		    cerr << "could not find object of type " 
			 << d->elemcode.objectType << " / " 
			 << d->elemcode.dataType << " in current vertex" 
			 << endl;
		}
	      catch( exception & e )
		{
		  cerr << "error saving object of type " 
		       << d->elemcode.objectType << " / " 
		       << d->elemcode.dataType << " in current vertex: ";
                  cerr << e.what() << endl;
		}
	  }
	else if( d->elemcode.storageType == GraphElementCode::GlobalPacked 
		 && ao->hasProperty( d->elemcode.global_index_attribute ) 
		 && info.graph->hasProperty( d->elemcode.global_attribute ) )
	  {
	    /*cout << d->elemcode.attribute << ": global packed\n";
	    cout << "global_attribute: " << d->elemcode.global_attribute 
		 << endl;
	    cout << "global_index_attribute: " 
		 << d->elemcode.global_index_attribute << endl;
		 cout << "object type: " << d->elemcode.objectType << endl; */
	    es.fdr.setObjectType( d->elemcode.objectType );
	    es.fdr.setDataType( d->elemcode.dataType );
	    info.element = info.graph;
	    info.attribute = d->elemcode.global_attribute;
	    d->delayindex = false;
	    d->modified = false;
	    ige->second = d->elemcode;	// back to objTable
	    try
	      {
		if( execute( es.fdr, "dummy" ) )
		  {
		    info.object->write( *this );
		    delete info.object;
		  }
		else
		  cerr << "could not find object of type " 
		       << d->elemcode.objectType << " / " 
		       << d->elemcode.dataType << " in current graph" 
		       << endl;
	      }
	    catch( exception & )
	      {
		cerr << "could not find object of type " 
		     << d->elemcode.objectType << " / " 
		     << d->elemcode.dataType << " in current graph" 
		     << endl;
	      }
	  }
      }
  //}
}


void AimsGraphWriter::writeElements( Graph & g, SavingMode newmode, 
				     SavingMode oldmode, bool onlyModif )
{
  string	reader;
  int		loaded = 0;
  if( g.getProperty( "aims_reader_filename", reader ) )
    {
      g.getProperty( "aims_reader_loaded_objects", loaded );
      if( loaded != 3 )
        {
          cout << "Graph " << reader << " has not been loaded entirely. " 
               << "Loading missing elements...\n";
          try
            {
              AimsGraphReader	gr( reader );
              gr.readElements( g, (~loaded) & 3 );
              g.setProperty( "aims_reader_loaded_objects", int(3) );
            }
          catch( exception & e )
            {
              cerr << e.what() << endl;
            }
        }
    }

  Graph::const_iterator				iv, fv = g.end();
  string					directory, fname_base;
  internal::AimsGraphWriter_ElemStruct		es;
  rc_ptr<map<string, map<string, GraphElementCode> > >	objTable;
  Finder					& fin = es.fdr;
  map<string, map<string, GraphElementCode> >::iterator	iot, eot;
  map<string, GraphElementCode>::iterator		ige, ege;

  // cout << "writeElements...\n";
  d->saveall = !onlyModif;
  if( !g.getProperty( "aims_objects_table", objTable ) )
    return;

  es.objTable = objTable;
  info.graph = &g;
  d->genindexcount = 0;
  d->prevmode = Global;
  for( iot=objTable->begin(), eot=objTable->end(); 
       iot!=eot && d->prevmode == Global; ++iot )
    for( ige=iot->second.begin(), ege=iot->second.end(); 
	 ige!=ege && d->prevmode == Global; ++ige )
      if( ige->second.storageType == GraphElementCode::Local )
	d->prevmode = Local;

  d->oldmode = oldmode;
  if( newmode == Keep )
  {
    if( oldmode != Keep )
      newmode = oldmode;
    else
      newmode = d->prevmode;
  }
  d->newmode = newmode;

  char	s = FileUtil::separator();
  //	set up output directory
  directory = FileUtil::dirname( d->filename );
  g.getProperty( "filename_base", fname_base );
  if( fname_base == "*"
      || ( !reader.empty() && FileUtil::dirname( reader ) != directory 
           && FileUtil::removeExtension( fname_base ) 
           == FileUtil::removeExtension( FileUtil::basename( reader ) ) ) )
    {
      fname_base 
        = FileUtil::removeExtension( FileUtil::basename( d->filename ) ) 
        + ".data";
    }
  if( !fname_base.empty() )
    {
      directory += s;
      directory += fname_base;
    }
  d->directory = directory + s;

  //	create output directory if it doesn't exist
  if( !fname_base.empty() )
    {
      string	dirname = directory;
      Directory dir( dirname );
      if( !dir.isValid() )
	{
	  cout << "saving all\n";
	  d->saveall = true;
          dir.mkdir();
	}
      if( !dir.isValid() )
	{
	  throw file_error( "can't create directory", dirname );
	}
    }

  g.getProperty( "object_attributes_colors", d->colors );

  typedef pair<ElementInfo,GraphElementCode> DelayStruct;

  //	write node elements
  for( iv=g.begin(); iv!=fv; ++iv )
    writeElement( *iv, es );

  //	write relation elements
  const set<Edge *>		&edg = g.edges();
  set<Edge *>::const_iterator	ie, ee = edg.end();

  for( ie=edg.begin(); ie!=ee; ++ie )
    writeElement( *ie, es );

  //	delayed global objects: assign index
  map<AttributedObject *, map<string, DelayStruct> >::iterator 
    id, ed = es.delayed.end();
  map<string, DelayStruct>::iterator	idd, edd;

  // cout << "writing delayed objects: " << es.delayed.size() << "...\n";
  for( id=es.delayed.begin(); id!= ed; ++id )
    for( idd=id->second.begin(), edd=id->second.end(); idd!=edd; ++idd )
      {
	// cout << idd->second.second.global_filename << endl;
	Wrapper	*wr = d->globalobjects[ idd->second.second.global_filename ];
        if( !wr )
        {
          cerr << "error in AimsGraphWriter (delayed object have no wrapper)"
              << endl;
          continue;
        }
	int	index = wr->freeindex();
	// cout << "new index: " << index << endl;
	id->first->setProperty( idd->second.second.global_index_attribute, 
				 index );
	d->elemindex = index;
	d->delayindex = false;
	d->elemcode = idd->second.second;
	d->elemfname = d->directory + idd->second.second.global_filename;
	d->modified = false;
	/* cout << "delayed elem " << idd->second.second.global_index_attribute
        << " = " << index << endl; */
	info = idd->second.first;
	fin.setObjectType( d->elemcode.objectType );
	fin.setDataType( d->elemcode.dataType );
	if( execute( fin, "dummy" ) )
	  {
	    info.object->write( *this );
	    delete info.object;
	  }
	//wr->write( *this );
      }

  d->mode = Local;
  //	write global objects
  // cout << "global objects: " << d->globalobjects.size() << endl;
  map<string,Wrapper *>::iterator	ig, eg = d->globalobjects.end();
  for( ig=d->globalobjects.begin(); ig!=eg; ++ig )
    {
      d->elemfname = d->directory + ig->first;
      d->modified = d->globalmodified[ ig->first ];
      // cout << "global object: " << d->elemfname << endl;
      if( !ig->second )
      {
        cerr << "error in AimsGraphWriter (gloabl object have no wrapper)"
            << endl;
        continue;
      }
      ig->second->write( *this );
    }

  // put back colors attributes after modifs
  g.setProperty( "object_attributes_colors", d->colors );

  // set up syntax in graph
  map<string,string>		typeloc, typeglob;
  string			oid, type;
  map<string, string>		ids;
  map<string, string>		typec;
  map<string, string>::iterator	itc, etc = typec.end();

  typec[ "Bucket" ] = "bck";
  typec[ "Mesh" ] = "tri";
  typec[ "Segments" ] = "tri";
  typec[ "Mesh4" ] = "tri";
  typec[ "Texture" ] = "tex";
  typec[ "Volume" ] = "vol";

  for( iot=objTable->begin(); iot!=eot; ++iot )
    //{ cout << "objTable synt: " << iot->first << endl;
    for( ige=iot->second.begin(), ege=iot->second.end(); ige!=ege; ++ige )
      {
	const GraphElementCode	& gec = ige->second;
	// cout << "id: " << gec.id << endl;
	itc = typec.find( gec.objectType );
	if( itc == etc )
	  type = gec.objectType;
	else
	  type = itc->second;
	if( gec.storageType != GraphElementCode::Local )
	  {
	    oid = gec.syntax + ".global." + type;
	    string	& descr = ids[ oid ];
	    if( descr.empty() )
	      {
		descr = gec.syntax;
		string	& tg = typeglob[ type ];
		if( !tg.empty() )
		  tg += ' ';
		tg += oid;
	      }
	    descr += string( " " ) + gec.global_filename + ' ' 
	      + gec.global_index_attribute;
	  }
	else
	  {
	    oid = gec.syntax + '.' + type;
	    string	& descr = ids[ oid ];
	    if( descr.empty() )
	      {
		descr = gec.syntax;
		string	& tl = typeloc[ type ];
		if( !tl.empty() )
		  tl +=  ' ';
		tl += oid;
	      }
	    descr += string( " " ) + gec.local_file_attribute;
	  }

	// delete global object
	ig = d->globalobjects.find( gec.global_filename );
	if( ig != eg )
	  {
	    if( gec.storageType != GraphElementCode::GlobalPacked
                && ig->second )
              ig->second->destroy();
            delete ig->second;
	    d->globalobjects.erase( ig );
	  }
      }
  //}

  // hum, this isn't really generic - should maybe be removed at load time ?
  if( g.hasProperty( "type.bck" ) )
    g.removeProperty( "type.bck" );
  if( g.hasProperty( "type.tri" ) )
    g.removeProperty( "type.tri" );
  if( g.hasProperty( "type.tex" ) )
    g.removeProperty( "type.tex" );
  if( g.hasProperty( "type.global.bck" ) )
    g.removeProperty( "type.global.bck" );
  if( g.hasProperty( "type.global.tri" ) )
    g.removeProperty( "type.global.tri" );
  if( g.hasProperty( "type.global.tex" ) )
    g.removeProperty( "type.global.tex" );

  for( itc=typeglob.begin(), etc=typeglob.end(); itc!= etc; ++itc )
    g.setProperty( string( "type.global." ) + itc->first, itc->second );
  for( itc=typeloc.begin(), etc=typeloc.end(); itc!= etc; ++itc )
    g.setProperty( string( "type." ) + itc->first, itc->second );
  for( itc=ids.begin(), etc=ids.end(); itc!=etc; ++itc )
    g.setProperty( itc->first, itc->second );
}


#ifdef __APPLE__
template struct AimsGraphWriter::ObjectWrapper<AimsSurfaceTriangle>;
template struct AimsGraphWriter::ObjectWrapper<BucketMap<Void> >;
#endif
