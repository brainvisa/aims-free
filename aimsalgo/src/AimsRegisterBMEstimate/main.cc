/* Copyright (C) 2000-2013 CEA
 *
 * This software and supporting documentation were developed by
 *     bioPICSEL
 *     CEA/DSV/I²BM/MIRCen/LMN, Batiment 61,
 *     18, route du Panorama
 *     92265 Fontenay-aux-Roses
 *     France
 */

#include <aims/getopt/getopt2.h>
#include <aims/data/data_g.h>

// Entrees sorties generiques
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/motionW.h>
#include <aims/io/motionR.h>
#include <aims/resampling/resampling_g.h> 

// Gestion des fichiers
#include <stdio.h>
#include <string>
#include <iomanip>		// setprecision

// A mettre dans le main.h
#include <aims/math/math_g.h>
#include <vector>
#include <cartobase/config/verbose.h>
#include <cartobase/type/string_conversion.h>
#include <cartobase/type/limits.h>
#include <cartobase/stream/fileutil.h>
#include <aims/io/channelR.h>
#include <aims/registration/displacement_field.h>
#include <aims/registration/scale_control.h>
#include <aims/registration/transformation.h>
#include <aims/registration/minimisation.h>

#ifdef AIMS_USE_MPI
#include <mpi.h>

//root MPI process
#define ROOT 0
//length of dim&size info to transmit to slaves
#define INFO_SPAN 8
//length of motion info to transmit to slaves
#define MOTION_SPAN 12
#endif

using namespace aims;
using namespace carto;
using namespace std;

#ifdef AIMS_USE_MPI
using namespace MPI;
#include <aims/mpi/types.h>
#endif

template <class T>
bool doit( Process &, const string &, Finder & );

class ABM : public Process
{
public:
  ABM( const string & fileout_param, 
       const string & fileint_param, 
       const string & filefield_param, 
       const string & filemotion_param, 
       const string & Info_param, 
       int channelref_param,
       int channeltest_param,
       int level_start_param, 
       int level_stop_param, 
       int iterMax_param, 
       double cutVar_param, 
       double stopVar_param, 
       double seuilCorrel_param, 
       double Pkept_param, 
       Point3d tailleBloc_param, 
       double seuils_paramlr, 
       double seuils_paramhr, 
       double seuils_paramlt, 
       double seuils_paramht, 
       int transfo_param, 
       bool inverse_param,
       string outmotion_param, 
       string background_param);

  template <class T>
  friend bool doit( Process &, const string &, Finder & );

private:
  string  fileout;
  string  fileint;
  string  filefield;
  string  filemotion;
  string  Info;
  int channelref;
  int channeltest;
  int level_start;
  int level_stop;
  int iterMax;
  double cutVar;
  double stopVar;
  double seuilCorrel;
  double Pkept;
  Point3d tailleBloc;
  double seuils[4];
  int transfo;
  bool inverse;
  string outmotion;
  string background;
};


