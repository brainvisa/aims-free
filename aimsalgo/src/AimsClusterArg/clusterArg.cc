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
#include <aims/roi/clusterArg.h>
#include <aims/getopt/getopt2.h>
#include <aims/getopt/getoptProcess.h>
#include <aims/io/reader.h>
#include <aims/io/process.h>
#include <aims/io/finder.h>
#include <aims/connectivity/component.h>
#include <aims/io/aimsGraphW.h>
#include <aims/io/aimsGraphR.h>
#include <aims/def/path.h>
#include <aims/bucket/bucket.h>
#include <aims/mesh/surface.h>
#include <aims/mesh/mesher.h>
#include <aims/utility/converter_bucket.h>
#include <aims/utility/converter_volume.h>
#include <aims/resampling/motion.h>
#include <graph/graph/graph.h>
#include <cartobase/object/syntax.h>
#include <cartobase/object/sreader.h>
#include <graph/graph/gwriter.h>
#include <cartobase/config/paths.h>
#include <stdio.h>
#include <float.h>

using namespace aims;
using namespace carto;
using namespace std;


template<typename T> 
bool makearg( Process &, const string &, Finder & );

class ClusterArg : public Process
{
public:
  ClusterArg();
  virtual ~ClusterArg() {}

  template<typename T> bool makeArg( VolumeRef<T> & );

  string		fileout;
  Connectivity::Type	connectivity;
  int			minsize;
  string		type;
  string		matrix;
  bool			domesh;
  float			deciMaxClearance;
  float			deciMaxError;
  uint			minFacetNumber;
  bool			binarize;
  float			lowth;
  float			upth;
};


ClusterArg::ClusterArg()
  : Process()
{
  registerProcessType( "Volume", "U8", &makearg<byte> );
  registerProcessType( "Volume", "S16", &makearg<short> );
  registerProcessType( "Volume", "FLOAT", &makearg<float> );
}


template<typename T> bool
makearg( Process & p, const string & fname, Finder & f )
{
  VolumeRef<T>		datain;
  string		format = f.format();
  cout << "Reading input data...\n";
  Reader<VolumeRef<T> >	r( fname );
  if( !r.read( datain, 1, &format ) )
    return( false );
  cout << "done.\n";

  ClusterArg	& cp = (ClusterArg &) p;
  return cp.makeArg( datain );
}


template<typename T>
bool ClusterArg::makeArg( VolumeRef<T> & data )
{
  // graph and global attributes

  string		graphsyntax = "ClusterArg";

  if( type == "f" )
    graphsyntax = "CorticalFoldArg";

  Graph		gr( graphsyntax );

  ClusterArgMaker	cm( fileout, connectivity, minsize, matrix, domesh, 
                        deciMaxClearance, deciMaxError, minFacetNumber,
                        binarize, lowth, upth );
  cm.make( gr, data );

  // write arg

  cout << "writing cluster graph..." << endl;
  carto::SyntaxSet	ss;
  SyntaxReader	sr( Paths::findResourceFile(
    "nomenclature/syntax/graph.stx", "aims" ) );
  sr >> ss;
  AimsGraphWriter	agw( fileout );
  agw.writeElements( gr, AimsGraphWriter::Global );
  GraphWriter	gw( fileout, ss );
  gw << gr;
  cout << "done." << endl;

  return( true );
}


