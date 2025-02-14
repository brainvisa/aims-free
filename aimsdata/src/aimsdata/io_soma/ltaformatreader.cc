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

//-------------------------------------------------------------------
#include <aims/io_soma/ltaformatreader.h>
#include <aims/transformation/affinetransformation3d.h>
#include <aims/resampling/standardreferentials.h>
#include <aims/data/pheader.h>
#include <soma-io/io/formatdictionary.h>
#include <soma-io/datasource/datasource.h>
#include <soma-io/io/reader.h>
#include <soma-io/datasourceinfo/datasourceinfoloader.h>
#include <cartodata/transformation/referential.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/stream/stringutil.h>
//--- debug ------------------------------------------------------------------
#include <cartobase/config/verbose.h>
#define localMsg( message ) cartoCondMsg( 4, message, "LTAFORMATREADER" )
// localMsg must be undef at end of file
//----------------------------------------------------------------------------

using namespace aims;
using namespace soma;
using namespace carto;
using namespace std;

//============================================================================
//   I N I T
//============================================================================

namespace
{

  bool initltaformat()
  {
    LtaFormatReader  *r = new LtaFormatReader;
    vector<string>  exts;
    exts.push_back( "lta" );
    FormatDictionary<AffineTransformation3d>::registerFormat( "LTA", r, exts );

    LtaT3DFormatReader *r2 = new LtaT3DFormatReader;
    exts.push_back( "lta" );
    FormatDictionary<Transformation3d>::registerFormat( "LTA3D", r2,
                                                        exts );

    return true;
  }

  bool dummy __attribute__((unused)) = initltaformat();

}


//============================================================================
//   N E W   M E T H O D S
//============================================================================

AffineTransformation3d*
LtaFormatReader::createAndRead( rc_ptr<DataSourceInfo> dsi,
                                const AllocatorContext & context,
                                Object options )
{
  rc_ptr<DataSource> ds = dsi->list().dataSource();
  localMsg( "createAndRead " + ds->url() );
  AffineTransformation3d *tr = new AffineTransformation3d;
  try
  {
    read( *tr, dsi, context, options );
  }
  catch( ... )
  {
    delete tr;
    throw;
  }
  tr->header()->copyProperties( dsi->header() );
  return tr;
}


namespace
{

  template <typename T> T _invalid()
  {
  }


  template <> int _invalid()
  {
    return -1;
  }

  template <> float _invalid()
  {
    return std::numeric_limits<float>::quiet_NaN();
  }

  template <> double _invalid()
  {
    return std::numeric_limits<double>::quiet_NaN();
  }


  template <typename T> bool _isvalid( T val )
  {
    return true;
  }

  template <> bool _isvalid( int val )
  {
    return val != -1;
  }

  template <> bool _isvalid( float val )
  {
    return !std::isnan( val );
  }

  template <> bool _isvalid( double val )
  {
    return !std::isnan( val );
  }


  template <typename T>
  vector<T> readVec( const string & str )
  {
    vector<T> vec;
    stringstream ss( str );
    T s;
    while( true )
    {
      s = _invalid<T>();
      ss >> s;
      if( !_isvalid( s ) )
        break;
      vec.push_back( s );
    }
    return vec;
  }


  template <typename T> T readVal( const string & str )
  {
    stringstream ss( str );
    T s;
    ss >> s;
    return s;
  }


  void readValue( Object dst, const string & key, const string & val )
  {
    if( key == "mean" || key == "cras" || key == "xras" || key == "yras"
        || key == "zras" || key == "voxelsize" )
    {
      vector<float> vec = readVec<float>( val );
      dst->setProperty( key, vec );
    }
    else if( key == "volume" )
    {
      vector<int> vec = readVec<int>( val );
      dst->setProperty( key, vec );
    }
    else if( key == "nxforms" || key == "type" || key == "valid" )
    {
      int ival = readVal<int>( val );
      dst->setProperty( key, ival );
    }
    else if( key == "sigma" )
    {
      float fval = readVal<float>( val );
      dst->setProperty( key, fval );
    }
    else
      dst->setProperty( key, val );
  }

}


