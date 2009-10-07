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


/***************************************************************/
/*                        include                              */
/***************************************************************/
#include <cstdlib>
#include <aims/data/data_g.h>
#include <aims/bucket/bucket_g.h>
#include <aims/math/math_g.h>

/***************************************************************/
/*                declaration classes et fonctions             */
/***************************************************************/
class Index;
float relativeAniso( DtiTensor& thing );
float baseLIn ( DtiTensor& thing1 , DtiTensor& thing2 );



/***************************************************************/
/*                     classe Index                            */
/***************************************************************/
class Index 
{
  private:
     AimsBucket < DtiTensor > bucket;
     int dimX, dimY,dimZ;
     std::list< AimsBucketItem<DtiTensor> >::iterator it;
     
     AimsData< float > *trace ;
     AimsData< float > *lambda1;
     AimsData< float > *lambda2;
     AimsData< float > *lambda3 ;
     AimsData< float > *fa;
     AimsData< float > *vr;
     AimsData< Point3df >*dir;
     AimsData< float > *ra;
     AimsData< float > *coherenceIndex;
     AimsData< float > *latticeIndex;
     
     AimsData< float > *linearMeasure;
     AimsData< float > *planarMeasure;
     AimsData< float > *sphericalMeasure;

  public:
     // Constructeur
	Index( AimsBucket<DtiTensor> thing, int xdim, int ydim,int zdim );
     
     // Destructeur
        ~Index();

     // Fonction pour recuperer la trace
        AimsData<float> Trace()  {return *trace;}
     
     // Fonction permettamt de calculer la trace
        void calcul_Trace();

     // Fonction pour recuperer lambda1
        AimsData<float> Lambda1()  {return *lambda1;}
     
     // Fonction permettamt de calculer lambda1
        void calcul_Lambda1();

     // Fonction pour recuperer lambda2
        AimsData<float> Lambda2()  {return *lambda2;}
     
     // Fonction permettamt de calculer lambda2
        void calcul_Lambda2();

     // Fonction pour recuperer lambda3
        AimsData<float> Lambda3()  {return *lambda3;}
     
     // Fonction permettamt de calculer lambda3
        void calcul_Lambda3();

     // Fonction pour recuperer Fractionnal Anisotropy
        AimsData<float> FracAniso()  {return *fa;}
     
     // Fonction permettamt de calculer Fractionnal Anisotropy
        void calcul_FA();
     
     // Fonction pour recuperer Volume Ratio
        AimsData<float> VolRatio()  {return *vr;}
     
     // Fonction permettamt de calculer Volume Ratio
        void calcul_VR();
     
     // Fonction pour recupreer la direction principale
        AimsData<Point3df> Direction()  {return *dir;}

     // Fonction permettamt de calculer la direction principale
        void calcul_Direction();
     
     // Fonction pour recuperer Relative Ainotropy
        AimsData<float> RelativAniso()  {return *ra;}

     // Fonction permettamt de calculer Relative Anisotropy
        void calcul_RA();
     
     // Fonction pour recuperer Coherence Index
        AimsData<float> CoherenceIndex () {return *coherenceIndex;}
  
     // Fonction permettamt de calculer Coherence Index
        void calcul_coherenceIndex();
     
     // Fonction pour recuperer Lattice Index
        AimsData<float> LatticeIndex () {return *latticeIndex;}
  
     // Fonction permettamt de calculer Lattice Index
        void calcul_latticeIndex();
     
     // Fonction pour recuperer Linear Measure
	AimsData<float> LinearMeasure() {return *linearMeasure; }
     
     // Fonction pour recuperer Planar Measure
        AimsData<float> PlanarMeasure() {return *planarMeasure; }
	
     // Fonction  pour recuperer Spherical Measure
       	AimsData<float> SphericalMeasure() {return *sphericalMeasure; }

     // Fonction permettamt de calculer Linear Measure, Planar Measure,Spherical Measure
        void calcul_Measure();
	
    
     // Fonction amie permettant de calculer RA pour un tenseur
        friend float relativeAniso( DtiTensor& thing );

