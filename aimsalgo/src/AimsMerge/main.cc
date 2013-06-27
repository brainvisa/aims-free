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
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/data/data.h>
#include <aims/io/process.h>
#include <aims/io/finder.h>
#include <aims/getopt/getopt2.h>
// #include <aims/getopt/getoptProcess.h>
#include <aims/utility/utility_g.h>

using namespace aims;
using namespace carto;
using namespace std;


template<class T>
static bool doit( Process &, const string &, Finder & );

template<typename T, typename U>
static bool doit2( Process &, const string &, Finder & );

class Merger;

template <typename T>
class Merger2 : public Process
{
public:
  Merger2( Merger &, const string & );

//   template <typename U>
//   friend /*template <typename T, typename U>*/ bool doit2( Process &, const string &, Finder & );

  Merger & merger;
  string format;
};


class Merger : public Process
{
public:
  Merger( const string & fout, double val, int lab, merge_t m,
          const string & msk );

private:
  template<typename T>
  friend class Merger2;

  template<typename T>
  friend bool doit( Process &, const string &, Finder & );
  template<typename T, typename U>
  friend bool doit2( Process &, const string &, Finder & );

  string                fileout;
  double                value;
  int                   label;
  merge_t               mode;
  string                mask;
};


template <typename T>
Merger2<T>::Merger2( Merger & merger, const string & format )
  : merger( merger ), format( format )
{
  registerProcessType( "Volume", "S8", &doit2<T, int8_t> );
  registerProcessType( "Volume", "U8", &doit2<T, uint8_t> );
  registerProcessType( "Volume", "S16", &doit2<T, int16_t> );
  registerProcessType( "Volume", "U16", &doit2<T, uint16_t> );
  registerProcessType( "Volume", "S32", &doit2<T, int32_t> );
  registerProcessType( "Volume", "U32", &doit2<T, uint32_t> );
}


template<typename T, typename U>
bool doit2( Process & p, const string & fname, Finder & f )
{
  Merger2<T>            & mp = (Merger2<T> &) p;
  Merger                & m = mp.merger;
  string                format = mp.format;
  Reader<AimsData<T> >  r( fname );
  AimsData<T>           in;

  if( !r.read( in, 0, &format ) )
    return false;

  AimsData<U> mask;
  Reader<AimsData<U> > mreader( m.mask );
  mreader.read( mask );

  AimsMerge<T,U> merge( m.mode, (T) m.value, (U) m.label );
  Writer<AimsData<T> > writer( m.fileout );
  return writer.write( merge( in, mask ) );
}


Merger::Merger( const string & fout, double val, int lab, merge_t m,
        const string & msk )
  : Process(), fileout( fout ), value( val ), label( lab ), mode( m ),
    mask( msk )
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


template<class T>
bool doit( Process & p, const string & fname, Finder & f )
{
  Merger		& mp = (Merger &) p;
  string		format = f.format();

  Merger2<T> m2( mp, format );
  return m2.execute( mp.mask );
}


int main( int argc, const char **argv )
{
  string filein, fileout, smask, smode;
  double value = 0;
  int label = 0;
  merge_t mode;

  AimsApplication app( argc, argv, "Merge a volume and a byte label volume" );
  app.addOption( filein, "-i", "origin file" );
  app.alias( "--input", "-i" );
  app.addOption( fileout, "-o", "output file" );
  app.alias( "--output", "-o" );
  app.addOption( smask, "-M", "mask file (int voxel type)" );
  app.alias( "--Mask", "-M" );
  app.addOption( smode, "-m", "mode that can be:\nsv --> same values\n"
    "oo --> one to one\n"
    "ao --> all to one\n"
    "om --> one to maximum plus 1\n"
    "am --> all to maximum plus 1" );
  app.alias( "--mode", "-m" );
  app.addOption( label, "-l", "only label to get into account", true );
  app.alias( "--label", "-l" );
  app.addOption( value, "-v", "replacement value", true );
  app.alias( "--value", "-v" );

  try
  {
    app.initialize();

    if      (smode=="sv" ) mode = AIMS_MERGE_SAME_VALUES;
    else if (smode=="oo" ) mode = AIMS_MERGE_ONE_TO_ONE;
    else if (smode=="ao" ) mode = AIMS_MERGE_ALL_TO_ONE;
    else if (smode=="om" ) mode = AIMS_MERGE_ONE_TO_MAXP1;
    else if (smode=="am" ) mode = AIMS_MERGE_ALL_TO_MAXP1;
    else
      AimsError("AimsMerge : bad mode" );

    Merger	proc( fileout, value, label, mode, smask );
    if( !proc.execute( filein ) )
      throw runtime_error( "couldn't process" );

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