ABM::ABM( const string & fileout_param, 
          const string & fileint_param, 
          const string & filefield_param, 
          const string & filemotion_param, 
          const string & Info_param, 
          int channelref_param,
          int channeltest_param,
          int level_start_param, 
          int level_stop_param, 
          int iterMax_param, 
          double cutVar_param, 
          double stopVar_param, 
          double seuilCorrel_param, 
          double Pkept_param, 
          Point3d tailleBloc_param, 
          double seuils_paramlr, 
          double seuils_paramhr, 
          double seuils_paramlt, 
          double seuils_paramht, 
          int transfo_param, 
          bool inverse_param,
          string outmotion_param,
          string background_param ) : Process(), 
                                     fileout(fileout_param), 
                                     fileint(fileint_param), 
                                     filefield(filefield_param), 
                                     filemotion(filemotion_param),
                                     Info(Info_param), 
                                     channelref(channelref_param),
                                     channeltest(channeltest_param),
                                     level_start(level_start_param), 
                                     level_stop(level_stop_param), 
                                     iterMax(iterMax_param), 
                                     cutVar(cutVar_param), 
                                     stopVar(stopVar_param), 
                                     seuilCorrel(seuilCorrel_param), 
                                     Pkept(Pkept_param), 
                                     tailleBloc(tailleBloc_param), 
                                     transfo(transfo_param), 
                                     inverse(inverse_param),
                                     outmotion(outmotion_param),
                                     background(background_param)
{
  seuils[0]=seuils_paramlr;
  seuils[1]=seuils_paramhr;
  seuils[2]=seuils_paramlt;
  seuils[3]=seuils_paramht;


  registerProcessType( "Volume", "S8", &doit<int8_t> );
  registerProcessType( "Volume", "U8", &doit<uint8_t> );
  registerProcessType( "Volume", "S16", &doit<int16_t> );
  registerProcessType( "Volume", "U16", &doit<uint16_t> );
  registerProcessType( "Volume", "S32", &doit<int32_t> );
  registerProcessType( "Volume", "U32", &doit<uint32_t> );
  registerProcessType( "Volume", "FLOAT", &doit<float> );
  registerProcessType( "Volume", "DOUBLE", &doit<double> );
  registerProcessType( "Volume", "RGB", &doit<int16_t> );
  registerProcessType( "Volume", "RGBA", &doit<int16_t> );
}