     // Fonction amie permettant de calculer l'element de base de LI pour un tenseur
        friend float baseLIn ( DtiTensor& thing1 , DtiTensor& thing2 );
};


/***************************************************************/
/*    Definition des fonctions membres de la classe Index      */
/***************************************************************/


//constructeur
Index :: Index( AimsBucket<DtiTensor> thing,int xdim, int ydim,int zdim ) 
{  
     trace = NULL;
     fa = NULL;
     vr = NULL;
     dir = NULL;
     ra = NULL;
     coherenceIndex = NULL;
     latticeIndex = NULL;
     linearMeasure = NULL;
     planarMeasure = NULL;
     sphericalMeasure = NULL;
     bucket = thing;
     dimX = xdim;
     dimY = ydim;
     dimZ = zdim;
};


// destructeur
Index :: ~Index()
{
  if(trace != NULL)
     delete trace;
  if(fa    != NULL)
     delete fa;
  if(vr    != NULL)
     delete vr;
  if(dir   != NULL)
     delete dir;
  if(ra    != NULL)
     delete ra;
  if(coherenceIndex != NULL)
     delete coherenceIndex;
  if(latticeIndex !=NULL)
     delete latticeIndex;
  if(linearMeasure !=NULL)
     delete linearMeasure;
  if(planarMeasure !=NULL)
     delete planarMeasure;
  if(sphericalMeasure !=NULL)
     delete sphericalMeasure;
}


// Fonction permettant de calculer la trace en chaque point de l'objet bucket
// trace = somme des valeurs propres du tenseur

void Index :: calcul_Trace()
{
  trace = new AimsData<float>(dimX, dimY,dimZ);

  trace->setSizeX( bucket.sizeX() );
  trace->setSizeY( bucket.sizeY() );
  trace->setSizeZ( bucket.sizeZ() );
  *trace=0;
  for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
    if ( it->value().category() <= DtiTensor::NO_PROBLEM )
      (*trace)( it->location().item(0),
                it->location().item(1),
                it->location().item(2) ) = it->value().base().trace();
}

void Index :: calcul_Lambda1()
{
  lambda1 = new AimsData<float>(dimX, dimY,dimZ);

  lambda1->setSizeX( bucket.sizeX() );
  lambda1->setSizeY( bucket.sizeY() );
  lambda1->setSizeZ( bucket.sizeZ() );
  *lambda1=0;
  for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
    if ( it->value().category() <= DtiTensor::NO_PROBLEM )
      (*lambda1)( it->location().item(0),
                it->location().item(1),
                it->location().item(2) ) =
	it->value().base().eigenvalue().item(2);
}

void Index :: calcul_Lambda3()
{
  lambda3 = new AimsData<float>(dimX, dimY,dimZ);

  lambda3->setSizeX( bucket.sizeX() );
  lambda3->setSizeY( bucket.sizeY() );
  lambda3->setSizeZ( bucket.sizeZ() );
  *lambda3=0;
  for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
    if ( it->value().category() <= DtiTensor::NO_PROBLEM )
      (*lambda3)( it->location().item(0),
                it->location().item(1),
                it->location().item(2) ) =
	it->value().base().eigenvalue().item(0);
}

void Index :: calcul_Lambda2()
{
  lambda2 = new AimsData<float>(dimX, dimY,dimZ);

  lambda2->setSizeX( bucket.sizeX() );
  lambda2->setSizeY( bucket.sizeY() );
  lambda2->setSizeZ( bucket.sizeZ() );
  *lambda2=0;
  for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
    if ( it->value().category() <= DtiTensor::NO_PROBLEM )
      (*lambda2)( it->location().item(0),
                it->location().item(1),
                it->location().item(2) ) =
	it->value().base().eigenvalue().item(1);
}

