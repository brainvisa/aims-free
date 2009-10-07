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

#include <aims/getopt/getopt2.h>
#include <aims/io/motionR.h>
#include <aims/io/motionW.h>
#include <aims/resampling/motion.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char** argv )
{
  try
    {
      list< string > list_r1;
      string	r2="";
      string	w;
      AimsApplication	app( argc, argv, "Composes two Aims/anatomist "
			     "transformations: M1 x M2 "
			     "[Possibility to use a list of motions with -i option]");
      app.addOptionSeries( list_r1, "-i", "M1: 1st input transformation" );
      app.addOption( r2, "-j", "M2: 2nd input transformation [option]", true );
      app.addOption( w, "-o", "output transformation "
		     "(default: same as 1st input)", true );
      app.initialize();

      list<string>::const_iterator it = list_r1.begin();

      Motion	m, n;
      MotionReader mr1( it->c_str() );	
      mr1 >> m;	   //  First motion of the list
      
      for(++it; it != list_r1.end(); ++it )
	{
	  MotionReader mrdd( it->c_str() );	
	  mrdd >> n;
	  m = m * n;
	}
      
      if( !r2.empty() )
	{
	  MotionReader mr2( r2 ); 	
	  mr2 >> n;
	  m = m * n;	  
	}

      if( w.empty() )
	w = *list_r1.begin();

      MotionWriter	mw( w );
      mw.write( m );

    }
  catch( user_interruption & )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
    }
}

