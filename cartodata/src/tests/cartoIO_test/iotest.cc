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
#ifdef USE_SOMA_IO
  ERROR_FILE_SHOULDNT_BE_INCLUDED;
#endif
#include <cstdlib>
#include <cartobase/io/datasourceinfo.h>
#include <cartobase/type/limits.h>
#include <cartobase/datasource/filedatasource.h>
#include <cartobase/object/object.h>
#include <cartobase/object/pythonwriter.h>
#include <cartobase/io/readeralgorithm.h>
#include <cartobase/io/reader.h>
#include <cartobase/io/writer.h>
#include <cartobase/io/formatdictionary.h>
#include <cartodata/volume/volumeview.h>
#include <cartobase/getopt/getopt.h>
#include <cartobase/config/verbose.h>
#include <iostream>

using namespace carto;
using namespace std;

class Readalgo : public ReaderAlgorithm
{
public:
  Readalgo( const string & ofname, bool partial );

  string	output;
  bool		partialtest;
};


template <typename T>
class IOTest
{
public:
  static bool iotest( ReaderAlgorithm &, Object, rc_ptr<DataSource> );
};


template <typename T>
class IOTest<Volume<T> >
{
public:
  static bool iotest( ReaderAlgorithm &, Object, rc_ptr<DataSource> );
};


void printheader( Object hdr )
{
  PythonWriter	pw;
  pw.attach( cout );
  pw.write( hdr );
}


template <typename T>
void write( T & obj, const string & fname )
{
  cout << "Writing volume to " << fname << endl;
  cout << "WARNING: Writing is not completely implemented !" << endl;
  Writer<T>	w( fname );
  w.write( obj );
}


template<typename T>
bool IOTest<T>::iotest( ReaderAlgorithm & algo, Object hdr, 
                       rc_ptr<DataSource> source )
{
  cout << "reading object of type " << DataTypeCode<T>::name() << endl;
  Readalgo	& ralgo = (Readalgo &) algo;
  Reader<T>	r( source );
  r.setAllocatorContext( AllocatorContext( AllocatorStrategy::ReadOnly ) );
  auto_ptr<T>	obj;
  obj.reset( r.read( hdr ) );
  cout << "object read\n";

  if( !ralgo.output.empty() )
    write( obj, ralgo.output );

  return true;
}


template<>
bool IOTest<GenericObject>::iotest( ReaderAlgorithm & algo, Object hdr, 
                                    rc_ptr<DataSource> source )
{
  cout << "reading object of type " << DataTypeCode<GenericObject>::name() 
       << endl;
  Readalgo	& ralgo = (Readalgo &) algo;
  Reader<GenericObject>	r( source );
  r.setAllocatorContext( AllocatorContext( AllocatorStrategy::ReadOnly ) );
  Object	obj;
  obj.reset( r.read( hdr ) );
  cout << "object:\n";
  PythonWriter	pw;
  pw.attach( cout );
  pw.write( obj );

  if( !ralgo.output.empty() )
    write( *obj, ralgo.output );

  return true;
}


