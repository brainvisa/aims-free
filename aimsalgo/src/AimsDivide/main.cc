/* Copyright (c) 1995-2007 CEA
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


//	Uses 3 levels of templates, hope your compiler is strong enough...

#include <cstdlib>
#include <aims/data/data_g.h>
#include <aims/io/finder.h>
#include <aims/io/process.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/getopt/getopt.h>
#include <aims/utility/utility_g.h>
#include <cstdlib>
#include <assert.h>

using namespace aims;
using namespace std;


BEGIN_USAGE(usage)
  "------------------------------------------------------------------",
  "AimsDivide  -n[um] <i1>                                           ",
  "            -d[en] <i2>                                           ",
  "            -o[utput] <fileout>                                   ",
  "           [-t[ype] <datatype>]                                   ",
  "           [-h[elp]]                                              ",
  "------------------------------------------------------------------",
  "Division I1/I2                                                    ",
  "------------------------------------------------------------------",
  "     i1      : numerator data                                     ",
  "     i2      : denominator data                                   ",
  "     fileout : destination data                                   ",
  "     datatype: output data type (S16, U8, ...). Default = i1 type ",
  "------------------------------------------------------------------",
END_USAGE


static void Usage()
{
  AimsUsage( usage );
}


class Divider : public Process
{
public:
  Divider( const string & den, const string & out, const string & t );

  string	denom;
  string	fout;
  string	type;
};


template<class T>
class DenomReader : public Process
{
public:
  DenomReader( Divider & d, T & dat );

  Divider	& div;
  T		& data;
};


template<class T, class U>
class DivProc : public Process
{
public:
  DivProc( DenomReader<T> & d, U & dat );

  DenomReader<T>	& den;
  U			& data;
};


template<class T,class U,class V>
bool divproc( Process & p, const string & fname, Finder & )
{
  DivProc<T,U>& dp = (DivProc<T,U>&) p;
  int		x,y,z,t;
  T		& in1 = dp.den.data;
  U		& in2 = dp.data;
  AimsData<V>	data( in1.dimX(), in1.dimY(), in1.dimZ(), in1.dimT() );

  data.setSizeXYZT( in1.sizeX(), in1.sizeY(), in1.sizeZ(), in1.sizeT() );
  cout << "processing..." << endl;
  ForEach4d( data , x , y , z , t)
    if( in2(x,y,z,t) != 0 )
      data(x,y,z,t) = V( double( in1(x,y,z,t) ) / double( in2(x,y,z,t) ) );
    else
      data(x,y,z,t) = V(0);
  cout << "done\n";
  Writer<AimsData<V> > writer( fname );
  cout << "writing result..." << endl;
  return writer.write( data );
}


template<class T, class U>
DivProc<T,U>::DivProc( DenomReader<T> & d, U & dat ) : Process(), den( d ), data( dat )
{
  registerProcessType( "Volume", "S8", &divproc<T,U,int8_t> );
  registerProcessType( "Volume", "U8", &divproc<T,U,uint8_t> );
  registerProcessType( "Volume", "S16", &divproc<T,U,int16_t> );
  registerProcessType( "Volume", "U16", &divproc<T,U,uint16_t> );
  registerProcessType( "Volume", "S32", &divproc<T,U,int32_t> );
  registerProcessType( "Volume", "U32", &divproc<T,U,uint32_t> );
  registerProcessType( "Volume", "FLOAT", &divproc<T,U,float> );
  registerProcessType( "Volume", "DOUBLE", &divproc<T,U,double> );
}


template<class T, class U>
bool denom( Process & p, const string & fname, Finder & f )
{
  DenomReader<T>& dp( (DenomReader<T>&)p ) ;
  U		data;
  Reader<U>	r( fname );
  string	format = f.format();
  cout << "reading denominator image..." << endl;
  if( !r.read( data, 0, &format ) )
    return( false );

  cout << "done" << endl;
  assert( dp.data.dimX() == data.dimX() &&
	  dp.data.dimY() == data.dimY() &&
	  dp.data.dimZ() == data.dimZ() &&
	  dp.data.dimT() == data.dimT() );

  DivProc<T,U>	proc( dp, data );
  Finder	f2;
  f2.setObjectType( f.objectType() );
  f2.setDataType( dp.div.type );
  return proc.execute( f2, dp.div.fout );
}


template<class T>
DenomReader<T>::DenomReader( Divider & d, T & dat ) : Process(), div( d ), data( dat )
{
  registerProcessType( "Volume", "S8", &denom<T, AimsData<int8_t> > );
  registerProcessType( "Volume", "U8", &denom<T, AimsData<uint8_t> > );
  registerProcessType( "Volume", "S16", &denom<T, AimsData<int16_t> > );
  registerProcessType( "Volume", "U16", &denom<T, AimsData<uint16_t> > );
  registerProcessType( "Volume", "S32", &denom<T, AimsData<int32_t> > );
  registerProcessType( "Volume", "U32", &denom<T, AimsData<uint32_t> > );
  registerProcessType( "Volume", "FLOAT", &denom<T, AimsData<float> > );
  registerProcessType( "Volume", "DOUBLE", &denom<T, AimsData<double> > );
}


template<class T>
bool divider( Process & p, const string & fname, Finder & f )
{
  Divider& dp( (Divider&)p );
  T		data;
  Reader<T>	r( fname );
  string	format = f.format();
  cout << "reading numerator image..." << endl;
  if( !r.read( data, 0, &format ) )
    return false ;

  cout << "done" << endl;
  if( dp.type.empty() )
    dp.type = f.dataType();

  DenomReader<T>	proc( dp, data );
  return proc.execute( dp.denom );
}


Divider::Divider( const string & den, const string & out, const string & t ) 
  : Process(), denom( den ), fout( out ), type( t )
{
  registerProcessType( "Volume", "S8", &divider<AimsData<int8_t> > );
  registerProcessType( "Volume", "U8", &divider<AimsData<uint8_t> > );
  registerProcessType( "Volume", "S16", &divider<AimsData<int16_t> > );
  registerProcessType( "Volume", "U16", &divider<AimsData<uint16_t> > );
  registerProcessType( "Volume", "S32", &divider<AimsData<int32_t> > );
  registerProcessType( "Volume", "U32", &divider<AimsData<uint32_t> > );
  registerProcessType( "Volume", "FLOAT", &divider<AimsData<float> > );
  registerProcessType( "Volume", "DOUBLE", &divider<AimsData<double> > );
}


int main( int argc, char **argv )
{
  char *i1 = NULL, *i2 = NULL, *fileout = NULL, *type = 0;

  AimsOption opt[] = {
  { 'h',"help"  ,AIMS_OPT_FLAG  ,( void* )Usage       ,AIMS_OPT_CALLFUNC,0,},
  { 'n',"num"   ,AIMS_OPT_STRING,&i1         ,0                ,1},
  { 'd',"den"   ,AIMS_OPT_STRING,&i2         ,0                ,1},
  { 'o',"output",AIMS_OPT_STRING,&fileout    ,0                ,1},
  { 't',"type"  ,AIMS_OPT_STRING,&type       ,0                ,0},
  { 0  ,0       ,AIMS_OPT_END   ,0           ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  string stype;
  if( type )
    stype = type;

  Divider proc( i2, fileout, stype );
  if( !proc.execute( i1 ) )
    {
      cerr << "Could not process.\n";
      return EXIT_FAILURE;
    }
  cout << "done\n";
  return EXIT_SUCCESS;
}