template <class T>
bool doit( Process & process, const string & fileinr, Finder & )
{
  ABM & abm = (ABM & ) process;
  T bv;
  stringTo(abm.background, bv);

  cout << "Type is: " <<DataTypeCode<T>().dataType() << endl;

  // Instanciation des differentes classes externes
  VolumeRef< T > ref, test; //image fixe et flottante respectivement
  vector<float> vs_test, vs_ref;
  vector<int> dim_test, dim_ref;
  ScaleControl scaleControl; // Gere la pyramide
  DisplacementField<T> displacementField; // Calcule le champ d'appariemment
  Minimisation minimisation(abm.Pkept, abm.transfo); // Calcule la transformation a partir du champ

  // Declaration des differents motions necessaires
  // p transfo totale precedente, q transfo ponctuelle calculee, r nouvelle transfo totale composee
  Motion p, q, r ;
  q.setToIdentity();
  r.setToIdentity();

#ifdef AIMS_USE_MPI
  /* PAR: MPI specific global (master and slave) declaration area */
  int rank = COMM_WORLD.Get_rank();
  int masterIntInfo[INFO_SPAN]; //integer info (image dimensions) to transmit to slaves
  float masterFInfo[INFO_SPAN]; //float info (voxel sizes) to transmit to slaves
  float masterMotionInfo[MOTION_SPAN]; //Motion info (translation and rotation coefficients) to transmit to slaves

  /* PAR: Master process */
  if(rank==ROOT) {
#endif
    // Chargement de l'image de reference et de l'image flottante appelee test
    ChannelReader< VolumeRef<T> > rdref(fileinr);
    ChannelReader< VolumeRef<T> > rdtest(abm.fileint);
    cout << "Reading channel " << carto::toString(abm.channelref) << " of reference image" << endl << flush;
    rdref.read(ref, abm.channelref);
    ref.header().getProperty("voxel_size", vs_ref);
    dim_ref = ref.getSize();
    cout << "Reading channel " << carto::toString(abm.channeltest) << " of test image" << endl << flush;
    rdtest.read(test, abm.channeltest);
    test.header().getProperty("voxel_size", vs_test);
    dim_test = test.getSize();
    //  Writer< AimsData<T> > checkWriter("ref");  
    //  checkWriter.write(ref);
    //  Writer< AimsData<T> > check2Writer("test");  
    //  check2Writer.write(test);


    //  Lecture d'une transfo initiale éventuelle
    if(abm.filemotion.length() != 0) {
      MotionReader mord(abm.filemotion);
      mord >> p;
    }
    else p.setToIdentity();

#ifdef AIMS_USE_MPI
    /* Transmit dim and size info to slaves, normally enough for correct recreation of
      images in slaves. No direct mem copy of whole AimsData because header and contents
      not of constant length, and it's not faster. Same for Motion p.*/
    masterIntInfo[0] = dim_ref[0];
    masterIntInfo[1] = dim_ref[1];
    masterIntInfo[2] = dim_ref[2];
    masterIntInfo[3] = dim_ref[3];
    masterIntInfo[4] = dim_test[0];
    masterIntInfo[5] = dim_test[1];
    masterIntInfo[6] = dim_test[2];
    masterIntInfo[7] = dim_test[3];

    masterFInfo[0] = vs_ref[0];
    masterFInfo[1] = vs_ref[1];
    masterFInfo[2] = vs_ref[2];
    masterFInfo[3] = vs_ref[3];
    masterFInfo[4] = vs_test[0];
    masterFInfo[5] = vs_test[1];
    masterFInfo[6] = vs_test[2];
    masterFInfo[7] = vs_test[3];

    if(abm.filemotion.length() != 0) {
      masterMotionInfo[0] = p.translation()[0];
      masterMotionInfo[1] = p.translation()[1];
      masterMotionInfo[2] = p.translation()[2];
      masterMotionInfo[3] = p.rotation()[0];
      masterMotionInfo[4] = p.rotation()[1];
      masterMotionInfo[5] = p.rotation()[2];
      masterMotionInfo[6] = p.rotation()[3];
      masterMotionInfo[7] = p.rotation()[4];
      masterMotionInfo[8] = p.rotation()[5];
      masterMotionInfo[9] = p.rotation()[6];
      masterMotionInfo[10] = p.rotation()[7];
      masterMotionInfo[11] = p.rotation()[8];
    }
  } //end master

  COMM_WORLD.Bcast(masterIntInfo, INFO_SPAN, INT, ROOT);
  COMM_WORLD.Bcast(masterFInfo, INFO_SPAN, FLOAT, ROOT);
  if(abm.filemotion.length() != 0) {
    COMM_WORLD.Bcast(masterMotionInfo, MOTION_SPAN, FLOAT, ROOT);
  }
  COMM_WORLD.Barrier(); //unnecessary? Non-synch Bcast would mean that it is non-blocking...& collective comms are blocking in mpich2

  if(rank!=ROOT) {
    dim_ref[0] = masterIntInfo[0];
    dim_ref[1] = masterIntInfo[1];
    dim_ref[2] = masterIntInfo[2];
    dim_ref[3] = masterIntInfo[3];
    dim_test[0] = masterIntInfo[4];
    dim_test[1] = masterIntInfo[5];
    dim_test[2] = masterIntInfo[6];
    dim_test[3] = masterIntInfo[7];
    
    vs_ref[0] = masterFInfo[0];
    vs_ref[1] = masterFInfo[1];
    vs_ref[2] = masterFInfo[2];
    vs_ref[3] = masterFInfo[3];
    vs_test[0] = masterFInfo[4];
    vs_test[1] = masterFInfo[5];
    vs_test[2] = masterFInfo[6];
    vs_test[3] = masterFInfo[7];
    
    ref = VolumeRef<T>(dim_ref); //useless memCopy, to avoid using pointers
    test = VolumeRef<T>(dim_test);
    ref.setVoxelSize(vs_ref);
    test.setVoxelSize(vs_test);
    
    if(abm.filemotion.length() != 0)
    {
      p.setTranslation(
        Point3df( masterMotionInfo[0], masterMotionInfo[1],
                  masterMotionInfo[2]) );
      VolumeRef<float> tmp(3,3);
      for(int i = 0;i < 9; i++)
        tmp[i] = masterMotionInfo[i+3];
      p.setMatrix(tmp);
    }
    else p.setToIdentity();
  } //end slave

  Datatype usedType = MpiDataTypeCode<T>::mpiDataType();

  COMM_WORLD.Bcast(&ref(0), dim_ref[0] * dim_ref[1] * dim_ref[2] * dim_ref[3], usedType, ROOT);
  COMM_WORLD.Bcast(&test(0), dim_test[0] * dim_test[1] * dim_test[2] * dim_test[3], usedType, ROOT);
  COMM_WORLD.Barrier(); //unnecessary?
#endif
  /* Inits */
  if(dim_ref[2]==1) abm.tailleBloc[2] = 1;
  scaleControl.init<T>( ref, abm.level_start, abm.level_stop, abm.cutVar, abm.stopVar, abm.seuilCorrel, abm.tailleBloc);
  int count = 0 ;
  T templateSeuils[4];
  templateSeuils[0]=  std::numeric_limits<T>::min();
  templateSeuils[1]=  std::numeric_limits<T>::max();
  templateSeuils[2]=  std::numeric_limits<T>::min();
  templateSeuils[3]=  std::numeric_limits<T>::max();

  if( (DataTypeCode<T>().dataType() == "FLOAT") || (DataTypeCode<T>().dataType() == "DOUBLE") )
  {
    templateSeuils[0]= -templateSeuils[1];
    templateSeuils[2]= -templateSeuils[1];
  }

  templateSeuils[0]=  (abm.seuils[0] < (double)templateSeuils[0] ) ? templateSeuils[0] : (T)abm.seuils[0] ;
  templateSeuils[1]=  (abm.seuils[1] > (double)templateSeuils[1] ) ? templateSeuils[1] : (T)abm.seuils[1] ;
  templateSeuils[2]=  (abm.seuils[2] < (double)templateSeuils[2] ) ? templateSeuils[2] : (T)abm.seuils[2] ;
  templateSeuils[3]=  (abm.seuils[3] > (double)templateSeuils[3] ) ? templateSeuils[3] : (T)abm.seuils[3] ;

#ifdef AIMS_USE_MPI
  if(rank==ROOT) {
#endif
    // Affichage de tous les parametres avant debut :
    if(abm.Info=="y")
      {
        cout << endl << "PARAMETRES INITIAUX :" << endl;
        cout << "Image  reference : " << fileinr << endl;
        cout << "Image  test : " << abm.fileint << endl;
        if ( abm.fileout.length() == 0 )
        {
          cout << "Pas de sauvegarde de l'image recalee" << endl;
        }
        else
        {
          cout << "Nom image resultat test recalee : " << abm.fileout << endl;
        }

        if(abm.filemotion=="") cout << "Pas de transfo initiale" << endl;
        else
          { 
            cout << "Transfo initiale : " << flush;
            cout << p;
          }
        cout << "Transformation recherchee : ";
        switch(abm.transfo){
        case 1: cout << "RIGIDE" << endl; break;
        case 2: cout << "SIMILITUDE" << endl; break;
        case 3: cout << "AFFINE" << endl; break;}

        cout << "Pyramide debut : " << scaleControl.getScale() << "  fin : " << scaleControl.getlevel_stop() << endl;
        cout << "Taille des blocs : x = " << abm.tailleBloc[0] 
                              << "  y = " << abm.tailleBloc[1] 
                              << "  z = " << abm.tailleBloc[2] << endl;
        cout << "Pourcentage de blocs conserves par variance : %initial = " << scaleControl.getcutVar() 
                                                         << "  %final = " << scaleControl.getstopVar() << endl;
        cout << "Seuil sur la mesure de similarite = " << abm.seuilCorrel << endl;
        cout << "Seuils sur les niveaux de gris : " << endl;
        cout << "     seuilBasRef = " << templateSeuils[0] << "  seuilHautRef = " << templateSeuils[1] << endl;
        cout << "     seuilBasTest = " << templateSeuils[2] << "  seuilHautTest = " << templateSeuils[3] << endl;
        cout << "Nombre d'iterations par niveau de pyramide : " << abm.iterMax << endl;
      }
#ifdef AIMS_USE_MPI
  } //end master
#endif


  VolumeRef<T> test_orig = test.deepcopy();
  // Mise de l'image test a la resolution de l'image de reference si besoin
  if( (vs_ref[0]!=vs_test[0]) || (vs_ref[1]!=vs_test[1]) || (vs_ref[2]!=vs_test[2]) )
  {
    Motion identity;
    identity.setToIdentity();
    LinearResampler<T> reech;
    reech.setRef( test );
    reech.setDefaultValue( bv );
  
//     std::cout << "main, resampled dims: ["
//               << carto::toString(std::max((unsigned)(test.dimX()*test.sizeX()/ref.sizeX() + .5), (unsigned)1)) << ", "
//               << carto::toString(std::max((unsigned)(test.dimY()*test.sizeY()/ref.sizeY() + .5), (unsigned)1)) << ", "
//               << carto::toString(std::max((unsigned)(test.dimZ()*test.sizeZ()/ref.sizeZ() + .5), (unsigned)1)) << "]"
//               << std::endl << std::flush;
    test = reech.doit( identity, 
                       std::max((unsigned)(dim_test[0]*vs_test[0]/vs_ref[0] + .5), (unsigned)1),
                       std::max((unsigned)(dim_test[1]*vs_test[1]/vs_ref[1] + .5), (unsigned)1),
                       std::max((unsigned)(dim_test[2]*vs_test[2]/vs_ref[2] + .5), (unsigned)1),
                       Point3df(vs_ref[0],vs_ref[1],vs_ref[2]));
  }


  // Quelques initialisations en millimetres
  /*transformation.setX(int(test.dimX()*test.sizeX()));
  transformation.setY(int(test.dimY()*test.sizeY())); 
  transformation.setZ(int(test.dimZ()*test.sizeZ())); 
  transformation.setiterMax(abm.iterMax);   //on fixe le nombre maxi d'iter par niveau
  transformation.setcx(0);
  transformation.setcy(0);
  transformation.setcz(0);*/
  
  // Initialisation de delta au carre de 4 fois un majorant de 2 fois la diagonale en mm...
  //float delta_init = 8*pow(test.dimX()*test.sizeX() + test.dimY()*test.sizeY() + test.dimZ()*test.sizeZ(), 2);

  // Declaration de l image transformee a chaque etape
  VolumeRef<T> testtrans = test.deepcopy();
  
  
  //Prise en compte d'une transfo initiale éventuelle
  if(abm.filemotion.length() != 0)
    {
      LinearResampler<T> reech;
      reech.setRef( test );
      reech.setDefaultValue( bv );
      
      testtrans = reech.doit( p, 
                              dim_test[0],
                              dim_test[1],
                              dim_test[2],
                              Point3df(vs_test[0],vs_test[1],vs_test[2]) );
    }

  // MISE DE p EN VOXELS !
  p.matrix()(0, 3) /= vs_test[0];
  p.matrix()(1, 3) /= vs_test[1];
  p.matrix()(2, 3) /= vs_test[2];
  
  do
  {
    // boucle echelle
    int pyra = scaleControl.getScale();
    //cerr << endl << endl << "PYRAMID LEVEL: " << pyra << endl;
    //transformation.setdeltaprev(delta_init);
    displacementField.init( ref,  scaleControl, templateSeuils);
    count = 1; // count compte le nbre d iterations sans changer d echelle

    do 
    {  
      // boucle delta
      cout << "        ITERATION N " << count << endl;
      
      // mise a jour transfo ancienne a ancienne nouvelle...	  
      p = q * p ;
//       std::cout << "!! New p: " << p << std::endl;

      // Calcul effectif du champ
      cartoMsg( 2, "Processing displacement field...", "BlockMatching" );
      AimsData< Point3d > df = displacementField.getField( testtrans);
    
//       int dfcount = 0;
//       int dfsx = df.dimX(), dfsy = df.dimY(), dfsz = df.dimZ(), dfst = df.dimT();
//       for(int t=0; t<dfst; ++t)
//           for(int z=0; z<dfsz; ++z)
//               for(int y=0; y<dfsy; ++y)
//                   for(int x=0; x<dfsx; ++x)
//                       if (df(x, y, z, t) != Point3d(0,0,0))
//                           dfcount++;
//                       
//           
//       std::cout << "!! Displacement field non null values: " << dfcount<< std::endl; 
      
      if(abm.filefield.length() != 0)
      {
        Writer< AimsData<Point3d> > PtWriter(string(abm.filefield) + toString(pyra) + "_" + toString(count) );  
        PtWriter.write(df);
        
        /*
        // Ecriture des donnees dans un fichier :
        ofstream f("champs.txt" );
        if (f.is_open())
        {
        int x,y,z,n=0;	
        ForEach3d(df,x,y,z)
        {
        f <<"Element "<< n <<"   Noeuds : (" <<setprecision(3)<< x*df.sizeX()<<","<<setprecision(3)<<y*df.sizeY()<<") (";
        f <<setprecision(3)<<(x+1)*df.sizeX()<<","<<setprecision(3)<<y*df.sizeY()<<") (";
        f <<setprecision(3)<<x*df.sizeX()<<","<<setprecision(3)<<(y+1)*df.sizeY()<<") (";
        f <<setprecision(3)<<(x+1)*df.sizeX()<<","<<setprecision(3)<<(y+1)*df.sizeY()<<")   Vecteur : ";
        f <<setprecision(3)<<df(x,y,z)[0]*df.sizeX() <<" "<<setprecision(3)<<df(x,y,z)[1]*df.sizeY()<<endl;
        n++;
        }
        f.close();
        }*/
      }
      
      // Attention, le champ va de Ref a Test et non l'inverse !!!!!!!!!!!!!!!!!!!!!!!!
      
      cartoMsg( 2, "Processing quaternion...", "BlockMatching" );
      // Calcul de la transformation d'etape a partir du champ estime, rappel : q est un motion
      q = minimisation.quaternion(displacementField);
//       std::cout << "!! Quaternion: " << q << std::endl;
      //transfo de test vers ref !!!!
      
      // Rappel : r est la transformation totale actualisee, q la calculee courante et p l'ancienne totale
      // On actualise effectivement r en composant q avec r
      r = q * p ;
      
      // MISE DES TRANSLATIONS EN MILLIMETRES !
      r.matrix()(0, 3) *= test.getVoxelSize()[0];
      r.matrix()(1, 3) *= test.getVoxelSize()[1];
      r.matrix()(2, 3) *= test.getVoxelSize()[2];
      
//       std::cout << "!! New matrix: " << r << std::endl;
      
      cartoMsg( 2, "Resampling using transformation ...", "BlockMatching" );
      // Resampling de test en testtrans par r la transfo totale la plus recente
      LinearResampler<T> resampler;
      resampler.setRef( test );
      resampler.setDefaultValue( bv );
      testtrans = resampler.doit( r, 
                                  test.getSizeX(), 
                                  test.getSizeY(), 
                                  test.getSizeZ(),
                                  Point3df(test.getVoxelSize()[0],
                                           test.getVoxelSize()[1],
                                           test.getVoxelSize()[2]));
      // Ecriture de testrans pour verification du bon deroulement des iterations
      //	  		Writer< AimsData<T> > wri( "testtrans" + toString(count) ) ;
      //	  		wri.write( testtrans ) ;
      
      // Compteur du nombre d'iterations a cette échelle
      ++count ;
      cartoMsg( 2, "Increasing iterations...", "BlockMatching" );
    } while ( count < (abm.iterMax + 1) );		// On fait iterMax iterations exactement a chaque niveau de pyramide
  
    // } while ( 0 /*transformation.testdelta(p,q,scaleControl)*/);	 test delta < epsilon INACTIVE
    
    // On a fini d'iterer a cette echelle, on monte un niveau de pyramide
    scaleControl.nextScale();
    
    
  } while (scaleControl.goOn());		// test fin pyramide


  
  //Resampling de testtrans dans sa geometrie initiale
  LinearResampler<T> resampler;
  resampler.setRef( test_orig );
  resampler.setDefaultValue( bv );
  testtrans = resampler.doit( r, test_orig.getSizeX(), test_orig.getSizeY(), test_orig.getSizeZ(), 
            Point3df(test_orig.getVoxelSize()[0], test_orig.getVoxelSize()[1], test_orig.getVoxelSize()[2] ) );
  
#ifdef AIMS_USE_MPI
  if(rank==ROOT) {
#endif  
  // Ecriture sur disque de l'image finale recalee
  if (abm.fileout.length() != 0)
  {
    Writer< AimsData<T> > finalWriter(abm.fileout);
    finalWriter.write(testtrans);
  }

  // Ecriture de la transformation finale obtenue r
  string outmotion = abm.outmotion;
  if (abm.inverse)
    r = r.inverse();
    
  if( outmotion.empty() )
  {
    string n1, n2;
    if (abm.inverse)
    {
      n1 = fileinr;
      n2 = abm.fileint;
    }
    else
    {
      n1 = abm.fileint;
      n2 = fileinr;
    }
    outmotion =  FileUtil::dirname(string(abm.fileint)) +  FileUtil::separator() 
                  + FileUtil::removeExtension( FileUtil::basename(string(n1))) + "_TO_" 
                  + FileUtil::removeExtension( FileUtil::basename(string(n2))) + ".trm";
  }
  MotionWriter wm( outmotion );
  wm.write( r );
  
  cout << "End of the process" << endl << "Motion estimated is: " << endl;
  cout << r << endl;
#ifdef AIMS_USE_MPI
  } //end master
#endif

  return(1);
}


