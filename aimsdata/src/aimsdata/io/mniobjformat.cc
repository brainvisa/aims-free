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

#include <aims/io/mniobjheader.h>
#include <aims/io/baseFormats_mesh.h>
#include <cartobase/io/asciidatasourcetraits.h>
#include <cartobase/datasource/streamdatasource.h>
#include <cartobase/datasource/filedatasource.h>
#include <cartobase/type/string_conversion.h>
#include <cartobase/exception/ioexcept.h>

using namespace aims;
using namespace carto;
using namespace std;


MniObjFormat::~MniObjFormat()
{
}


bool MniObjFormat::read( const std::string & filename,
  AimsSurfaceTriangle & obj, const carto::AllocatorContext & /*context*/,
  carto::Object /* options */ )
{
  MniObjHeader  hdr( filename );
  uint32_t  buf;
  if( !hdr.read( &buf ) )
    io_error::launchErrnoExcept( hdr.filename() );
  ifstream::off_type offset = buf;

  ios::openmode  omd = ios::in;
  if( hdr.openMode() == "binar" )
    omd |= ios::binary;
  ifstream is( hdr.filename().c_str(), omd );
  is.unsetf( ios::skipws );
  if( !is )
    io_error::launchErrnoExcept( hdr.filename() );
  is.seekg( offset );
  if( !is )
    io_error::launchErrnoExcept( hdr.filename() );

  if( hdr.openMode() == "ascii" )
    is.setf( std::ios::skipws );
  IStreamDataSource ds( is );
  obj.erase();
  obj.setHeader( hdr );

  int nvert = 0, i;
  string l;
  Point3df p;
  vector<string> lspl;
  set<string> sspaces;
  sspaces.insert( " " );
  sspaces.insert( "\t" );
  sspaces.insert( "\n" );
  sspaces.insert( "\r" );

  hdr.getProperty( "vertex_number", nvert );
//   cout << "obj vertex: " << nvert << endl;
  vector<Point3df> & vert = obj.vertex();
  vert.reserve( nvert );

  for( i=0; i<nvert; ++i )
  {
    lspl.clear();
    while( lspl.empty() )
    {
      if( !StreamUtil::getline( ds, l ) )
        throw wrong_format_error( filename );
      lspl = split( l, sspaces );
    }
    if( lspl.size() != 3 )
      throw wrong_format_error( filename );
    stringTo( lspl[0], p[0] );
    stringTo( lspl[1], p[1] );
    stringTo( lspl[2], p[2] );
    vert.push_back( p );
  }

//   cout << "vertices read\n";

  vector<Point3df> & norm = obj.normal();
  norm.reserve( nvert );
  for( i=0; i<nvert; ++i )
  {
    lspl.clear();
    while( lspl.empty() )
    {
      if( !StreamUtil::getline( ds, l ) )
        throw wrong_format_error( filename );
      lspl = split( l, sspaces );
    }
    if( lspl.size() != 3 )
    {
      /* cout << "line with !=3 items: " << lspl.size() << ", line " << i << endl;
      cout << ">" << l << "<" << endl; */
      throw wrong_format_error( filename );
    }
    stringTo( lspl[0], p[0] );
    stringTo( lspl[1], p[1] );
    stringTo( lspl[2], p[2] );
    norm.push_back( p );
  }

  if( !StreamUtil::skip( ds ) )
    throw wrong_format_error( filename );
  unsigned npoly = 0, n;
  if( !AsciiDataSourceTraits<unsigned>::read( ds, npoly ) /* || npoly == 0 */ )
    throw wrong_format_error( filename );

  if( !StreamUtil::skip( ds ) )
    throw wrong_format_error( filename );
  int colflag = 0;
  if( !AsciiDataSourceTraits<int>::read( ds, colflag ) )
    throw wrong_format_error( filename );
  if( colflag == 0 )
  {
    lspl.clear();
    while( lspl.empty() )
    {
      if( !StreamUtil::getline( ds, l ) )
        throw wrong_format_error( filename );
      lspl = split( l, sspaces );
    }
    vector<float> cols( 4, 1. );
    for( i=0; i<4; ++i )
    {
      stringTo( lspl[i], cols[i] );
      if(  cols[i] < 0. || cols[i] > 1. )
        throw wrong_format_error( filename );
    }
    Object mat = Object::value( Dictionary() );
    mat->setProperty( "diffuse", cols );
    obj.header().setProperty( "material", mat );
  }
  else
  {
    for( i=0; i<nvert; ++i )
    {
      lspl.clear();
      while( lspl.empty() )
      {
        if( !StreamUtil::getline( ds, l ) )
          throw wrong_format_error( filename );
        lspl = split( l, sspaces );
      }
    }
    // TODO: do something with all these colors
  }

  // skip crappy numbers
  /* I don't know what the following block is for,
     there is one int number per triangle, seems to be just
     a counter increasing 3 by 3 (3 6 9 12 etc)
  */
  unsigned nu;
  for( n=0; n<npoly; ++n )
  {
    if( !StreamUtil::skip( ds ) )
      throw wrong_format_error( filename );
    if( !AsciiDataSourceTraits<unsigned>::read( ds, nu ) )
      throw wrong_format_error( filename );
  }

  // read polygons
  unsigned j;
  AimsVector<uint32_t, 3> pol;
  vector< AimsVector<uint32_t, 3> > & polygons = obj.polygon();
  polygons.reserve( npoly );

  for( n=0; n<npoly; ++n )
  {
    for( j=0; j<3; ++j )
    {
      if( !StreamUtil::skip( ds ) )
        throw wrong_format_error( filename );
      if( !AsciiDataSourceTraits<unsigned>::read( ds, pol[j] )
        || pol[j] >= (unsigned) nvert )
        throw wrong_format_error( filename );
    }
    polygons.push_back( pol );
  }

  return true;
}


