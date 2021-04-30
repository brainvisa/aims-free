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
#include <aims/data/data_g.h>
#include <aims/io/io_g.h>
#include <aims/roi/maskIterator.h>
#include <aims/getopt/getopt2.h>
#include <aims/math/math_g.h>
#include <aims/estimation/estimation_g.h>
#include <aims/rgb/rgb.h>
#include <iomanip>
#include <cartobase/object/object.h>
#include <aims/io/reader.h>
#include <exception>
#include <algorithm>

using namespace std;
using namespace carto;
using namespace aims;



int main( int argc, const char *argv[] )
{
  int result = EXIT_SUCCESS;

  try {
    Reader< AimsData<short> > filein;
    Reader<  AimsData< AimsVector< float, 6 > > > bmatrix;
    Writer< AimsBucket<DtiTensor> > dtiFile;
    string filemask;
    bool ascii = false, replace = false, 
      uncorrect = false;
    float correct = 0;
    Writer< AimsData<float> > trFile;
    Writer< AimsData<float> > vrFile;
    Writer< AimsData<float> > faFile;
    Writer<AimsData<Point3df> > dirFile;
    Writer<AimsData<AimsRGB> > rgbFile;
    Writer<AimsData<byte> > emFile;
    float gamma = 267475199;
    bool verbose = false;
    float bvalue = -1;

    // Building application
    AimsApplication application( argc, argv,
				 "Estimation of the Tensor Diffusion from a diffusion MR image" );
    application.addOption( filein, "-i", "Diffusion image" );
    // application.addOption( bmatrix, "-b", "b coeficients preprocessed by AimsBValue", true );
    application.addOption( bvalue, "-b", "bvalue used during acquisition (default = read from image)", true );
    application.addOption( dtiFile, "-dti", "DTI tensor output file name (default = not saved)", true );
    application.addOption( filemask, "-m",
			   "Mask of brain or white matter", true );
    application.addOption( trFile, "-tr", "trace output file (default = not saved)", true );
    application.addOption( vrFile, "-vr", "volume ratio output file (default = not saved)", true );
    application.addOption( faFile, "-fa", "fractional anisotropy output file (default = not saved)", true );
    application.addOption( dirFile, "-dir", "directions output file (default = not saved)", true );
    application.addOption( rgbFile, "-rgb", "RGB directions output file (default = not saved)", true );
    application.addOption( emFile, "-em", "Binary mask of bad voxels (default = not saved)", true );
    application.addOption( gamma, "-g",
			   "gamma diffusion coefficient for protons (default=42,57e6 . 2 Pi)", true );
    application.addOption( replace, "-r", "when a point is NOT_POSITIVE_MATRIX, replace by an isotropic tensor which mean diffusivity is estimated on the N26 neighborhood of the point rather than the mean diffusivity equal to trace / 3.0", true );
    application.addOption( correct, "--correct",
			   "correct sites with negative eigen value", true );
   application.addOption( verbose, "-verbose",
			   "show as much information as possible", true );
    application.initialize();
    

    AimsData<short> amp;

    if ( verbose ) cout << "reading diffusion data : " << flush;
    // filein.setMode( MAP_RO );
    filein >> amp;
    if ( verbose ) cout << "done" << endl;
    
    rc_ptr< MaskIterator > maskIterator;
    if ( filemask != "" ) {
      if ( verbose )  cout << "reading mask : " << flush;
      maskIterator = getMaskIterator( filemask );
      if ( verbose ) cout << "done" << endl;
    } else {
      if ( verbose )  cout << "using non null T2 image points as mask" << endl;
      maskIterator = getMaskIterator( amp.volume() );
    }
    
    AimsData< AimsVector< float, 6 > > bcoef;
    if ( bmatrix.fileName() != "" ) {
      if ( verbose ) cout << "reading b-values : " << flush;
      // Problem with writer: ugly workaround
      Reader< AimsData< AimsVector<float,6> > > bReader(  bmatrix.fileName() );
      bReader.read( bcoef );
      //bmatrix >> bcoef;
      if ( verbose ) cout << "done" << endl;
    } else {
      if ( verbose ) cout << "computing b-values :" << endl;
      try {
	const PythonHeader *header = dynamic_cast<PythonHeader *>( amp.header() );
	if ( bvalue < 0 ) {
	  try {
	    bvalue = float( header->getProperty( "bvalue" )
                            ->value< double >() ) *1e6;
	  }
	  catch( exception & ) {
	    cerr << "bvalue is missing in header" << endl;
	    throw;
	  }
	  if ( verbose ) cout << "bvalue = " << bvalue << endl;
	  ObjectVector diffusion_amp = 
	    header->getProperty( "diffusion_amp" )->value< ObjectVector >();
	  bcoef = AimsData< AimsVector< float, 6 > >( diffusion_amp.size() );
	  for( unsigned i = 0; i < diffusion_amp.size(); ++i ) {
	    Point3dd d( diffusion_amp[ i ]->getArrayItem( 0 )->value<double>(),
			diffusion_amp[ i ]->getArrayItem( 1 )->value<double>(),
			diffusion_amp[ i ]->getArrayItem( 2 )->value<double>()
			);
	    if ( ! d.isNull() ) d.normalize();
	    bcoef( i ).item( 0 ) = bvalue * d[0] * d[0];
	    bcoef( i ).item( 1 ) = bvalue * d[0] * d[1];
	    bcoef( i ).item( 2 ) = bvalue * d[0] * d[2];
	    bcoef( i ).item( 3 ) = bvalue * d[1] * d[1];
	    bcoef( i ).item( 4 ) = bvalue * d[1] * d[2];
	    bcoef( i ).item( 5 ) = bvalue * d[2] * d[2];
/**
	    const float gradientX = 
	      diffusion_amp[ i ]->operator []( 0 )->value<double>() / 1e6;
	    const float gradientY = 
	      diffusion_amp[ i ]->operator []( 1 )->value<double>() / 1e6;
	    const float gradientZ = 
	      diffusion_amp[ i ]->operator []( 2 )->value<double>() / 1e6;
	    bcoef( i ).item( 0 ) = sqr( gamma ) * ( te * te * te ) / 4 * 
	      gradientX * gradientX;
	    bcoef( i ).item( 1 ) = sqr( gamma ) * ( te * te * te ) / 4 * 
	      gradientX * gradientY;
	    bcoef( i ).item( 2 ) = sqr( gamma ) * ( te * te * te ) / 4 * 
	      gradientX * gradientZ;
	    bcoef( i ).item( 3 ) = sqr( gamma ) * ( te * te * te ) / 4 * 
	      gradientY * gradientY;
	    bcoef( i ).item( 4 ) = sqr( gamma ) * ( te * te * te ) / 4 * 
	      gradientY * gradientZ;
	    bcoef( i ).item( 5 ) = sqr( gamma ) * ( te * te * te ) / 4 * 
	      gradientZ * gradientZ;
**/	  }
	}
      }
      catch( exception &e ) {
	throw runtime_error( string( e.what() ) + ": Cannot build b-values."
			     " Try to check input image headers some"
			     " information may be missing." );
      }
    }
    if ( verbose ) {
      cout << "b-values:" << endl;
      for( int i = 0; i < bcoef.dimX(); ++i ) {
	cout << "  n° " << i << ": " <<   bcoef( i ).item( 0 ) << ", "
	     <<  bcoef( i ).item( 1 ) << ", "
	     <<  bcoef( i ).item( 2 ) << ", "
	     <<  bcoef( i ).item( 3 ) << ", "
	     <<  bcoef( i ).item( 4 ) << ", "
	     <<  bcoef( i ).item( 5 ) << endl;
      }
    }
  
    AimsBucket< DtiTensor > bucket;
    bucket.setSizeX( amp.sizeX() );
    bucket.setSizeY( amp.sizeY() );
    bucket.setSizeZ( amp.sizeZ() );
    bucket.setSizeT( amp.sizeT() );

    const int dimX = amp.dimX();
    const int dimY = amp.dimY();
    const int dimZ = amp.dimZ();
    const int dimT = amp.dimT();

    if ( verbose ) {
      cout << "dimX = " << dimX << endl;
      cout << "dimY = " << dimY << endl;
      cout << "dimZ = " << dimZ << endl;
      cout << "dimT = " << dimT << endl;
    }

    AimsBucketItem< DtiTensor > item;
    int x, y, z;
    
    int nPointNoProblem = 0;
    int nPointNotPositiveMatrix = 0;
    int nPointBadEigenSystem = 0;
    
    ///AimsData< AimsVector< float, 6 > > bvalue( dimT );
    AimsVector< float, 6 > b0;
    AimsData< float > matrix( 3, 3 );
    AimsData< float > eigenvalue;
    
    // Build matrix and compute SVD
    if ( verbose ) {
      cout << "Build matrix and compute SVD" << endl;
    }
    AimsSVD<float> svd( AimsSVD<float>::VectorOfSingularValues );
    AimsData< float > u( dimT, 6 ), w, v;
    const float coefs[ 6 ] = { -1, -2, -2, -1, -2, -1 };
    for( int n = 0; n < dimT; ++n ) {
      for( int m = 0; m < 6; ++m ) {
        u( n, m ) = coefs[ m ] * bcoef( n )[ m ];
      }
    }
    w = svd.doit( u, &v );
    if ( verbose ) {
      cout << "SVD result" << endl
           << "u: " << u.dimX() << "x" << u.dimY() << endl
           << "w: " << w.dimX() << endl
           << "v: " << v.dimX() << "x" << v.dimY() << endl;
    }

    uint voxelCount = 0;
    for( maskIterator->restart(); maskIterator->isValid();
	 maskIterator->next() ) {
      ++voxelCount;
    }
    const uint showCount = std::max( 1U, voxelCount / 1000 );
    uint count = 0;
    for( maskIterator->restart(); maskIterator->isValid();
	 maskIterator->next() ) {
      const Point3d p = maskIterator->value();
      const short &x = p[0];
      const short &y = p[1];
      const short &z = p[2];
      
      if ( verbose && ( count % showCount == 0 || count == voxelCount-1 )) {
	cout << "\r" << count << "/ " << voxelCount+1 << ": " << p
	     << flush;
      }
      ++count;

      item.location().item( 0 ) = short( x );
      item.location().item( 1 ) = short( y );
      item.location().item( 2 ) = short( z );
      
      AimsData< float > vectorS( dimT );
      if ( amp( x, y, z, 0 ) > 0 ) {
	for ( int d = 0; d < dimT; ++d ) {
	  if ( amp( x, y, z, d ) > 1e-5 ) {
	    vectorS( d ) = log( float( amp( x, y, z, d ) ) / 
				amp( x, y, z, 0 ) );
	  } else {
	    vectorS( d ) = 0;
	  }
	}
      } else {
	vectorS = 0;
      }
      AimsData< float > vectorD = svd.backwardSubstitution( u, w, v, vectorS );
      for ( unsigned i = 0; i < 6; ++i ) {
	item.value().base().coef()[ i ] = vectorD( i ) ;
      }
      
      // estimate trace of tensor
      item.value().base().trace() = item.value().base().coef().item(0) +
	item.value().base().coef().item(3) +
	item.value().base().coef().item(5);
      
      // calculate eigen system
      matrix( 0, 0 ) = item.value().base().coef().item( 0 );
      matrix( 0, 1 ) = item.value().base().coef().item( 1 );
      matrix( 0, 2 ) = item.value().base().coef().item( 2 );
      matrix( 1, 0 ) = item.value().base().coef().item( 1 );
      matrix( 1, 1 ) = item.value().base().coef().item( 3 );
      matrix( 1, 2 ) = item.value().base().coef().item( 4 );
      matrix( 2, 0 ) = item.value().base().coef().item( 2 );
      matrix( 2, 1 ) = item.value().base().coef().item( 4 );
      matrix( 2, 2 ) = item.value().base().coef().item( 5 );

      // cerr << matrix << endl;
      AimsEigen< float > eigen;
      bool eigenOk = true;
      try {
	eigenvalue = undiag( eigen.doit( matrix ) );
      }
      catch( exception &e ) {
	if ( verbose ) cout << ", cannot compute eigen system" << endl;
	eigenOk = false;
      }

      if ( eigenOk ) {
	item.value().base().eigenvalue().item(0) = eigenvalue( 0 );
	item.value().base().eigenvalue().item(1) = eigenvalue( 1 );
	item.value().base().eigenvalue().item(2) = eigenvalue( 2 );
	
	item.value().base().trieder().dirX().item( 0 ) = matrix( 0, 0 );
	item.value().base().trieder().dirX().item( 1 ) = matrix( 1, 0 );
	item.value().base().trieder().dirX().item( 2 ) = matrix( 2, 0 );
	item.value().base().trieder().dirY().item( 0 ) = matrix( 0, 1 );
	item.value().base().trieder().dirY().item( 1 ) = matrix( 1, 1 );
	item.value().base().trieder().dirY().item( 2 ) = matrix( 2, 1 );
	item.value().base().trieder().dirZ().item( 0 ) = matrix( 0, 2 );
	item.value().base().trieder().dirZ().item( 1 ) = matrix( 1, 2 );
	item.value().base().trieder().dirZ().item( 2 ) = matrix( 2, 2 );
	
	// Sort eigen values
	int eigenOrder[ 3 ];
	if ( eigenvalue( 0 ) > eigenvalue( 1 ) ) {
	  if ( eigenvalue( 0 ) > eigenvalue( 2 ) ) {
	    if (  eigenvalue( 1 ) > eigenvalue( 2 ) ) {
	      eigenOrder[ 0 ] = 0;
	      eigenOrder[ 1 ] = 1;
	      eigenOrder[ 2 ] = 2;
	    } else {
	      eigenOrder[ 0 ] = 0;
	      eigenOrder[ 1 ] = 2;
	      eigenOrder[ 2 ] = 1;
	    }
	  } else {
	    eigenOrder[ 0 ] = 2;
	    eigenOrder[ 1 ] = 0;
	    eigenOrder[ 2 ] = 1;
	  }
	} else {
	  if ( eigenvalue( 0 ) > eigenvalue( 2 ) ) {
	    eigenOrder[ 0 ] = 1;
	    eigenOrder[ 1 ] = 0;
	    eigenOrder[ 2 ] = 2;
	  } else {
	    if ( eigenvalue( 1 ) > eigenvalue( 2 ) ) {
	      eigenOrder[ 0 ] = 1;
	      eigenOrder[ 1 ] = 2;
	      eigenOrder[ 2 ] = 0;
	    } else {
	      eigenOrder[ 0 ] = 2;
	      eigenOrder[ 1 ] = 1;
	      eigenOrder[ 2 ] = 0;
	    }
	  }
	}
	
	if ( correct ) {
	  if ( eigenvalue( eigenOrder[ 2 ] ) < 0 ) {
	    eigenvalue( eigenOrder[ 2 ] ) = correct;
	  }
	  if ( eigenvalue( eigenOrder[ 1 ] ) < 0 ) {
	    eigenvalue( eigenOrder[ 1 ] ) = correct;
	  }
	  if ( eigenvalue( eigenOrder[ 0 ] ) < 0 ) {
	    eigenvalue( eigenOrder[ 0 ] ) = correct;
	  }
	}
	
	// look if tensor is positive
	// if not, assign NOT_POSITIVE_MATRIX to category
	if ( eigenvalue( 0 ) <= 0 || 
	     eigenvalue( 1 ) <= 0 ||
	     eigenvalue( 2 ) <= 0   ) {
	  eigenOk = false;
// 	  if ( verbose ) {
// 	    cout << ", Eigen values = " << eigenvalue( eigenOrder[ 0 ] ) << " "
// 		 << eigenvalue( eigenOrder[ 1 ] ) << " "
// 		 << eigenvalue( eigenOrder[ 2 ] ) << endl;
// 	  }
	} else {
	  // look for direction associated with biggest eigenvalue
	  if ( eigenvalue( 0 ) > eigenvalue( 1 ) &&
	       eigenvalue( 0 ) > eigenvalue( 2 )  )
	    item.value().dir() = item.value().base().trieder().dirX();
	  else if ( eigenvalue( 1 ) > eigenvalue( 0 ) &&
		    eigenvalue( 1 ) > eigenvalue( 2 )  )
	    item.value().dir() = item.value().base().trieder().dirY();
	  else if ( eigenvalue( 2 ) > eigenvalue( 0 ) &&
		    eigenvalue( 2 ) > eigenvalue( 1)  )
	    item.value().dir() = item.value().base().trieder().dirZ();
	  
	  // process anisotropy ( volume ratio & fractional anisotropy )
	  item.value().anisotropyVR() = volumeRatio( item.value() );
	  item.value().anisotropyFA() = fractionalAniso( item.value() );
	  
	  // more stuff
	  item.value().index() = -1;
	  item.value().location() = Point3df( 0.0, 0.0, 0.0 );
	  item.value().category() = DtiTensor::NO_PROBLEM;
	  nPointNoProblem++;
	}
	if ( ! eigenOk ) {
	  item.value().location() = Point3df( 0.0, 0.0, 0.0 );
	  item.value().category() = DtiTensor::BAD_EIGENSYSTEM;
	  nPointBadEigenSystem++;
	}
	bucket.push_back( item );
      }
    }

    if ( verbose ) {
      cout << endl;

      cout << "total number of points : " << setw(5) << bucket[0].size() 
	   << endl;
      cout << "no problem             : " << setw(5) << nPointNoProblem  
	   << endl;
      cout << "not positive matrix    : " << setw(5) 
	   << nPointNotPositiveMatrix 
	   << endl;
      cout << "bad eigen system       : " << setw(5) << nPointBadEigenSystem 
	   << endl;
      cout << "building lut : " << flush;
    }
    AimsData< DtiTensor* >* lut =
      new AimsData< DtiTensor* >( dimX, dimY, dimZ );
    *lut = NULL;
    
    list< AimsBucketItem<DtiTensor> >::iterator it;
    for ( it = bucket[0].begin(); it != bucket[0].end(); it++ )
      (*lut)( it->location().item(0),
	      it->location().item(1),
	      it->location().item(2) ) = &it->value();
    if ( verbose ) cout << "done" << endl;


    if ( verbose ) cout << "correcting bad points : " << flush;
    int ox=0,oy=0,oz=0,nNeigh=0;
    float meanDif=0;
    
    int nPointCorrectedN26=0, nPointCorrectedTC=0;
    int nPointToCorrect = nPointNotPositiveMatrix;
    
    int step=0;
    if ( verbose ) cout << setw(3) << step << flush;
    while ( nPointCorrectedN26 + nPointCorrectedTC != nPointToCorrect   ) {
      if ( verbose ) cout << "\b\b\b" << setw(3) << step + 1 << flush;
      for ( z = 0; z < lut->dimZ(); z++ )  
	for ( y = 0; y < lut->dimY(); y++ )  
	  for ( x = 0; x < lut->dimX(); x++ )  
	    if ( (*lut)( x, y, z ) )
	      if ( (*lut)( x, y, z )->category() > DtiTensor::PROBLEM ) { 
		if ( ( (*lut)( x, y, z )->category() == 
		       DtiTensor::NOT_POSITIVE_MATRIX && replace ) ||
		     ( (*lut)( x, y, z )->category() == 
		       DtiTensor::NOT_POSITIVE_MATRIX &&
		       (*lut)( x, y, z )->base().trace() < 0 )  ) {
		  nNeigh = 0;
		  meanDif = 0.0;
		  for ( oz = -1; oz < 2; oz++ )
		    for ( oy = -1; oy < 2; oy++ )
		      for ( ox = -1; ox < 2; ox++ )
			if ( x + ox >= 0  && x + ox < dimX &&
			     y + oy >= 0  && y + oy < dimY &&
			     z + oz >= 0  && z + oz < dimZ &&
			     !( ox == 0 && oy == 0 && oz == 0 ) )
			  if ( (*lut)( x + ox, y + oy, z + oz ) ) {
			    if ( (*lut)( x + ox, y + oy, z + oz )->category()
				 <= DtiTensor::NO_PROBLEM ) {
			      meanDif += 
				(*lut)( x+ox, y+oy, z+oz )->base().trace();
			      nNeigh++;
			    }
			  }
		  if ( nNeigh ) {
		    if ( (*lut)( x, y, z )->category() == 
			 DtiTensor::NOT_POSITIVE_MATRIX )
		      nPointNotPositiveMatrix--;
		    (*lut)( x, y, z )->category() = 
		      DtiTensor::CORRECTED_N26;
		    nPointCorrectedN26++;
		  } else {
		    (*lut)( x, y, z )->base().coef().item( 0 ) = 2e-9;
		    (*lut)( x, y, z )->base().coef().item( 1 ) = 0;
		    (*lut)( x, y, z )->base().coef().item( 2 ) = 0;
		    (*lut)( x, y, z )->base().coef().item( 3 ) = 2e-9;
		    (*lut)( x, y, z )->base().coef().item( 4 ) = 0;
		    (*lut)( x, y, z )->base().coef().item( 5 ) = 2e-9;
		    
		    (*lut)( x, y, z )->base().eigenvalue().item( 0 ) = 
		      2e-9;
		    (*lut)( x, y, z )->base().eigenvalue().item( 1 ) = 
		      2e-9;
		    (*lut)( x, y, z )->base().eigenvalue().item( 2 ) = 
		      2e-9;
		    
		    (*lut)( x, y, z )->anisotropyVR() = 0.0;
		    (*lut)( x, y, z )->anisotropyFA() = 0.0;
		    
		    (*lut)( x, y, z )->base().trace() = 3.0 * 2e-9;
		    
		    (*lut)( x, y, z )->dir() = 
		      Point3df( 2 * UniformRandom() - 1,
				2 * UniformRandom() - 1,
				2 * UniformRandom() - 1 );
		    (*lut)( x, y, z )->dir() /=
		      norm( (*lut)( x, y, z )->dir() );
		    (*lut)( x, y, z )->base().trieder() = 
		      AimsOrthonormalTrieder( (*lut)( x, y, z )->dir() );
		    
		    (*lut)( x, y, z )->index() = -1;
		    nPointToCorrect --;
		    cerr << endl << "warning ! : isolated point "
			 << Point3d( x, y, z ) << endl;
		  }
		} else {
		  nNeigh  = 1;
		  meanDif = (*lut)( x, y, z )->base().trace();
		  (*lut)( x, y, z )->category() = DtiTensor::CORRECTED_TC;
		  nPointNotPositiveMatrix--;
		  nPointCorrectedTC++;
		}
		
		if ( nNeigh ) {
		  // replace by an isotropic tensor with eigen value equal
		  // to the mean diffusivity over the N26 neighborhood
		  meanDif /= 3.0 * float( nNeigh );
		  (*lut)( x, y, z )->base().coef().item( 0 ) = meanDif;
		  (*lut)( x, y, z )->base().coef().item( 1 ) = 0;
		  (*lut)( x, y, z )->base().coef().item( 2 ) = 0;
		  (*lut)( x, y, z )->base().coef().item( 3 ) = meanDif;
		  (*lut)( x, y, z )->base().coef().item( 4 ) = 0;
		  (*lut)( x, y, z )->base().coef().item( 5 ) = meanDif;
		  
		  (*lut)( x, y, z )->base().eigenvalue().item( 0 ) = 
		    meanDif;
		  (*lut)( x, y, z )->base().eigenvalue().item( 1 ) =
		    meanDif;
		  (*lut)( x, y, z )->base().eigenvalue().item( 2 ) =
		    meanDif;
		  
		  (*lut)( x, y, z )->anisotropyVR() = 0.0;
		  (*lut)( x, y, z )->anisotropyFA() = 0.0;
		  
		  if ( !uncorrect )
		    (*lut)( x, y, z )->base().trace() = 3.0 * meanDif;
		  
		  (*lut)( x, y, z )->dir() = 
		    Point3df( 2 * UniformRandom() - 1,
			      2 * UniformRandom() - 1,
			      2 * UniformRandom() - 1 );
		  (*lut)( x, y, z )->dir() /= 
		    norm( (*lut)( x, y, z )->dir() );
		  (*lut)( x, y, z )->base().trieder() = 
		    AimsOrthonormalTrieder( (*lut)( x, y, z )->dir() );
		  
		  (*lut)( x, y, z )->index() = -1;
		}
	      }
      step++;
    }
    if ( verbose ) cout << endl;
    
    
    if ( verbose ) {
      cout << "total number of points : " << setw(5) << bucket[0].size() 
	   << endl;
      cout << "no problem             : " << setw(5) << nPointNoProblem  
	   << endl;
      cout << "not positive matrix    : " << setw(5) 
	   << nPointNotPositiveMatrix << endl;
      cout << "corrected with N26     : " << setw(5) << nPointCorrectedN26 
	   << endl;
      cout << "corrected with TC      : " << setw(5) << nPointCorrectedTC
	   << endl;
    }
    
    delete lut;
    
    if ( dtiFile.fileName() != "" ) {
      if ( verbose ) cout << "saving DTI tensor : " << flush;
      dtiFile.write( bucket, ascii );
      if ( verbose ) cout << "done" << endl;
    }
    
    if ( trFile.fileName() != "" ) {
      if ( verbose ) cout << "saving trace : " << flush;
      AimsData< float >* trace = new AimsData<float>( dimX, dimY, dimZ );
      trace->setSizeX( bucket.sizeX() );
      trace->setSizeY( bucket.sizeY() );
      trace->setSizeZ( bucket.sizeZ() );
      *trace = 0;
      for ( it = bucket[ 0 ].begin(); it != bucket[ 0 ].end(); it++ )
	if ( it->value().category() <= DtiTensor::NO_PROBLEM )
	  (*trace)( it->location() ) = it->value().base().trace();
      trFile << *trace;
      delete trace;
      if ( verbose ) cout << "done" << endl;
    }

    if ( vrFile.fileName() != "" ) {
      if ( verbose ) cout << "saving volume ratio : " << flush;
      AimsData<float>* vr = new AimsData<float>( dimX, dimY, dimZ );
      vr->setSizeX( bucket.sizeX() );
      vr->setSizeY( bucket.sizeY() );
      vr->setSizeZ( bucket.sizeZ() );
      *vr = 0;
      for ( it = bucket[ 0 ].begin(); it != bucket[ 0 ].end(); it++ )
	if ( it->value().category() <= DtiTensor::NO_PROBLEM )
	  (*vr)( it->location() ) = it->value().anisotropyVR();
      vrFile << *vr;
      delete vr;
      if ( verbose ) cout << "done" << endl;
    }

    if ( faFile.fileName() != "" ) {
      if ( verbose ) cout << "saving fractional anisotropy in " 
                          << faFile.fileName() << ": " << flush;
      AimsData<float> fa( dimX, dimY, dimZ );
      fa.setSizeX( bucket.sizeX() );
      fa.setSizeY( bucket.sizeY() );
      fa.setSizeZ( bucket.sizeZ() );
      fa = 0;
      for ( it = bucket[ 0 ].begin(); it != bucket[ 0 ].end(); it++ )
        if ( it->value().category() <= DtiTensor::NO_PROBLEM )
          fa( it->location() ) = it->value().anisotropyFA();
      faFile.write( fa );
      if ( verbose ) cout << "done" << endl;
    }

    if ( emFile.fileName() != "" ) {
      if ( verbose ) cout << "saving error mask : " << flush;
      AimsData<byte> em( dimX, dimY, dimZ );
      em.setSizeX( bucket.sizeX() );
      em.setSizeY( bucket.sizeY() );
      em.setSizeZ( bucket.sizeZ() );
      em = 255;
      for ( it = bucket[ 0 ].begin(); it != bucket[ 0 ].end(); it++ ) {
        if ( it->value().category() >= DtiTensor::PROBLEM ) {
          em( it->location() ) = 255;
        } else {
          em( it->location() ) = 0;
        }
      }
      emFile << em;
      if ( verbose ) cout << "done" << endl;
    }
    if ( dirFile.fileName() != "" ) {
      if ( verbose ) cout << "saving direction : " << flush;
      AimsData<Point3df>* dir = new AimsData<Point3df>( dimX, dimY, dimZ );
      dir->setSizeX( bucket.sizeX() );
      dir->setSizeY( bucket.sizeY() );
      dir->setSizeZ( bucket.sizeZ() );
      *dir = Point3df( 0.0, 0.0, 0.0 );
      for ( it = bucket[ 0 ].begin(); it != bucket[ 0 ].end(); it++ )
	if ( it->value().category() <= DtiTensor::NO_PROBLEM )
	  (*dir)( it->location() ) = it->value().dir();
      dirFile << *dir;
      delete dir;
      if ( verbose ) cout << "done" << endl;
    }

    if ( rgbFile.fileName() != "" ) {
      if ( verbose ) cout << "saving direction in RGB : " << flush;
      AimsData<AimsRGB>* dirRGB = new AimsData<AimsRGB>( dimX, dimY, dimZ );
      dirRGB->setSizeX( bucket.sizeX() );
      dirRGB->setSizeY( bucket.sizeY() );
      dirRGB->setSizeZ( bucket.sizeZ() );
      *dirRGB = AimsRGB( 0, 0, 0 );
      for ( it = bucket[ 0 ].begin(); it != bucket[ 0 ].end(); it++ )
	if ( it->value().category() <= DtiTensor::NO_PROBLEM )
	  {
	    (*dirRGB)( it->location() ).red() = 
	      byte( fabs( it->value().dir().item( 0 ) ) * 255.0 );
	    (*dirRGB)( it->location() ).green() = 
	      byte( fabs( it->value().dir().item( 1 ) ) * 255.0 );
	    (*dirRGB)( it->location() ).blue() = 
	      byte( fabs( it->value().dir().item( 2 ) ) * 255.0 );
	  }
      rgbFile << *dirRGB;
      delete dirRGB;
      if ( verbose ) cout << "done" << endl;
    }
  }
  catch( user_interruption &e ) {
  }
  catch( std::exception &e ) {
    cerr << argv[ 0 ] << ": " << e.what() << endl;
    result = EXIT_FAILURE;
  }
  return result;
}
