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

#ifdef __APPLE__
/* On MacOS, buggy <cmath> undefines isnan, and tries to make it a 
   template function. But it doesn't work so we end up with no 
   isnan() at all.
   So we have to include <cmath> before <cartobase/config/cartobase_config.h>
   in order to have a working macro afterwards.
*/
#include <cmath>
#endif

#include <cartobase/config/cartobase_config.h>
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


class NaNRemover : public Process
{
public:
  NaNRemover();

  template<class T>
  friend bool doit( Process &, const string &, Finder & );

  bool noPercentage;
  double value;
  string fileout;
};


NaNRemover::NaNRemover() 
  : Process()
{
  registerProcessType( "Volume", "FLOAT", &doit<float> );
  registerProcessType( "Volume", "DOUBLE", &doit<double> );
}


template<class T> bool 
doit( Process & p, const string & fname, Finder & f )
{
  NaNRemover &nr = static_cast<NaNRemover &>( p );
  AimsData<T> data;
  Reader<AimsData<T> >	r( fname );
  string	format = f.format();
  if ( verbose ) cout << "reading " << fname << "...\n";
  if( ! r.read( data ) )
    throw runtime_error( "Cannot read input image" );
  if ( verbose ) cout << "reading done\n";

  if ( ! nr.noPercentage ) {
    if ( verbose ) cout << "Computing minimum and maximum" << endl;
    T mini, maxi;
    size_t countNaN = 0;
    size_t countNormal = 0;
    int x, y, z, t;
    bool firstValue = true;
    ForEach4d( data, x, y ,z, t ) {
      if ( ! isnan( data( x, y, z, t ) ) ) {
        if ( firstValue ) {
          mini = maxi = data( x, y, z, t );
          firstValue = false;
        } else {
          if ( mini > data( x, y, z, t ) ) mini = data( x, y, z, t );
          if ( maxi < data( x, y, z, t ) ) maxi = data( x, y, z, t );
        }
        ++countNormal;
      } else {
        ++countNaN;
      }
    }
    nr.value = (T) mini + ( maxi - mini ) * ( (T) nr.value / 100 );
    if ( verbose ) {
      cout << "  minimum = " << mini << endl
           << "  maximum = " << maxi << endl
           << "  value = " << nr.value << endl
           << "  number of NaN = " << countNaN << endl
           << "  number of valid numbers = " << countNormal << endl;
    }
  }
  
  if ( verbose ) cout << "processing...\n";
  int x, y, z, t;
  ForEach4d( data, x, y ,z, t ) {
    if ( isnan( data( x, y, z, t ) ) ) data( x, y, z, t ) = (T) nr.value;
  }
  
  if ( ! nr.fileout.empty() ) {
    if ( verbose ) {
      cout << "writing result...\n";
    }
    Writer< AimsData<T> > writer( nr.fileout );
    writer.write( data );
  }
  return true;
}


int main( int argc, const char **argv )
{
  try
    {
      AimsApplication	app( argc, argv, "Remove NaN (Not a Number)  values from an image" );
      NaNRemover proc;
      ProcessInput filein( proc );

      proc.value = 0;
      proc.noPercentage = false;

      app.addOption( filein, "-i", "input file" );
      app.addOption( proc.fileout, "-o", "output file (if ommited, only information about NaN will be showed)", true );
      app.addOption( proc.value, "--value", "value used to replace NaN value "
                     "(default = 0, with default -np value it corresponds to "
                     "the image minimum)", true );
      app.addOption( proc.noPercentage, "-np", 
                     "By default value is expressed in percentage of voxel "
                     "value: value = min+(max-min)*(percentage/100). If this "
                     "option is used, value is used direclty", true );

      app.initialize();

      if ( proc.fileout.empty() and verbose < 1 ) {
        verbose = 1;
      }

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
