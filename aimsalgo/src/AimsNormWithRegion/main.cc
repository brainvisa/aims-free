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


#include <aims/getopt/getopt.h>
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


BEGIN_USAGE(usage)
  "----------------------------------------------------------------",
  "AimsNormWithRegion                                              ",
  "            -i[nput series]   < dynamic series >                ",
  "            -v[oi]      < voifile >                             ",
  "            -m[otion]    < motionfile (default is identity) >    ",
  "            -n[ame]     < reference ROI name (from selectionlist)>",
  "            -o[utput]   < norm image >                          ",
  "            -s[elector] < user defined selector file name >     ",
  "            -l[ist]     < list of available template selectors> ",
  "            -d[ose]     < injected dose > ",
  "            --hierarchy < hierarchyfile >                       ",
  "            --selection < template selector>                    ",
  "           [-h[elp]]                                            ",
  "----------------------------------------------------------------",
  "     dyn series   : input dynamic data                           ",
  "     motionfile   : displacement file default val : Identity     ",
  "     voifile      : file containing roi information see(--voitype)",
  "     injected dose: if injected dose is specified, ROI          ",
  "                    activity is normalized by this dose :       ",
  "                    (Region - Reference)/Dose.                  ",
  "                    Otherwise it is normalized by the reference ",
  "                    ROI name : (Region - Reference)/Reference   ",
  "                                                                ",
  "       --selection:   template selector name  available         ",
  "                 :        selectors may be listed (switch -l)   ",
  "                 :    OR                                        ",
  "                 :    all to disable selection mode             ",
  "                                                                ",
  "----------------------------------------------------------------",
END_USAGE



void Usage( void )
{
  AimsUsage( usage );
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


int main(int argc,char **argv)
{
  char *fileseries=NULL, *filevoi=NULL; // Mandatory file name input

  char  *filemotion=NULL, *filehierarchy=NULL, 
        *fileoutput=NULL, *fileroiselector=NULL, *fileroiseluser=NULL;
  char *refname=NULL;
  float dose = 0. ;

  AimsOption opt[] = {
  { 'h',"help"     ,AIMS_OPT_FLAG  ,( void* )Usage       ,AIMS_OPT_CALLFUNC,0},
  { 'i',"input series",AIMS_OPT_STRING,&fileseries ,0             ,1},
  { 'v',"voi"      ,AIMS_OPT_STRING,&filevoi    ,0                ,1},
  { 'm',"motion"    ,AIMS_OPT_STRING,&filemotion ,0                ,0},
  { 'n',"name"     ,AIMS_OPT_STRING,&refname       ,0                ,1},
  { 's',"selector" ,AIMS_OPT_STRING,&fileroiseluser,0            ,0},
  { 'o',"output"   ,AIMS_OPT_STRING,&fileoutput ,0                ,1},
  { 'l',"list"     ,AIMS_OPT_FLAG   ,( void* )Listsel    ,AIMS_OPT_CALLFUNC,0},
  { 'd',"injected dose"     ,AIMS_OPT_STRING, &dose, 0    ,0},
  { ' ',"hierarchy",AIMS_OPT_STRING,&filehierarchy,0              ,0},
  { ' ',"selection",AIMS_OPT_STRING,&fileroiselector,0            ,0},
  { 0  ,0          ,AIMS_OPT_END   ,0           ,0                ,0}};

  AimsParseOptions(&argc,argv,opt,usage);
  if( (string(fileroiselector) != string("all") ) )
      {
	cerr << "only --selection all is implemented as for now\n";
	return( 1 );
      }
  // Read Hierarchy info =====================================================
  Hierarchy hie;
  try {
  if (filehierarchy)
    {
    HierarchyReader hrd( filehierarchy, hie.syntax());
    hrd >> hie;
    }
  else
    {
      HierarchyReader hrd( (Path::singleton()).hierarchy()+
			  "/neuronames.hie", hie.syntax());
      hrd >> hie;
    }
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
    Reader<Graph> rrd( filevoi );
    rrd.read( roi );
  }
  catch(exception& e)
    {
      cerr << e.what() << endl;
      throw;
    }

  //Read Motion info =========================================================
  Motion motion;  // set with Identity
  if (filemotion)
    {
      MotionReader mrd( filemotion );
      mrd >> motion;
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
    if (fileroiselector)
      {
	if( string(fileroiselector) == string("all") || 
	    string(fileroiselector) == string("all.sel") )
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
  Reader< AimsData< float > > rd( fileseries );
    rd >> image;
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
      if (surname == string(refname))
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
  
  Writer< AimsData< float > > wt( fileoutput );
  wt << image;
   }
  catch(exception& e)
    {
      cerr << e.what() << endl;
      throw;
    }
  return EXIT_SUCCESS;
}
