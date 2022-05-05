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


#include <aims/data/data.h>
#include <aims/vector/vector.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/getopt/getopt2.h>
#include <aims/bucket/bucket.h>
#include <aims/math/distspheric.h>
#include <aims/math/random.h>

using namespace aims;
using namespace carto;
using namespace std;


//
// nearest
//
inline
short nearest( const Point3df& dir, const AimsData<Point3df>& distrib )
{
  short best=0;
  float scalar=-1,tmp;
  int sampling = distrib.dimX();

  for ( int k = 0; k < sampling; k++ )
    if ( ( tmp = dir.dot( distrib( k ) ) ) > scalar )
    {
      best = k;
      scalar = tmp;
    }
  return best;
}


//
// attachment
//
inline
float attachment( int k,
                  const Point3df& dir,
                  const AimsData<float>& normOfDtiTensor,
                  const AimsData<float>& diffOfDtiTensor,
                  const AimsData< DtiTensor* >& lut1d_pDtiTensor )
{
  float x2 = ( lut1d_pDtiTensor( k )->base().diffusion( dir ) -
               diffOfDtiTensor( k ) ) / normOfDtiTensor( k );
  x2 *= x2;
  return x2;
}


//
// spaghetti
//
float spaghetti( int k,
                 const AimsData< list<short> >& neighbor_lShort,
                 const AimsData< list<int> >& neighbor_lLong,
                 const AimsData< ::byte >& isForwNeigh,
                 const AimsData< ::byte >& isBackNeigh,
                 const AimsData< float >& dir2dir,
                 const AimsData< DtiTensor* >& lut1d_pDtiTensor,
                 AimsData< float >& potForw,
                 AimsData< float >& potBack,
                 int nNeighbor )
{ 
  float forward, backward;
  list<short>::const_iterator ti;
  list<int>::const_iterator ni;
  short dk, dn;

  int countF = 0, countB = 0;

  dk = lut1d_pDtiTensor( k )->index();

  potForw = +1e38;
  potBack = +1e38;

  for ( ti = neighbor_lShort( k ).begin(), ni = neighbor_lLong( k ).begin();
        ti != neighbor_lShort( k ).end(); ti++, ni++ )
  {
    dn = lut1d_pDtiTensor( *ni )->index();
    if ( isForwNeigh( dk, *ti ) )
    {
      forward = dir2dir( dk, dn ); // 0.1;
      //forward = sqr( forward ) / ( 1.0 + sqr( forward ) );
      potForw( countF ) = sqr( forward );
      countF++;
    }
    else if ( isBackNeigh( dk, *ti ) )
    {
      backward = dir2dir( dk, dn ); // 0.1;
      //backward = sqr( backward ) / ( 1.0 + sqr( backward ) );
      potBack( countB ) = sqr( backward );
      countB++;
    }
  }

  incSorting( potForw );
  incSorting( potBack );

  int n, nmaxF, nmaxB;
  if ( nNeighbor == 0 )
  {
    nmaxF = min( countF, 26 );
    nmaxB = min( countB, 26 );
  }
  else
  {
    nmaxF = min( countF, nNeighbor );
    nmaxB = min( countB, nNeighbor );
  }

  float res = 0.0;
  for ( n = 0; n < nmaxF; n++ )
    res += potForw( n );
  for ( n = 0; n < nmaxB; n++ )
    res += potBack( n );
  
  return res;
}



