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
#include <aims/getopt/getopt2.h>
#include <aims/data/data_g.h>
#include <aims/roi/roi_g.h>
#include <aims/roi/roigtm.h>
#include <aims/io/roiR.h>
#include <aims/io/motionR.h>
#include <aims/resampling/resampling_g.h>
#include <aims/roi/hie.h>
#include <aims/io/hieR.h>
#include <aims/io/roiselectorR.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/datatypecode.h>
#include <aims/def/path.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <dirent.h>

using namespace aims;
using namespace carto;
using namespace std;


static void Listsel()
{
  DIR   *dir;
  dir = opendir( (Path::singleton().dependencies()+string("/Rsrc")).c_str() );

  if( !dir )
    {
      AimsError("cannot open directory");
    }

  struct dirent *dent;
  string         fname;

  cout << "List of selector available" << endl;
  cout << "             ==============================" << endl;
  cout << "             " 
       << "all"
       << endl;
  
  for( dent=readdir( dir ); dent; dent = readdir( dir ) )
    {
      fname = dent->d_name;
      string::size_type pos = fname.find( ".sel" );
      if( pos != string::npos)
	cout << "             " 
	     << fname.substr( 0, pos )
	     << endl;
    }
  cout << "             ==============================" << endl;
  closedir( dir );      
}


void selectAll (const AimsRoi& roi, RoiSelector& sel )
{
  AimsRoi::const_iterator iter( roi.begin() ), last(roi.end() ) ;
  
  string selector = "selector" ;
  sel.setProperty( "graph_syntax", selector ) ;
  
  int nonameNumber = 1 ;
  int roiNumber = 1 ;
  string name ;
  char  temp[64] ;
  while( iter != last )
    {
      if( ! (*iter)->getProperty( "name", name ) )
	AimsError( "There hould be a region name for each roi node") ;
      
      if ( name == "Unknown" )
	{
	  snprintf(temp, 10, "%d", nonameNumber) ;
	  name += string( temp ) ;
	  ++nonameNumber ;
	}
      else if ( name == "roi" )
	{
	  snprintf(temp, 10, "%d", roiNumber) ;
	  name += string( temp ) ;
	  ++roiNumber ;
	}

      Tree * node = new RoiSelector() ;
      node->setProperty( "surname", name ) ;
      Tree * subNode = new RoiSelector() ;
      subNode->setProperty( "nomenName", name ) ;
    
    
      node->insert(subNode) ;
      sel.insert( node ) ;
      (*iter)->setProperty( "name", name ) ;
      ++iter ;
    }



  return ;
}


class Helper
{
public:
  Helper() {};
  void message();
};


void Helper::message()
{
  Listsel();
  throw user_interruption();
}


namespace carto
{

  template <>
  bool SingleOption<Helper>::feed( const std::string &value )
  {
    if ( _valueRead )
    {
      return false;
    }
    else
    {
      _valueRead = true;
      return true;
    }

  }

  template <>
  bool SingleOption<Helper>::recognizeName( const std::string &n )
  {
    if( _nameInList( n ) ) {
      if ( _valueRead ) {
        throw unexpected_option( n );
      }
      _valueRead = true;
      _value.message();
      return true;
    }
    return false;
  }

}




int main(int argc, const char **argv)
{
  Reader< AimsData< float > > rd;
  Reader<Graph> rrd;
  Reader<AffineTransformation3d> mrd;
  Reader<Hierarchy > hrd;
  Writer< AimsData< float > > wt;

  string fileroiselector;
//   string fileroiseluser;
  string refname;
  float dose = 0.;
  Helper listsel;
  bool nothing = false;

  AimsApplication app( argc, argv,
                       "? - Anybody knows what this command does ?" );
  app.addOption( rd, "-i", "dynamic series: input dynamic data" );
  app.addOption( wt, "-o", "norm image" );
  app.addOption( rrd, "-v",
                 "voifile: ile containing roi information see(--voitype" );
  app.addOption( refname, "-n", "reference ROI name (from selectionlist)" );
  app.addOption( mrd, "-m", "displacement file default val : Identity", true );
  app.addOption( listsel, "-l", "list of available template selectors", true );
  app.addOption( dose, "-d",
                 "injected dose: if injected dose is specified, ROI "
                 "activity is normalized by this dose: "
                 "(Region - Reference)/Dose. Otherwise it is normalized by "
                 "the reference ROI name : (Region - Reference)/Reference",
                 true );
  app.addOption( hrd, "--hierarchy", "hierarchyfile", true );
  app.addOption( fileroiselector, "--selection", "template selector: "
                 "template selector name  available selectors may be "
                 "listed (switch -l), or all to disable selection mode",
                 true );
  app.addOption( nothing, "-x", "nothing", true );
//   app.addOption( fileroiseluser, "-s", "user defined selector file name",
//                  true );
  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "--motion", "-m" );
  app.alias( "--name", "-n" );
  app.alias( "--voi", "-v" );
