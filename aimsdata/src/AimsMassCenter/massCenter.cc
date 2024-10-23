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


#include <cartobase/config/verbose.h>
#include <aims/getopt/getopt2.h>
#include <aims/getopt/getoptProcess.h>
#include <aims/io/reader.h>
#include <aims/io/finder.h>
#include <aims/io/process.h>
#include <aims/roi/roiIterator.h>
#include <aims/utility/masscenter.h>

using namespace aims;
using namespace carto;
using namespace std;

template<typename T> 
static bool masscenter( Process &, const string &, Finder & );

class MassCenter : public Process
{
public:
  MassCenter( bool bin = false, const string & roi = "" );
  virtual ~MassCenter();

  bool   binary;
  string roi;
};

MassCenter::MassCenter( bool bin, const string & roi ) : binary( bin ), roi(roi)
{
  registerProcessType( "Volume", "S8", &masscenter<int8_t> );
  registerProcessType( "Volume", "U8", &masscenter<uint8_t> );
  registerProcessType( "Volume", "S16", &masscenter<int16_t> );
  registerProcessType( "Volume", "U16", &masscenter<int16_t> );
  registerProcessType( "Volume", "S32", &masscenter<int32_t> );
  registerProcessType( "Volume", "U32", &masscenter<int32_t> );
  registerProcessType( "Volume", "FLOAT", &masscenter<float> );
  registerProcessType( "Volume", "DOUBLE", &masscenter<double> );
}


MassCenter::~MassCenter()
{
}

template<typename T>
bool masscenter( Process & p, const string & filein, Finder & f )
{
  VolumeRef<T>   data;
  Reader<Volume<T> >  r( filein );
  string    format = f.format();
  r.setAllocatorContext( AllocatorContext( AllocatorStrategy::ReadOnly, 
                                           DataSource::none(), false, 0.01 ) );
  data.reset( r.read( 0, &format ) );
  
  MassCenter & m = ((MassCenter &) p);
  bool   bin = m.binary;
  string roi = m.roi;
  
  if (!roi.empty()) {
      rc_ptr<RoiIterator> roiIterator = getRoiIterator( roi );
      MassCenters<T>( data,
                      roiIterator,
                      bin).doit();
  }
  else {
    MassCenters<T>( data, bin).doit();
  }
  return( true );
}

int main( int argc, const char **argv )
{
  verbose = true;
  bool bin = false;
  string  roi = "";
  MassCenter proc;
  ProcessInput filein( proc );

  AimsApplication app( argc, argv,
                       "Computes position of the mass center of the image. "
                       "Also computes volume and mass of the data. \n\n"
                       "Change in AIMS 5.2: \n"
                       "Mass center position is now in mm, which is logical "
                       "and consistent with volumes which are in mm3. They "
                       "used to be in voxels, which was as a bug." );
  app.addOption( filein, "-i", "input data" );
  app.alias( "--input", "-i" );
  app.addOption( bin, "-b", "consider input image as binary data", true );
  app.alias( "--binary", "-b" );
  app.addOption( roi, "-r", "input ROI file (either ROI graph or image)", true );
  app.alias( "--roi", "-r" );
  app.alias( "-v", "--verbose" );

  try
    {
      app.initialize();
      proc.binary = bin;
      proc.roi = roi;
      if( !proc.execute( filein.filename ) )
        cout << "Couldn't process file - aborted\n";
    }
  catch( user_interruption &e )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      return( 1 );
    }

  return EXIT_SUCCESS;
}