int main(int argc,const char **argv)
{
#ifdef AIMS_USE_MPI
  char** argv2 = (char**) argv;     //init_thread needs a non-const argv
  if(Init_thread(argc, argv2, THREAD_SINGLE) != THREAD_SINGLE) {
    cerr << "MPI:Provided thread level does not match required" << endl;
    return(EXIT_FAILURE);
  }
#endif

  try	{
    
    string fileinr, fileint , fileout = "", filefield="", filemotion="", Info = "y", outmotion;
    int refchannel = -1, testchannel = -1; // Default is the norm channel
    int level_start = -1 , level_stop = -1;		// Pyramide
    int iterMax = 3;
    double cutVar = -1, stopVar = -1, seuilCorrel = -1, Pkept=0.5;
    Point3d tailleBloc(4, 4, 4);
    double seuils[4];
    seuils[0]=-std::numeric_limits<double>::max();
    seuils[1]=-seuils[0];
    seuils[2]=seuils[0];
    seuils[3]=seuils[1];
    int transfo = 1;
    bool inverse = false;
    string bv = "0";
    
    
    AimsApplication app( argc, argv, "Compute registrations between two 2D or 3D volumes" );
    app.addOption( fileinr,  "-r", "Input reference data" );
    app.addOption( fileint,  "-t", "Input test data" );
    app.addOption( fileout,  "-o", "Output data", true);
    app.addOption( bv,       "-bv", "Background value to use", true );
    app.addOption( refchannel, "-cr",  "Channel of multi-channel reference image to use during registration estimation\n"
                                       "Possible types and values:\n"
                                       "type : RGB or RGBA\n"
                                       "values: 0=red, 1=green, 2=blue, 3=alpha, 4=norm\n\n"
                                       "type : HSV\n"
                                       "values: 0=hue, 1=saturation, 2=value, 4=norm\n\n"
                                       "[default is set to norm]\n\n", true);
    app.addOption( testchannel, "-ct",  "Channel of multi-channel test image to use during registration estimation\n"
                                        "Possible types and values:\n"
                                        "type : RGB or RGBA\n"
                                        "values: 0=red, 1=green, 2=blue, 3=alpha, 4=norm\n\n"
                                        "type : HSV\n"
                                        "values: 0=hue, 1=saturation, 2=value, 4=norm\n\n"
                                        "[default is set to same value as reference channel]\n\n", true);
    app.addOption( filemotion, "-m", "Initialisation motion (default = identity)", true);
    app.addOption( outmotion, "-n", "Output motion (default = <test data directory>/<test>_TO_<ref>.trm)", true);
    app.addOption( inverse, "--inv", "Inverse output motion, if 1 default output motion name will be = <test data directory>/<ref>_TO_<test>.trm (default = 0)", true);
    app.addOption( level_start,  "--start", "Starting pyramid level (default = auto)" ,true);
    app.addOption( level_stop,  "--stop", "End pyramid level (default = auto)" ,true);
    app.addOption( transfo,  "--transfo", "Transformation type : 1 for rigid, 2 for similitude, 3 for affine (default = 1)" ,true);
    app.addOption( tailleBloc[0],  "--Xsize", "X size of the bloc in voxel (default = 4)" ,true);
    app.addOption( tailleBloc[1],  "--Ysize", "Y size of the bloc in voxel (default = 4)" ,true);
    app.addOption( tailleBloc[2],  "--Zsize", "Z size of the bloc in voxel (default = 4)" ,true);
    app.addOption( cutVar,   "--cutVar", "% initial de blocs de plus forte variance conserves (default=auto)" ,true);
    app.addOption( stopVar, "--stopVar", "Minimum du % de blocs conserves (default=auto)" ,true);
    app.addOption( Pkept, "--%kept", "% de blocs de plus faibles residus conserves pour l'estimation robuste (default=0.5)" ,true);
    app.addOption( seuilCorrel, "--correlThresh", "Threshold on the similarity criterion norm (default=-1, no threshold)" ,true);
    app.addOption( seuils[0],  "--lthRef", "Low grey level threshold on ref image (default = -100000)" ,true);
    app.addOption( seuils[1],  "--hthRef", "High grey level threshold on ref image (default = +100000)" ,true);
    app.addOption( seuils[2],  "--lthTest", "Low grey level threshold on test image (default = -100000)" ,true);
    app.addOption( seuils[3],  "--hthTest", "High grey level threshold on test image (default = +100000)" ,true);
    app.addOption( iterMax, "--iter", "Maximum number of iterations per pyramid level (default=3)" ,true);
    app.addOption( filefield,"--field", "Displacement field",true);
    app.addOption( Info,"--Info", "List all the initial parameters : y or n (default=y)",true);
    app.initialize(); 
    
    if (testchannel == -1) {
      testchannel = refchannel;
    }
    
    ABM proc(fileout, fileint, filefield, filemotion, Info, 
             refchannel, testchannel,
             level_start,  level_stop,  iterMax,  cutVar,  stopVar,  seuilCorrel,  Pkept, 
             tailleBloc, seuils[0], seuils[1], seuils[2], seuils[3], transfo, inverse, outmotion, bv );

    proc.execute(fileinr);
    
  }
  catch(user_interruption &e) 
  {
  }
  catch(std::exception &e)
  {
    cerr<<argv[0]<<":"<<e.what()<<endl;
    return EXIT_FAILURE;
  }

#ifdef AIMS_USE_MPI
  Finalize();             //MPI finished
#endif
  
  return EXIT_SUCCESS;
}