void LtaFormatReader::read( AffineTransformation3d & obj,
                            rc_ptr<DataSourceInfo> dsi,
                            const AllocatorContext & /* context */,
                            Object options )
{
  // cout << "LtaFormatReader::read " << dsi->list().dataSource()->url() << endl;
  rc_ptr<DataSource> ds = dsi->list().dataSource();

  if( !ds->isOpen() )
    ds->open( DataSource::Read );
  if( !ds->isOpen() )
    throw file_not_found_error( ds->url() );

  AsciiDataSourceTraits<float>		fir;
  string s;
  bool r;
  int type = -1;
  unsigned l = 0, nmax = 20; // give up after 20 lines
  map<string, string> dict;
  do
  {
    r = StreamUtil::getline( *ds, s );
    if( s.length() == 0 || s[0] == '#' )
      continue;  // comment
    vector<string> spl = StringUtil::split( s, "=", 1 );
    if( spl.size() != 2 )
      break;

    string key = StringUtil::strip( spl[0] );
    string val = StringUtil::strip( spl[1] );
    int c = val.find( '#' );
    if( c != string::npos )
      val = StringUtil::strip( val.substr( 0, c ) );
    dict[key] = val;

    ++l;
  }
  while( r && l < nmax && !ds->eof() );

  if( l == nmax || !r || ds->eof() || dict.find("type") == dict.end() )
    throw wrong_format_error( ds->url() );

  stringstream t( dict[ "type" ] );
  t >> type;
  if( type < 0 )
    throw wrong_format_error( ds->url() );

  if( s != "1 4 4" )
    cout << "I expected \"1 4 4\", got: \"" << s << "\"\n";

  unsigned	i, j;
  for( i=0; i<4; ++i )
    for( j=0; j<4; ++j )
      {
        if( !StreamUtil::skip( *ds, " \t\n\r" ) )
          throw wrong_format_error( ds->url() );
        if( !fir.read( *ds, obj.matrix()( i, j ) ) )
          throw wrong_format_error( ds->url() );
      }

  if( !StreamUtil::skip( *ds, " \t\n\r" ) )
    throw wrong_format_error( ds->url() );

  // read source vol info
  r = StreamUtil::getline( *ds, s );
  if( !r || ds->eof() || StringUtil::strip( s ) != "src volume info" )
    throw wrong_format_error( ds->url() );

  map<string, string> sdict;
  do
  {
    r = StreamUtil::getline( *ds, s );
    if( s.length() == 0 || s[0] == '#' )
      continue;  // comment
    vector<string> spl = StringUtil::split( s, "=", 1 );
    if( spl.size() != 2 )
      break;

    string key = StringUtil::strip( spl[0] );
    string val = StringUtil::strip( spl[1] );
    int c = val.find( '#' );
    if( c != string::npos )
      val = StringUtil::strip( val.substr( 0, c ) );
    sdict[key] = val;

    ++l;
  }
  while( r && l < nmax && !ds->eof() );

  if( !r || ds->eof() || s != "dst volume info" )
    throw wrong_format_error( ds->url() );

  map<string, string> ddict;
  do
  {
    r = StreamUtil::getline( *ds, s );
    if( s.length() == 0 || s[0] == '#' )
      continue;  // comment
    vector<string> spl = StringUtil::split( s, "=", 1 );
    if( spl.size() != 2 )
      break;

    string key = StringUtil::strip( spl[0] );
    string val = StringUtil::strip( spl[1] );
    int c = val.find( '#' );
    if( c != string::npos )
      val = StringUtil::strip( val.substr( 0, c ) );
    ddict[key] = val;

    ++l;
  }
  while( r && l < nmax && !ds->eof() );

  if( !r || ds->eof() )
    throw wrong_format_error( ds->url() );

  ds->close();

  Object  hdr = Object::value( PropertySet() );

  map<string, string>::iterator id, ed = dict.end();
  for( id=dict.begin(); id!=ed; ++id )
    readValue( hdr, id->first, id->second );

  Object sh = Object::value( Dictionary() );
  Object dh = Object::value( Dictionary() );
  for( id=sdict.begin(), ed=sdict.end(); id!=ed; ++id )
    readValue( sh, id->first, id->second );
  for( id=ddict.begin(), ed=ddict.end(); id!=ed; ++id )
    readValue( dh, id->first, id->second );
  hdr->setProperty( "source_volume_info", sh );
  hdr->setProperty( "destination_volume_info", dh );

  rc_ptr<DataSource> mds( 0 );
  try
  {
    mds = dsi->list().dataSource( "minf" );
    if( mds.get() )
      DataSourceInfoLoader::readMinf( *mds, hdr, options );
  }
  catch( ... )
  {
  }
  if( type == 12 ) // MNI_TRANSFORM_TYPE
    hdr->setProperty(
      "destination_referential",
      StandardReferentials::mniTemplateReferentialID() );

  obj.setHeader( hdr );

  bool apply_vs = true;
  bool apply_s2m = true;
  bool apply_lpi = true;
  try
  {
    Object ovs = options->getProperty( "apply_vs" );
    apply_vs = bool( ovs->getScalar() );
  }
  catch( runtime_error & )
  {
  }
  try
  {
    Object ovs = options->getProperty( "apply_s2m" );
    apply_s2m = bool( ovs->getScalar() );
  }
  catch( runtime_error & )
  {
  }
  try
  {
    Object ovs = options->getProperty( "apply_lpi" );
    apply_lpi = bool( ovs->getScalar() );
  }
  catch( runtime_error & )
  {
  }
  // not sure if vs are in RAS or in disk order
  // if disk, then should be applied after RAS transforms
  vector<float> svs = vector<float>( 3, 1.f );
  vector<float> dvs = vector<float>( 3, 1.f );
  vector<int> sdim = vector<int>( 3, 1 );
  vector<int> ddim = vector<int>( 3, 1 );
  if( apply_s2m || apply_lpi )
  {
    sh->getProperty( "volume", sdim );
    dh->getProperty( "volume", ddim );
  }
  if( apply_vs || apply_lpi )
  {
    sh->getProperty( "voxelsize", svs );
    dh->getProperty( "voxelsize", dvs );
  }

  if( apply_s2m && ( type == 0 || type == 12 ) )
  {
    AffineTransformation3d sm2s, dm2s;
    vector<float> xl;
    sh->getProperty( "xras", xl );
    for( int j=0; j<3; ++j )
      if( xl.size() > j )
        sm2s.matrix()( 0, j ) = xl[j];
    if( xl[0] + xl[1] + xl[2] < 0.5 )
      sm2s.matrix()( 0, 3 ) = sdim[0] - 1;

    sh->getProperty( "yras", xl );
    for( int j=0; j<3; ++j )
      if( xl.size() > j )
        sm2s.matrix()( 1, j ) = xl[j];
    if( xl[0] + xl[1] + xl[2] < 0.5 )
      sm2s.matrix()( 1, 3 ) = sdim[1] - 1;

    sh->getProperty( "zras", xl );
    for( int j=0; j<3; ++j )
      if( xl.size() > j )
        sm2s.matrix()( 2, j ) = xl[j];
    if( xl[0] + xl[1] + xl[2] < 0.5 )
      sm2s.matrix()( 2, 3 ) = sdim[2] - 1;

//     sh->getProperty( "cras", xl );
//     for( int j=0; j<3; ++j )
//       if( xl.size() > j )
//         sm2s.matrix()( j, 3 ) = xl[j];

    dh->getProperty( "xras", xl );
    for( int j=0; j<3; ++j )
      if( xl.size() > j )
        dm2s.matrix()( 0, j ) = xl[j];
    if( xl[0] + xl[1] + xl[2] < 0.5 )
      dm2s.matrix()( 0, 3 ) = ddim[0] - 1;

    dh->getProperty( "yras", xl );
    for( int j=0; j<3; ++j )
      if( xl.size() > j )
        dm2s.matrix()( 1, j ) = xl[j];
    if( xl[0] + xl[1] + xl[2] < 0.5 )
      dm2s.matrix()( 1, 3 ) = ddim[1] - 1;

    dh->getProperty( "zras", xl );
    for( int j=0; j<3; ++j )
      if( xl.size() > j )
        dm2s.matrix()( 2, j ) = xl[j];
    if( xl[0] + xl[1] + xl[2] < 0.5 )
      dm2s.matrix()( 2, 3 ) = ddim[2] - 1;

//     dh->getProperty( "cras", xl );
//     for( int j=0; j<3; ++j )
//       if( xl.size() > j )
//         dm2s.matrix()( j, 3 ) = xl[j];

    obj = *dm2s.inverse() * obj * sm2s;
  }

  if( apply_vs )
  {
    AffineTransformation3d svst, dvst;
    svst.matrix()( 0, 0 ) = 1. / svs[0];
    svst.matrix()( 1, 1 ) = 1. / svs[1];
    svst.matrix()( 2, 2 ) = 1. / svs[2];
    dvst.matrix()( 0, 0 ) = dvs[0];
    dvst.matrix()( 1, 1 ) = dvs[1];
    dvst.matrix()( 2, 2 ) = dvs[2];
    obj = dvst * obj * svst;
  }

  if( apply_lpi && sh->hasProperty( "volume" ) && dh->hasProperty( "volume" ) )
  {
    Referential ref;
    vector<float> strans( 3, 0.f ), dtrans( 3, 0.f );
    if( apply_vs )
    {
      strans[0] = sdim[0] * svs[0];
      strans[1] = sdim[1] * svs[1];
      strans[2] = sdim[2] * svs[2];
      dtrans[0] = ddim[0] * dvs[0];
      dtrans[1] = ddim[1] * dvs[1];
      dtrans[2] = ddim[2] * dvs[2];
    }
    else
    {
      strans[0] = sdim[0] - 1.f;
      strans[1] = sdim[1] - 1.f;
      strans[2] = sdim[2] - 1.f;
      dtrans[0] = ddim[0] - 1.f;
      dtrans[1] = ddim[1] - 1.f;
      dtrans[2] = ddim[2] - 1.f;
    }
    rc_ptr<Transformation> slpi = ref.toOrientation( "RAS", strans, true );
    ref.setOrientation( "RAS" );
    rc_ptr<Transformation> dlpi = ref.toOrientation( "LPI", dtrans, true );
    AffineTransformationBase lpitr =
      *dynamic_cast<AffineTransformationBase *>( dlpi.get() ) * obj
        * *dynamic_cast<AffineTransformationBase *>( slpi.get() );
    obj = AffineTransformation3d( lpitr.toVector() );
  }
  obj.setHeader( hdr );

  bool inv = false;
  try
  {
    Object oinv = options->getProperty( "inv" );
    inv = bool( oinv->getScalar() );
    if( inv )
      obj = *obj.inverse();
  }
  catch( runtime_error & )
  {
  }

  // this is done after inv because the dsi header has also been inverted
  if( dsi->header().get() )
    obj.header()->copyProperties( dsi->header() );

  // cout << "read LTA OK\n";
}