// Fonction permettant de calculer l'index Fractionnal Anisotropy en chaque point de l'objet bucket
// Fractionnal Anisotropy = rapport entre de la norme de la partie anisotropique du tenseur et la norme
//                          du tenseur

void Index :: calcul_FA()
{
  fa = new AimsData<float>( dimX, dimY,dimZ );
  fa->setSizeX( bucket.sizeX() );
  fa->setSizeY( bucket.sizeY() );
  fa->setSizeZ( bucket.sizeZ() );
  *fa = 0;
  for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
    if ( it->value().category() <= DtiTensor::NO_PROBLEM )
      (*fa)( it->location().item(0),
             it->location().item(1),
             it->location().item(2) ) = it->value().anisotropyFA();
}



// Fonction permettant de calculer l'index Volume Ratio en chaque point de l'objet bucket
// Volume Ratio = 27 * det(tenseur)/(trace^3)
//              = rapport entre volume d'une ellipsoide et le volume d'un sphere de rayon la moyenne des
//                valeurs propres

void Index :: calcul_VR()
{
  vr = new AimsData<float>( dimX, dimY,dimZ );  
  vr->setSizeX( bucket.sizeX() );
  vr->setSizeY( bucket.sizeY() );
  vr->setSizeZ( bucket.sizeZ() );
  *vr = 0;
  for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
    if ( it->value().category() <= DtiTensor::NO_PROBLEM )
      (*vr)( it->location().item(0),
             it->location().item(1),
             it->location().item(2) ) = it->value().anisotropyVR();
}


// Fonction permettant de calculer la direction principale en chaque point de l'objet bucket
// direction principale = vecteur propre associe a la plus grande valeur propre

void Index :: calcul_Direction()
{
  dir = new AimsData<Point3df>( dimX, dimY,dimZ );
  dir->setSizeX( bucket.sizeX() );
  dir->setSizeY( bucket.sizeY() );
  dir->setSizeZ( bucket.sizeZ() );
  *dir = Point3df(0,0,0);
  for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
    if ( it->value().category() <= DtiTensor::NO_PROBLEM )
      (*dir)( it->location().item(0),
              it->location().item(1),
              it->location().item(2) ) = it->value().dir();
}


// Fonction permettant de calculer l'index Relative Anisotropy en chaque point de l'objet bucket
// Relative Anisotropy = rapport entre la norme de la partie anisotropique du tenseur et la norme de le
//                       partie isotropique du tenseur.

void Index :: calcul_RA()
{
  ra = new AimsData<float>( dimX, dimY,dimZ );
  ra->setSizeX( bucket.sizeX() );
  ra->setSizeY( bucket.sizeY() );
  ra->setSizeZ( bucket.sizeZ() );
  *ra = 0;

  for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
   if ( it->value().category() <= DtiTensor::NO_PROBLEM )
      (*ra)( it->location().item(0),
             it->location().item(1),
             it->location().item(2) ) = relativeAniso(it->value());
}


// Fonction permettant de calculer Coherence Index en chaque point du bucket
// Coherence Index = produit scalaire moyen des directions principales en un point (i,j) avec les
//                   directions principales de ces 8 voisins

void Index :: calcul_coherenceIndex()
{
  coherenceIndex = new AimsData<float>( dimX, dimY,dimZ );  
  int x, y, z, r ,c,j ;
  float sum=0 ;
  float scalarProduct=0;
  
  for( z = 0 ; z < dir -> dimZ(); z++ )
    for( y = 1 ; y < (dir -> dimY() - 1); y++ )
     for( x = 1 ; x < (dir -> dimX() - 1); x++ )
      {
         for( sum = 0, r = (x - 1); r <= (x + 1) ; r++ )
            for( c = (y - 1); c <= (y + 1) ; c++ )
            {
                for(scalarProduct = 0, j = 0 ; j < 3 ; j++)
	            {
		     scalarProduct = scalarProduct + ( (*dir)(x,y,z)[j] ) * ( (*dir)(r,c,z)[j] );
		    }
	          sum = sum + fabs(scalarProduct);
            }
         (*coherenceIndex)(x,y,z) = sum / 8;
      }
}


