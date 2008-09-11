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


#include <aims/io/io_g.h>
#include <aims/topology/topoClassifier.h>
#include <aims/getopt/getopt.h>
#include <iostream>

using namespace aims;
using namespace std;


BEGIN_USAGE(usage)
  "--------------------------------------------------------------------------",
  "AimsTopologicalClassification -i[nput]   <filein>                         ",
  "                              -o[output] <fileout>                        ",
  "                              [-h[elp]]                                   ",
  "--------------------------------------------------------------------------",
  "Topological classification of each point of a labeled image               ",
  "--------------------------------------------------------------------------",
  "  filein      : input volume                                              ",
  "  fileout     : prefix name for output file name                          ",
  "--------------------------------------------------------------------------",
END_USAGE


void Usage( void )
{
  AimsUsage( usage );
}


template< class T >
static bool doit( Process&, const string&, Finder& );


class Classifier : public Process
{
public:

  Classifier( const string& fileout );

private:

  template< class T >
  friend bool doit( Process&, const string&, Finder& );

  string fout;
};


Classifier::Classifier( const string& fileout ) : Process(), fout( fileout )
{
  registerProcessType( "Volume", "S8", &doit< int8_t > );
  registerProcessType( "Volume", "U8", &doit< uint8_t > );
  registerProcessType( "Volume", "S16", &doit< int16_t > );
  registerProcessType( "Volume", "U16", &doit< uint16_t > );
  registerProcessType( "Volume", "S32", &doit< int32_t > );
  registerProcessType( "Volume", "U32", &doit< uint32_t > );
}


template< class T >
bool doit( Process& p, const string& fname, Finder& f )
{
  Classifier& cl = (Classifier &)p;
  AimsData< T > data;
  Reader< AimsData< T > > r( fname );
  string format = f.format();

  cout << "Reading " << fname << "..." << endl;
  if ( !r.read( data, 0, &format ) )  return false;

  cout << "Classifying..." << endl;
  TopologicalClassifier< T > topoClass;
  Writer< AimsData< short > > writer( cl.fout );

  return writer.write( topoClass.doit( data ) );
}


int main( int argc, char* argv[] )
{
  char *filein = NULL, *fileout = NULL;

  //
  // Getting options
  //
  AimsOption opt[] = {
  { 'h',"help"        ,AIMS_OPT_FLAG  ,( void* )Usage,AIMS_OPT_CALLFUNC,0},
  { 'i',"input"       ,AIMS_OPT_STRING,&filein     ,0                ,1},
  { 'o',"output"      ,AIMS_OPT_STRING,&fileout    ,0                ,1},
  { 0  ,0             ,AIMS_OPT_END   ,0           ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );

  string theFilein = filein;
  string theFileout = fileout;

  Classifier proc( theFileout );
  if ( !proc.execute( theFilein ) )
    cerr << "Topological classification failed" << endl;

  return EXIT_SUCCESS;
}
