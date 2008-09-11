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


/*

AimsEssai-JD -i ../AimsThalamus-JD/sortie_classe -f ~/clust/sortie_cla 
-g ../AimsThalamus-JD/tab_classes -o image

*/

#include <aims/getopt/getopt.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/data/data_g.h>
#include <iostream>
#include <iomanip>
#include <stdio.h>

using namespace aims ;
using namespace std;

BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsKMeans	-f[ile]  <data file>",
  "             -g[ile2] <data file>",
  "             -i[nput] <image>",
  "             -o[utput]",
  "             -n[umber]",
  "             [-h[elp]]",
  "-------------------------------------------------------------------------",
  "	Image des classes produite par le kmeans",
  "-------------------------------------------------------------------------",
  "     file        : nom fichier containing the classification             ",
  "     file2       : nom fichier des donnees initiales                     ",
  "     input       : nom image segmentee                                   ",
  "     fileout     : nom image sortie	des classes",
  "     number      : number of vectors                                     ",
  "-------------------------------------------------------------------------",
END_USAGE


static void Usage()
{
  AimsUsage(usage);
}


//  Debut du programme principal

int main( int argc, char* argv[] )


{
  // Default values
  const char	*filein =NULL, *fileread = NULL,*fileread2 = NULL,
    *fileout = "out";
  int n;


  // Collecte des arguments.
  AimsOption opt[] = {
  { 'h',"help"         ,AIMS_OPT_FLAG  ,( void* )Usage      ,AIMS_OPT_CALLFUNC,0},
  { 'i',"input"        ,AIMS_OPT_STRING,&filein     ,0                ,1},
  { 'f',"file"         ,AIMS_OPT_STRING,&fileread   ,0                ,1},
  { 'g',"file2"        ,AIMS_OPT_STRING,&fileread2  ,0                ,1},
  { 'n',"integer"      ,AIMS_OPT_INT   ,&n          ,0                ,1},
  { 'o',"output"       ,AIMS_OPT_STRING,&fileout    ,0                ,0},
  { 0  ,0              ,AIMS_OPT_END   ,0          ,0                ,0}};



  AimsParseOptions(&argc,argv,opt,usage);

  cout<<"Nombre d'individus: "<<n<<endl;
  
    
  // Lecture image entree segmentee
  AimsData<short> seg;
  Reader<AimsData<short> > dataR1( filein );
  dataR1.read(seg);

  AimsData<short> im(seg.dimX(),seg.dimY(),seg.dimZ() );			im.setSizeXYZT( seg.sizeX(),seg.sizeY(),seg.sizeZ());
//  image de sortie
  im = 0;

  int e;
  int tab;
  Point3df p;
  FILE *pfichier, *pfichier2;

  pfichier=fopen(fileread, "r");
  pfichier2=fopen(fileread2, "r");
  
  cout<<"ok jusque la"<<endl;
  
  for(int i =0; i<n;i++)
  {
 		for(int j=0;j<3;j++)
			{ 	
				fscanf(pfichier2,"%d", &tab);
  				p[j] = tab;
				//cout<<p[j]<<" ";
			}
		fscanf(pfichier,"%u", &e);
		im( (unsigned) p[0], (unsigned) p[1], (unsigned) p[2] ) = e;
		cout<<p[0]<<" "<<p[1]<<" "<<p[2]<<" classe : "<<e<<endl;
	}

  fclose(pfichier);
  fclose(pfichier2);

  // Ecriture de l'image resultat
  Writer< AimsData<short> > dataWtr(fileout);
  dataWtr.write( im );
  
  return EXIT_SUCCESS; 
}