//   app.alias( "--selector", "-s" );
  app.alias( "--list", "-l" );
  app.alias( "--dose", "-d" );

  try
  {
    app.initialize();

    if( (fileroiselector != "all" ) )
        {
          cerr << "only --selection all is implemented as for now\n";
          return( 1 );
        }
    // Read Hierarchy info =====================================================
    Hierarchy hie;
    try {
      if( hrd.fileName().empty() )
        hrd.setFileName( (Path::singleton()).hierarchy() + "/neuronames.hie" );
      hrd.read( hie );
    }
    catch(exception& e)
      {
        cerr << e.what() << endl;
        throw;
      }
    // Read roi info (arg mode) ==================================
    AimsRoi roi(&hie);
    RoiGtm rgtm;
    try {
      rrd.read( roi );
    }
    catch(exception& e)
      {
        cerr << e.what() << endl;
        throw;
      }

    //Read Motion info =========================================================
    Motion motion;  // set with Identity
    if (!mrd.fileName().empty() )
      {
        mrd.read( motion );
      }

    // Initialisation
  //   bool	gtm = ( string(voitype) == string("gtm") );
  //   AimsRoi	& newroi =
  //     gtm ? rgtm.getMaskRoi() : roi;
  //   RoiGtm	*newrgtm = 0;
  //   if( gtm )
  //     newrgtm = &rgtm;


    // Read RoiSelection Info =================================================
    RoiSelector roiSel;
    try {
      if( !fileroiselector.empty() )
        {
          if( fileroiselector == string("all") ||
              fileroiselector == string("all.sel") )
            {
              selectAll( roi, roiSel ) ;
            }
        }
    }
    catch(exception& e)
      {
        cerr << e.what() << endl;
        throw;
      }


    // Get a Stat Actuator Object with chosen interpolator and perf. stats
    try {
    Resampler< float > *interpolator = new LinearResampler< float >();
    RoiStats rsa( interpolator, &roiSel );


    // Lecture des données ===================================================
    AimsData< float > image;
    rd.read( image );
    rsa.populate( image, roi, motion );

    // Récupération de la valeur lue dans le pons.
    set<Vertex*>    sv = rsa.vertices();

    vector<float>   taCurv;
    string          surname;
    int x, y, z, t;
    bool found=false;

    if( dose == 0. )
      cout << " Normalization : (Voxel - Reference) / Dose " << endl ;
    else
      cout << " Normalization : (Voxel - Reference) / Reference " << endl ;

    ASSERT( sv.begin() != sv.end() );         // There at least one frame !!!
    (*sv.begin())->getProperty("mean_ac", taCurv);
    ASSERT( image.dimT() == (int) taCurv.size()); //Checking


    for (set< Vertex* >::const_iterator it=sv.begin();
        it != sv.end();
        it++)
      {
        (*it)->getProperty("mean_ac", taCurv);
        (*it)->getProperty("surname", surname);
        if( surname == refname )
          {
            cout << "Chosen ROI : " << surname << endl;
            found = true;
            for (t=0; t < image.dimT();++t)
              {
                cout << "Compute for each voxels\n"
                    << "    (vox - Roi(pons) )/ Roi(Pons)  with Roi(Pons) = " << taCurv[t]<< endl;
                if ( dose != 0. )
                  ForEach3d(image, x, y, z)
                    image(x, y, z) = ( image(x, y, z) -taCurv[t] )/ dose ;
                else
                  ForEach3d(image, x, y, z)
                    image(x, y, z) = ( image(x, y, z) -taCurv[t] )/ taCurv[t];

              }
          }
        if (! found)
          {
            cerr << "No convenient region to perform normalization" << endl;
            exit ( EXIT_FAILURE );
          }
      }

    wt.write( image );
    }
    catch(exception& e)
      {
        cerr << e.what() << endl;
        throw;
      }
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