// Fonction permettant de calculer Lattice Index en chaque point du bucket
// Latice Index pour un point ref.= somme(distance entre le point et le point ref. X l'element 
//              de base) / somme de toutes les distances

void Index :: calcul_latticeIndex()
{
  latticeIndex = new AimsData<float>( dimX, dimY,dimZ);
  latticeIndex ->setSizeX( bucket.sizeX() );
  latticeIndex ->setSizeY( bucket.sizeY() );
  latticeIndex ->setSizeZ( bucket.sizeZ() ); 
  *latticeIndex = 0;

  float an = 1,sum = 0;
  int r, tmp,tmp2, i =0;
  int tempX, tempY, tempZ; 
  std::list< AimsBucketItem<DtiTensor> >::iterator itTmp;
  std::list< AimsBucketItem<DtiTensor> >::iterator itTmp2;
 
  for ( i=0 ,it = bucket[0].begin(); it != bucket[0].end(); i++, it++ )
  {
      if  (it->value().category() <= DtiTensor::NO_PROBLEM )  
      {
        an=1;
	sum =( an * baseLIn( it->value(), (it--) -> value() ) ) + 
             ( an * baseLIn( it->value(), (it++) -> value() ) ) ;

         tmp = i; 
	 tmp2 = i;
         itTmp = it;
         itTmp2 = it;
	 tempX = it->location().item(0);
	 tempY = it->location().item(1);
	 tempZ = it->location().item(2);
	 
         while( itTmp->location().item(1) != tempY-1 && itTmp->location().item(0) != tempX-1 )
         { itTmp --;
	   tmp --; }
	 for( r = 0 ; r < 3 ; r++ )
           {
             if(r==1)
	         an = 1;
             else
	        an = 1/::sqrt(2);
	     sum = sum + an * baseLIn( it->value(), (itTmp) -> value() ) ;
	     itTmp ++;
          }
         while(itTmp2->location().item(1) != tempY-1 && itTmp2->location().item(0) != tempX-1 && 
	 itTmp2->location().item(2) != tempZ-1)
         { itTmp2 --;
	   tmp2 --; }
	 for( r = 0 ; r < 9 ; r++ )
           {
             if(r==4)
	         an = 1;
             else
             {
	       if((r==0) || (r==2) || (r==6) || (r==8))
	         an = 1/::sqrt(3);
	       else
	        an = 1/::sqrt(2);
             }
	     sum = sum + an * baseLIn( it->value(), (itTmp2) -> value() ) ;
	     itTmp2 ++;
          }



         itTmp = it;
         tmp=i;
         itTmp2 = it;
         tmp2=i;
         while(itTmp->location().item(1) != tempY-1 && itTmp->location().item(0) != tempX+1)
         { itTmp ++;
	   tmp ++;}
         for( r = 0 ; r < 3 ; r++ )
         {
	   if(r==1)
	       an = 1;
           else
	       an = 1/::sqrt(2);
	   sum = sum + an * baseLIn( it->value(), (itTmp) -> value() ) ;
	   itTmp ++;
         }
         while(itTmp2->location().item(1) != tempY-1 && itTmp2->location().item(0) != tempX-1 && 
	 itTmp2->location().item(2) != tempZ+1)
         { itTmp2 ++;
	   tmp2 ++; }
	 for( r = 0 ; r < 9 ; r++ )
           {
             if(r==4)
	         an = 1;
             else
             {
	       if(r==0 || r==2 || r==6 || r==8)
	         an = 1/::sqrt(3);
	       else
	        an = 1/::sqrt(2);
             }
	     sum = sum + an * baseLIn( it->value(), (itTmp2) -> value() ) ;
	     itTmp2 ++;
          }
         

         (*latticeIndex)( it->location().item(0),
                          it->location().item(1),
                          it->location().item(2) ) = sum / (4*1+4*1/::sqrt(2));
     }
   }
   
  
  int x, y;
  for(y=0; y<dimY ; y++)
    for(x=0; x<dimX ; x++)
        (*latticeIndex)(x,y,0) = 0;
  for(y=0; y<dimY ; y++)
    for(x=0; x<dimX ; x++)
        (*latticeIndex)(x,y,dimZ-1) = 0;
}

