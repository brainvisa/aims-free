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

/*
 *  Apply threshold
 */
#include <cstdlib>
#include <aims/io/io_g.h>
#include <aims/data/data_g.h>
#include <cartobase/config/verbose.h>
#include <aims/getopt/getopt2.h>
#include <aims/getopt/getoptProcess.h>
#include <aims/utility/utility_g.h>

using namespace aims;
using namespace std;
using namespace carto;

template<class T>
static bool doit( Process &, const string &, Finder & );


class Thresholder : public Process
{
public:
  Thresholder();

  template<class T>
  friend bool doit( Process &, const string &, Finder & );

  threshold_t	mode;
  string	fout;
  float		t1;
  float		t2;
  bool		bin;
  bool		clip;
  bool    	percentage;
  float		bg;
  float		fg;
};


Thresholder::Thresholder() 
  : Process()
{
  registerProcessType( "Volume", "S8", &doit<int8_t> );
  registerProcessType( "Volume", "U8", &doit<uint8_t> );
  registerProcessType( "Volume", "S16", &doit<int16_t> );
  registerProcessType( "Volume", "U16", &doit<uint16_t> );
  registerProcessType( "Volume", "S32", &doit<int32_t> );
  registerProcessType( "Volume", "U32", &doit<uint32_t> );
  registerProcessType( "Volume", "FLOAT", &doit<float> );
  registerProcessType( "Volume", "DOUBLE", &doit<double> );
}


template<class T> bool 
doit( Process & p, const string & fname, Finder & f )
{
  Thresholder		&tc = (Thresholder &) p;
  AimsData<T>		data;
  Reader<AimsData<T> >	r( fname );
  string	format = f.format();
  if ( verbose ) cout << "reading " << fname << "...\n";
  if( !r.read( data ) )
    return( false );
  if ( verbose ) cout << "reading done\n";

  if ( tc.percentage ) {
    if ( verbose ) cout << "Computing minimum and maximum" << endl;
    T mini, maxi;
    minmax( data, mini, maxi );
    tc.t1 = mini + ( maxi - mini ) * ( tc.t1 / 100 );
    tc.t2 = mini + ( maxi - mini ) * ( tc.t2 / 100 );
    if ( verbose ) {
      cout << "  minimum = " << carto::toString(mini) << endl
           << "  maximum = " << carto::toString(maxi) << endl
           << "  threshold1 = " << carto::toString(tc.t1) << endl
           << "  threshold2 = " << carto::toString(tc.t2) << endl
           << "  background = " << carto::toString(tc.bg) << endl
           << "  foreground = " << carto::toString(tc.fg) << endl;
    }
  }
  
  if ( verbose ) cout << "thresholding...\n";
  AimsThreshold<T,short> thresh( tc.mode, (T) tc.t1, (T) tc.t2, (T) tc.bg, (T) tc.fg );
  if( tc.bin )
    {
      Writer<AimsData<short> > writer( tc.fout );
      return( writer.write( thresh.bin( data ) ) );
    }
  else if( tc.clip )
    {
      Writer<AimsData<T> > writer( tc.fout );
      return( writer.write( thresh.clip( data ) ) );
    }
  else
    {
      Writer<AimsData<T> > writer( tc.fout );
      return( writer.write( thresh( data ) ) );
    }
}


int main( int argc, const char **argv )
{
  try
    {
      AimsApplication	app( argc, argv, "Threshold on data" );
      Thresholder		proc;
      ProcessInput		filein( proc );
      string		fileout, smode = "ge";
      float			t1 = 0, t2 = 0, bg = 0, fg = 32767;
      bool			binary = false;
      bool			clip = false;
      bool			percentage = false;
      threshold_t		mode;

      app.addOption( filein, "-i", "input file" );
      app.addOption( fileout, "-o", "output file" );
      app.addOption( smode, "-m", "mode that can be:\n" 
                     "lt   --> lower than\n" 
                     "le   --> lower or equal to\n" 
                     "gt   --> greater than\n"
                     "ge   --> greater or equal to\n" 
                     "eq   --> equal to\n" 
                     "di   --> differ\n" 
                     "be   --> between\n" 
                     "beel --> between, exclude lower bound\n" 
                     "beeh --> between, exclude higher bound\n" 
                     "bee  --> between, exclude both bounds\n" 
                     "ou   --> outside\n" 
                     "ouil --> outside, include lower bound\n" 
                     "ouih --> outside, include higher bound\n" 
                     "oui  --> outside, include both bounds\n" 
                     "[default: ge]", true );
      app.addOption( t1, "-t", "first threshold" );
      app.addOption( t2, "-u", "second threshold", true );
      app.addOption( percentage, "-p", 
                     "thresholds are expressed in percentage of voxel value: "
                     "threshold = min+(max-min)*(percentage/100)", true );
      app.addOption( binary, "-b", 
                     "returns a short binary data [default: false]", true );
      app.addOption( bg, "--bg", "background value set on thresholded out " 
                     "voxels in non-binary mode [default: 0]", true );
      app.addOption( fg, "--fg", "foreground value set on thresholded in " 
                     "voxels in binary mode [default: 32767]", true );
      app.addOption( clip, "--clip",
                     "clip thresholded-out values [default: false]", true );
      app.alias( "--input", "-i" );
      app.alias( "--output", "-o" );
      app.alias( "--mode", "-m" );
      app.alias( "--threshold1", "-t" );
      app.alias( "--threshold2", "-u" );
      app.alias( "--binary", "-b" );

      app.initialize();

      if      ( smode == "lt" ) mode = AIMS_LOWER_THAN;
      else if ( smode == "le" ) mode = AIMS_LOWER_OR_EQUAL_TO;
      else if ( smode == "gt" ) mode = AIMS_GREATER_THAN;
      else if ( smode == "ge" ) mode = AIMS_GREATER_OR_EQUAL_TO;
      else if ( smode == "eq" ) mode = AIMS_EQUAL_TO;
      else if ( smode == "di" ) mode = AIMS_DIFFER;
      else if ( smode == "be" ) mode = AIMS_BETWEEN;
      else if ( smode == "beel" ) mode = AIMS_BETWEEN_EXCLUDE_LOWER_BOUND;
      else if ( smode == "beeh" ) mode = AIMS_BETWEEN_EXCLUDE_HIGHER_BOUND;
      else if ( smode == "bee" ) mode = AIMS_BETWEEN_EXCLUDE_BOUNDS;
      else if ( smode == "ou" ) mode = AIMS_OUTSIDE;
      else if ( smode == "ouil" ) mode = AIMS_OUTSIDE_INCLUDE_LOWER_BOUND;
      else if ( smode == "ouih" ) mode = AIMS_OUTSIDE_INCLUDE_HIGHER_BOUND;
      else if ( smode == "oui" ) mode = AIMS_OUTSIDE_INCLUDE_BOUNDS;
      else
        AimsError("AimsThreshold : bad mode" );

      if(binary && clip)
        AimsError("AimsThreshold: --binary and --clip cannot be used together");

      proc.mode = mode;
      proc.fout = fileout;
      proc.t1 = t1;
      proc.t2 = t2;
      proc.bin = binary;
      proc.clip = clip;
      proc.bg = bg;
      proc.fg = fg;
      proc.percentage = percentage;
      proc.execute( filein.filename );
      if ( verbose ) cout << "done\n";

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