//
// main
//
int main( int argc, const char **argv )
{
  Reader<AimsBucket<DtiTensor> > bucketR;
  Writer<AimsBucket<DtiTensor> > bucketW;

  float alpha  = 1.0, modifAngle = 45.0, neighborAngle = 46.0;
  int sampling = 100, maxIter = 30, nNeighbor=0;
  int x, y, z, k;

  AimsApplication app( argc, argv,
                       "Regularization of the tensor maps with a Spaghetti "
                       "Markov Field" );
  app.addOption( bucketR, "-i", "bucket of dtitensor to be regularized" );
  app.addOption( bucketW, "-o", "result of the regularization" );
  app.addOption( alpha, "-a",
                 "energy balancing [default=1.0]: "
                 "V = V(spaghetti) + alpha x V(attachment)", true );
  app.addOption( modifAngle, "-m",
                 "maximum angle in degrees between the direction of a "
                 "neighbor of a voxel and the spin of that voxel "
                 "[default=45deg]", true );
  app.addOption( neighborAngle, "-n",
                 "maximum angle in degrees beetween 2 spins to be "
                 "considered in the same fiber [default=46deg]", true );
  app.addOption( sampling, "-s",
                 "minimum number of discrete unit vectors uniformly "
                 "distributed on a unit sphere [default=100]", true );
  app.addOption( nNeighbor, "--neighbor",
                 "number of neighbor by semi-conic neighborhood "
                 "[default=all]", true );
  app.addOption( maxIter, "-M",
                 "stop the ICM after a maximum number of iterations "
                 "[default=30]", true );
  app.alias( "--input", "-i" );
  app.alias( "--output", "-o" );
  app.alias( "--alpha", "-a" );
  app.alias( "--modifAngle", "-m" );
  app.alias( "--neighborAngle", "-n" );
  app.alias( "--sampling", "-s" );
  app.alias( "--MaxIter", "-M" );

  try
  {
    app.initialize();

    nNeighbor = 0;

    //
    // Cosinus of the neighbor solid angle, modification angle and beta^2
    //
    float cosNeighborAngle = cos( neighborAngle * M_PI / 180.0 );
    float cosModifAngle    = cos( modifAngle * M_PI / 180.0 );
    if ( cosNeighborAngle < 0.0 ) cosNeighborAngle = 0.0;

    //
    // Read the bucket of DtiTensor
    //
    cout << "reading tensor : " << flush;
    AimsBucket<DtiTensor> bucket;
    bucketR >> bucket;
    int size = bucket[0].size();
    float sizeX = bucket.sizeX();
    float sizeY = bucket.sizeY();
    float sizeZ = bucket.sizeZ();
    cout << "done" << endl;

    //
    // Save the old directions
    //
    cout << "getting old directions : " << flush;
    list< AimsBucketItem<DtiTensor> >::iterator it;
    AimsData< Point3df > oldDir( size );
    for ( it = bucket[0].begin(), k = 0; it != bucket[0].end(); it++, k++ )
      oldDir( k ) = it->value().dir();
    cout << "done" << endl;

    //
    // Distribution of unit vectors on a sphere
    //
    cout << "spheric distribution  : " << flush;
    vector<Point3df*>* vect = new vector<Point3df*>;
    *vect = AimsIcosahedronDiscreteSphericDistribution( sampling );
    sampling = vect->size();
    AimsData<Point3df> distrib( sampling );
    for ( int x = 0; x < sampling; x++ )
    {
      distrib( x ) = *(*vect)[ x ];
      delete (*vect)[ x ];
    }
    delete vect;
    cout << "done" << endl;
    cout << "true spheric sampling : " << sampling << endl << flush;


    //
    // Preprocessing of all dot products
    //
    cout << "preprocessing dot : " << flush;
    AimsData<float> dir2dir( sampling, sampling );
    for ( y = 0; y < sampling; y++ )
      for ( x = 0; x <= y; x++ )
      {
        float dp = distrib( x ).dot( distrib( y ) );
        dp = max( 0.0, 1.0 - sqr( dp ) );
        dir2dir( x, y ) =
  #if defined(__GNUC__) && defined (__sun__)
        dir2dir( y, x ) = fabs( ::asin( sqrt( dp ) ) ) / M_PI_2;
  #else
        dir2dir( y, x ) = fabs( asin( sqrt( dp ) ) ) / M_PI_2;
  #endif
      }

    AimsData<Point3d> neighbor26( 26 );
    AimsData<float> dist2neighbor26( 26 );
    k = 0;
    for ( z = -1; z < 2; z++ )
      for ( y = -1; y < 2; y++ )
        for ( x = -1; x < 2; x++ )
          if ( x!=0 || y!=0 || z!=0 )
          {
            neighbor26( k ) = Point3d( x, y, z );
            dist2neighbor26( k ) = sqrt( sqr( float( x ) * sizeX ) +
                                        sqr( float( y ) * sizeY ) +
                                        sqr( float( z ) * sizeZ ) );
            k++;
          }

    AimsData<float> cosDir2neighbor( sampling, 26 );
    AimsData<::byte>  isForwNeigh( sampling, 26 );
    AimsData<::byte>  isBackNeigh( sampling, 26 );
    Point3df neigh;
    isForwNeigh = false;
    isBackNeigh = false;
    for ( y = 0; y < 26; y++ )
      for ( x = 0; x < sampling; x++ )
      {
        neigh.item( 0 ) = float( neighbor26( y ).item( 0 ) ) * sizeX;
        neigh.item( 1 ) = float( neighbor26( y ).item( 1 ) ) * sizeY;
        neigh.item( 2 ) = float( neighbor26( y ).item( 2 ) ) * sizeZ;
        cosDir2neighbor( x, y ) = distrib(x).dot( neigh ) / dist2neighbor26( y );
        if ( cosDir2neighbor( x, y ) >= cosNeighborAngle )
          isForwNeigh( x, y ) = true;
        else if ( cosDir2neighbor( x, y ) <= -cosNeighborAngle )
          isBackNeigh( x, y ) = true;
      }
    cout << "done" << endl;

    AimsData<float> dir2neighbor( sampling, 26 );
    for ( y = 0; y < 26; y++ )
      for ( x = 0; x < sampling; x++ )
      {
        float dp = cosDir2neighbor( x, y );
        dp = max( 0.0, 1.0 - dp * dp );
  #if defined(__GNUC__) && defined (__sun__)
        dir2neighbor( x, y ) = fabs( ::asin( sqrt( dp ) ) ) / M_PI_2;
  #else
        dir2neighbor( x, y ) = fabs( asin( sqrt( dp ) ) ) / M_PI_2;
  #endif
      }



    //
    // Building L.U.T.
    //
    int dimX=-1, dimY=-1, dimZ=-1;
    for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
    {
      if ( it->location().item(0) > dimX )
        dimX = it->location().item(0);
      if ( it->location().item(1) > dimY )
        dimY = it->location().item(1);
      if ( it->location().item(2) > dimZ )
        dimZ = it->location().item(2);
    }
    dimX++; dimY++; dimZ++;
    cout <<   "dimx = " << dimX
        << "  dimy = " << dimY
        << "  dimz = " << dimZ
        << endl;

    cout << "size = " << size << endl;

    cout << "building luts : " << flush;
    cout << "lut3d_pDtiTensor" << flush;
    AimsData<DtiTensor*> lut3d_pDtiTensor( dimX, dimY, dimZ );
    lut3d_pDtiTensor = NULL;
    for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
      lut3d_pDtiTensor( it->location().item(0),
                        it->location().item(1),
                        it->location().item(2) ) = &it->value();

    cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
        << "lut1d_pDtiTensor" << flush;
    AimsData<DtiTensor*> lut1d_pDtiTensor( size );
    for ( it = bucket[0].begin(), k = 0; it != bucket[0].end(); it++, k++ )
      lut1d_pDtiTensor( k ) = &it->value();

    cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
        << "lut1d_pPoint3d  " << flush;
    AimsData<Point3d*> lut1d_pPoint3d( size );
    for ( it = bucket[0].begin(), k = 0; it != bucket[0].end(); it++, k++ )
      lut1d_pPoint3d( k ) = &it->location();

    cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
        << "lut3d_long      " << flush;
    AimsData< int > lut3d_long( dimX, dimY, dimZ );
    lut3d_long = -1;
    k = 0;
    for ( z = 0; z < dimZ; z++ )
      for ( y = 0; y < dimY; y++ )
        for ( x = 0; x < dimX; x++ )
          if ( lut3d_pDtiTensor( x, y, z ) != NULL )
            lut3d_long( x, y, z ) = k++;

    cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
        << "done            " << endl;


    //
    // Building spheric segment distribution for each discrete direction
    //
    cout << "building spheric segment : " << flush;
    AimsData< list<short> > sphereSegment( sampling );
    short d, e;
    for ( d = 0; d < sampling; d++ )
    {
      for ( e = 0; e < sampling; e++ )
        if ( distrib( d ).dot( distrib( e ) ) >= cosModifAngle && d != e )
          sphereSegment( d ).push_back( e );
    }
    cout << "done" << endl;


    //
    // Building neighbor lists of a point
    //
    cout << "building neighbor lists : " << flush;
    AimsData< list< DtiTensor* > > neighbor_lpDtiTensor( size );
    AimsData< list< short > > neighbor_lShort( size );
    AimsData< list< int > > neighbor_lLong( size );
    AimsData< Point3d >::const_iterator pi;
    DtiTensor* tensor=NULL;
    int nx=0, ny=0, nz=0;
    short type=0;
    k = 0;
    for ( z = 0; z < dimZ; z++ )
      for ( y = 0; y < dimY; y++ )
        for ( x = 0; x < dimX; x++ )
          if ( lut3d_pDtiTensor( x, y, z ) != NULL )
          {
            for ( pi=neighbor26.begin(),type=0; pi!=neighbor26.end(); pi++,type++)
              if ( ( nx = x + pi->item( 0 ) ) >= 0 && nx < dimX &&
                  ( ny = y + pi->item( 1 ) ) >= 0 && ny < dimY &&
                  ( nz = z + pi->item( 2 ) ) >= 0 && nz < dimZ   )
                if ( ( tensor = lut3d_pDtiTensor( nx, ny, nz ) ) != NULL )
                {
                  neighbor_lpDtiTensor( k ).push_back( tensor );
                  neighbor_lShort( k ).push_back( type );
                  neighbor_lLong( k ).push_back( lut3d_long( nx, ny, nz ) );
                }
            k++;
          }
    cout << "done" << endl;

    AimsData<int> histoNeigh(26);
    histoNeigh = 0;
    for (k=0;k<size;k++)
      histoNeigh(neighbor_lpDtiTensor( k ).size())++;
    cout << "number of neighbors histogram : " << histoNeigh << endl;

    //
    // Initialize direction map to closest discrete directions
    //
    cout << "discretizing direction : " << flush;
    for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
      it->value().index() = nearest( it->value().dir(), distrib );
    cout << "done" << endl;


    //
    // Estimate norm of tensors
    //
    cout << "estimating norm of tensors : " << flush;
    AimsData< float > normOfDtiTensor( size );
    for ( k = 0; k < size; k++ )
      normOfDtiTensor( k ) = norm( lut1d_pDtiTensor( k )->base() );
    cout << "done" << endl;


    //
    // Estimate initial diffusivity of tensors
    //
    cout << "estimating initial diffusivity of tensors : " << flush;
    AimsData< float > diffOfDtiTensor( size );
    for ( k = 0; k < size; k++ )
      diffOfDtiTensor( k ) =  lut1d_pDtiTensor( k )->base().diffusion(
                              distrib( lut1d_pDtiTensor( k )->index() ) );
    cout << "done" << endl;



    //
    // Sweeping of the sites
    //
    cout << "sweeping list : " << flush;
    AimsData<int32_t> sweeping = AimsRandomList( size );
    cout << "done" << endl;


    //
    // Iterated Conditional Mode
    //
    AimsData<float> old_Va( size );
    AimsData<float> new_Va( size );
    AimsData<float> old_Vs( size );
    AimsData<float> new_Vs( size );
    float old_Vs_N27 = 0.0;
    float new_Vs_N27 = 0.0;

    float energy_a = 0.0;
    float energy_s = 0.0;

    int nModification = size;
    int permutation = 0;
    int site = 0;
    short old_state = 0;
    short new_state = 0;
    list< short >:: const_iterator si;
    list< int >::const_iterator ni;
    bool isModified = false;


    //
    // Statistics file
    //
    string fileStat = string( bucketW.fileName() ) + ".stat";
    ofstream os( fileStat.c_str() );
    assert( os );
    os << "parameters : " << endl;
    os << "==========" << endl;
    os << "filein            : " << bucketR.fileName() << endl;
    os << "fileOut           : " << bucketW.fileName() << endl;
    os << "sampling          : " << sampling << " directions" << endl;
    if ( nNeighbor )
      os << "number of neigh.  : " << nNeighbor << endl;
    else
      os << "number of neigh.  : 0" << endl;
    os << "alpha             : " << alpha << endl;
    os << "modif. angle      : " << modifAngle << " degrees" << endl;
    os << "neigh. angle      : " << neighborAngle << " degrees" << endl;
    os << "number of points  : " << size << endl;
    os << "dimension X       : " << dimX << " pixels" << endl;
    os << "dimension Y       : " << dimY << " pixels" << endl;
    os << "dimension Z       : " << dimZ << " pixels" << endl;
    os << "resolution X      : " << sizeX << " mm" << endl;
    os << "resolution Y      : " << sizeY << " mm" << endl;
    os << "resolution Z      : " << sizeZ << " mm" << endl;
    os << "maximum iteration : " << maxIter << endl;
    os << "algorithm         : ICM" << endl;
    os << "regularisation : " << endl;
    os << "==============" << endl;

    cout << "nneighbor = " << nNeighbor << endl;

    /////////////// initialiser old_Vs, .....
    cout << "initializing potentials : " << flush;
    AimsData< float > potForw( 26 ), potBack( 26 );
    for ( k = 0; k < size; k++ )
    {
      old_Va( k ) = 0.0;
      old_Vs( k ) = spaghetti( k, neighbor_lShort, neighbor_lLong,
                              isForwNeigh, isBackNeigh,
                              dir2dir, lut1d_pDtiTensor,
                              potForw, potBack, nNeighbor );
      energy_a += old_Va( k );
      energy_s += old_Vs( k );
    }
    cout << "done" << endl;

    int iteration = 0,segment;
    cout << "Iterated Conditional Mode : " << endl;

    cout << setw(6) << 0 << " " << flush;
    cout << setw(6) << 0 << " " << flush;
    cout << setw(6) << 0 << " " << flush;
    cout << setw(10) << alpha * energy_a << " " << flush;
    cout << setw(10) << energy_s << " " << flush;
    cout << setw(10) << alpha * energy_a + energy_s << endl;

    os << setw(6) << 0 << " " << flush;
    os << setw(6) << 0 << " " << flush;
    os << setw(6) << 0 << " " << flush;
    os << setw(10) << alpha * energy_a << " " << flush;
    os << setw(10) << energy_s << " " << flush;
    os << setw(10) << alpha * energy_a + energy_s << endl;

    float energy = alpha * energy_a + energy_s;
    float old_energy = 2 * energy;  // to be sure to enter the loop

    while ( nModification > size / 10000 &&
            fabs( ( energy - old_energy ) / old_energy ) > 0.001 &&
            iteration < maxIter )
    {
      old_energy = energy;

      cout << setw(6) << iteration + 1 << " " << flush;
      os << setw(6) << iteration + 1 << " " << flush;
      nModification = 0;
      permutation = UniformRandom( 0, size - 1 );

      // print current index
      cout << setw(6) << 0 << " " << flush;
      // print modifications
      cout << setw(6) << 0 << " " << flush;
      for ( site = 0; site < size; site++ )
      {
        if ( (site+1)%100 == 0 )
        {
          // print current index
          cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;
          cout << setw(6) << site+1 << " " << flush;
          // print modifications
          cout << setw(6) << nModification << " " << flush;
        }
        k = sweeping( ( site + permutation ) % size );
        x = lut1d_pPoint3d( k )->item( 0 );
        y = lut1d_pPoint3d( k )->item( 1 );
        z = lut1d_pPoint3d( k )->item( 2 );

        segment = old_state = lut1d_pDtiTensor( k )->index();
        isModified = false;

        for ( si = sphereSegment( segment ).begin();
              si != sphereSegment( segment ).end(); si++ )
        {
          new_state = lut1d_pDtiTensor( k )->index() = *si;

          new_Va( k ) = attachment( k, distrib( new_state ),
                                    normOfDtiTensor, diffOfDtiTensor,
                                    lut1d_pDtiTensor );
          new_Vs( k ) = spaghetti( k, neighbor_lShort, neighbor_lLong,
                                  isForwNeigh, isBackNeigh,
                                  dir2dir, lut1d_pDtiTensor,
                                  potForw, potBack, nNeighbor );
          for ( ni = neighbor_lLong( k ).begin();
                ni != neighbor_lLong( k ).end(); ni++ )
            new_Vs( *ni ) = spaghetti( *ni, neighbor_lShort, neighbor_lLong,
                                      isForwNeigh, isBackNeigh,
                                      dir2dir,lut1d_pDtiTensor,
                                      potForw, potBack, nNeighbor );
          old_Vs_N27 = old_Vs( k );
          for ( ni = neighbor_lLong( k ).begin();
                ni != neighbor_lLong( k ).end(); ni++ )
            old_Vs_N27 += old_Vs( *ni );

          new_Vs_N27 = new_Vs( k );
          for ( ni = neighbor_lLong( k ).begin();
                ni != neighbor_lLong( k ).end(); ni++ )
            new_Vs_N27 += new_Vs( *ni );

          if ( alpha * new_Va( k ) + new_Vs_N27 <
              alpha * old_Va( k ) + old_Vs_N27 )
          {
            energy_a += new_Va(k)  - old_Va(k);
            energy_s += new_Vs_N27 - old_Vs_N27;
            old_Va( k ) = new_Va( k );
            old_Vs( k ) = new_Vs( k );
            for ( ni = neighbor_lLong( k ).begin();
                  ni != neighbor_lLong( k ).end(); ni++ )
              old_Vs( *ni ) = new_Vs( *ni );
            old_state = new_state;
            isModified = true;
          }
          else
          {
            lut1d_pDtiTensor( k )->index() = old_state;
          }

        } // end for ( si = ...

        if ( isModified )
          nModification++;

      } // end for ( site = ...

      cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << flush;
      // print current index
      cout << setw(6) << site << " " << flush;
      // print modifications
      cout << setw(6) << nModification << " " << flush;
      cout << setw(10) << alpha * energy_a << " " << flush;
      cout << setw(10) << energy_s << " " << flush;
      cout << setw(10) << alpha * energy_a + energy_s << endl;

      os << setw(6) << site << " " << flush;
      os << setw(6) << nModification << " " << flush;
      os << setw(10) << alpha * energy_a << " " << flush;
      os << setw(10) << energy_s << " " << flush;
      os << setw(10) << alpha * energy_a + energy_s << endl;

      iteration++;
      energy = alpha * energy_a + energy_s;


    } // end while

    cout << endl;
    os.close();

    //
    // Direction map correction
    //
    AimsData<DtiTensor*>::iterator ptensor;
    for ( ptensor = lut1d_pDtiTensor.begin();
          ptensor != lut1d_pDtiTensor.end(); ptensor++ )
      (*ptensor)->dir() = distrib( (*ptensor)->index() );


    //
    // Save corrected direction map
    //
    cout << "saving corrected tensor bucket : " << flush;
    bucketW << bucket;
    cout << "done" << endl;

    //
    // Create image of modifications
    //
    cout << "building modification image : " << flush;
    AimsData< float > modif( dimX, dimY, dimZ );
    modif.setSizeX( sizeX );
    modif.setSizeY( sizeY );
    modif.setSizeZ( sizeZ );
    modif = 0.0;
    for ( k = 0; k< size; k++ )
    {
      float dp = lut1d_pDtiTensor( k )->dir().dot( oldDir( k ) );
      dp = max( 0.0, 1.0 - sqr( dp ) );
  #if defined(__GNUC__) && defined (__sun__)
      modif( *lut1d_pPoint3d( k ) ) = fabs( ::asin( sqrt( dp ) ) ) * 90.0;
  #else
      modif( *lut1d_pPoint3d( k ) ) = fabs( asin( sqrt( dp ) ) ) * 90.0;
  #endif
    }
    cout << "done" << endl;

    //
    // Save image of modifications
    //
    cout << "saving modification image : " << flush;
    Writer<AimsData< float > > dataW( string( bucketW.fileName() ) + "_modif" );
    dataW << modif;
    cout << "done" << endl;

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


// compilation of some Volume classes on Aims types

#include <cartodata/volume/volume_d.h>

template class carto::Volume< list<short> >;
template class carto::Volume< list<int> >;