// Fonction permettant de calculer Linear Measure en chaque point du bucket
void Index :: calcul_Measure()
{
  linearMeasure = new AimsData<float>( dimX, dimY,dimZ );
  linearMeasure->setSizeX( bucket.sizeX() );
  linearMeasure->setSizeY( bucket.sizeY() );
  linearMeasure->setSizeZ( bucket.sizeZ() );
  *linearMeasure = 0;

  planarMeasure = new AimsData<float>( dimX, dimY,dimZ );
  planarMeasure->setSizeX( bucket.sizeX() );
  planarMeasure->setSizeY( bucket.sizeY() );
  planarMeasure->setSizeZ( bucket.sizeZ() );
  *planarMeasure = 0;

  sphericalMeasure = new AimsData<float>( dimX, dimY,dimZ );
  sphericalMeasure->setSizeX( bucket.sizeX() );
  sphericalMeasure->setSizeY( bucket.sizeY() );
  sphericalMeasure->setSizeZ( bucket.sizeZ() );
  *sphericalMeasure = 0;
  
  float lambda1, lambda2, lambda3;
  
  for (  it = bucket[0].begin(); it != bucket[0].end(); it++ )
   if ( it->value().category() <= DtiTensor::NO_PROBLEM )
    {
      if(it->value().base().eigenvalue().item(0) > it->value().base().eigenvalue().item(1))
      {
           if(it->value().base().eigenvalue().item(0) > it->value().base().eigenvalue().item(2))
	   {
	     lambda1 = it->value().base().eigenvalue().item(0);
	     if(it->value().base().eigenvalue().item(1)>it->value().base().eigenvalue().item(2))
	     {
	       lambda2 = it->value().base().eigenvalue().item(1);
	       lambda3 = it->value().base().eigenvalue().item(2);
	     }
	     else
	     {
	       lambda2 = it->value().base().eigenvalue().item(2);
	       lambda3 = it->value().base().eigenvalue().item(1);
	     }
	 }
	 else
	 { 
	    lambda1 = it->value().base().eigenvalue().item(2);
	    lambda2 = it->value().base().eigenvalue().item(0);
	    lambda3 = it->value().base().eigenvalue().item(1);
	 }
      }
      else		
      {
        if(it->value().base().eigenvalue().item(1) > it->value().base().eigenvalue().item(2))
	{
	     lambda1 = it->value().base().eigenvalue().item(1);
	     if(it->value().base().eigenvalue().item(0) > it->value().base().eigenvalue().item(2))
	     {
	       lambda2 = it->value().base().eigenvalue().item(0);
	       lambda3 = it->value().base().eigenvalue().item(2);
	     }
	     else
	     {
	       lambda2 = it->value().base().eigenvalue().item(2);
	       lambda3 = it->value().base().eigenvalue().item(0);
	     }
	 }
	 else
	 { 
	    lambda1 = it->value().base().eigenvalue().item(2);
	    lambda2 = it->value().base().eigenvalue().item(1);
	    lambda3 = it->value().base().eigenvalue().item(0);
	 }      
      
      }
      
     (*linearMeasure)( it->location().item(0),
             it->location().item(1),
             it->location().item(2) ) = (lambda1 - lambda2 )/ lambda1;

     (*planarMeasure)( it->location().item(0),
             it->location().item(1),
             it->location().item(2) ) = (lambda2 - lambda3 )/ lambda1;
     
     (*sphericalMeasure)( it->location().item(0),
             it->location().item(1),
             it->location().item(2) ) = lambda3/ lambda1;
   }
}

/******************************************************/
/*           Definition des fonctions amies           */
/******************************************************/

