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


#include <cstdlib>
#include <aims/data/data_g.h>
#include <cartobase/allocator/mappingcopy.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt2.h>
#include <aims/rgb/rgb.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/surfaceOperation.h>
#include <aims/bucket/bucket.h>
#include <stdlib.h>


using namespace aims;
using namespace carto;
using namespace std;



template<class T>
static bool doit( Process &, const string &, Finder & );

template<int D>
bool doitMesh( Process &, const string &, Finder & );
bool doitBucket( Process &, const string &, Finder & );


class ZCat : public Process
{
public:
  ZCat( const list<string> & names, const string & out, 
	const vector<int> & dim, const vector<float> voxs );

private:
  template<class T>
  friend bool doit( Process &, const string &, Finder & );
  template<int D>
  friend bool doitMesh( Process &, const string &, Finder & );
  friend bool doitBucket( Process &, const string &, Finder & );
  template<class T>
  bool cat( AimsData<T> & );
  template<int D>
  bool catMesh( AimsTimeSurface<D, Void> & );
  bool catBucket( BucketMap<Void> & );

  list<string>	listName;
  string	fileOut;
  vector<int>	dims;
  vector<float>	vs;
};


ZCat::ZCat( const list<string> & names, const string & out, 
	const vector<int> & dim, const vector<float> voxs )
    : Process(), listName( names ), fileOut( out ), 
      dims( dim ), vs( voxs )
{
  registerProcessType( "Volume", "S8", &doit<int8_t> );
  registerProcessType( "Volume", "U8", &doit<uint8_t> );
  registerProcessType( "Volume", "S16", &doit<int16_t> );
  registerProcessType( "Volume", "U16", &doit<uint16_t> );
  registerProcessType( "Volume", "S32", &doit<int32_t> );
  registerProcessType( "Volume", "U32", &doit<uint32_t> );
  registerProcessType( "Volume", "FLOAT", &doit<float> );
  registerProcessType( "Volume", "DOUBLE", &doit<double> );
  registerProcessType( "Volume", "CFLOAT", &doit<cfloat> );
  registerProcessType( "Volume", "CDOUBLE", &doit<cdouble> );
  registerProcessType( "Volume", "RGB", &doit<AimsRGB> );
  registerProcessType( "Volume", "RGBA", &doit<AimsRGBA> );
  registerProcessType( "Volume", "POINT3DF", &doit<Point3df> );
  registerProcessType( "Mesh", "VOID", &doitMesh<3> );
  registerProcessType( "Mesh2", "VOID", &doitMesh<2> );
  registerProcessType( "Mesh4", "VOID", &doitMesh<4> );
  registerProcessType( "Bucket", "VOID", &doitBucket );
}


template<class T>
bool doit( Process & p, const string &, Finder & )
{
  ZCat &zp = (ZCat &) p;

  cout << "concatenate...\n";
  Allocator	al;

  AimsData<T> out( zp.dims[0], zp.dims[1], zp.dims[2], zp.dims[3] );

  return( zp.cat( out ) );
}


template<int D>
bool doitMesh( Process & p, const string &, Finder & )
{
  ZCat	& zp = (ZCat &) p;

  AimsTimeSurface<D, Void> out;

#if (__GNUC__==4) && (__GNUC_MINOR__==2) && (__GNUC_PATCHLEVEL__==2)
  /* This cout, by chance, fixes a compiler bug of gcc 4.2.2 on Mandriva
     2008. This bug caused some constructors (on the "out" variable) not
     to be properly called, and lead to memory corruptions, use of
     uninitialized memory, and crashes.
     see:
     https://brainvisa:Soma2009@bioproj.extra.cea.fr/redmine/issues/2832
  */
  cout << "Meshes concatenation\n";
#endif
  return( zp.catMesh( out ) );
}


template<class T> bool ZCat::cat( AimsData<T> & out )
{
  int x, y, z, t, Z = 0;  
  list<string>::const_iterator	it;
  unsigned n = 0;
  bool copy_header = true;
  
  for ( it = listName.begin() ; it != listName.end(); it++, ++n )
    {
      cout << n << " : adding " << *it << "..." << endl;
      Reader<AimsData<T> > dataR( *it );
      dataR.setAllocatorContext( AllocatorContext
                                 ( AllocatorStrategy::ReadOnly, 
                                   DataSource::none(), false, 0.1 ) );
      AimsData<T> in;
      dataR >> in;

      if ( copy_header ) {
        out.volume()->header() = in.volume()->header();
        copy_header = false;
      }

      ForEach4d( in, x, y, z, t )
        out( x, y, Z + z, t ) = in( x, y, z, t );
      Z += in.dimZ();
    }
  // It is necessary to set voxel size here, because copy_header could have set wrong voxel sizes
  out.setSizeXYZT( vs[0], vs[1], vs[2], vs[3] );
  Writer<AimsData<T> > dataW( fileOut );
  dataW << out;

  return true;
}


template<int D>
bool ZCat::catMesh( AimsTimeSurface<D, Void> &out )
{
  list<string>::const_iterator	it;
  unsigned			n = 0;

  for ( it = listName.begin() ; it != listName.end(); it++, ++n )
    {
      cout << n << " : adding " << *it << "..." << endl;
      Reader< AimsTimeSurface<D, Void> > dataR( *it );
      AimsTimeSurface<D, Void> in;
      dataR >> in;
      SurfaceManip::meshMerge( out, in );
    }
  Writer< AimsTimeSurface<D, Void> > dataW( fileOut );
  dataW << out;

  return true;
}