int main( int argc, const char **argv )
{
  string	fileout, conn = "18", type = "c", matrix;
  int		minsize = 0;
  float		deciMaxClearance = 5.;
  float		deciMaxError = 0.1;
  uint		minFacetNumber = 50;
  bool		bin = false, mesh = true;
  float		low = -FLT_MAX, up = FLT_MAX;
  ClusterArg	proc;
  ProcessInput	filein( proc );

  AimsApplication	app( argc, argv, 
                             "Builds a graph (cluster graph) from an image" );
  app.addOption( filein, "-i", "input data" );
  app.addOption( fileout, "-o", "output .arg graph file (default: " 
                 "<input>.arg)", true );
  app.addOption( type, "-g", "graph type to produce: c (cluster) or f " 
                 "(cortical folds). default: cluster", true );
  app.addOption( matrix, "-t", "transformation matrix to Talairach space " 
                 "(default: none)", true );
  app.addOption( conn, "-c", "connectivity type (4xy / 4xz / 4yz / 6 / 8xy / " 
                 "8xz / 8yz / 18 / 26), default: 18", true );
  app.addOption( minsize, "-s", "min cluster size (default: 0)", true );
  app.addOption( bin, "-b", "binarize volume (after thresholding) before " 
                 "processing connectivity stuff", true );
  app.addOption( low, "-l", "lower threshold, values under this one are set " 
                 "to zero (background) (default: none)", true );
  app.addOption( up, "-u", "upper threshold, values above this one are set " 
                 "to zero (background) (default: none)", true );
  app.addOption( mesh, "-m", "make cluster meshes (default: y)", true );
  app.addOption( deciMaxClearance, "--deciMaxClearance", 
                 "maximum clearance of the decimated mesh expected is " 
                 "<clearance> mm [default=5.]", true );
  app.addOption( deciMaxError, "--deciMaxError", 
                 "maximum error distance from the original mesh " 
                 "expected is <error> mm , [default=0.1]", true );
  app.addOption( minFacetNumber, "--minFacetNumber", 
                 "minimum number of facets to allow decimation " 
                 "[default=50]", true );

  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "--graphtype", "-g" );
  app.alias( "--trans", "-t" );
  app.alias( "--connectivity", "-c" );
  app.alias( "--size", "-s" );
  app.alias( "--binarize", "-b" );
  app.alias( "--lowth", "-l" );
  app.alias( "--upth", "-u" );
  app.alias( "--mesh", "-m" );

  try
    {
      app.initialize();

      if( fileout.empty() )
        {
          fileout = filein.filename;
          string::size_type	pos = fileout.rfind( '.' );
          if( pos != string::npos )
            fileout.erase( pos, fileout.length() - pos );
          fileout += ".arg";
        }

      Connectivity::Type	connectivity;

      if ( conn == "4xy" ) 
        connectivity = Connectivity::CONNECTIVITY_4_XY;
      else if ( conn == "4xz" ) 
        connectivity = Connectivity::CONNECTIVITY_4_XZ;
      else if ( conn == "4yz" ) 
        connectivity = Connectivity::CONNECTIVITY_4_YZ;
      else if ( conn == "6" ) 
        connectivity = Connectivity::CONNECTIVITY_6_XYZ;
      else if ( conn == "8xy" ) 
        connectivity = Connectivity::CONNECTIVITY_8_XY;
      else if ( conn == "8xz" ) 
        connectivity = Connectivity::CONNECTIVITY_8_XZ;
      else if ( conn == "8yz" ) 
        connectivity = Connectivity::CONNECTIVITY_8_YZ;
      else if ( conn == "18" ) 
        connectivity = Connectivity::CONNECTIVITY_18_XYZ;
      else if ( conn == "26" ) 
        connectivity = Connectivity::CONNECTIVITY_26_XYZ;
      else
        throw invalid_number( "Invalid connectivity value" );

      ASSERT( deciMaxClearance >= 0.0 );
      ASSERT( deciMaxError >= 0.0 );

      /* ClusterArg	proc( fileout, connectivity, minsize, type, matrix, 
                              mesh, deciMaxClearance, deciMaxError, 
                              minFacetNumber, bin, low, up ); */
      proc.fileout = fileout;
      proc.connectivity = connectivity;
      proc.minsize = minsize;
      proc.type = type;
      proc.matrix = matrix;
      proc.domesh = mesh;
      proc.deciMaxClearance = deciMaxClearance;
      proc.deciMaxError = deciMaxError;
      proc.minFacetNumber = minFacetNumber;
      proc.binarize = bin;
      proc.lowth = low;
      proc.upth = up;

      if( !proc.execute( filein.filename ) )
	{
	  cerr << "Failed - wrong input data file ?\n";
	  return( 1 );
	}
      return EXIT_SUCCESS;
    }
  catch( user_interruption & )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
    }

  return EXIT_FAILURE;
}


