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
#include <aims/getopt/getopt.h>
#include <aims/distancemap/distancemap_g.h>
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <iostream>

using namespace aims;
using namespace std;


template<class T>
static bool doit( Process &, const string &, Finder & );

class Voronoi : public Process
{
public:
  Voronoi( string filein,
           string fileout,
           int xmask,
           int ymask,
           int zmask,
           float factor,
           double val_domain,
           double val_outside ) :
    Process(),
    _filein(filein),
    _fileout(fileout),
    _xmask(xmask),
    _ymask(ymask),
    _zmask(zmask),
    _factor(factor),
    _val_domain(val_domain),
    _val_outside(val_outside)
  {
    registerProcessType( "Volume", "S8", &doit<int8_t> );
    registerProcessType( "Volume", "U8", &doit<uint8_t> );
    registerProcessType( "Volume", "S16", &doit<int16_t> );
    registerProcessType( "Volume", "U16", &doit<uint16_t> );
    registerProcessType( "Volume", "S32", &doit<int32_t> );
    registerProcessType( "Volume", "U32", &doit<uint32_t> );
  }

  template<class T>
  friend bool doit( Process &, const string &, Finder & );


private:
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
bool doit( Process & p, const string &, Finder & ) {

  Voronoi & v = (Voronoi &)p;
  AimsData<T> seed;
  Reader<AimsData<T> > dataR( v._filein );
  cout << "reading seed " << v._filein << endl;
  if( !dataR.read( seed ) )
    {
      cerr << "could not read " << v._filein << endl;
      return( 1 );
    }

  AimsData<T> label;
  /* use round() to convert _val_domain and _val_outside from double to T
     if T is an integer type, otherwise rounding may change the value.
  */
  T val_domain, val_outside;
  _round_value( v._val_domain, v._val_outside, val_domain, val_outside );
  label = AimsVoronoiFrontPropagation( seed, (T)v._val_domain, (T)v._val_outside,
                                       v._xmask, v._ymask, v._zmask, v._factor );

  Writer<AimsData<T> > dataW( v._fileout );
  dataW << label;

  return true;
}

BEGIN_USAGE(usage)
  "--------------------------------------------------------------------------",
  "AimsVoronoi -i[nput]   <filein>                                           ",
  "            -o[output] <fileout>                                          ",
  "            [-d[omain] <val_domain>                                       ",
  "            [-f[orbiden] <val_outside>                                    ",
  "            [-x[mask] <X>] [-y[mask] <Y>] [-z[mask] <Z>] [-F[actor] <F>]  ",
  "            [-h[elp]]                                                     ",
  "--------------------------------------------------------------------------",
  "Construct a Voronoi's diagram from labelled seed                          ",
  "--------------------------------------------------------------------------",
  "  filein      : input seed SHORT volume                                   ",
  "  fileout     : prefix name for output SHORT Voronoi's diagram            ",
  "  val_domain  :  value of the domain where to propagate the diagram       ",
  "                                                   [default=0]            ",
  "  val_outside : value of the outside of the domain [default=-1]           ",
  "  X           : X size of the distance mask [default=3]                   ",
  "  Y           : Y size of the distance mask [default=3]                   ",
  "  Z           : Z size of the distance mask [default=3]                   ",
  "  F           : chamfer multiplication factor [default=50]                ",
  "--------------------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}


int main( int argc, char* argv[] )
{
  char *filein = NULL, *fileout = NULL;
  int xmask = 3, ymask = 3, zmask = 3;
  float factor = 50;
  double val_domain = 0, val_outside = -1;

  //
  // Getting options
  //
  AimsOption opt[] = {
  { 'h',"help"        ,AIMS_OPT_FLAG  ,( void* )Usage    ,AIMS_OPT_CALLFUNC,0},
  { 'i',"input"       ,AIMS_OPT_STRING,&filein     ,0                ,1},
  { 'o',"output"      ,AIMS_OPT_STRING,&fileout    ,0                ,1},
  { 'd',"domain"      ,AIMS_OPT_DOUBLE,&val_domain ,0                ,0},
  { 'f',"forbiden"    ,AIMS_OPT_DOUBLE,&val_outside,0                ,0},
  { 'x',"xmask"       ,AIMS_OPT_INT   ,&xmask      ,0                ,0},
  { 'y',"ymask"       ,AIMS_OPT_INT   ,&ymask      ,0                ,0},
  { 'z',"zmask"       ,AIMS_OPT_INT   ,&zmask      ,0                ,0},
  { 'F',"Factor"      ,AIMS_OPT_FLOAT ,&factor     ,0                ,0},
  { 0  ,0             ,AIMS_OPT_END   ,0           ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  Voronoi p( string(filein),
             string(fileout),
             xmask,
             ymask,
             zmask,
             factor,
             val_domain,
             val_outside );

  if( !p.execute( string(filein) ) ) {
      return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