bool ZCat::catBucket( BucketMap<Void> &out )
{
  list<string>::const_iterator	it;
  unsigned			n = 0;
  BucketMap<Void>::Bucket	&outb = out[0];

  for ( it = listName.begin() ; it != listName.end(); it++, ++n )
    {
      cout << n << " : adding " << *it << "..." << endl;
      Reader< BucketMap<Void> > dataR( *it );
      BucketMap<Void> in;
      dataR >> in;
      outb.insert( in.begin()->second.begin(), in.begin()->second.end() );
    }
  Writer< BucketMap<Void> > dataW( fileOut );
  dataW << out;

  return true;
}


bool doitBucket( Process & p, const string &, Finder & )
{
  ZCat	& zp = (ZCat &) p;

  BucketMap<Void>	out;

#if (__GNUC__==4) && (__GNUC_MINOR__==2) && (__GNUC_PATCHLEVEL__==2)
  /* This cout, by chance, fixes a compiler bug of gcc 4.2.2 on Mandriva
     2008. This bug caused some constructors (on the "out" variable) not
     to be properly called, and lead to memory corruptions, use of
     uninitialized memory, and crashes.
     see:
     https://brainvisa:Soma2009@bioproj.extra.cea.fr/redmine/issues/2832
  */
  cout << "Buckets concatenation\n";
#endif
  return( zp.catBucket( out ) );
}


int main( int argc, const char** argv )
{ 
  int result = EXIT_SUCCESS;
 
  try {
    list< string > listName;
    string fileOut;
    bool memmap = false;
    bool nocheckvs = false;
    Point3df voxelsize(0.,0.,0.);  

    AimsApplication application( argc, argv,
                                 "concatenates volumes (along Z axis), " 
                                 "meshes or buckets" );
    application.addOptionSeries( listName, "-i",
                                 "input files (1 minimum)", 1 );
    application.addOption( fileOut, "-o",
                           "Output file name" );
    application.addOption( memmap, "--memmap", 
                           "read in Memory Mapping mode (obsolete, " 
                           "automatic)", true );
    application.addOption( nocheckvs, "--nocheckvs", "Don't check if voxel " 
                           "sizes of all volumes to concatenate match", true );
    application.addOption( voxelsize[0], "--vsx", "Force voxelsize X value", true );
    application.addOption( voxelsize[1], "--vsy", "Force voxelsize Y value", true );
    application.addOption( voxelsize[2], "--vsz", "Force voxelsize Z value", true );
    application.initialize();

    //
    // check homogeneity of sizes
    //
    list<string>::const_iterator it = listName.begin();
    Finder	f;
    
    cout << "check homogeneity of sizes...\n0" << flush;
    ASSERT( f.check( *it ) );
    const PythonHeader	*hdr 
      = dynamic_cast<const PythonHeader *>( f.header() );
    if( !hdr )
      {
	cerr << "Could not read header of " << *it << endl;
	return EXIT_FAILURE;
      }
    
    string	otype = f.objectType();
    string	dtype = f.dataType();
    
    vector<int>	dims, dims2;
    vector<float> vs, vs2;
    if ( otype == "Volume" ) {
      ASSERT( hdr->getProperty( "volume_dimension", dims ) );
      ASSERT( hdr->getProperty( "voxel_size", vs ) );

      while( dims.size() < 4 )
	  dims.push_back( 1 );

      while( vs.size() < 4 )
	vs.push_back( 1 );
      
      for(int i=0; i<3; ++i)
      if( voxelsize[i] != 0 )
        {
          vs[i] = voxelsize[i];
          nocheckvs = 1;
        }
      
      int dimZ = dims[2];
      unsigned	n = 1;
      
      for ( ++it; it != listName.end(); ++it, ++n ) {
        cout << "\rn" << flush;
        ASSERT( f.check( *it ) );
        hdr = dynamic_cast<const PythonHeader *>( f.header() );
        if( !hdr ) {
          cerr << "Could not read header of " << *it << endl;
          return EXIT_FAILURE;
        }
        ASSERT( hdr->getProperty( "volume_dimension", dims2 ) );
        ASSERT( hdr->getProperty( "voxel_size", vs2 ) );
        ASSERT( f.objectType() == otype );
        ASSERT( f.dataType() == dtype );
        ASSERT( dims[0] == dims2[0] && dims[1] == dims2[1] 
          && ( ( dims2.size() < 4 && dims[3] == 1 ) 
              || ( dims2.size() == 4 &&  dims[3] == dims2[3] ) ) );
        dimZ += dims2[2];
              if( !nocheckvs )
          ASSERT( vs[0] == vs2[0] && vs[1] == vs2[1] && vs[2] == vs2[2] );
      }
      cout << endl << dimZ << " slices\n";
      
      dims[2] = dimZ;
    }
    ZCat proc( listName, fileOut, dims, vs );
    if( !proc.execute( f, "toto" ) )
    {
      cerr << "Failed\n";
      return EXIT_FAILURE;
    }
  }
  catch( user_interruption &e ) {
  }
  catch( std::exception &e ) {
    cerr << argv[ 0 ] << ": " << e.what() << endl;
    result = EXIT_FAILURE;
  }    
  return result;
}
