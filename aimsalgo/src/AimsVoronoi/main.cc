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


// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <cstdlib>
#include <aims/getopt/getopt2.h>
#include <aims/getopt/getoptProcess.h>
#include <aims/distancemap/distancemap_g.h>
#include <cartodata/volume/volume.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/finder.h>
#include <iostream>

using namespace aims;
using namespace carto;
using namespace std;


template<class T>
static bool doit( Process &, const string &, Finder & );

class Voronoi : public Process
{
public:
  Voronoi() :
    Process(),
    _xmask(3),
    _ymask(3),
    _zmask(3),
    _factor(50),
    _val_domain(0),
    _val_outside(-1)
  {
    registerProcessType( "Volume", "S8", &doit<int8_t> );
    registerProcessType( "Volume", "U8", &doit<uint8_t> );
    registerProcessType( "Volume", "S16", &doit<int16_t> );
    registerProcessType( "Volume", "U16", &doit<uint16_t> );
    registerProcessType( "Volume", "S32", &doit<int32_t> );
    registerProcessType( "Volume", "U32", &doit<uint32_t> );
  }

  void initialize( const string & filein,
                   const string & fileout,
                   int xmask,
                   int ymask,
                   int zmask,
                   float factor,
                   double val_domain,
                   double val_outside )
  {
    _filein = filein;
    _fileout = fileout;
    _xmask = xmask;
    _ymask = ymask;
    _zmask = zmask;
    _factor = factor;
    _val_domain = val_domain;
    _val_outside = val_outside;
  }

  template<class T>
  friend bool doit( Process &, const string &, Finder & );

  string _filein, _fileout;
  int _xmask, _ymask, _zmask;
  float _factor;
  double _val_domain, _val_outside;

};


template <typename T>
void _raw_value( double ddomain, double doutside, T & val_domain,
                 T & val_outside )
{
  val_domain = (T) ddomain;
  val_outside = (T) doutside;
}


template <typename T>
void _use_round( double ddomain, double doutside, T & val_domain,
                 T & val_outside )
{
  val_domain = (T) round( ddomain );
  val_outside = (T) round( doutside );
}


template <typename T>
void _round_value( double ddomain, double doutside, T & val_domain,
                   T & val_outside )
{
  _raw_value( ddomain, doutside, val_domain, val_outside );
}


template <> void _round_value( double ddomain, double doutside,
                               int8_t & val_domain, int8_t & val_outside )
{
  _use_round( ddomain, doutside, val_domain, val_outside );
}


template <> void _round_value( double ddomain, double doutside,
                               uint8_t & val_domain, uint8_t & val_outside )
{
  _use_round( ddomain, doutside, val_domain, val_outside );
}


template <> void _round_value( double ddomain, double doutside,
                               int16_t & val_domain, int16_t & val_outside )
{
  _use_round( ddomain, doutside, val_domain, val_outside );
}


template <> void _round_value( double ddomain, double doutside,
                               uint16_t & val_domain, uint16_t & val_outside )
{
  _use_round( ddomain, doutside, val_domain, val_outside );
}


template <> void _round_value( double ddomain, double doutside,
                               int32_t & val_domain, int32_t & val_outside )
{
  _use_round( ddomain, doutside, val_domain, val_outside );
}


template <> void _round_value( double ddomain, double doutside,
                               uint32_t & val_domain, uint32_t & val_outside )
{
  _use_round( ddomain, doutside, val_domain, val_outside );
}


template <> void _round_value( double ddomain, double doutside,
                               int64_t & val_domain, int64_t & val_outside )
{
  _use_round( ddomain, doutside, val_domain, val_outside );
}


template <> void _round_value( double ddomain, double doutside,
                               uint64_t & val_domain, uint64_t & val_outside )
{
  _use_round( ddomain, doutside, val_domain, val_outside );
}


template <class T>
bool doit( Process & p, const string & filein, Finder & f ) {

  Voronoi & v = (Voronoi &)p;
  VolumeRef<T> seed;
  Reader<VolumeRef<T> > dataR( filein );
  string format = f.format();
  cout << "reading seed " << filein << endl;
  if( !dataR.read( seed, 0, &format ) )
  {
    cerr << "could not read " << v._filein << endl;
    return( 1 );
  }

  VolumeRef<T> label;
  /* use round() to convert _val_domain and _val_outside from double to T
     if T is an integer type, otherwise rounding may change the value.
  */
  T val_domain, val_outside;
  _round_value( v._val_domain, v._val_outside, val_domain, val_outside );
  label = AimsVoronoiFrontPropagation( seed, val_domain,
                                       val_outside,
                                       v._xmask, v._ymask, v._zmask,
                                       v._factor );

  Writer<Volume<T> > dataW( v._fileout );
  dataW.write( *label );

  return true;
}


int main( int argc, const char* argv[] )
{
  Voronoi voronoi;
  ProcessInput pi( voronoi );

  AimsApplication app( argc, argv,
                       "Construct a Voronoi diagram from labelled seeds" );
  app.addOption( pi, "-i", "input seeds volume filename" );
  app.addOption( voronoi._fileout, "-o",
                 "output Voronoi diagram filename (same voxel type as input)" );
  app.addOption( voronoi._val_domain, "-d",
                 "value of the domain where to propagate the diagram "
                 "[default=0]", true );
  app.addOption( voronoi._val_outside, "-f",
                 "value of the outside (forbidden) of the domain [default=-1]",
                 true );
  app.addOption( voronoi._xmask, "-x",
                 "X size of the distance mask [default=3]", true );
  app.addOption( voronoi._ymask, "-y",
                 "Y size of the distance mask [default=3]", true );
  app.addOption( voronoi._zmask, "-z",
                 "Z size of the distance mask [default=3]", true );
  app.addOption( voronoi._factor, "-F",
                 "chamfer multiplication factor [default=50]",
                 true );
  app.alias( "--input", "-i" );
  app.alias( "-input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "-output", "-o" );
  app.alias( "--domain", "-d" );
  app.alias( "-domain", "-d" );
  app.alias( "--forbidden", "-f" );
  app.alias( "-forbidden", "-f" );
  app.alias( "--xmask", "-x" );
  app.alias( "-xmask", "-x" );
  app.alias( "--ymask", "-y" );
  app.alias( "-ymask", "-y" );
  app.alias( "--zmask", "-z" );
  app.alias( "-zmask", "-z" );
  app.alias( "--factor", "-F" );
  app.alias( "-Factor", "-F" );

  try
  {
    //
    // Getting options
    //
    app.initialize();

    cout << "in: " << pi.filename << endl;

    if( !voronoi.execute( pi.filename ) )
    {
      return EXIT_FAILURE;
    }
  }
  catch( user_interruption & )
  {
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
