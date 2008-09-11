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


#include <aims/def/def_g.h>
#include <aims/getopt/getopt.h>
#include <aims/io/io_g.h>
#include <aims/utility/utility_g.h>
#include <aims/moment/moment_g.h>
#include <stdio.h>

using namespace aims;
using namespace std;


BEGIN_USAGE(usage)
  "----------------------------------------------------------------",
  "AimsMoment -i[nput] <filein>                                    ",
  "           [-l[abel] <label>]                                   ",
  "           [-o[utput] <out>]                                    ",
  "           [-m[ode] <mode>]                                     ",
  "           [-t[ype] <type>]                                     ",
  "           [-h[elp]]                                            ",
  "----------------------------------------------------------------",
  "Compute the moment invariant                                    ",
  "----------------------------------------------------------------",
  "     filein : input data (in 8 or 16 bits) or triangulated mesh ",
  "     label  : object label in raster image  [default=all label] ",
  "     out    : ASCII result file name           [default=stdout] ",
  "     mode   : mode  f -> full  i -> invariant only  [default=f] ",
  "     type   : type  v -> volumic  s -> surfacic     [default=v] ",
  "              (only for triangulated mesh)                      ",
  "----------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}


template<class U>
static bool doit( Process &, const string &, Finder & );


class Momentor : public Process
{
public:
  Momentor( const string & fout, const string & mode, const string& mtype, 
	    int lbl );

private:
  template<class U>
  friend bool doit( Process &, const string &, Finder & );
  template<class T> bool domom( AimsData< T > & );
  bool domom( AimsTimeSurface<3,Void> & );

  string	fileout;
  string	dmode;
  string	dtype;
  int		label;
};


Momentor::Momentor( const string & fout, const string & mode, const string& mtype, 
	    int lbl )
    : Process(), fileout( fout ), dmode( mode ), dtype( mtype ), label( lbl )
{
  registerProcessType( "Volume", "S8", &doit< AimsData<int8_t> > );
  registerProcessType( "Volume", "U8", &doit< AimsData<uint8_t> > );
  registerProcessType( "Volume", "S16", &doit< AimsData<int16_t> > );
  registerProcessType( "Volume", "U16", &doit< AimsData<uint16_t> > );
  registerProcessType( "Mesh", "VOID", &doit< AimsTimeSurface<3,Void> > );
}


template<class T> bool
Momentor::domom( AimsData<T> & data )
{
  T	lablst[ 0x10000 ];
  int	nbLab = 0;
  
  if ( label != -1 )
    {
      lablst[ 0 ] = label;
      nbLab = 1;
    }
  else
    {
      int	dx = data.dimX();
      int	dy = data.dimY();
      int	dz = data.dimZ();
      int	k, x, y, z;
      T		val;
      
      for ( z=0; z< dz; z++ )
        for ( y=0; y<dy; y++ )
	  for ( x=0; x<dx; x++ )
	    {
	       val = data( x, y, z );
	       
	       if ( val )
	         {
		   k = 0;
		   while( k < nbLab && lablst[ k ] != val )  k++;
		   if ( k == nbLab )  lablst[ nbLab++ ] = val;
		 }
	    }
    }

  GeometricMoment< T > *geom = new GeometricMoment< T >;
  MomentInvariant< T > inv( geom );

  for ( int i=0; i<nbLab; i++ )
    {
      inv.doit( data, lablst[ i ] );
      
      if ( !fileout.empty() )
        {
	  char filename[ 1024 ];
	  sprintf( filename, "%s_%d.inv", fileout.c_str(), lablst[ i ] );
	  
	  ofstream file( filename );
	  
	  if ( file )
	    {
	      if ( dmode.empty() == 0 || dmode == "f" )
	        {
	          file << inv.moment() << endl;
	          file << "Moment invariant: " << endl;
	          file << inv << endl << endl;	       
	        }
	      else if ( dmode == "i" )  file << inv << endl;
	      
	      file.close();
	    }
	}
      else
        {
	  if ( dmode.empty() || dmode == "f" )
	    {
	      cout << "Results for label :  " << lablst[ i ] << endl;
	      cout << inv.moment() << endl;
	      cout << "Moment invariant: " << endl;
	      cout << inv << endl << endl;
	    }
	  else if ( dmode == "i" )
	    {
	      cout << "Results for label :  " << lablst[ i ] << endl;
	      cout << "Moment invariant: " << endl;
	      cout << inv << endl << endl;
	    }
	}
    }

  return( true );
}


bool 
Momentor::domom( AimsTimeSurface<3,Void>& data )
{
  MomentBase::MomentType mType = MomentBase::Volumic;

  if ( dtype.empty() || dtype == "v" )
    mType = MomentBase::Volumic;
  else if ( dtype == "s" )
    mType = MomentBase::Surfacic;

  TriangulationMoment tmom( mType );
  MomentInvariant<int16_t> inv( &tmom );

  inv.doit( data );

  if ( !fileout.empty() )
    {
      char filename[ 1024 ];
      sprintf( filename, "%s.inv", fileout.c_str() );
	  
      ofstream file( filename );
	  
      if ( file )
	{
	  if ( dmode.empty() == 0 || dmode == "f" )
	    {
	      file << inv.moment() << endl;
	      file << "Moment invariant: " << endl;
	      file << inv << endl << endl;	       
	    }
	  else if ( dmode == "i" )  file << inv << endl;
	      
	  file.close();
	}
    }
  else
    {
      if ( dmode.empty() || dmode == "f" )
	{
	  cout << "Results for triangulation :  " << endl;
	  cout << inv.moment() << endl;
	  cout << "Moment invariant: " << endl;
	  cout << inv << endl << endl;
	}
      else if ( dmode == "i" )
	{
	  cout << "Results for triangulation :  " << endl;
	  cout << "Moment invariant: " << endl;
	  cout << inv << endl << endl;
	}
    }
  
  return true;
}


template<class U>
bool doit( Process & p, const string & fname, Finder & f )
{
  Momentor	& mp = (Momentor &) p;
  string	format = f.format();
  Reader< U >	r( fname );
  U		data;
  cout << "reading data " << fname << "..." << endl;
  if( !r.read( data, 0, &format ) )
    return( false );
  cout << "done\n";
  return( mp.domom( data ) );
}


int main( int argc, char **argv )
{
  char *filein = 0, *fileout = 0, *dmode = 0, *dtype = 0;
  int label = -1;
  
  AimsOption opt[] = {
  { 'h', "help"  , AIMS_OPT_FLAG  , ( void* )Usage   , AIMS_OPT_CALLFUNC, 0,},
  { 'i', "input" , AIMS_OPT_STRING, &filein , 0                , 1 },
  { 'l', "label" , AIMS_OPT_INT   , &label  , 0                , 0 },
  { 'o', "output", AIMS_OPT_STRING, &fileout, 0                , 0 },
  { 'm', "mode"  , AIMS_OPT_STRING, &dmode  , 0                , 0 },
  { 't', "type"  , AIMS_OPT_STRING, &dtype  , 0                , 0 },
  { 0  , 0       , AIMS_OPT_END   , 0       , 0                , 0 }};

  AimsParseOptions( &argc, argv, opt, usage );

  string theFileout, theMode, theType;
  if ( fileout ) theFileout = fileout;
  if ( dmode ) theMode = dmode;
  if ( dtype ) theType = dtype;

  Momentor	proc( theFileout, theMode, theType, label );
  if( !proc.execute( filein ) )
    {
      cerr << "Failed.\n";
      Usage();
    }
  return( EXIT_SUCCESS );
}
