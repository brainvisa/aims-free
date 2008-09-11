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

         
#include <aims/io/ecatR.h>
#include <aims/getopt/getopt2.h>
#include <aims/data/data_g.h>    
#include <aims/io/io_g.h>	 
#include <aims/connectivity/component.h>
#include <aims/utility/utility_g.h>
#include <aims/pyramid/pyramid_g.h>
#include <aims/morphology/morphology_g.h>
#include <aims/morphology/morphology_g.h>
#include <cartobase/type/string_conversion.h>
#include <cartobase/stream/directory.h>
#include <string>
#include <unistd.h>
#include <sys/stat.h>    
#include <time.h>
#include <assert.h> 

using namespace aims;
using namespace std;
using namespace carto ;

#define MINSIZE 500
#define THRESHOLD_INIT 20000
#define THRESHOLD_RATIO 10

   

int main(int argc, const char **argv)
{
  int result = EXIT_SUCCESS;
 
  try {
    enum FileSplit {
      ONEFILE = 1,
      MANYFILES = 2
    } ;

    FileSplit inputSplitFormat ;
    const float sizeLimit = 700000. ;
    unsigned NBRAT = 0, COEF_Y_PENALTY = 3;
    bool verbose = false;
    bool attToSplit = false;
    Connectivity::Type connectivity = Connectivity::CONNECTIVITY_26_XYZ;
    int           minSize = MINSIZE , tmp;
    list <string> listName;
    vector<string> outputFileNames ;
    list <string>::const_iterator it ;
    string        attFileName = "";
    time_t        time_origin;
    float         isotope_hl;
    vector<int>   ref_dims;
    vector<float> ref_voxsize;
    vector<float> gcA(3, -1.), gcB(3, -1.), gcC(3, -1.), gcD(3, -1.) ;
    
    string        dirn = "";
    bool          do_unlink = false;
    bool          binary = false ;
    short test_comp_connex, cpt;
    float t1 = THRESHOLD_INIT;
    float ratio = THRESHOLD_RATIO;
    float pas_seuil, init_t1, radius;
    int   emiX, emiY, emiZ;

    AimsApplication application( argc, argv, 
				 "Premier program  rats" );
    application.addOptionSeries( listName, "-i", "input files (1 minimum)", 1);
    application.addOptionSeries( outputFileNames, "-o",  "output file names (give as many as you want of rats if the automatic names do not suit you)", 
				 0, 4 );
    application.addOption( NBRAT, "-r",  "Nbre de rat [default: number of output file names, of by default : 4 rats] ", true );
    application.addOption( minSize, "-m",  "Taille minimale des composantes connexes [default: 1000] ", true );
    application.addOption( t1, "-t",  "Seuil initial de recherche [default: 20000] ", true );
    application.addOption( ratio, "-p",  "Pas de d�r�ent du seuil [default: 10] ", true );
    application.addOption( binary, "--binary",  "Image used to split is almost binary", true );
    application.addOption( dirn, "-d",  "dirname", true );
    application.addOption( do_unlink, "-u",  "perform unlink", true );
    application.addOption( attFileName, "-a",  "attenuation file name", true );
    application.addOption( attToSplit, "--a2split",  "use attenuation file to split", true );
    application.addOptionSeries( gcA, "-gcA", "Force first gravity center(optional) : must set others too", 3, 3) ;
    application.addOptionSeries( gcB, "-gcB", "Force second gravity center(optional) : must set others too", 3, 3) ;
    application.addOptionSeries( gcC, "-gcC", "Force third gravity center(optional) : must set others too", 3, 3) ;
    application.addOptionSeries( gcD, "-gcD", "Force fourth gravity center(optional) : must set others too", 3, 3) ;
    application.addOption( verbose, "-verbose", "yields some more info", true ); 
    application.initialize(); 
	
    if( outputFileNames.size() != 0 && outputFileNames.size() != NBRAT && NBRAT != 0 )
      throw runtime_error("The number of output file names is not coherent with the number of rats") ;

    if( NBRAT == 0 )
    {
      if( outputFileNames.size() == 0 )
        NBRAT = 4 ;
      else
        NBRAT = outputFileNames.size() ;
    }

    vector<Point3df> fgc(4) ;
    bool forceGravityCenters = false ;
    if( gcA[0] != -1. ){
    	for(int i = 0 ; i < 3 ; ++i )
	{
    		fgc[0][i] = gcA[i] ;
   		fgc[1][i] = gcB[i] ;
   		fgc[2][i] = gcC[i] ;
   		fgc[3][i] = gcD[i] ;
    	}
    	for(unsigned rat = 0 ; rat < NBRAT ; ++rat )
    		for(int i = 0 ; i < 3 ; ++i )
    			if( fgc[rat][i] == -1. ){
   			cerr << "If you force one gravity center, you have to force all of them" << endl ;
			return(EXIT_FAILURE) ;    
		}
	forceGravityCenters = true ;	
    }
    
    Finder f;
    f.check( *( listName.begin() ) );
    const Header *head = f.header();
    const PythonHeader *ph = dynamic_cast<const PythonHeader *>( head );
    vector<int>     dims;
    if( ph ){
      ph->getProperty( "volume_dimension", dims);      
      
      int tSize ;
      if ( (dims.size() >= 4) && (dims[3] != 1) ){
	inputSplitFormat = ONEFILE ;
	tSize = dims[3] ;
      }
      else{
	inputSplitFormat = MANYFILES ;
	tSize = listName.size() ;
      }
      
      bool frameByFrame = false ;
      
      
      if( 2. * (dims[0] * dims[1] * dims[2] * tSize) * 4. > sizeLimit )
	frameByFrame = true ;
      
      //cout << "tSize = " << tSize << "\tFile size on memory " << 2 * dims[0] * dims[1] * dims[2] * tSize * 4. << endl ;
      
      if( frameByFrame && ( f.format() != "ECAT" ) ){
	cout << "This mode : frame by frame from a dynamic series file can only be ran on ecat files" << endl
	     << "Data are too large to fit in memory. Converte them to Ecat format" 
	     << endl ;
	return (EXIT_FAILURE) ;
      }
	
      //cout  << "Frame by Frame "  << frameByFrame << "\tInput Split Format " << inputSplitFormat 
      //      << "\t Time Size = " << tSize << endl ;
      

      if ( NBRAT == 1 ){
// 	int imageNumber = 0 ;
// 	if( imageNumber == 0 )
	  ref_dims = dims ;
	vector<float>	voxsize;
	ph->getProperty( "voxel_size", voxsize );
	
	time_t                   time_curr;
	AimsData<float> in ;
	AimsData<float> out( dims[0], dims[1], dims[2], tSize ) ;
	out.setSizeXYZT( voxsize[0], voxsize[1], voxsize[2], 1.0) ;
	
	int tMult = 0 ;
	if( !frameByFrame && inputSplitFormat == ONEFILE ){
	  Reader < AimsData <float> > reader( *(listName.begin() ) ) ;
	  reader.read( in ) ;
	  tMult = 1 ;
	}
	vector<int> dimOut( 4 ) ;
	dimOut[0] = dims[0] ;
	dimOut[1] = dims[1] ;
	dimOut[2] = dims[2] ;
	dimOut[3] = tSize ;
	
	PythonHeader * outHeader = dynamic_cast<PythonHeader *>(ph->cloneHeader() ) ;
	if( ! outHeader )
	   cerr << "Bad output file header" << endl ;
	else
	  {
	    outHeader->setProperty( "volume_dimension", dimOut ) ;
	    out.setHeader( outHeader->cloneHeader() ) ;
	  }
	
	it = listName.begin() ;
	vector<int> st, dt;
	time_t zst ;
	for( int t = 0 ; t < tSize ; ++t, ++it )
	  {
	    if( inputSplitFormat == ONEFILE  ){
	      if( frameByFrame ){
		EcatReader<float> dataR( *(listName.begin()) );
		dataR.frameRead(in, in.allocator(), t) ;
	      }
	    }else {
	      Reader < AimsData <float> > reader( *it ) ;
	      reader >> in ;
	    }
	    
	    const PythonHeader *pheader = dynamic_cast<const PythonHeader *>( in.header() );
	    
	    if ( !pheader->hasProperty("zero_start_time") )
	      {
		cerr << "Image " << *listName.begin() 
			 << " has no zero_start_time info." 
		     << endl;
		return ( EXIT_FAILURE );
	      }
	    else
	      {
		int tmp;
		if( !pheader->getProperty( "zero_start_time", tmp ) )
		  {
		    cerr << "Image " << *listName.begin()  
			 << " has no zero_start_time info." 
			     << endl;
		    return ( EXIT_FAILURE );
		  }
		time_curr = zst = (time_t) tmp;
	      }
	    if( t == 0 )
	      {
		
		if ( !pheader->hasProperty("zero_start_time") )
		  {
		    cerr << "Image " << *listName.begin()  
			 << " has no zero_start_time info." 
			 << endl;
		    return ( EXIT_FAILURE );
		  }
		else
		  {
		    int tmp;
		    if( !pheader->getProperty( "zero_start_time", tmp ) )
		      {
			cerr << "Image " << *listName.begin()  
			     << " has no zero_start_time info." 
			     << endl;
			return ( EXIT_FAILURE );
		      }
		    time_origin = (time_t) tmp;
		  }
	      }
	    if ( !pheader->hasProperty("isotope_halflife") )
	      {
		cerr << "Image " << *listName.begin()  
		     << " has no isotope_halflife info." 
		     << endl;
		return ( EXIT_FAILURE );
	      }
	    else
	      {
		if( !pheader->getProperty( "isotope_halflife", isotope_hl ) )
		  {
		    cerr << "Image " << *listName.begin()  
			 << " has no isotope_halflife info." 
			 << endl;
		    return ( EXIT_FAILURE );
		  }
	      }
	    
	    if ( !pheader->hasProperty("start_time") )
	      {
		cerr << "Image " << *listName.begin() 
		     << " has no start_time info." 
		     << endl;
		return ( EXIT_FAILURE );
	      }
	    else
	      {
		vector<int> tmp;
		if( !pheader->getProperty( "start_time", tmp ) )
		  {
		    cerr << "Image " << *listName.begin()  
			 << " has no start_time info." 
			 << endl;
		    return ( EXIT_FAILURE );
		  }
		st.push_back( tmp[t * tMult] + (zst - time_origin)* 1000 ) ;
	      }
	    
	    if ( !pheader->hasProperty("duration_time") )
	      {
		cerr << "Image " << *listName.begin() 
		     << " has no duration_time info." 
		     << endl;
		return ( EXIT_FAILURE );
	      }
	    else
	      {
		vector<int> tmp;
		if( !pheader->getProperty( "duration_time", tmp ) )
		  {
		    cerr << "Image " << *listName.begin()  
			 << " has no duration_time info." 
			 << endl;
		    return ( EXIT_FAILURE );
		  }
		dt.push_back( tmp[t * tMult] ) ;
	      }
	    
	    float weight = 
	      (float)pow(2, difftime(time_curr, time_origin)/(double)isotope_hl);
	    
	    
	    if ( do_unlink )
	      try 
		{
		  unlink( (*(listName.begin())).c_str() ); 
		}
	      catch ( std::exception &e )
		{
		  cerr << argv[ 0 ] << ": " << e.what() << endl;
		  result = EXIT_FAILURE;
		}
	    
	    for (int z = 0 ; z < in.dimZ() ; ++z )
	      for (int y = 0 ; y < in.dimY() ; ++y )
		for (int x = 0 ; x < in.dimX() ; ++x )
		  out(x, y, z, t) =  in( x, y, z, t * tMult ) * weight;
	    
	  }
	
// 	for( int i = 0 ; i < st.size() ; ++i ){
// 	  cout << "Start Time  = " << st[i] 
// 	       << "\tDuration Time  = " << dt[i] << endl ;
// 	}
	if( inputSplitFormat == MANYFILES || frameByFrame){
	  outHeader->setProperty("start_time", st);
	  outHeader->setProperty("duration_time", dt);		  
	  out.setHeader ( outHeader->cloneHeader() );
	}
	//   Ecriture des imagettes
	string fileName = "";
	if ( dirn != "" )  fileName += dirn + "/";
	char tmp_num[4]; sprintf(tmp_num, "_%c", int('A'));
	string fn =   ph->removeExtension( *(listName.begin()) );
	uint p;
	if ((p=fn.rfind("/")) > fn.size() ) p = 0;
	fn = fn.substr( p, fn.size() );
	fileName += fn + string(tmp_num) + ".v";
	if( outputFileNames.size() == 0 ){
	  Writer< AimsData<float> > dataW( fileName );
	  dataW.write( out ); 
	} else {
	  Writer< AimsData<float> > dataW( outputFileNames[0] );
	  dataW.write( out ); 
	}
	return (EXIT_SUCCESS) ;
      }
      
      //   Instanciation des images
      AimsData< short > imageSeuil;    
      AimsData<short > erode;
      
      //   Processus pyramidal 
      int tMult ;
      AimsData<float> in;
      AimsData<float> att ;
      if( attToSplit ){
	Reader < AimsData <float> > reader( attFileName.c_str() ) ;
        reader.read(att) ;
	//cout << "Attenuation lue ! "<< endl ;
      }
      if( !frameByFrame && inputSplitFormat == ONEFILE ){
        //cout << "Reading emission 1" << endl ;
	Reader < AimsData <float> > reader( *(listName.begin() ) ) ;
	reader.read( in ) ;
	tMult = 1 ;
	tmp = dims[3] / 2 ;
      } else {
	tMult = 0 ;
	if( inputSplitFormat == ONEFILE ){
          //cout << "Reading emission 2" << endl ;
	  tmp = dims[3] / 2 ;
          //cout << "Emission frame to read " << tmp << endl ;
	  
	  EcatReader<float> dataR( *(listName.begin()) );
	  dataR.frameRead( in, in.allocator(), tmp ) ;
	} else {
	  tmp = listName.size() / 2 ;
	  it = listName.begin() ;
	  for ( int i = 0; i < (tmp-1); ++i) ++it;
	  Reader<AimsData<float> > dataR( *it );
	  dataR >> in;
	}
      }
 
      emiX = in.dimX();
      emiY = in.dimY();
      emiZ = in.dimZ();
      
      // Lecture & affichage des dimensions et taille de voxel de l'image
      // (lecture d'infos du header d'image)
      const PythonHeader 
	*ph = dynamic_cast<const PythonHeader *>( in.header() );
      if( ph )      {
	// la liste des attributs est donn� dans <aims/data/pheader.h>
        ref_dims = ph->getProperty( "volume_dimension" )
          ->value<vector<int> >();
	    // variante �uivalente:
	    ph->getProperty( "voxel_size", ref_voxsize );
	    
	    
	    if ( !ph->hasProperty("zero_start_time") )
	      {
		cerr << "Image " << *listName.begin()  
		     << " has no zero_start_time info." 
		     << endl;
		return ( EXIT_FAILURE );
	      }
	    else
	      {
		int tmp;
		if( !ph->getProperty( "zero_start_time", tmp ) )
		  {
		    cerr << "Image " << *listName.begin()  
			 << " has no zero_start_time info." 
			 << endl;
		    return ( EXIT_FAILURE );
		  }
		time_origin = (time_t) tmp;
	      }
	    if ( !ph->hasProperty("isotope_halflife") )
	      {
		cerr << "Image " << *listName.begin()  
		     << " has no isotope_halflife info." 
		     << endl;
		return ( EXIT_FAILURE );
	      }
	    else
	      {
		if( !ph->getProperty( "isotope_halflife", isotope_hl ) )
		  {
		    cerr << "Image " << *listName.begin()  
			 << " has no isotope_halflife info." 
			 << endl;
		    return ( EXIT_FAILURE );
		  }
	      }
	  } 
	  
      //     t1 = in.maximum() / 300 ;
      // 	  t1 = 20000;
          init_t1 = t1;
	  radius = 0.0;
	  cpt = 1 ;
	  
	  vector<Point3df> gc( NBRAT );
	  double coef = 1;
	  //coef = pow( (double) 2,  (double) LEVEL);
	  int coefficient = (int)coef;
	  
	  //cout << "LEVEL = " << LEVEL << "   & coef = " << coef << endl ; 
	  
	  // -- added --
	  if( attToSplit ){
	    t1 = 0.11 ;
	  }
	  // -- added --
	  
	  //cout << "Splitting ! " << endl ;
	  test_comp_connex = 0 ;
	  //cout << (binary ? "Binary !" : "Not Binary !") << endl << endl ;
	  if(!forceGravityCenters)
	    {
	      do
		{ 
		  assert(THRESHOLD_RATIO != 0); 
		  pas_seuil = t1 / THRESHOLD_RATIO ;
		  t1 -= pas_seuil ;
		  cout << endl << "---> Iteration n " << cpt << "\tseuil = " << t1 << "\tpas = " 
		       << pas_seuil << endl << endl;
		  
		  if( (attToSplit && t1 < 0.001) || (!attToSplit &&  t1 < 1000.) || (binary && test_comp_connex ) )
		    {
		      t1 = attToSplit ? 0.11 : init_t1 ;
		      radius += 5. ;
		      cout << endl << "RADIUS INCREMENTED = " << radius << endl << endl ;
		      
		      if( radius > 30.)
			{
			  cout << endl << endl << " *****   PROBLEME, IMPOSSIBLE DE DETECTER LES 4 RATS !!!   ***** " 
			       << endl << endl ;
			  exit(0) ;
			}
		    }
		  
		  /*cout << "in dims = " << in.dimX() 
		  	<< " , " << in.dimX()
			<< " , " << in.dimZ() << endl ;*/
		  AimsData<float> inSmall( ( attToSplit ? att.dimX() : in.dimX() ),
		  			   ( attToSplit ? att.dimY() : in.dimY() ),
					   ( attToSplit ? att.dimZ() : in.dimZ() ), 1 ) ;
		  inSmall.setSizeXYZT( (attToSplit ? att.sizeX() : in.sizeX() ),
		  		       (attToSplit ? att.sizeY() : in.sizeY() ),
		  		       (attToSplit ? att.sizeZ() : in.sizeZ() ) );
		  
		  int x, y, z;
		  ForEach3d( inSmall, x, y, z ){
		    if( attToSplit )
		      inSmall(x, y, z) = att(x, y, z ) ;
		    else
		      inSmall(x, y, z) = in(x, y, z, tmp*tMult ) ;
		  }
		  //   Instanciation d'un seuilleur de FLOAT rendant un SHORT
		  AimsThreshold< float, short > thresholder(AIMS_GREATER_THAN, t1, 0);
		  imageSeuil = thresholder.bin( inSmall ); 
		  
		  //   Pyramide niveau LEVEL
/*		  pyramidfunc = new MaxPyramidFunc<short>();
		  Pseuil = new Pyramid<short>(*pyramidfunc);
		  Pseuil->setRef( imageSeuil );
		  Pseuil->setLevel( LEVEL );
		  
		  AimsData<short> data;
		  data = Pseuil->item(LEVEL).clone();		  
		  
		  delete Pseuil ;
		  delete pyramidfunc ;
		  
		  //  Radius modified (init = 3.0)
		  AimsData<short> data1(data.dimX(), data.dimY(), data.dimZ(), 1,1);
		  data1.setSizeXYZT(data.sizeX(), data.sizeY(), data.sizeZ());
*/		  
		  AimsData<short> data1( ( attToSplit ? att.dimX() : in.dimX() ),
		  			 ( attToSplit ? att.dimY() : in.dimY() ),
					 ( attToSplit ? att.dimZ() : in.dimZ() ), 1 ,1 );
		  data1.setSizeXYZT( (attToSplit ? att.sizeX() : in.sizeX() ),
		  		     (attToSplit ? att.sizeY() : in.sizeY() ),
		  		     (attToSplit ? att.sizeZ() : in.sizeZ() ) );
				     
		  int i, j, k ;
		  ForEach3d(data1, i, j, k)
		    data1(i, j, k) = /*data*/imageSeuil(i, j, k );
		  if( radius > 1. )
		  	erode = AimsMorphoChamferErosion( data1, radius, 3,3,3,50 );
		  else
		  	erode = data1 ;
// 		  Writer<AimsData< short > > erodeWri( "/volatile/temp/Erode_" + toString(radius) 
// 						       + "_" + toString(t1)  ) ;
// 		  erodeWri.write( erode ) ;

// 		  Writer<AimsData<short> > wriErode( string("erode_") + toString(radius) + string("_") + toString(cpt) + string(".ima") ) ;
// 		  wriErode.write(erode) ;

		  //   Instanciation de l'image labellis
		  AimsConnectedComponent( erode, connectivity, short(0), true, 
                                          minSize /*/ (LEVEL*LEVEL*LEVEL)*/ ) ;


		  test_comp_connex = erode.maximum() ;
		  ++cpt ;
		}
	      while( (unsigned) test_comp_connex != NBRAT );
	      
	      
	      //   Recherche des dimensions maximales
	      vector<float> nbvoxels( NBRAT);
	      
	      //   Recherche du CG sur image seuil degradee
	      for(unsigned l = 0 ; l < NBRAT ; ++l )
		{
		  nbvoxels[l] = 0.;
		  
		  for( int z = 0; z < erode.dimZ(); ++z )
		    for( int y = 0; y < erode.dimY(); ++y )
		      for( int x = 0; x < erode.dimX(); ++x )
			{
			  if( erode( x, y, z ) == (short)( l+1 ) ) //   Verif de l intensite
			    {		    
			      gc[l][0] += (float) x ;   //   Somme pour le calcul du CG 
			      gc[l][1] += (float) y ;
			      gc[l][2] += (float) z ;		    
			      nbvoxels[l] += 1. ;   //   Calcul du nb de voxels / rats		    
			    }
			}
		}
	      	      
	      for( unsigned l = 0 ; l < NBRAT ; ++l )
		for( int k = 0 ; k < 3 ; ++k )
		  {
		    gc[l][k] /= (float) nbvoxels[l] ;    //   Calcul du centre de gravite
		    gc[l][k] *= (float) coef;
		  }
	      
	      //   Permutation des images  avec CG
	      AimsData<float> ordre( NBRAT );
	      AimsData<float> tmp_ordre( NBRAT );
	      vector<Point3df> tmp_gc( NBRAT );
	      tmp_gc = gc ;
	      
	      for( unsigned l = 0 ; l < NBRAT ; ++l )
		{ // mirror effect corrected
		  // 	if( gc[l][1] < in.dimY() / 2 ) ordre(l) = gc[l][0] + gc[l][1] ;
		  // 	else ordre(l) = COEF_Y_PENALTY * gc[l][0] + COEF_Y_PENALTY * gc[l][1] ;
		  if( gc[l][1] < in.dimY() / 2 ) ordre(l) = (in.dimX() - gc[l][0]) + gc[l][1] ;
		  else ordre(l) = COEF_Y_PENALTY * (in.dimX()- gc[l][0]) + COEF_Y_PENALTY * gc[l][1] ;
		}
	      tmp_ordre = ordre.clone() ;
	      
	      // Tri des valeurs par incSorting (data.h)
	      incSorting( ordre );
	      
	      for( unsigned l = 0 ; l < NBRAT ; ++l )
		{
		  if( ordre(l) != tmp_ordre(l) ) 
		    {
		      for(unsigned m =  0 ; m < NBRAT ; ++m )
			{
			  if( ( l != m ) && (ordre(l) == tmp_ordre(m)) )
			    {
			      for(int n = 0; n < 3 ; ++n ) gc[l][n] = tmp_gc[m][n];
			    }
			}
		    }
		}
	      
	      cout << "Rat A : centre de gravit�" << gc[0] << endl ;
	      cout << "Rat B : centre de gravit�" << gc[1] << endl ;
	      cout << "Rat C : centre de gravit�" << gc[2] << endl ;
	      cout << "Rat D : centre de gravit�" << gc[3] << endl ;
	    }
	  else {
	    cout << endl << "Using input gravity centers" << endl ;
	    gc = fgc ;
	  }
	  //   Extraction des rats 
	  // Cr�tion du directory de travail if any specified
	  try
	    {
	      if ( dirn != "" )
	      { cout << "make dir " << dirn << endl;
	        Directory dir( dirn );
		dir.mkdir();
		//mkdir(dirn.c_str(), 07777);
	      }
	    }
	  catch ( std::exception &e )
	    {
	      cerr << argv[ 0 ] << ": " << e.what() << endl;
	      result = EXIT_FAILURE;
	    }

	  
	  //   Extraction des rats dans chaque image
	  for ( unsigned l = 0 ; l < NBRAT ; ++l )
	    {
	      cout << "rat no " << l << endl;
	      int minx, miny, minz, maxx, maxy, maxz;
	      time_t                   time_curr;
	      
	      Finder geo;
	      geo.check(*(listName.begin()));
	      const Header *h = geo.header();
	      const PythonHeader *ph = dynamic_cast<const PythonHeader *>( h );
	      if( ph )
		{		      
		  vector<float>	voxsize;
		  ph->getProperty( "voxel_size", voxsize );
		  AimsData <float> rat( in.dimX() / 2,
					in.dimY() / 2,
					in.dimZ(),
					tSize) ;
		  
		  rat.setSizeXYZT( in.sizeX(),
				   in.sizeY(),
				   in.sizeZ(),1.0 ) ;
		  
		  // 	    Header *npheader = NULL;
		  // 	    npheader = ( pheader->cloneHeader() );
		  PythonHeader *npheader =  NULL;
		  npheader = dynamic_cast< PythonHeader *>( ph->cloneHeader() );
		  vector<int> vd;
		  npheader->getProperty("volume_dimension", vd);
		  vd[0]/=2; vd[1] /=2;
		  npheader->setProperty("volume_dimension", vd);
		  rat.setHeader ( npheader->cloneHeader() );
		  
		  minx = max( int(gc[l][0]-in.dimX()/4), 0) ;
		  miny = max( int(gc[l][1]-in.dimY()/4), 0) ;
		  minz = 0 ;
		  
		  maxx = min(minx+in.dimX()/2,in.dimX());
		  maxy = min(miny+in.dimY()/2,in.dimY());
		  maxz = in.dimZ() ;	    
		  it = listName.begin() ;
		  
		  vector<int> st, dt;
		  time_t zst ;
		  
		  //cout << "tSize: " << tSize << endl;
		  for ( int t = 0 ; t < tSize ; ++t )
		    {
		      cout << "time: " << t << endl;
		      int tMult = 1 ;
		      if( inputSplitFormat == ONEFILE  ){
			if( frameByFrame ){
			  cout << "read frame\n";
			  EcatReader<float> dataR( *(listName.begin()) );
			  dataR.frameRead(in, in.allocator(), t) ;
			  tMult = 0 ;
			  cout << "read OK\n";
			}
		      }else {
			Reader < AimsData <float> > reader( *it ) ;
			reader >> in ;
			tMult = 0 ;
		      }
		      
		      const PythonHeader *pheader = dynamic_cast<const PythonHeader *>( in.header() );
		      
		      if( t == 0 )
			{
			  if ( !pheader->hasProperty("zero_start_time") )
			    {
			      cerr << "Image " << *listName.begin()  
				   << " has no zero_start_time info." 
				   << endl;
			      return ( EXIT_FAILURE );
			    }
			  else
			    {
			      int tmp;
			      if( !pheader->getProperty( "zero_start_time", tmp ) )
				{
				  cerr << "Image " << *listName.begin()  
				       << " has no zero_start_time info." 
				       << endl;
				  return ( EXIT_FAILURE );
				}
			      time_origin = (time_t) tmp;
			    }
			}
		      
		      // Case with a single file containing all dynamic seres
		      if ( !pheader->hasProperty("zero_start_time") )
			{
			  cerr << "Image " << *listName.begin() 
			       << " has no zero_start_time info." 
				   << endl;
			  return ( EXIT_FAILURE );
			}
		      else
			{
			  int tmp;
			  if( !pheader->getProperty( "zero_start_time", tmp ) )
			    {
			      cerr << "Image " << *listName.begin()  
				   << " has no zero_start_time info." 
				   << endl;
			      return ( EXIT_FAILURE );
			    }
			  time_curr = zst = (time_t) tmp ;
			}
		      
		      if ( !pheader->hasProperty("start_time") )
			{
			  cerr << "Image " << *listName.begin() 
			       << " has no start_time info." 
			       << endl;
			  return ( EXIT_FAILURE );
			}
		      else
			{
			  vector<int> tmp;
			  if( !pheader->getProperty( "start_time", tmp ) )
			    {
			      cerr << "Image " << *listName.begin()  
				   << " has no start_time info." 
				   << endl;
			      return ( EXIT_FAILURE );
			    }
			  st.push_back( tmp[t * tMult] + (zst - time_origin)* 1000 ) ;
			}
		      
		      if ( !pheader->hasProperty("duration_time") )
			{
			  cerr << "Image " << *listName.begin() 
			       << " has no duration_time info." 
			       << endl;
			  return ( EXIT_FAILURE );
			}
		      else
			{
			  vector<int> tmp;
			  if( !pheader->getProperty( "duration_time", tmp ) )
			    {
			      cerr << "Image " << *listName.begin()  
				   << " has no duration_time info." 
				   << endl;
			      return ( EXIT_FAILURE );
			    }
			  dt.push_back( tmp[t * tMult] ) ;
			}
		      
		      
		      float weight = (float)pow(2,
						difftime(time_curr, time_origin)/
						(double) isotope_hl);
		      
		      
		      if ( do_unlink )
			try 
			  {
			    unlink( ( *(listName.begin()) ).c_str() ); 
			  }
			catch ( std::exception &e )
			  {
			    cerr << argv[ 0 ] << ": " << e.what() << endl;
			    result = EXIT_FAILURE;
			  }
		      for (int z = minz ; z < maxz ; ++z )
			for (int y = miny ; y < maxy-coefficient ; ++y )
			  for (int x = minx ; x < maxx-coefficient ; ++x )
			    {
			      // 		    if( data( x/coefficient, y/coefficient, z/coefficient) == 
				  // 			l+1 ) 
			      {
				rat( x-minx, y-miny, z-minz, t) =
				  in( x, y, z, t*tMult ) * weight;
			      }
			    }
		      /*cout << "Weight = " << weight << "  minz = " << minz << " miny = " << miny << " minx = " << minx 
		      	   << "  maxz = " << maxz << " maxy = " << maxy << " maxx = " << maxx << endl ;*/
// 		      Writer< AimsData<float> > wriIn(string("in_") + toString(t) + ".v" ) ;
// 		      wriIn.write(in) ;
		      if( inputSplitFormat == MANYFILES )
			++it ;
		    }

		  if( inputSplitFormat == MANYFILES || frameByFrame){
		    npheader->setProperty("start_time", st);
		    npheader->setProperty("duration_time", dt);		  
		    rat.setHeader ( npheader->cloneHeader() );
		  }
		  
		  //   Ecriture des imagettes
		  string fileName = "";
		  if ( dirn != "" )  fileName += dirn + "/";
		  char tmp_num[4]; sprintf(tmp_num, "_%c", int('A')+l);
		  string fn =   ph->removeExtension( *( listName.begin() ) );
		  uint p;
		  if ((p=fn.rfind("/")) > fn.size() ) p = 0;
		  fn = fn.substr( p, fn.size() );
		  fileName += fn + string(tmp_num) + ".v";
		  
		  //cout << "writing " << fileName << endl ; 
		  if( outputFileNames.size() == 0 ){
		    Writer< AimsData<float> > dataW( fileName );
		    dataW.write( rat );  
		  } else {
		    Writer< AimsData<float> > dataW( outputFileNames[l] );
		    dataW.write( rat ) ;  		    
		  }
		  
		  //cout << "Wrote " << fileName << endl ;
		}
	    }
	  
	  return ( EXIT_SUCCESS );
    }
  } catch ( std::exception &e ){
    cerr << argv[ 0 ] << ": " << e.what() << endl;
    result = EXIT_FAILURE;
  }
  
  return ( EXIT_FAILURE );
}