FormatReader<AffineTransformation3d>* LtaFormatReader::clone() const
{
  return new LtaFormatReader;
}


// ---

Transformation3d*
LtaT3DFormatReader::createAndRead( rc_ptr<DataSourceInfo> dsi,
                                   const AllocatorContext & context,
                                   Object options )
{
  Reader<AffineTransformation3d> reader( dsi );
  reader.setAllocatorContext( context );
  reader.setOptions( options );
  return reader.read();
}


Transformation3d* LtaT3DFormatReader::create( carto::Object header,
                                              const AllocatorContext & context,
                                              carto::Object options )
{
  return new AffineTransformation3d;
}


void LtaT3DFormatReader::read( Transformation3d & obj,
                               rc_ptr<DataSourceInfo> dsi,
                               const AllocatorContext & context,
                               Object options )
{
  // cout << "LtaT3DFormatReader::read\n";
  AffineTransformation3d *affobj
    = dynamic_cast<AffineTransformation3d *>( &obj );
  if( !affobj )
    throw wrong_format_error( "Not an affine transformation" );

  Reader<AffineTransformation3d> reader( dsi );
  reader.setAllocatorContext( context );
  reader.setOptions( options );
  reader.read( *affobj );
  // cout << "read LTA OK\n";
}


FormatReader<Transformation3d>* LtaT3DFormatReader::clone() const
{
  return new LtaT3DFormatReader;
}


#undef localMsg