template<typename T>
bool IOTest<Volume<T> >::iotest( ReaderAlgorithm & algo, Object hdr, 
                                 rc_ptr<DataSource> source )
{
  cout << "reading object of type " << DataTypeCode<Volume<T> >::name() 
       << endl;
  Readalgo	& ralgo = (Readalgo &) algo;

  Reader<Volume<T> >	r( source );
  r.setAllocatorContext( AllocatorContext( AllocatorStrategy::ReadOnly ) );
  VolumeRef<T>		vol;
  vol.reset( r.read( hdr ) );
  cout << "volume read\n";

  T	M = -numeric_limits<T>::max(), m = numeric_limits<T>::max();
  int	x, y, z, t, nx = vol->getSizeX(), ny = vol->getSizeY(), 
    nz = vol->getSizeZ(), nt = vol->getSizeT();
  T	*buf;
  int	xm = 0, xM = 0, ym = 0, yM = 0, zm = 0, zM = 0, tm = 0, tM = 0;

  for( t=0; t<nt; ++t )
    for( z=0; z<nz; ++z )
      for( y=0; y<ny; ++y )
        {
          buf = &vol->at( 0, y, z, t );
          for( x=0; x<nx; ++x, ++buf )
            {
              if( *buf < m )
                {
                  m = *buf;
                  xm = x;
                  ym = y;
                  zm = z;
                  tm = t;
                }
              if( *buf > M )
                {
                  M = *buf;
                  xM = x;
                  yM = y;
                  zM = z;
                  tM = t;
                }
            }
        }
  cout << "min: " << m << " (" << xm << "," << ym << "," << zm << "," 
       << tm << "), max: " << M << " (" << xM << "," << yM << "," 
       << zM << "," << tM << ")" << endl;

  if( !ralgo.output.empty() )
    write( *vol, ralgo.output );

  if( ralgo.partialtest )
    {
      cout << "partial reading test..." << endl;

      Reader<Volume<T> >	r2( source );
      Object	options = Object::value( PropertySet() );
      options->setProperty( "unallocated", true );
      cout << "open big unallocated volume..." << endl;
      r2.setOptions( options );
      VolumeRef<T>	vol2( r2.read() );
      std::cout << "size: " << vol2->getSizeX() << ", " 
                << vol2->getSizeY() << "," << vol2->getSizeZ() << ", " 
                << vol2->getSizeT() << endl;
      
      Reader<Volume<T> > r3( vol2->allocatorContext().dataSource() );
      options = Object::value( PropertySet() );
      options->setProperty( "partial_reading", true );
      r3.setOptions( options );
      VolumeRef<T>	vol3
        ( new VolumeView<T>( vol2, 
                             typename VolumeView<T>::Position4Di
                             ( vol2->getSizeX() / 4, 
                               vol2->getSizeY() / 4, 
                               vol2->getSizeZ() / 4, 0 ), 
                             typename VolumeView<T>::Position4Di
                             ( vol2->getSizeX() / 2, 
                               vol2->getSizeY() / 2, 
                               vol2->getSizeZ() / 2, 1 )
                             ) );
      /* std::cout << "view size: " << vol3->getSizeX() << ", " 
         << vol3->getSizeY() << ", " << vol3->getSizeZ() << ", " 
         << vol3->getSizeT() << endl; */
      cout << "reading subvolume..." << endl;
      r3.read( *vol3, Object::reference( vol->header() ) );
      std::cout << "view size: " << vol3->getSizeX() << ", " 
                << vol3->getSizeY() << ", " << vol3->getSizeZ() << ", " 
                << vol3->getSizeT() << endl;

      nx = vol3->getSizeX();
      ny = vol3->getSizeY();
      nz = vol3->getSizeZ();
      nt = vol3->getSizeT();
      M = -numeric_limits<T>::max();
      m = numeric_limits<T>::max();
      xm = 0;
      xM = 0;
      ym = 0;
      yM = 0;
      zm = 0;
      zM = 0;
      tm = 0;
      tM = 0;
      for( t=0; t<nt; ++t )
        for( z=0; z<nz; ++z )
          for( y=0; y<ny; ++y )
            {
              buf = &vol3->at( 0, y, z, t );
              for( x=0; x<nx; ++x, ++buf )
                {
                  if( *buf < m )
                    {
                      m = *buf;
                      xm = x;
                      ym = y;
                      zm = z;
                      tm = t;
                    }
                  if( *buf > M )
                    {
                      M = *buf;
                      xM = x;
                      yM = y;
                      zM = z;
                      tM = t;
                    }
                }
            }
      cout << "min: " << m << " (" << xm << "," << ym << "," << zm << "," 
           << tm << "), max: " << M << " (" << xM << "," << yM << "," 
           << zM << "," << tM << ")" << endl;
      typename VolumeView<T>::Position4Di 
        p = static_cast<VolumeView<T> *>( vol3.get() )->posInRefVolume();
      if( M == vol->at( xM + p[0], yM + p[1], zM + p[2], tM + p[3] ) )
        cout << "maximums match." << endl;
      else
        {
          cerr << "ERROR: maximums don't match !" << endl;
          return false;
        }
    }
  return true;
}


template<typename T>
bool fileinfo( ReaderAlgorithm & algo, Object hdr, 
               rc_ptr<DataSource> source )
{
  printheader( hdr );
  Readalgo	ralgo = (Readalgo &) algo;
  if( carto::verbose || ralgo.partialtest || !ralgo.output.empty() )
    return IOTest<T>::iotest( algo, hdr, source );
  return true;
}


Readalgo::Readalgo( const string & ofname, bool partial )
  : ReaderAlgorithm( "IO test" ), output( ofname ), partialtest( partial )
{
  registerAlgorithmType( "Volume of S16", &fileinfo<Volume<int16_t> > );
  registerAlgorithmType( DataTypeCode<GenericObject>::name(), 
                         &fileinfo<GenericObject> );
}


int main( int argc, const char** argv )
{
  string		fname;
  try
    {
      string		ofname;
      bool		partialtest = false;

      CartoApplication	app( argc, argv, "Test for cartograph IO system" );
      app.addOption( fname, "-i", "input filename to be read\n" );
      app.addOption( ofname, "-o", "output file (copy of input) to perform " 
                     "a writing test", true );
      app.addOption( partialtest, "-p", "perform patial reading test", true );
      app.alias( "-v", "--verbose" );

      app.initialize();

      Readalgo	algo( ofname, partialtest );
      algo.execute( fname );
    }
  catch( user_interruption & )
    {
    }
  catch( datatype_format_error & e )
    {
      DataSourceInfo		f;
      rc_ptr<DataSource>	ds( new FileDataSource( fname ) );
      Object			hdr = f.check( *ds );
      if( !hdr.isNone() )
        printheader( hdr );
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}