bool MniObjFormat::write( const std::string & filename,
  const AimsSurfaceTriangle & obj, carto::Object )
{
  string fname = filename;
  if( fname.substr( fname.length() - 4, 4 ) != ".obj" )
    fname += ".obj";
  FileDataSource ds( fname, 0, DataSource::Write );
  if( !ds.open( DataSource::Write ) )
    io_error::launchErrnoExcept( fname );

  ds << "P 0.3 0.3 0.4 10 1 " << obj.vertex().size() << "\n";

  const vector<Point3df> & vert = obj.vertex();
  const vector<Point3df> & norm = obj.normal();
  const vector<AimsVector<uint32_t, 3> > & poly = obj.polygon();

  unsigned n, nv = vert.size(), nn = norm.size();

  for( n=0; n<nv; ++n )
  {
    const Point3df & p = vert[n];
    ds << " " << p[0] << " " << p[1] << " " << p[2] << "\n";
  }
  ds << "\n";

  if( nn > nv )
    nn = nv;
  for( n=0; n<nn; ++n )
  {
    const Point3df & p = norm[n];
    ds << " " << p[0] << " " << p[1] << " " << p[2] << "\n";
  }
  for( ; n<nv; ++n )
    ds << " 0. 0. 0.\n";
  ds << "\n";

  unsigned npol = obj.polygon().size();
  ds <<  " " << npol << "\n 0 ";
  vector<float> cols( 4, 1. );
  try
  {
    Object col;
    col = obj.header().getProperty( "material" );
    if( col )
      col = col->getProperty( "diffuse" );
    if( col && col->size() > 0 )
    {
      cols[0] = col->getArrayItem( 0 )->getScalar();
      if( col->size() > 1 )
      {
        cols[1] = col->getArrayItem( 1 )->getScalar();
        if( col->size() > 2 )
        {
          cols[2] = col->getArrayItem( 2 )->getScalar();
          if( col->size() > 3 )
          {
            cols[3] = col->getArrayItem( 3 )->getScalar();
          }
        }
      }
    }
  }
  catch( ... )
  {
  }
  ds << cols[0] << " " << cols[1] << " " << cols[2] << " " << cols[3] << "\n";

  for( n=0; n<npol; ++n )
  {
    if( ( n & 7 ) == 0 )
      ds << "\n";
    ds << " " << n*3+3;
  }
  ds << "\n";

  unsigned count = 0, i;
  for( n=0; n<npol; ++n )
  {
    const AimsVector<uint32_t, 3> & pol = poly[n];
    for( i=0; i<3; ++i, ++count )
    {
      if( ( count & 7 ) == 0 )
        ds << "\n";
      ds << " " << pol[i];
    }
  }
  ds << "\n\n";

  MniObjHeader hdr( fname );
  hdr.copy( obj.header() );
  if( hdr.hasProperty( "nb_t_pos" ) )
    hdr.removeProperty( "nb_t_pos" );
  if( hdr.hasProperty( "vertex_number" ) )
    hdr.removeProperty( "vertex_number" );
  hdr.writeMinf( hdr.filename() + ".minf" );
  return true;
}


