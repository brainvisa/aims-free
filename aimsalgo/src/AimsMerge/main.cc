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


#include <cstdlib>
#include <aims/io/io_g.h>
#include <aims/data/data_g.h>
#include <aims/getopt/getopt.h>
#include <aims/utility/utility_g.h>

using namespace aims;
using namespace std;


BEGIN_USAGE(usage)
  "----------------------------------------------------------------",
  "AimsMerge -i[nput] <filein>                                     ",
  "          -M[ask] <mask>                                        ",
  "          -o[utput] <fileout>                                   ",
  "          -m[ode] <mm>                                          ",
  "          [-l[abel] <label>]                                    ",
  "          [-v[alue] <value>]                                    ",
  "          [-h[elp]]                                             ",
  "----------------------------------------------------------------",
  "Merge a data and a byte label data                              ",
  "----------------------------------------------------------------",
  "     filein    : origin file                                    ",
  "     fileout   : output file                                    ",
  "     mask      : label  file                                    ",
  "     mm        : mode that can be                               ",
  "                   sv --> same values                           ",
  "                   oo --> one to one                            ",
  "                   ao --> all to one                            ",
  "                   om --> one to maximum plus 1                 ",
  "                   am --> all to maximum plus 1                 ",
  "     label      : only label to get into account                ",
  "     value      : replacement value                             ",
  "----------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}


template<class T>
static bool doit( Process &, const string &, Finder & );


class Merger : public Process
{
public:
  Merger( const string & fout, float val, byte lab, merge_t m, AimsData<byte> & msk );

private:
  template<class T>
  friend bool doit( Process &, const string &, Finder & );

  string		fileout;
  float			value;
  byte			label;
  merge_t		mode;
  AimsData<byte>	& mask;
};


Merger::Merger( const string & fout, float val, byte lab, merge_t m, 
        AimsData<byte> & msk ) 
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
  Reader<AimsData<T> >	r( fname );
  AimsData<T>		in;

  if( !r.read( in, 0, &format ) )
    return false;

  AimsMerge<T,byte> merge( mp.mode, (char)mp.value, mp.label );
  Writer<AimsData<T> > writer( mp.fileout );
  return writer.write( merge( in, mp.mask ) );
}


int main( int argc, char **argv )
{
  char *filein = NULL, *fileout = NULL, *smask = NULL, *smode = NULL;
  float value = 0;
  byte label = 0;
  merge_t mode;

  AimsOption opt[] = {
  { 'h',"help"  ,AIMS_OPT_FLAG  ,( void* )Usage       ,AIMS_OPT_CALLFUNC,0,},
  { 'i',"input" ,AIMS_OPT_STRING,&filein     ,0                ,1},
  { 'o',"output",AIMS_OPT_STRING,&fileout    ,0                ,1},
  { 'M',"Mask"  ,AIMS_OPT_STRING,&smask      ,0                ,1},
  { 'm',"mode"  ,AIMS_OPT_STRING,&smode      ,0                ,1},
  { 'l',"label" ,AIMS_OPT_BYTE  ,&label      ,0                ,0},
  { 'v',"value" ,AIMS_OPT_FLOAT ,&value      ,0                ,0},
  { 0  ,0       ,AIMS_OPT_END   ,0           ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  if      (string(smode)=="sv" ) mode = AIMS_MERGE_SAME_VALUES;
  else if (string(smode)=="oo" ) mode = AIMS_MERGE_ONE_TO_ONE;
  else if (string(smode)=="ao" ) mode = AIMS_MERGE_ALL_TO_ONE;
  else if (string(smode)=="om" ) mode = AIMS_MERGE_ONE_TO_MAXP1;
  else if (string(smode)=="am" ) mode = AIMS_MERGE_ALL_TO_MAXP1;
  else
    AimsError("AimsMerge : bad mode" );

  AimsData<byte> mask;
  Reader<AimsData<byte> > mreader( smask );
  mreader >> mask;

  Merger	proc( fileout, value, label, mode, mask );
  if( !proc.execute( filein ) )
    {
      cerr << "couldn't process\n";
      Usage();
    }
  return EXIT_SUCCESS;
}