//Fonction amie permettamt de calculer pour un tenseur l'index Relative Anisotropy

inline
float relativeAniso( DtiTensor& thing )
{
  float traceDiv3 = thing.base().meanDiffusivity();

  float sum = ( thing.base().coef().item( 1 ) * thing.base().coef().item( 1 ) +
                thing.base().coef().item( 2 ) * thing.base().coef().item( 2 ) +
                thing.base().coef().item( 4 ) * thing.base().coef().item( 4 ) )
              * 2.0;

  float num = ( thing.base().coef().item( 0 ) - traceDiv3 ) *
              ( thing.base().coef().item( 0 ) - traceDiv3 ) +
              ( thing.base().coef().item( 3 ) - traceDiv3 ) *
              ( thing.base().coef().item( 3 ) - traceDiv3 ) +
              ( thing.base().coef().item( 5 ) - traceDiv3 ) *
              ( thing.base().coef().item( 5 ) - traceDiv3 ) +
              sum;
              
  float den = (thing.base().coef().item( 0 ) + 
               thing.base().coef().item( 3 ) +
	       thing.base().coef().item( 5 ))/::sqrt(3);
  
   return float( ::sqrt(num /den) );
}


//Fonction amie permettamt de calculer l'element de base de Lattice Index
// LIn = rapport entre produit tensoriel anisotropique et le produit tensoriel + rapport
//       entre le produit tensoriel et le produit des normes.

inline
float baseLIn ( DtiTensor& thing1 , DtiTensor& thing2 )
{
  float temp1[3][3];
  float temp2[3][3];

  temp1[0][0]=thing1.base().coef().item(0);
  temp1[0][1]=thing1.base().coef().item(1);
  temp1[1][0]=thing1.base().coef().item(1);
  temp1[0][2]=thing1.base().coef().item(2);
  temp1[2][0]=thing1.base().coef().item(2);
  temp1[1][1]=thing1.base().coef().item(3);
  temp1[1][2]=thing1.base().coef().item(4);
  temp1[2][1]=thing1.base().coef().item(4);
  temp1[2][2]=thing1.base().coef().item(5);

  temp2[0][0]=thing2.base().coef().item(0);
  temp2[0][1]=thing2.base().coef().item(1);
  temp2[1][0]=thing2.base().coef().item(1);
  temp2[0][2]=thing2.base().coef().item(2);
  temp2[2][0]=thing2.base().coef().item(2);
  temp2[1][1]=thing2.base().coef().item(3);
  temp2[1][2]=thing2.base().coef().item(4);
  temp2[2][1]=thing2.base().coef().item(4);
  temp2[2][2]=thing2.base().coef().item(5);

  float prod = 0;
  float norme1 = 0;
  float norme2 = 0;
  
  int i,j;
  
  for(i=0; i<3; i++) 
     for(j=0;j <3; j++)
     {
        prod   += temp1[i][j]*temp2[i][j];
        norme1 += temp1[i][j]*temp1[i][j];
        norme2 += temp2[i][j]*temp2[i][j];
     }
  
  //produit de tenseur isotrope : trace(thing1)*trace(thing2)
  float den = ( thing1.base().coef().item( 0 ) + 
                thing1.base().coef().item( 3 ) + 
                thing1.base().coef().item( 5 )) * 
	      ( thing2.base().coef().item( 0 ) + 
	        thing2.base().coef().item( 3 ) + 
                thing2.base().coef().item( 5 )) ;
  
  //Produit tensoriel anisotropique
  float anisoTensorProduct =  prod - ( den / 3 ) ;
  // Element de base du Lattice Index
  float LIn;
  if(norme1 * norme2 != 0)
    LIn = ( ::sqrt( fabs(anisoTensorProduct) *3 / 8 )  /prod ) + 
      ( 3 * prod )/( 4 * ::sqrt( ::fabs(norme1 * norme2)) );
  else
    LIn = 0;
  
   return LIn;
}
