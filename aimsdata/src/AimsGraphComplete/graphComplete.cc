/* Copyright (c) 1995-2006 CEA
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

#include <aims/getopt/getopt2.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/graph/graphmanip.h>

using namespace aims;
using namespace carto;
using namespace std;

void splitver( const string & iver, vector<int> & over )
{
  string::size_type	x = 0, y, n = iver.length();
  string	s;
  int		i;
  while( x < n )
    {
      y = iver.find( '.', x );
      if( y == string::npos )
        y = n;
      s = iver.substr( x, y - x );
      istringstream	ss( s );
      ss >> i;
      over.push_back( i );
      x = y+1;
    }
}


string maxver( const string & ver1, const string & ver2 )
{
  vector<int>	iver1, iver2;
  splitver( ver1, iver1 );
  splitver( ver2, iver2 );
  string::size_type	i, n = iver1.size();
  for( i=0; i<n; ++i )
    if( iver1[i] > iver2[i] )
      return ver1;
    else if( iver1[i] < iver2[i] )
      return ver2;
  if( iver2.size() > n )
    return ver2;
  return ver1;
}


int main( int argc, const char** argv )
{
  try
    {
      AimsApplication	app( argc, argv, 
			     "Completes missing attributes in graphs " 
			     "(cortical folds graphs generally)" );
      Reader<Graph>		rg;
      Writer<Graph>		wg;
      string			mv, dv;
      bool			forceversion = false;
      app.addOption( rg, "-i", "input graph" );
      app.alias( "-input", "-i" );
      app.addOption( wg, "-o", "output (completed) graph [default=input]", 
		     true );
      app.alias( "-output", "-o" );
      app.addOption( dv, "--dversion", "set data graph version", true );
      app.addOption( mv, "--mversion", "set model graph compatibility " 
                     "version", true );
      app.addOption( forceversion, "--forceversion", "force data/model graph " 
                     "versions [default: only if newer]", true );
      app.initialize();

      Graph	*g = rg.read();
      GraphManip::completeGraph( *g );
      if( ( !dv.empty() || !mv.empty() ) && !forceversion )
        {
          string	olddv, oldmv, oldcv;
          g->getProperty( "CorticalFoldArg_VERSION", oldcv );
          g->getProperty( "datagraph_VERSION", olddv );
          g->getProperty( "datagraph_compatibility_model_VERSION", oldmv );
          if( !oldcv.empty() )
            {
              if( olddv.empty() )
                olddv = oldcv;
              if( oldmv.empty() )
                oldmv = oldcv;
            }
          if( !dv.empty() && !olddv.empty() )
            dv = maxver( dv, olddv );
          if( !mv.empty() && !oldmv.empty() )
            mv = maxver( mv, oldmv );
        }
      if( !dv.empty() )
        {
          g->setProperty( "CorticalFoldArg_VERSION", dv );
          g->setProperty( "datagraph_VERSION", dv );
        }
      if( !mv.empty() )
        g->setProperty( "datagraph_compatibility_model_VERSION", mv );

      if( wg.fileName().empty() )
        wg.setFileName( rg.fileName() );
      wg.write( *g );
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


