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
#include <aims/resampling/resampling_g.h>
#include <aims/transform/least_square_estimation.h>
#include <aims/morphology/morphology_g.h>
#include <aims/io/io_g.h>
#include <aims/getopt/getopt2.h>
#include <iostream>
#include <aims/data/data_g.h>
#include <aims/resampling/motion.h>
#include <aims/distancemap/voronoi.h>
#include <aims/roi/roiutilities.h>
#include <aims/math/distspheric.h>
#include <aims/morphology/morphology_g.h>
#include <aims/distancemap/distancemap_g.h>
#include <graph/graph/graph.h>
#include <graph/tree/tree.h>
#include <aims/io/process.h>
#include <aims/io/reader.h>
#include <aims/io/motionR.h>
#include <aims/io/motionW.h>
#include <aims/io/writer.h>
#include <aims/io/finder.h>
#include <aims/getopt/getopt2.h>
#include <aims/connectivity/connectivity.h>
#include <aims/bucket/bucketMap.h>
#include <iostream>
#include <set>
#include <list>
#include <map>
#include <queue>
#include <cmath>
#include <aims/math/mathelem.h>
#include <cartobase/stream/sstream.h>

using namespace carto ;
using namespace aims;
using namespace std;

int curvIndex( const vector<float>& curvAbs, int origin, float abs ) 
{
  int ind = 0 ;
  if( abs < 0. ){
    while( curvAbs[origin+ind] > abs ){
      --ind ;
    }
    return origin + ind + 1 ;
  }else{
    while( curvAbs[origin+ind] < abs ){
      ++ind ;
    }
    return origin + ind - 1 ;
  }
}



void
saveLinkBuckets( const vector<AimsVector<AimsVector<vector<Point3df>, 2>, 2> >& mainLinesEchant,
		 const vector<AimsVector<AimsVector<vector<float>, 2>, 2> >& areaEchant,
		 int refSubject, const Point3df& /*voxelSize*/ ) 
{
  for(unsigned int i = 0 ; i < mainLinesEchant.size() ; ++i ){
//     if( int (i) != refSubject ){	
      vector<Point3df> ref, other ;
      for(unsigned int l = 0 ; l < 2 ; ++l )
	{
	  BucketMap<Void> linkBetween ;
	  //linkBetween.setSizeXYZT( voxelSize[0],voxelSize[1], voxelSize[2], 1.0) ;
	  
	  for( unsigned int p = 0 ; p < mainLinesEchant[i][l][0].size()
		 && p < mainLinesEchant[refSubject][l][0].size() ; ++p )
	    if( areaEchant[i][l][0][p] > 0. && areaEchant[refSubject][l][0][p] > 0. ){
	      //-------------- TEST -----------------------------
	      for( float alpha = 0. ; alpha <= 1. ; alpha += 0.05 ){
		Point3df point = alpha * mainLinesEchant[refSubject][l][0][p] 
		  + (1 - alpha) * mainLinesEchant[i][l][0][p]  ;
		Point3d pInt( static_cast<int>(point[0] + 0.5), 
			      static_cast<int>(point[1] + 0.5), 
			      static_cast<int>(point[2] + 0.5) ) ;
		linkBetween[p][pInt] ;
	      }
	      //-------------- TEST -----------------------------
	    }
	  for( unsigned int p = 0 ; p < mainLinesEchant[i][l][1].size() 
		 && p < mainLinesEchant[refSubject][l][1].size() ; ++p )
	    if( areaEchant[i][l][1][p] > 0. && areaEchant[refSubject][l][1][p] > 0. ){
	      //-------------- TEST -----------------------------
	      for( float alpha = 0. ; alpha <= 1. ; alpha += 0.05 ){
		Point3df point = alpha * mainLinesEchant[refSubject][l][1][p]
		  + (1 - alpha) * mainLinesEchant[i][l][1][p]  ;
		Point3d pInt( static_cast<int>(point[0] + 0.5), 
			      static_cast<int>(point[1] + 0.5), 
			      static_cast<int>(point[2] + 0.5) ) ;
		linkBetween[p][pInt] ;
	      }
	      //-------------- TEST -----------------------------
	    }
	  
	  if( int(i) != refSubject ){
	    cout << "Size T = " << linkBetween.size() << endl ;
	    Writer< BucketMap<Void> > wriLink( string("linkNoRecal_") + toString(i) + ( l == 0 ? string("_Left.bck") : 
									  string("_Right.bck") ) ) ;
	    wriLink.write( linkBetween ) ;
	  }
	  
	  BucketMap<Void> ml ;
	  for( unsigned int pl = 0 ; pl < linkBetween.size() ; ++pl ){
	    for( unsigned int p = 0 ; p < mainLinesEchant[i][l][0].size() ; ++p ){
	      Point3df point = mainLinesEchant[i][l][0][p]  ;
	      Point3d pInt( static_cast<int>(point[0] + 0.5), 
			    static_cast<int>(point[1] + 0.5), 
			    static_cast<int>(point[2] + 0.5) ) ;
	      //cout << point << "->" << pInt << endl ; 
	      ml[pl][pInt] ;
	    }
	    for( unsigned int p = 0 ; p < mainLinesEchant[i][l][1].size() ; ++p ){
	      Point3df point = mainLinesEchant[i][l][1][p]  ;
	      Point3d pInt( static_cast<int>(point[0] + 0.5), 
			    static_cast<int>(point[1] + 0.5), 
			    static_cast<int>(point[2] + 0.5) ) ;
	      //cout << point << "->" << pInt << endl ; 
	      ml[pl][pInt] ;
	    }
	  }
	  Writer< BucketMap<Void> > wriML( string("ml_") + toString(i) + ( l == 0 ? string("_Left.bck") : 
									string("_Right.bck") ) ) ;
	  wriML.write( ml ) ;
	}
    }
}


Point2d
areaMax( const vector<float>& area, int origin )
{
  int maxInPutInd = origin, maxInCaudInd = origin ;
  
  for( unsigned int i = origin ; i < area.size() ; ++i )
    if( area[i] > area[maxInCaudInd] )
      maxInCaudInd = i ;
  for( int i = origin ; i >= 0 ; --i )
    if( area[i] > area[maxInPutInd] )
      maxInPutInd = i ;
  
  return Point2d( maxInPutInd, maxInCaudInd ) ;
}

vector<Point3df> smoothTangent( const vector<Point3df>& tangents, float sigma)
{
  vector<Point3df> result( tangents.size() ) ;
  for( int n = 0 ; n < int(tangents.size()) ; ++n ){
    int imin = max( n - int(2*sigma+1), 0 ), imax = min( n + int(2*sigma+1), int(tangents.size() - 1) ) ;
    imin = max( imin, 2 * n - imax ) - n ;
    imax = min( imax, 2 * n - imin ) - n ;
    result[n] = Point3df(0., 0., 0.) ;
    for(int i = imin ; i <= imax ; ++i ){
      //      if( n + i < int(tangents.size()) && n + i >= 0 ){
	result[n] += tangents[n + i] * exp( float( -i*i/(sigma*sigma) ) ) ;
	//      }
    }
  }
  for( unsigned int n = 0 ; n < tangents.size() ; ++n ){
    result[n].normalize() ;
    //cout << "Before : " << tangents[n] << "\tand after : " << result[n] << endl ;
  }
  return result ;
}

vector<Point3df> smoothTangent( const vector<Point3df>& tangents, const vector<float>& curvature )
{
//   vector<float> gaussKernel( 5 ) ;
//   for(int i = 0 ; i < 5 ; ++i )
//     gaussKernel[i] = exp( float(-(i-2)*(i-2) ) ) ;

  vector<float> curvR(curvature.size()) ;
  for( unsigned int n = 0 ; n < curvature.size() ; ++n )
    curvR[n] = 1. / curvature[n] ;
  
  vector<float> norm(tangents.size(), 0.) ;
  vector<Point3df> result( tangents.size() ) ;
  for( int n = 0 ; n < int(tangents.size()) ; ++n ){
    result[n] = Point3df(0., 0., 0.) ;
    int imin = max( n - int(2*curvR[n]+1), 0 ), imax = min( n + int(2*curvR[n]+1), int(tangents.size() - 1) ) ;
    imin = max( imin, 2 * n - imax ) - n ;
    imax = min( imax, 2 * n - imin ) - n ;
    for(int i = imin ; i <= imax ; ++i )
      if( n + i < int( tangents.size() ) && n + i >= 0 ){
	result[n] += tangents[n + i] * exp( float( -i*i/(curvR[n]*curvR[n]) ) ) ;
	norm[n] += exp( float( -i*i/(curvR[n]*curvR[n]) ) ) ;
      }
  }
  for( unsigned int n = 0 ; n < tangents.size() ; ++n )
    result[n] = result[n] / norm[n] ;
  
  return result ;
}

vector<AimsVector<vector<Point3df>, 2> >
getTangents( const vector<AimsVector<vector<Point3df>, 2> >& mainLine,
	     const vector< AimsVector<Motion, 2> > & motions ) 
{
  vector<AimsVector<vector<Point3df>, 2> > tangents( mainLine.size() ) ;
  Point3df itTransf ;
  for( unsigned int i = 0 ; i < mainLine.size() ; ++i )
    for( int l = 0 ; l < 2 ; ++l ){
      BucketMap<Void> bck ;
      tangents[i][l]= vector<Point3df>( mainLine[i][l].size() ) ;
      int count = 0 ;
      Point3df oldMainPoint ;
      for( vector<Point3df>::const_iterator it = mainLine[i][l].begin() ; it != mainLine[i][l].end() ; 
	   ++it, ++count ){
	if( it == mainLine[i][l].begin() ){
	  itTransf = motions[i][l].transform(*it) ;
	  tangents[i][l][count] = Point3df(0., -1, 0.) ;
	}else{
	  itTransf = motions[i][l].transform(*it) ;
	  Point3d pToInt( static_cast<int>(itTransf[0]+0.5), 
			  static_cast<int>(itTransf[1]+0.5), 
			  static_cast<int>(itTransf[2]+0.5) ) ;
	  bck[0][pToInt ] ;
	  tangents[i][l][count] = itTransf - oldMainPoint ;
	  tangents[i][l][count].normalize() ;
	}
	oldMainPoint = itTransf ;	
      }
      //vector<AimsVector<vector<Point3df>, 2> > smTangs = smoothTangent( tangents[i][l], 2. ) ;
      tangents[i][l] = smoothTangent( tangents[i][l], 2. ) ;
//   for( unsigned int i = 0 ; i < smoothedTangents.size() ; ++i )
//     cout << tangents[i] << "\t" << smoothedTangents[i] << endl ;
      Writer< BucketMap<Void> > bckWri("line_" + toString(i) + (l ? "_Right.bck" : "_Left.bck") ) ;
      bckWri.write(bck) ;
    }
  return tangents ;
}


vector<AimsVector<vector<Point3df>, 2> >
getTangents( const vector<AimsVector<vector<Point3df>, 2> >& mainLine,
	     const vector< Motion > & motions ) 
{
  vector<AimsVector<vector<Point3df>, 2> > tangents( mainLine.size() ) ;
  Point3df itTransf ;
  for( unsigned int i = 0 ; i < mainLine.size() ; ++i )
    for( int l = 0 ; l < 2 ; ++l ){
      BucketMap<Void> bck ;
      tangents[i][l]= vector<Point3df>( mainLine[i][l].size() ) ;
      int count = 0 ;
      Point3df oldMainPoint ;
      for( vector<Point3df>::const_iterator it = mainLine[i][l].begin() ; it != mainLine[i][l].end() ; 
	   ++it, ++count ){
	if( it == mainLine[i][l].begin() ){
	  itTransf = motions[i].transform(*it) ;
	  tangents[i][l][count] = Point3df(0., -1, 0.) ;
	}else{
	  itTransf = motions[i].transform(*it) ;
	  Point3d pToInt( static_cast<int>(itTransf[0]+0.5), 
			  static_cast<int>(itTransf[1]+0.5), 
			  static_cast<int>(itTransf[2]+0.5) ) ;
	  bck[0][pToInt ] ;
	  tangents[i][l][count] = itTransf - oldMainPoint ;
	  tangents[i][l][count].normalize() ;
	}
	oldMainPoint = itTransf ;	
      }
      //vector<AimsVector<vector<Point3df>, 2> > smTangs = smoothTangent( tangents[i][l], 2. ) ;
      tangents[i][l] = smoothTangent( tangents[i][l], 2. ) ;
//   for( unsigned int i = 0 ; i < smoothedTangents.size() ; ++i )
//     cout << tangents[i] << "\t" << smoothedTangents[i] << endl ;
      Writer< BucketMap<Void> > bckWri("line_" + toString(i) + (l ? "_Right.bck" : "_Left.bck") ) ;
      bckWri.write(bck) ;
    }
  return tangents ;
}

Point3df
vectorInterpolation( const vector<float>& curvAbs, const vector<Point3df>& vectors, 
		     float at, int guess, bool normalize )
{
  if( at == curvAbs[curvAbs.size()-1] )
    return vectors[curvAbs.size()-1] ;
  else if( at == curvAbs[0] )
    return vectors[0] ;
  else if( at > curvAbs[curvAbs.size()-1] || at < curvAbs[0] )
    return Point3df(0., 0., 0.) ;
  bool less = ( curvAbs[guess] < at ) ;
  if( less ){
    for( unsigned int i = guess+1 ; i < curvAbs.size() ; ++i )
      if( curvAbs[i] > at ){
	float weight0 = curvAbs[i] - at ;
	float weight1 = at - curvAbs[i-1] ;
	weight0 /= curvAbs[i] - curvAbs[i-1] ;
	weight1 /= curvAbs[i] - curvAbs[i-1] ;
	
	if( normalize)
	  return weight0 * vectors[i-1] / vectors[i-1].norm() + weight1 * vectors[i] / vectors[i].norm() ;
	else
	  return weight0 * vectors[i-1] + weight1 * vectors[i] ;
      }
  }
  else 
    for( int i = guess-1 ; i >=0 ; --i )
      if( curvAbs[i] < at ){
	float weight0 = curvAbs[i+1] - at ;
	float weight1 = at - curvAbs[i] ;
	weight0 /= curvAbs[i+1] - curvAbs[i] ;
	weight1 /= curvAbs[i+1] - curvAbs[i] ;	

	if( normalize)
	  return weight1 * vectors[i+1] / vectors[i+1].norm() + weight0 * vectors[i] / vectors[i].norm() ;
	else
	  return weight1 * vectors[i+1] + weight0 * vectors[i] ;
      }
  
  return Point3df(0., 0., 0.) ;
}

vector<float>
vectorInterpolation( const vector<float>& curvAbs, const vector< vector<float> >& scalar, float at, 
		     int guess = 0 )
{
  if( at == curvAbs[curvAbs.size()-1] )
    return scalar[curvAbs.size()-1] ;
  else if( at == curvAbs[0] )
    return scalar[0] ;
  else if( at > curvAbs[curvAbs.size()-1] || at < curvAbs[0] )
    return vector<float>(12, 0.) ;
  bool less = ( curvAbs[guess] < at ) ;
  vector<float> interpolatedResult( scalar[0].size() ) ;
  if( less ){
    for( unsigned int i = guess+1 ; i < curvAbs.size() ; ++i )
      if( curvAbs[i] > at ){
	float weight0 = curvAbs[i] - at ;
	float weight1 = at - curvAbs[i-1] ;
	weight0 /= curvAbs[i] - curvAbs[i-1] ;
	weight1 /= curvAbs[i] - curvAbs[i-1] ;
	// 	float tmpi = scalar[i] ;
 	//float tmpiLess1 = scalar[i-1] ;
	for( unsigned int k = 0 ; k < scalar[i-1].size() && k < scalar[i].size() ; ++k )
	  interpolatedResult[k] = weight0 * scalar[i-1][k] + weight1 * scalar[i][k] ;
	
	return interpolatedResult ;
      }
  }
  else 
    for( int i = guess-1 ; i >=0 ; --i )
      if( curvAbs[i] < at ){
	float weight0 = curvAbs[i+1] - at ;
	float weight1 = at - curvAbs[i] ;
 	//float tmpi = scalar[i] ;
 	//float tmpiPlus1 = scalar[i+1] ;	
	weight0 /= curvAbs[i+1] - curvAbs[i] ;
	weight1 /= curvAbs[i+1] - curvAbs[i] ;	
	for( unsigned int k = 0 ; k < scalar[i-1].size() && k < scalar[i].size() ; ++k )
	  interpolatedResult[k] = weight1 * scalar[i+1][k] + weight0 * scalar[i][k] ;
	return interpolatedResult ;
      }
  cerr << "can't find any !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl ;
  return vector<float>(12, 0.) ;
}

float
scalarInterpolation( const vector<float>& curvAbs, const vector<float>& scalar, float at, 
		     int guess = 0 )
{
  if( at == curvAbs[curvAbs.size()-1] )
    return scalar[curvAbs.size()-1] ;
  else if( at == curvAbs[0] )
    return scalar[0] ;
  else if( at > curvAbs[curvAbs.size()-1] || at < curvAbs[0] )
    return 0. ;
  bool less = ( curvAbs[guess] < at ) ;
  if( less ){
    for( unsigned int i = guess+1 ; i < curvAbs.size() ; ++i )
      if( curvAbs[i] > at ){
	float weight0 = curvAbs[i] - at ;
	float weight1 = at - curvAbs[i-1] ;
	weight0 /= curvAbs[i] - curvAbs[i-1] ;
	weight1 /= curvAbs[i] - curvAbs[i-1] ;
	// 	float tmpi = scalar[i] ;
 	//float tmpiLess1 = scalar[i-1] ;	
	float val =  weight0 * scalar[i-1] + weight1 * scalar[i] ;
	if( val > 300. )
	  cout << "curvAbs.size() = " <<  curvAbs.size() << "\tand scalar's = " <<scalar.size() 
	       << "weight0 = "<< weight0 << "\tscalar[i-1] = "<< scalar[i-1]
	       << "\tweight1" << weight1 << "\tscalar[i]" << scalar[i] 
	       << "\tfor i = " << i << " on " << scalar.size() 
	       << "\tat = " << at << " on " <<  curvAbs[curvAbs.size()-1]
	       << endl ;
	return weight0 * scalar[i-1] + weight1 * scalar[i] ;
      }
  }
  else 
    for( int i = guess-1 ; i >=0 ; --i )
      if( curvAbs[i] < at ){
	float weight0 = curvAbs[i+1] - at ;
	float weight1 = at - curvAbs[i] ;
 	//float tmpi = scalar[i] ;
 	//float tmpiPlus1 = scalar[i+1] ;	
	weight0 /= curvAbs[i+1] - curvAbs[i] ;
	weight1 /= curvAbs[i+1] - curvAbs[i] ;	
	return weight1 * scalar[i+1] + weight0 * scalar[i] ;
      }
  cerr << "can't find any !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl ;
  return 0. ;
}

vector<AimsVector<vector<float>, 2> >
normalizeCurvAbsByTangent( const vector<AimsVector<vector<Point3df>, 2> >& tangents,
			   const vector<AimsVector<vector<float>, 2> >& curvAbs, 
			   const vector<AimsVector<int, 2> >& origins,
			   unsigned int refSubject )
{
  vector<AimsVector<vector<float>, 2> > newCurvAbs( curvAbs.size() ) ;
  ofstream os("temptres.txt") ;
  float fractionKept = 0.9 ;
  vector<AimsVector<AimsVector< vector<float>, 2 > , 2> > energy( curvAbs.size() ) ;
  for( unsigned int i = 0 ; i < curvAbs.size() ; ++i ){
    for( int l = 0 ; l < 2 ; ++l ){
      energy[i][l][0] = vector<float>(501, 0.) ;
      energy[i][l][1] = vector<float>(501, 0.) ;
      int cFactor = 0 ;
      float energyCaudMin = 1. ;
      float energyPutMin = 1. ;
      Point2d cFactorMax( 0, 0 ) ;
      Point2df factorMax( 0.5, 0.5 ) ;
      Point2df offsetMax( 0.5, 0.5 ) ;
      
      vector<float> energyByKeptFractionCaud(101) ;
      vector<float> energyByKeptFractionPut(101) ;
      vector<float> factorByKeptFractionCaud(101) ;
      vector<float> factorByKeptFractionPut(101) ;
      vector<float> eMinByKeptFractionCaud(101, 1.) ;
      vector<float> eMinByKeptFractionPut(101, 1.) ;
      for( float factor = 0.3 ; factor <= 3. ; factor += 0.01, ++cFactor ){
	int normalize = 0 ;	  
	float totalEnergyCaud = 0. ;
	float totalEnergyPut = 0. ;
	multiset<float> energies;
	for( int unsigned c = origins[refSubject][0] ; c < curvAbs[refSubject][0].size() ; ++c ){
	  // Pour le sujet de référence, on prend l = 0 pour recaler systématiquement 
	  // sur le striatum gauche 
	  Point3df tang = tangents[refSubject][0][c] ;
	  Point3df interpolatedTangent = vectorInterpolation( curvAbs[i][l], tangents[i][l],
							      curvAbs[refSubject][0][c] * factor, 
							      origins[i][l], true ) ;
	  if( interpolatedTangent != Point3df(0., 0., 0.) ){
	    tang.normalize() ;
	    if( l == 1 )
	      interpolatedTangent[0] *= -1. ;
	    interpolatedTangent.normalize() ;
	    //os << interpolatedTangent - tang  << " : " << 1 - interpolatedTangent.dot( tang ) << endl ;
	    float proj = interpolatedTangent.dot( tang ) ;
	    if( proj > 0. ){
	      energies.insert( 1. - proj ) ;
// 	      energy[i][l][0][cFactor] += 1 - proj ;
	      // 	    energy[i][l][0][cFactor] += (interpolatedTangent - tang).norm() ;
// 	      ++normalize ;
	    }
	  }
	}
	if( !energies.empty() ){
	  int count = 0 ; 
	  for( multiset<float>::iterator it = energies.begin() ; 
	       count < energies.size() * fractionKept ; ++it, ++count )
	    totalEnergyCaud += *it ;
	  totalEnergyCaud /= energies.size() ;
	  if( totalEnergyCaud < energyCaudMin ){
	    energyCaudMin = totalEnergyCaud ;
	    cFactorMax[0] = cFactor ;
	    factorMax[0] = factor ;
	  }
	  // ------- TEST BEGIN ---------
	  for( int percentageKept = 1 ; percentageKept <= 100 ; ++percentageKept ){
	    int count = 0 ; 
	    for( multiset<float>::iterator it = energies.begin() ; 
		 count < energies.size() * percentageKept/100. ; ++it, ++count )
	      energyByKeptFractionCaud[percentageKept] += *it ;
	    energyByKeptFractionCaud[percentageKept] /= energies.size() ;
	    if( energyByKeptFractionCaud[percentageKept] < eMinByKeptFractionCaud[percentageKept] ){
	      eMinByKeptFractionCaud[percentageKept] = energyByKeptFractionCaud[percentageKept] ;
	      factorByKeptFractionCaud[percentageKept] = factor ;
	    }
	  }
	  // ------- TEST END ---------

	}

	energies.clear() ;
	normalize = 0 ;
	for( int c = int(origins[refSubject][0] * 1./4.) ; c <= origins[refSubject][0] ; ++c ){
	  Point3df tang = tangents[refSubject][0][c] ;
	  Point3df interpolatedTangent = vectorInterpolation( curvAbs[i][l], tangents[i][l], 
							      curvAbs[refSubject][0][c] * factor, 
							      origins[i][l], true ) ;
	  if( interpolatedTangent != Point3df(0., 0., 0.) ){
	    tang.normalize() ;
	    if( l == 1 )
	      interpolatedTangent[0] *= -1. ;
	    interpolatedTangent.normalize() ;
	    //os << interpolatedTangent - tang << " : " << 1 - interpolatedTangent.dot( tang ) <<  endl ;
	    float proj = interpolatedTangent.dot( tang ) ;
	    if( proj > 0. )
	      energies.insert( 1. - proj ) ;
	    //energy[i][l][1][cFactor] += 1 - interpolatedTangent.dot( tang ) ;
	    // 	    energy[i][l][1][cFactor] += (interpolatedTangent - tang).norm() ;
	    //++normalize ;
	  }
	}
	if( !energies.empty() ){
	  int count = 0 ; 
	  for( multiset<float>::iterator it = energies.begin() ; 
	       count < energies.size() * fractionKept ; ++it, ++count )
	    totalEnergyPut += *it ;
	  totalEnergyPut /= energies.size() ;
	  if( totalEnergyPut < energyPutMin ){
	    energyPutMin = totalEnergyPut ;
	    cFactorMax[1] = cFactor ;
	    factorMax[1] = factor ;
	  }
	  // ------- TEST BEGIN ---------
	  for( int percentageKept = 1 ; percentageKept <= 100 ; ++percentageKept ){
	    int count = 0 ;
	    for( multiset<float>::iterator it = energies.begin() ; 
		 count < energies.size() * percentageKept/100. ; ++it, ++count )
	      energyByKeptFractionPut[percentageKept] += *it ;
	    energyByKeptFractionPut[percentageKept] /= energies.size() ;
	    if( energyByKeptFractionPut[percentageKept] < eMinByKeptFractionPut[percentageKept] ){
	      eMinByKeptFractionPut[percentageKept] = energyByKeptFractionPut[percentageKept] ;
	      factorByKeptFractionPut[percentageKept] = factor ;
	    }
	  }
	  // ------- TEST END ---------
	}
// 	os << "Factor " << factor 
// 	     << "\tEnergy Caud = " << totalEnergyCaud
// 	     << "\tEnergy Put = " << totalEnergyPut << endl ;
      }
      for( int percentageKept = 1 ; percentageKept <= 100 ; ++percentageKept )
	os << percentageKept/100. 
	   << " % : \t Caudate factor : " << factorByKeptFractionCaud[percentageKept]
	   << "\t Putamen factor : " << factorByKeptFractionPut[percentageKept] << endl << endl ;
	  
      os << "for " << i  << ( l ? " Right " : " Left ") 
	   << " : FactorMax = " <<  factorMax[0] 
	 << " and " << factorMax[1] << endl ;
      newCurvAbs[i][l].reserve( curvAbs[i][l].size() ) ;
      for( int c = 0 ; c < int(curvAbs[i][l].size()) ; ++c ){
	newCurvAbs[i][l].push_back( ( curvAbs[i][l][c] ) 
				    * factorMax[ c < origins[refSubject][l] ] ) ;
	os << "Curv abs = " << curvAbs[i][l][c] << " -> " << newCurvAbs[i][l][c] << endl ;
      }
    }
  }
  os.close() ;
  //----------------------------- JUST CHECKING ------------------------------------

//   for( unsigned int i = 0 ; i < curvAbs.size() ; ++i ){
//     for( int l = 0 ; l < 2 ; ++l ){
//       energy[i][l][0] = vector<float>(501) ;
//       energy[i][l][1] = vector<float>(501) ;
//       int cFactor = 0 ;
//       Point2d cFactorMax( 0, 0 ) ;
//       Point2df factorMax( 0.5, 0.5 ) ;
//       Point2df offsetMax( 0.5, 0.5 ) ;
      
//       int normalize = 0 ;
//       for( int unsigned c = origins[refSubject][0] ; c < curvAbs[refSubject][0].size() ; ++c ){
// 	Point3df tang = tangents[refSubject][0][c] ;
// 	Point3df interpolatedTangent = vectorInterpolation( curvAbs[i][l], tangents[i][l],
// 							    curvAbs[refSubject][l][0], 
// 							    origins[i][l], true ) ;
// 	if( interpolatedTangent != Point3df(0., 0., 0.) ){
// 	  tang.normalize() ;
// 	  if( l == 1 )
// 	    interpolatedTangent[0] *= -1. ;
// 	  interpolatedTangent.normalize() ;
// 	  //cout << interpolatedTangent - tang  << " : " << 1 - interpolatedTangent.dot( tang ) << endl ;
// 	  energy[i][l][0][cFactor] += 1 - interpolatedTangent.dot( tang ) ;
// 	  cout << curvAbs[i][l][c] << "\t" << 
// 	  // 	    energy[i][l][0][cFactor] += (interpolatedTangent - tang).norm() ;
// 	  ++normalize ;
// 	}
//       }
//       if( normalize != 0 ){
// 	energy[i][l][0][cFactor] /= normalize ;
// 	if( energy[i][l][0][cFactor] < energy[i][l][0][cFactorMax[0]] ){
// 	  cFactorMax[0] = cFactor ;
// 	  factorMax[0] = factor ;
// 	}
//       }
      
//       normalize = 0 ;
//       for( int c = origins[refSubject][0] / 2 ; c <= origins[refSubject][0] ; ++c ){
// 	Point3df tang = tangents[refSubject][0][c] ;
// 	Point3df interpolatedTangent = vectorInterpolation( curvAbs[i][l], tangents[i][l], 
// 							    curvAbs[refSubject][l][0] * factor, 
// 							    origins[i][l], true ) ;
// 	if( interpolatedTangent != Point3df(0., 0., 0.) ){
// 	  tang.normalize() ;
// 	  if( l == 1 )
// 	    interpolatedTangent[0] *= -1. ;
// 	  interpolatedTangent.normalize() ;
// 	  //cout << interpolatedTangent - tang << " : " << 1 - interpolatedTangent.dot( tang ) <<  endl ;
	  
// 	  energy[i][l][1][cFactor] += 1 - interpolatedTangent.dot( tang ) ;
// 	  // 	    energy[i][l][1][cFactor] += (interpolatedTangent - tang).norm() ;
// 	  ++normalize ;
// 	}
//       }
//       if( normalize != 0 ){
// 	energy[i][l][1][cFactor] /= normalize ;
// 	if( energy[i][l][1][cFactor] < energy[i][l][1][cFactorMax[1]] ){
// 	  cFactorMax[1] = cFactor ;
// 	  factorMax[1] = factor ;
// 	}
//       }
//       cout << "Factor " << factor 
// 	   << "\tEnergy Caud = " << energy[i][l][0][cFactor] 
// 	   << "\tEnergy Put = " << energy[i][l][1][cFactor] << endl ;
//     }
//     cout << "for " << i  << ( l ? " Right " : " Left ") 
// 	 << " : FactorMax = " <<  factorMax[0] << " energy " << energy[i][l][0][cFactorMax[0]]
// 	 << " and " << factorMax[1] << " energy " << energy[i][l][1][cFactorMax[1]] << endl ;
//     newCurvAbs[i][l].reserve( curvAbs[i][l].size() ) ;
//     for( int c = 0 ; c < int(curvAbs[i][l].size()) ; ++c ){
//       newCurvAbs[i][l].push_back( ( curvAbs[i][l][c] ) 
// 				  / factorMax[ c < origins[refSubject][l] ] ) ;
//       cout << "Curv abs = " << curvAbs[i][l][c] << " -> " << newCurvAbs[i][l][c] << endl ;
//     }
//   }
  //----------------------------- JUST CHECKING END ------------------------------------

  return newCurvAbs ;
}

// vector<AimsVector<vector<float>, 2> >
// normalizeCurvAbsByTangent( const vector<AimsVector<vector<Point3df>, 2> >& tangents,
// 			   const vector<AimsVector<vector<float>, 2> >& curvAbs, 
// 			   const vector<AimsVector<int, 2> >& origins,
// 			   unsigned int refSubject )
// {
//   vector<AimsVector<vector<float>, 2> > newCurvAbs( curvAbs.size() ) ;

//   vector<AimsVector<AimsVector< vector<float>, 2 > , 2> > energy( curvAbs.size() ) ;
//   for( unsigned int i = 0 ; i < curvAbs.size() ; ++i ){
//     for( int l = 0 ; l < 2 ; ++l ){
//       energy[i][l][0] = vector<float>(501) ;
//       energy[i][l][1] = vector<float>(501) ;
//       int cFactor = 0 ;
//       Point2d cFactorMax( 0, 0 ) ;
//       Point2df factorMax( 0.5, 0.5 ) ;
//       Point2df offsetMax( 0.5, 0.5 ) ;
      
//       for( float factor = 0.3 ; factor <= 3. ; factor += 0.01, ++cFactor ){
// 	int normalize = 0 ;
// 	for( int unsigned c = origins[refSubject][1] ; c < curvAbs[refSubject][l].size() ; ++c ){
// 	  Point3df tang = tangents[refSubject][l][c] ;
// 	  Point3df interpolatedTangent = vectorInterpolation( curvAbs[i][l], tangents[i][l],
// 							      curvAbs[refSubject][l][c] * factor, 
// 							      origins[i][l], true ) ;
// 	  if( interpolatedTangent != Point3df(0., 0., 0.) ){
// 	    tang.normalize() ;
// 	    interpolatedTangent.normalize() ;
// 	    //cout << interpolatedTangent - tang  << " : " << 1 - interpolatedTangent.dot( tang ) << endl ;
// 	    energy[i][l][0][cFactor] += 1 - interpolatedTangent.dot( tang ) ;
// 	    // 	    energy[i][l][0][cFactor] += (interpolatedTangent - tang).norm() ;
// 	    ++normalize ;
// 	    }
// 	}
// 	if( normalize != 0 ){
// 	  energy[i][l][0][cFactor] /= normalize ;
// 	  if( energy[i][l][0][cFactor] < energy[i][l][0][cFactorMax[0]] ){
// 	    cFactorMax[0] = cFactor ;
// 	    factorMax[0] = factor ;
// 	  }
// 	}
	
// 	normalize = 0 ;
// 	for( int c = origins[refSubject][l] / 2 ; c <= origins[refSubject][l] ; ++c ){
// 	  Point3df tang = tangents[refSubject][l][c] ;
// 	  Point3df interpolatedTangent = vectorInterpolation( curvAbs[i][l], tangents[i][l], 
// 							      curvAbs[refSubject][l][c] * factor, 
// 							      origins[i][l], true ) ;
// 	  if( interpolatedTangent != Point3df(0., 0., 0.) ){
// 	    tang.normalize() ;
// 	    interpolatedTangent.normalize() ;
// 	    //cout << interpolatedTangent - tang << " : " << 1 - interpolatedTangent.dot( tang ) <<  endl ;
	    
// 	    energy[i][l][1][cFactor] += 1 - interpolatedTangent.dot( tang ) ;
// 	    // 	    energy[i][l][1][cFactor] += (interpolatedTangent - tang).norm() ;
// 	    ++normalize ;
// 	  }
// 	}
// 	if( normalize != 0 ){
// 	  energy[i][l][1][cFactor] /= normalize ;
// 	  if( energy[i][l][1][cFactor] < energy[i][l][1][cFactorMax[1]] ){
// 	    cFactorMax[1] = cFactor ;
// 	    factorMax[1] = factor ;
// 	  }
// 	}
// 	cout << "Factor " << factor 
// 	     << "\tEnergy Caud = " << energy[i][l][0][cFactor] 
// 	     << "\tEnergy Put = " << energy[i][l][1][cFactor] << endl ;
//       }
//       cout << "for " << i  << ( l ? " Right " : " Left ") 
// 	   << " : FactorMax = " <<  factorMax[0] << " energy " << energy[i][l][0][cFactorMax[0]]
// 	   << " and " << factorMax[1] << " energy " << energy[i][l][1][cFactorMax[1]] << endl ;
//       newCurvAbs[i][l].reserve( curvAbs[i][l].size() ) ;
//       for( int c = 0 ; c < int(curvAbs[i][l].size()) ; ++c ){
// 	newCurvAbs[i][l].push_back( ( curvAbs[i][l][c] ) 
// 				    / factorMax[ c < origins[refSubject][l] ] ) ;
// 	cout << "Curv abs = " << curvAbs[i][l][c] << " -> " << newCurvAbs[i][l][c] << endl ;
//       }
//     }
//   }

// return newCurvAbs ;
// }

vector<AimsVector<vector<float>, 2> >
normalizeCurvAbs( const vector<AimsVector<vector<float>, 2> >& areas,
		  const vector<AimsVector<vector<float>, 2> >& curvAbs, 
		  const vector<AimsVector<int, 2> >& origins,
		  unsigned int refSubject )
{
  vector<AimsVector<vector<float>, 2> > newCurvAbs( curvAbs.size() ) ;
  vector<AimsVector<Point2d, 2> > maxPos( areas.size() ) ;

  for( int l = 0 ; l < 2 ; ++l )
    maxPos[refSubject][l] = areaMax( areas[refSubject][l], origins[refSubject][l] ) ;
  
  for( unsigned int i = 0 ; i < areas.size() ; ++i )
    for( int l = 0 ; l < 2 ; ++l ){
      if( i != refSubject ){
	maxPos[i][l] = areaMax( areas[i][l], origins[i][l] ) ;
	
	newCurvAbs[i][l] = vector<float>( curvAbs[i][l].size() ) ;
	
	//cout << maxPos[refSubject][l] << endl << maxPos[i][l] << endl ;

	for( unsigned  int c = origins[i][l] ; c < curvAbs[i][l].size() ; ++c )
	  newCurvAbs[i][l][c] = curvAbs[i][l][c] * float( curvAbs[i][l][maxPos[refSubject][l][1]] ) 
	    / float(curvAbs[i][l][maxPos[i][l][1]]) ;
	for( int c = 0 ; c < origins[i][l] ; ++c )
	  newCurvAbs[i][l][c] = curvAbs[i][l][c] * float( curvAbs[i][l][maxPos[refSubject][l][0]] ) 
	    / float(curvAbs[i][l][maxPos[i][l][0]]) ;
      }else
	newCurvAbs[i][l] = curvAbs[i][l] ;
    }
  return newCurvAbs ;
}

Point2df
getExponentialFitForCaudateTail( const vector<float>& caudAreaEchant )
{
  float sigma = 5. ;
  int border = int( ceil(3 * sigma) ) ;
  vector<float> borderedCaud( caudAreaEchant.size() + 2 * border, 0. ) ;
  for( unsigned int c = border ; c < borderedCaud.size() - border ; ++c )
    borderedCaud[c] = caudAreaEchant[c - border] ;
  
  vector<float> smoothedCaud( borderedCaud.size(), 0. ) ;
  for( unsigned int c = border ; c < borderedCaud.size() - border ; ++c ){
    float normfact = 0. ;
    for(int dc = -border ; dc <= border ; ++dc ){
      smoothedCaud[c] += exp( -float(dc * dc) / sigma * sigma ) * borderedCaud[c + dc] ;
      normfact += exp( -float(dc * dc) / sigma * sigma ) ;
    }
    smoothedCaud[c] /= normfact ;
  }    
  //recherche de points d'inflexion  correpondant à un début d'exp décroissante.
  vector<float> derivate( borderedCaud.size(), 0. ) ;
  vector<float> approxCurvature( borderedCaud.size(), 0. ) ;
  for( unsigned int c = 1 ; c < smoothedCaud.size() - 1 ; ++c ){
    derivate[c] = (smoothedCaud[c+1] - smoothedCaud[c-1]) / 2. ;
    approxCurvature[c] = smoothedCaud[c+1] + smoothedCaud[c-1] - 2. * smoothedCaud[c] ;
  }
  set<int> properIndices ;
  for( unsigned int c = border+2 ; c < smoothedCaud.size() - border ; ++c )
    if( derivate[c] < 0. && (approxCurvature[c-1] > 0. && approxCurvature[c+1] < 0.) )
      properIndices.insert( c ) ;

  if( properIndices.empty() ){
    cerr << "No proper point found" << endl ;
    return Point2df(0., 0.) ;
  }
  if( smoothedCaud.size() - border - *(properIndices.begin()) < 5. ){
    cerr << "Not enough points to estimate exponential" << endl ;
    return Point2df(0., 0.) ;
  }
  
  cout << "Lentgh taken into account for exponential fit" 
       << smoothedCaud.size() - border - *(properIndices.begin()) << endl ;
  
  //Exponential fit
  int inflexionPointIndex = *(properIndices.begin()) - border ;
  float expectedAmplitude = smoothedCaud[border + inflexionPointIndex] ;
  float expectedTau = -(caudAreaEchant.size() - 2/* - inflexionPointIndex*/ ) /
    log( smoothedCaud[border + caudAreaEchant.size() - 2] / expectedAmplitude ) ;
  cout << "expectedAmplitude = " << expectedAmplitude 
       << "\texpectedTau = " << expectedTau << endl ;
  float amplitudeFrom = 0.8 * expectedAmplitude ; 
  float tauFrom = 0.2 * expectedTau ; 
  
  float errorFitMin = 10000000. ;
  float errorFitMinAmplitude = amplitudeFrom ;
  float errorFitMinTau = amplitudeFrom ;
  
  for( float amplitude = amplitudeFrom ; amplitude <= 1.5 * expectedAmplitude ;
       amplitude += 0.2 )
    for( float tau = tauFrom  ; tau <= 8 * expectedTau; tau += 0.01 ){
      float errorFit = 0. ;
      for( unsigned int c = inflexionPointIndex ; c < caudAreaEchant.size() ; ++c ){
	float fitValue = amplitude * exp( -c / tau ) ;
	errorFit += (fitValue - caudAreaEchant[c]) * (fitValue - caudAreaEchant[c]) ;
      }
      if( errorFit < errorFitMin ){
	errorFitMin = errorFit ;
	errorFitMinAmplitude = amplitude ;
	errorFitMinTau = tau ;
      }
    }
  
  cout << " errorFitMinAmplitude = " << errorFitMinAmplitude 
       << "\terrorFitMinTau = " << errorFitMinTau << endl ;
  

  return Point2df( errorFitMinAmplitude, errorFitMinTau ) ;
//   for( int c = 0 ; c < caudAreaEchant.size() ; ++c )
//     if( caudAreaEchant[c] < 1. )
//       caudAreaEchant[c] = errorFitMinAmplitude * exp( -c / tau ) ;
}

vector<AimsVector<AimsVector<vector<float>, 2>, 2> >
correctZeroAreaValue( const vector<AimsVector<AimsVector<vector<float>, 2>, 2> >& areaEchant, 
		      int maxCaudateLentgh )
{
  vector<AimsVector<AimsVector<vector<float>, 2>, 2> > newAreaEchant( areaEchant ) ; 
  for( unsigned int i = 0 ; i < areaEchant.size() ; ++i )
    for( int l = 0 ; l < 2 ; ++l ){
      newAreaEchant[i][l][1] = vector<float>( maxCaudateLentgh, 0. ) ;
      for( unsigned int c = 0 ; c < areaEchant[i][l][1].size() ; ++c )
	newAreaEchant[i][l][1][c] = areaEchant[i][l][1][c] ;
      Point2df fitParam = getExponentialFitForCaudateTail( areaEchant[i][l][1] ) ;
      for( int c = 0 ; c < maxCaudateLentgh ; ++c )
	if( areaEchant[i][l][1][c] < 1. ){
	  int countPos = c ;
	  int countNeg = c ;
	  bool negNotFound = true ;
	  bool posNotFound = true ;
	  while( negNotFound && posNotFound){
	    if( countNeg >= 0 ){
	      --countNeg ;
	      if( areaEchant[i][l][1][countNeg] >= 1. )
		negNotFound = false ;
	    }
	    if( countPos < int(areaEchant[i][l][1].size()) ){
	      ++countPos ;
	      if( areaEchant[i][l][1][countPos] >= 1. )
		posNotFound = false ;
	    }
	  }	  
	  if( (c - countNeg <  countPos - c) || (countPos == int(areaEchant[i][l][1].size())) )
	    newAreaEchant[i][l][1][c] = 
	      fitParam[0] * exp( -c / fitParam[1] ) ;
	  else
	    newAreaEchant[i][l][1][c] = 
	      fitParam[0] * exp( -((countPos+countNeg)/2. - c) / fitParam[1] ) ;
	}
    }
  return newAreaEchant ;
}

void resample( const vector<AimsVector<vector<Point3df>, 2> >& mainLines,
	       const vector<AimsVector<vector<float>, 2> >& curvAbs,
	       const vector<AimsVector<vector<float>, 2> >& curvatures,
	       const vector<AimsVector<vector<float>, 2> >& areas,
	       const vector<AimsVector<vector<float>, 2> >& scales,
	       const vector<AimsVector<vector< vector<float> >, 2> >& invariants,      
	       const vector<AimsVector<int, 2> >& origins,
	       int orig,
	       vector<AimsVector<AimsVector<vector<Point3df>, 2>, 2> >& mainLinesEchant,
	       vector<AimsVector<AimsVector<vector<float>, 2>, 2> >& curvatureEchant,
	       vector<AimsVector<AimsVector<vector<float>, 2>, 2> >& areaEchant,
	       vector<AimsVector<AimsVector<vector<float>, 2>, 2> >& scaleEchant,
	       vector<AimsVector<AimsVector<vector< vector<float> >, 2>, 2> >& invariantEchant )
{
  mainLinesEchant = vector<AimsVector<AimsVector<vector<Point3df>, 2>, 2> >( mainLines.size() ) ;
  areaEchant = vector<AimsVector<AimsVector<vector<float>, 2>, 2> >( mainLines.size() ) ;
  curvatureEchant = vector<AimsVector<AimsVector<vector<float>, 2>, 2> >( mainLines.size() ) ;
  scaleEchant = vector<AimsVector<AimsVector<vector<float>, 2>, 2> >( mainLines.size() ) ;
  invariantEchant = vector<AimsVector<AimsVector<vector<vector<float> >, 2>, 2> >( mainLines.size() ) ;
  
  for(unsigned int i = 0 ; i < curvAbs.size() ; ++i )
    for(unsigned int l = 0 ; l < 2 ; ++l){
      mainLinesEchant[i][l][1].push_back(mainLines[i][l][origins[i][l]+orig]) ;
      areaEchant[i][l][1].push_back( areas[i][l][origins[i][l]+orig] ) ;
      curvatureEchant[i][l][1].push_back( curvatures[i][l][origins[i][l]+orig] ) ;
      scaleEchant[i][l][1].push_back( scales[i][l][origins[i][l]+orig] ) ;
      invariantEchant[i][l][1].push_back( invariants[i][l][origins[i][l]+orig] ) ;
      
      //float initCurvAbs = int( curvAbs[i][l][0] / origins[i][l] ) * origins[i][l] ;

      // ------------------ TEST     ---------------------------------
      
//       vector<AimsVector<vector<float>, 2> > areasNorm2 = areasNorm ;
//       vector<AimsVector<vector<float>, 2> > curvAbs2 = curvAbs ;
      
      
      
      
      // ------------------ TEST END ---------------------------------
      
//       float origArea = areaEchant[i][l][1][0] ;
//       float origArea2 = areas[i][l][origins[i][l]+orig] ;

      for( float ca = -1. ; ca > curvAbs[i][l][0] ; ca -= 1. ){
// 	int index = curvIndex( curvAbs[i][l], origins[i][l]+orig, ca ) ;
// 	float weight = (ca - curvAbs[i][l][index]) 
// 	  / ( curvAbs[i][l][index+(ca > 0.)-(ca < 0.)] - curvAbs[i][l][index] ) ;
	
	
	mainLinesEchant[i][l][0].push_back( vectorInterpolation( curvAbs[i][l], 
								 mainLines[i][l], 
								 ca, origins[i][l], false ) ) ;

// 	float tmp1 = scalarInterpolation( curvAbs[i][l], 
// 					  areas[i][l], 
// 					  ca, origins[i][l] ) ;
	areaEchant[i][l][0].push_back( scalarInterpolation( curvAbs[i][l], 
							    areas[i][l], 
							    ca, origins[i][l] ) ) ;
	curvatureEchant[i][l][0].push_back( scalarInterpolation( curvAbs[i][l], 
								 curvatures[i][l], 
								 ca, origins[i][l] ) ) ;
	scaleEchant[i][l][0].push_back( scalarInterpolation( curvAbs[i][l], 
							     scales[i][l], 
							     ca, origins[i][l] ) ) ;

	invariantEchant[i][l][0].push_back( vectorInterpolation( curvAbs[i][l], 
								 invariants[i][l], 
								 ca, origins[i][l] ) ) ;

// 	mainLinesEchant[i][l][0].push_back( mainLines[i][l][index+(ca > 0.)-(ca < 0.)] * weight
// 					    + mainLines[i][l][index] * (1-weight) ) ;
// 	areaEchant[i][l][0].push_back( areas[i][l][index+(ca > 0.)-(ca < 0.)] * weight
// 				       + areas[i][l][index] * (1-weight) ) ;
// 	curvatureEchant[i][l][0].push_back( curvatures[i][l][index+(ca > 0.)-(ca < 0.)] * weight
// 					    + curvatures[i][l][index] * (1-weight) ) ;
	
	// 	cout << areaEchant[0][0][0].size() << "\t" << areaEchant[1][0][0].size() << "\t" 
	// 	   << areaEchant[0][0][1].size() << "\t" << areaEchant[1][0][1].size() << "\t" 
	// 	   << areaEchant[0][1][0].size() << "\t" << areaEchant[1][1][0].size() << "\t" 
	// 	   << areaEchant[0][1][1].size() << "\t" << areaEchant[1][1][1].size() << endl ; 
      }
      for( float ca = 1. ; ca <= curvAbs[i][l][curvAbs[i][l].size()-1] ; ca += 1. ){
// 	int index = curvIndex( curvAbs[i][l], origins[i][l]+orig, ca ) ;
// 	float weight = (ca - curvAbs[i][l][index]) 
// 	  / ( curvAbs[i][l][index+(ca > 0.)-(ca < 0.)] - curvAbs[i][l][index] ) ;
	
	mainLinesEchant[i][l][1].push_back( vectorInterpolation( curvAbs[i][l], 
								 mainLines[i][l], 
								 ca, origins[i][l], false ) ) ;
// 	float tmp1 = scalarInterpolation( curvAbs[i][l], 
// 					  areas[i][l], 
// 					  ca, origins[i][l] ) ;
	areaEchant[i][l][1].push_back( scalarInterpolation( curvAbs[i][l], 
							    areas[i][l], 
							    ca, origins[i][l] ) ) ;
	curvatureEchant[i][l][1].push_back( scalarInterpolation( curvAbs[i][l], 
								 curvatures[i][l], 
								 ca, origins[i][l] ) ) ;
	scaleEchant[i][l][1].push_back( scalarInterpolation( curvAbs[i][l], 
							     scales[i][l], 
							     ca, origins[i][l] ) ) ;
	invariantEchant[i][l][1].push_back( vectorInterpolation( curvAbs[i][l], 
								 invariants[i][l], 
								 ca, origins[i][l] ) ) ;

// 	mainLinesEchant[i][l][1].push_back( mainLines[i][l][index+(ca > 0.)-(ca < 0.)] * weight
// 					    + mainLines[i][l][index] * (1-weight) ) ;
// 	areaEchant[i][l][1].push_back( areas[i][l][index+(ca > 0.)-(ca < 0.)] * weight
// 				       + areas[i][l][index] * (1-weight) ) ;
// 	curvatureEchant[i][l][1].push_back( curvatures[i][l][index+(ca > 0.)-(ca < 0.)] * weight
// 					    + curvatures[i][l][index] * (1-weight) ) ;
	//cout << areaEchant[0][0][0].size() << "\t" << areaEchant[1][0][0].size() << "\t" 
	//     << areaEchant[0][0][1].size() << "\t" << areaEchant[1][0][1].size() << "\t" 
	//     << areaEchant[0][1][0].size() << "\t" << areaEchant[1][1][0].size() << "\t" 
	//     << areaEchant[0][1][1].size() << "\t" << areaEchant[1][1][1].size() << endl ; 
      }
      
      cout << "sujet "<< i << (l==0? "Left" : "Right") << endl ;
      cout << "AreaBefore" << endl ;
      cout << curvAbs[i][l][origins[i][l] - 3 ] << "\t" << curvAbs[i][l][origins[i][l] - 2 ] 
	   << "\t" << curvAbs[i][l][origins[i][l] - 1 ] << "\t" 
	   << curvAbs[i][l][origins[i][l] ] << "\t" <<curvAbs[i][l][origins[i][l]+1 ] 
	   << "\t" <<curvAbs[i][l][origins[i][l]+2 ] << "\t" <<curvAbs[i][l][origins[i][l]+3 ] 
	   << "\t" <<curvAbs[i][l][origins[i][l]+4 ] << endl ; 
      cout << areas[i][l][origins[i][l]-3] << "\t"<< areas[i][l][origins[i][l]-2] << "\t" << areas[i][l][origins[i][l] - 1 ] << "\t" 
	   << areas[i][l][origins[i][l] ] << "\t" <<areas[i][l][origins[i][l]+1 ] 
	   << "\t" <<areas[i][l][origins[i][l]+2 ] << "\t" <<areas[i][l][origins[i][l]+3 ] << "\t" <<areas[i][l][origins[i][l]+4 ] << endl ; 
      cout << "AreaAfter" << endl ;
      cout << areaEchant[i][l][0][2] << "\t"<< areaEchant[i][l][0][1] << "\t"<< areaEchant[i][l][0][0] << "\t" << areaEchant[i][l][1][0] << "\t"
	   << areaEchant[i][l][1][1] << "\t"<< areaEchant[i][l][1][2] << "\t"<< areaEchant[i][l][1][3]<< "\t"<< areaEchant[i][l][1][4]<< endl ;
    }
}

  
void printAreas( const vector<string>& subjects, 
		 const  vector<AimsVector<AimsVector<vector<float>, 2>, 2> >& areaEchant, 
		 const string& fileout )
{
  unsigned int maxSizeNeg = 0, maxSizePos = 0 ;
  
  for( int unsigned n = 0 ; n < areaEchant.size() ; ++n ){
    if( areaEchant[n][0][0].size() > maxSizeNeg )
      maxSizeNeg = areaEchant[n][0][0].size() ;
    if( areaEchant[n][0][1].size() > maxSizePos )
      maxSizePos = areaEchant[n][0][1].size() ;
    if( areaEchant[n][1][0].size() > maxSizeNeg )
      maxSizeNeg = areaEchant[n][1][0].size() ;
    if( areaEchant[n][1][1].size() > maxSizePos )
      maxSizePos = areaEchant[n][1][1].size() ;
  }
  
  ofstream os(fileout.c_str()) ;
  
  for( unsigned int i = 0 ; i < areaEchant.size() ; ++i )
    for( int l = 0 ; l < 2 ; ++l )
      os << subjects[i] + "_" + ( l == 1 ? "Rght" : "Left" ) << "\t" ;
  os << endl ;

  for( int cu = maxSizeNeg - 1 ; cu >=0 ; --cu ){
    for( unsigned int i = 0 ; i < areaEchant.size()  ; ++i )
      for( unsigned int l = 0 ; l < 2 ; ++l ){
	// 	for( unsigned int pt = 0 ; pt < mainLines[i][l].size() ; ++pt )
	if( i != 0 || l != 0 )
	  os << "\t\t"  ;
	if( cu >= int(areaEchant[i][l][0].size()) )
	  os <<  0. ;
	else
	  os << areaEchant[i][l][0][cu] ;
      }
    os << endl ;
  }
  for( unsigned int cu = 0 ; cu < maxSizePos ; ++cu ){
    for( unsigned int i = 0 ; i < areaEchant.size()  ; ++i )
      for( unsigned int l = 0 ; l < 2 ; ++l ) 
	// 	for( unsigned int pt = 0 ; pt < mainLines[i][l].size() ; ++pt )
	if( cu >= areaEchant[i][l][1].size() )
	  os << "\t" << 0. ;
	else
	  os << "\t" << areaEchant[i][l][1][cu] ;
    os << endl ;
  }
  os.close() ;
}

// void
// registerML(   const vector<AimsVector<vector<Point3df>, 2> >& mainLines,
// 	      const vector<AimsVector<vector<float>, 2> >& curvAbs,
// 	      const vector<AimsVector<vector<float>, 2> >& areasNorm,
// 	      const vector<AimsVector<vector<float>, 2> >& areas,
// 	      const vector<AimsVector<vector<float>, 2> >& curvatures,
// 	      const vector<AimsVector<int, 2> > origins,
// 	      vector<AimsVector< Motion, 2 > >& motionsToLeftRef,
// 	      int refSubject )
// {
//   AimsVector<Motion, 2> lrMotion ;
//   lrMotion[0].setToIdentity() ;
//   lrMotion[1].setToIdentity() ;
//   lrMotion[1].rotation()(0, 0) = -1. ;
  
//   cout << "lr motion = " << lrMotion << endl ;
  
//   vector<AimsVector<AimsVector<vector<Point3df>, 2>, 2> > mainLinesEchant( mainLines.size() ) ;
//   vector<AimsVector<AimsVector<vector<float>, 2>, 2> > areaNormEchant( mainLines.size() ) ;
//   vector<AimsVector<AimsVector<vector<float>, 2>, 2> > areaEchant( mainLines.size() ) ;
//   vector<AimsVector<AimsVector<vector<float>, 2>, 2> > curvatureEchant( mainLines.size() ) ;
//   vector<AimsVector<AimsVector<vector<float>, 2>, 2> > scaleEchant( mainLines.size() ) ;
//   vector<AimsVector<AimsVector<vector< vector<float> >, 2>, 2> > invariantEchant( mainLines.size() ) ;
  
//   resample(  mainLines, curvAbs, curvatures, areas, scales, invariants, origins, 0,
// 	     mainLinesEchant, curvatureEchant,  areaEchant, scaleEchant, invariantEchant ) ;
//   resample(  mainLines, curvAbs, curvatures, areasNorm, scales, invariants, origins, 0,
// 	     mainLinesEchant, curvatureEchant,  areaNormEchant, scaleEchant, invariantEchant ) ;
  
//   motionsToLeftRef = vector<AimsVector< Motion, 2 > >( mainLines.size() ) ;
  
//   for(unsigned int i = 0 ; i < mainLines.size() ; ++i )
//     for( int l = 0 ; l < 2 ; ++l )
//       if( int(i) == refSubject && l == 0 )
// 	motionsToLeftRef[i][l].setToIdentity() ;
//       else{	
// 	vector<Point3df> ref, other ;
	
// 	for( unsigned int p = 0 ; p < mainLinesEchant[i][l][0].size() - 4
// 	       && p < mainLinesEchant[refSubject][0][0].size() - 4 ; ++p )
// 	  if( areaEchant[i][l][0][p] > 0. && areaEchant[refSubject][0][0][p] > 0. ){
// 	    ref.push_back(mainLinesEchant[refSubject][0][0][p]) ;
// 	    other.push_back(lrMotion[l].transform( mainLinesEchant[i][l][0][p] ) ) ;
// 	  }
// 	for( unsigned int p = 0 ; p < mainLinesEchant[i][l][1].size() 
// 	       && p < mainLinesEchant[refSubject][0][1].size() ; ++p )
// 	  if( areaEchant[i][l][1][p] > 0. && areaEchant[refSubject][0][1][p] > 0. ){
// 	    ref.push_back(mainLinesEchant[refSubject][0][1][p]) ;
// 	    other.push_back(lrMotion[l].transform( mainLinesEchant[i][l][1][p] ) ) ;
// 	  }
// 	for(unsigned int n = 0 ; n < 5 ; ++n ){
// 	  ref.push_back(mainLines[refSubject][0][origins[refSubject][0]]) ;
// 	  other.push_back( lrMotion[l].transform( mainLines[i][l][origins[i][l]]) ) ;
// 	}
	
	
// 	AffineLeastSquareEstimation aff( other, ref ) ;
// 	motionsToLeftRef[i][l] = *(aff.motion()) ;

// /****************************************************************************************************************************/
// 	//Partie Robustesse
// 	if(0){
// 	// Le tri des residus pour ref et test  
// 	multimap<float, Point3df> mt;
// 	multimap<float, Point3df> mr;
  
// 	multimap<float, Point3df>::iterator itt;
// 	multimap<float, Point3df>::iterator itr;
  
// 	// Diverses variables utiles
// 	Point3df r;		// point residu courant
// 	float r2;			// norme caree de r
// 	unsigned indice;		// indice de boucle multiple (variable locale boucle impossible)
// 	unsigned sizemoitie = unsigned ( double(other.size())*0.8 );

// 	// La liste des points ref et test ayant les plus petits residus
// 	vector<Point3df> othermoitie;
// 	vector<Point3df> refmoitie;
  
  
// 	for(int boucle=0;boucle<3;boucle++)
// 	  {
// 	    mt.clear();mr.clear();
// 	    othermoitie.clear();refmoitie.clear();

     
//   	    for(unsigned int o=0;o<other.size();o++)  
// 		{
// 			Point3df x = other[o];
// 			Point3df y = ref[o];

// 	  		r = y - motionsToLeftRef[i][l].transform(x);			//calcul du residu pour chaque point
// 	  		r2 = r.norm();						// sa norme carrée
// 	  		mt.insert(pair<float, Point3df>(r2, other[o]));		//insertion du residu et du point test dans map	
// 	  		mr.insert(pair<float, Point3df>(r2, ref[o]));		//insertion du residu et du point test dans map
// 		}

      
// 	      // On remplit les listes avec une partie seulement des meilleurs residus      
// 	      for (itt = mt.begin(),itr = mr.begin(), indice = 0 ;indice<sizemoitie;++itt,++itr,++indice)
// 		{
// 	  		othermoitie.push_back( (*itt).second );
// 	  		refmoitie.push_back( (*itr).second );
// 		}


// 	     // Calcul transfo affine robuste...
//   	     AffineLeastSquareEstimation leastSquareEstimation_moitie(othermoitie, refmoitie);
// 	     motionsToLeftRef[i][l] = *(leastSquareEstimation_moitie.motion());
	     
// 	  }					// fin boucle robustesse		
// /****************************************************************************************************************************/
// 	}
//       }
//   for(unsigned int i = 0 ; i < mainLines.size() ; ++i )
//     motionsToLeftRef[i][1] = motionsToLeftRef[i][1] * lrMotion[1] ;
  
//   float meanDist = 0. ;
//   long nbPts = 0 ;
//   Point2df meanMaxPos ;
//   Point2df varMaxPos ;
//   for(unsigned int i = 0 ; i < mainLines.size() ; ++i )
//     for( int l = 0 ; l < 2 ; ++l ){
//       for( unsigned int p = 0 ; p < mainLinesEchant[i][l][0].size() - 4
// 	     && p < mainLinesEchant[refSubject][0][0].size() - 4 ; ++p )
// 	if( areaEchant[i][l][0][p] > 0. && areaEchant[refSubject][0][0][p] > 0. ){
// 	  meanDist += (mainLinesEchant[refSubject][0][0][p] - motionsToLeftRef[i][l].transform( mainLinesEchant[i][l][0][p] )).norm() ;
// 	  ++nbPts ;
// 	}
	  
//       for( unsigned int p = 0 ; p < mainLinesEchant[i][l][1].size() 
// 	     && p < mainLinesEchant[refSubject][0][1].size() ; ++p )
// 	if( areaEchant[i][l][1][p] > 0. && areaEchant[refSubject][0][1][p] > 0. ){
// 	  meanDist += (mainLinesEchant[refSubject][0][1][p] - motionsToLeftRef[i][l].transform( mainLinesEchant[i][l][1][p] )).norm() ;
// 	  ++nbPts ;
// 	}
//       Point2d areaMaxima = areaMax( areas[i][l], origins[i][l] ) ;
//       cout << "area max " << areaMaxima << Point2df( curvAbs[i][l][areaMaxima[0]],  curvAbs[i][l][areaMaxima[1]] ) <<endl;
//       meanMaxPos += Point2df( curvAbs[i][l][areaMaxima[0]],  curvAbs[i][l][areaMaxima[1]] ) ;
//       varMaxPos += Point2df( curvAbs[i][l][areaMaxima[0]] * curvAbs[i][l][ areaMaxima[0] ],  
// 			     curvAbs[i][l][areaMaxima[1]] * curvAbs[i][l][areaMaxima[1]] ) ;
//     }
//   meanDist /= nbPts ;
//   meanMaxPos /= float(2 * areas.size()) ;
//   varMaxPos = varMaxPos / float(2. * areas.size()) - Point2df(meanMaxPos[0] * meanMaxPos[0], meanMaxPos[1] * meanMaxPos[1] ) ;
  
//   cout << "Mean Dist after registering = " << meanDist << endl ;
//   cout << "Mean area maxima curv abs = " << meanMaxPos << " and var = " << varMaxPos << endl ;
// }

vector<float> scaleCurvilinearAbscissa( const vector<float>& curvAbs, float caudateFactor, float putamenFactor )
{
  vector<float> scaled( curvAbs.size() ) ;
  for( int unsigned p = 0 ; p <  scaled.size() ; ++p )
    if( curvAbs[p] < 0 )
      scaled[p] = curvAbs[p] * putamenFactor ;
    else
      scaled[p] = curvAbs[p] * caudateFactor ;
  return scaled ;
}

vector<AimsVector<vector<float>, 2> >
registerToLeftAndNorm( const vector<AimsVector<vector<Point3df>, 2> >& mainLines,
		      const vector<AimsVector<vector<float>, 2> >& curvAbs,
		      const vector<AimsVector<vector<float>, 2> >& areasNorm,
		      const vector<AimsVector<vector<float>, 2> >& areas,
		       const vector<AimsVector<vector<float>, 2> >& curvatures, 
		       const vector<AimsVector<vector<float>, 2> >& scales, 
		       const vector<AimsVector<vector< vector<float> >, 2> >& invariants,
		      const vector<AimsVector<int, 2> > origins,
		      vector<AimsVector< Motion, 2 > >& motionsToLeftRef,
		      const vector<string>& subjects,
		      int refSubject )
{
  float fractionKept = 0.9 ;
  float minFact = 0.5 ; float maxFact = 2. ; float factStep = 0.01 ;
  int maxFactInd = int((maxFact - minFact) / factStep) ;
  
  motionsToLeftRef = vector<AimsVector< Motion, 2 > >(mainLines.size()) ;
  AimsData<float> tangentsEnergyField( maxFactInd+1, maxFactInd+1, 2, mainLines.size() ) ;
  tangentsEnergyField = 0. ;
  AimsData<float> pointsEnergyField( maxFactInd+1, maxFactInd+1, 2, mainLines.size() ) ;
  pointsEnergyField = 0. ;
  
  int caudFactInd, putFactInd ;
  float caudFact, putFact ;

  AimsVector<Motion, 2> lrMotion ;
  lrMotion[0].setToIdentity() ;
  lrMotion[1].setToIdentity() ;
  lrMotion[1].rotation()(0, 0) = -1. ;
  
  vector<AimsVector<vector<float>, 2> > scaledCurvAbs( mainLines.size() ) ;
  vector<AimsVector<AimsVector<vector<Point3df>, 2>, 2> > mainLinesEchant( mainLines.size() ) ;
  vector<AimsVector<AimsVector<vector<float>, 2>, 2> > areaNormEchant( mainLines.size() ) ;
  vector<AimsVector<AimsVector<vector<float>, 2>, 2> > areaEchant( mainLines.size() ) ;
  vector<AimsVector<AimsVector<vector<float>, 2>, 2> > curvatureEchant( mainLines.size() ) ;
  vector<AimsVector<AimsVector<vector<float>, 2>, 2> > scaleEchant( mainLines.size() ) ;
  vector<AimsVector<AimsVector<vector<vector<float> >, 2>, 2> > invariantEchant( mainLines.size() ) ;

  for( caudFactInd = 0, caudFact = minFact ; caudFactInd < maxFactInd ; 
       ++caudFactInd, caudFact += factStep )
    for( putFactInd = 0, putFact = minFact ; putFactInd < maxFactInd ; 
	 ++putFactInd, putFact += factStep ){
      cout << "Caudate Factor  " << caudFact << "\tPutamen Factor  " << putFact << endl ;
      for(unsigned int i = 0 ; i < mainLines.size() ; ++i )
	for( int l = 0 ; l < 2 ; ++l ){
	  if( int(i) != refSubject || l != 0 )
	    scaledCurvAbs[i][l] = scaleCurvilinearAbscissa( curvAbs[i][l], caudFact, putFact ) ;
	  else
	    scaledCurvAbs[i][l] = curvAbs[i][l] ;
	}
      resample(  mainLines, curvAbs, curvatures, areas, scales, invariants, origins, 0,
		 mainLinesEchant, curvatureEchant,  areaEchant, scaleEchant, invariantEchant ) ;
      resample(  mainLines, curvAbs, curvatures, areasNorm, scales, invariants, origins, 0,
		 mainLinesEchant, curvatureEchant,  areaNormEchant, scaleEchant, invariantEchant ) ;
  
      for( unsigned int i = 0 ; i < mainLines.size() ; ++i )
	for( int l = 0 ; l < 2 ; ++l )
	  if( int(i) == refSubject && l == 0 )
	    motionsToLeftRef[i][l].setToIdentity() ;
	  else{	
	    vector<Point3df> ref, other ;
	    
	    for( unsigned int p = 0 ; p < mainLinesEchant[i][l][0].size()
		   && p < mainLinesEchant[refSubject][0][0].size() ; ++p )
	      if( areaEchant[i][l][0][p] > 0. && areaEchant[refSubject][0][0][p] > 0. ){
		ref.push_back(mainLinesEchant[refSubject][0][0][p]) ;
		if( l == 0 )
		  other.push_back(mainLinesEchant[i][l][0][p] ) ;
		else{
		  Point3df point = mainLinesEchant[i][l][0][p] ;
		  point[0] = -point[0] ;
		  other.push_back(point) ;
		}
	      }
	    for( unsigned int p = 0 ; p < mainLinesEchant[i][l][1].size() 
		   && p < mainLinesEchant[refSubject][0][1].size() ; ++p )
	      if( areaEchant[i][l][1][p] > 0. && areaEchant[refSubject][0][1][p] > 0. ){
		ref.push_back(mainLinesEchant[refSubject][0][1][p]) ;
		if( l == 0 )		
		  other.push_back(mainLinesEchant[i][l][1][p] ) ;
		else{
		  Point3df point = mainLinesEchant[i][l][1][p] ;
		  point[0] = -point[0] ;
		  other.push_back(point) ;
		}
	      }
// 	    for(unsigned int n = 0 ; n < 5 ; ++n ){
// 	      ref.push_back(mainLines[refSubject][0][origins[refSubject][0]]) ;
// 	      other.push_back( mainLines[i][l][origins[i][l]] ) ;
// 	    }
	    
	    AffineLeastSquareEstimation aff( other, ref ) ;
	    motionsToLeftRef[i][l] = *(aff.motion()) ;
	  }
      for(unsigned int i = 0 ; i < mainLines.size() ; ++i )
	motionsToLeftRef[i][1] = motionsToLeftRef[i][1] * lrMotion[1] ;
      
      vector<AimsVector<vector<Point3df>, 2> > tangents = getTangents( mainLines, motionsToLeftRef ) ;
      
      for( unsigned int i = 0 ; i < mainLines.size() ; ++i )
	for( int l = 0 ; l < 2 ; ++l ){
	  multiset<float> energies;
	  for( int unsigned c = origins[refSubject][0] ; c < curvAbs[refSubject][0].size() ; ++c ){
	    // Pour le sujet de référence, on prend l = 0 pour recaler systématiquement 
	    // sur le striatum gauche 
	    Point3df tang = tangents[refSubject][0][c] ;
	    Point3df interpolatedTangent = vectorInterpolation( curvAbs[i][l], tangents[i][l],
								curvAbs[refSubject][0][c] * caudFact, 
								origins[i][l], true ) ;
	    if( interpolatedTangent != Point3df(0., 0., 0.) ){
	      tang.normalize() ;
	      
	      // Note : la tangente a déjà été tranformée dans le repère du striatum gauche du sujet
	      // de référence.
	      
	      interpolatedTangent.normalize() ;
	      float proj = interpolatedTangent.dot( tang ) ;
	      if( proj > 0. )
		energies.insert( 1. - proj ) ;
	    }
	  }
	  for( int c = 4/*int(origins[refSubject][0] * 1./4.)*/ ; c <= origins[refSubject][0] ; ++c ){
	    Point3df tang = tangents[refSubject][0][c] ;
	    Point3df interpolatedTangent = vectorInterpolation( curvAbs[i][l], tangents[i][l], 
								curvAbs[refSubject][0][c] * putFact, 
								origins[i][l], true ) ;
	    if( interpolatedTangent != Point3df(0., 0., 0.) ){
	      tang.normalize() ;
	      interpolatedTangent.normalize() ;
	      float proj = interpolatedTangent.dot( tang ) ;
	      if( proj > 0. )
		energies.insert( 1. - proj ) ;
	    }
	  }
	  if( !energies.empty() ){
	    int count = 0 ; 
	    for( multiset<float>::iterator it = energies.begin() ; 
		 count < energies.size() * fractionKept ; ++it, ++count )
	      tangentsEnergyField( caudFactInd, putFactInd, l, i ) += *it ;
	    tangentsEnergyField( caudFactInd, putFactInd, l, i ) /= energies.size() ;
	  }
	}
      for( unsigned int i = 0 ; i < mainLines.size() ; ++i )
	for( int l = 0 ; l < 2 ; ++l ){
	  multiset<float> energies;
	  for( int unsigned c = 0 ; c < curvAbs[refSubject][0].size() ; ++c ){
	    Point3df p = vectorInterpolation( scaledCurvAbs[i][l], mainLines[i][l], 
					      curvAbs[refSubject][0][c], 
					      origins[i][l], false ) ;
	    if( p != Point3df(0., 0., 0.) ){
	      p = motionsToLeftRef[i][l].transform(p) ;
	      energies.insert((p - mainLines[refSubject][0][c]).norm() ) ;
	    }
	  }
	  if( !energies.empty() ){
	    int count = 0 ; 
	    for( multiset<float>::iterator it = energies.begin() ; 
		 count < energies.size() * fractionKept ; ++it, ++count )
	      pointsEnergyField( caudFactInd, putFactInd, l, i ) += *it ;
	    pointsEnergyField( caudFactInd, putFactInd, l, i ) /= energies.size() ;
	  }
	}
    }
  for( unsigned int i = 0 ; i < mainLines.size() ; ++i )
    for(int l = 0 ; l < 2 ; ++l ){
      int minTangCaudEnIndex = 0 ;
      int minTangPutEnIndex = 0 ;
      int minPtsCaudEnIndex = 0 ;
      int minPtsPutEnIndex = 0 ;
      
      for( int x = 0; x < maxFactInd ; ++x )
	for( int y = 0; y < maxFactInd ; ++y ){
	  if( tangentsEnergyField(x, y, l, i) < tangentsEnergyField(minTangCaudEnIndex, minTangPutEnIndex, l, i) ){
	    minTangCaudEnIndex = x ;
	    minTangPutEnIndex = y ;
	  }
	  if( pointsEnergyField(x, y, l, i) < pointsEnergyField(minPtsCaudEnIndex, minPtsPutEnIndex, l, i) ){
	    minPtsCaudEnIndex = x ;
	    minPtsPutEnIndex = y ;
	  }
	}
      float caudateBestFactor = minFact + minTangCaudEnIndex * factStep ;
      float putamenBestFactor = minFact + minTangCaudEnIndex * factStep ;

      cout << "Subject " << i << " " << (l == 0 ? "Left" : "Right" ) 
	   << " factorCaudTang = " <<  minFact + minTangCaudEnIndex * factStep
	   << " factorPutTang = " <<  minFact + minTangPutEnIndex * factStep
	   << " factorCaudPts = " << minFact + minPtsCaudEnIndex * factStep
	   << " factorPutPts = " <<  minFact + minPtsPutEnIndex * factStep
	   << endl ;
      
      
      if( int(i) != refSubject || l != 0 )
	scaledCurvAbs[i][l] = scaleCurvilinearAbscissa( curvAbs[i][l], caudateBestFactor, putamenBestFactor ) ;
      else
	scaledCurvAbs[i][l] = curvAbs[i][l] ;
      resample(  mainLines, curvAbs, curvatures, areas, scales, invariants, origins, 0,
		 mainLinesEchant, curvatureEchant,  areaEchant, scaleEchant, invariantEchant ) ;
      resample(  mainLines, curvAbs, curvatures, areasNorm, scales, invariants, origins, 0,
	     mainLinesEchant, curvatureEchant,  areaNormEchant, scaleEchant, invariantEchant ) ;
  
      for( unsigned int i = 0 ; i < mainLines.size() ; ++i )
	for( int l = 0 ; l < 2 ; ++l )
	  if( int(i) == refSubject && l == 0 )
	    motionsToLeftRef[i][l].setToIdentity() ;
	  else{	
	    vector<Point3df> ref, other ;
	    
	    for( unsigned int p = 0 ; p < mainLinesEchant[i][l][0].size()
		   && p < mainLinesEchant[refSubject][0][0].size() ; ++p )
	      if( areaEchant[i][l][0][p] > 0. && areaEchant[refSubject][0][0][p] > 0. ){
		ref.push_back(mainLinesEchant[refSubject][0][0][p]) ;
		if( l == 0 )
		  other.push_back(mainLinesEchant[i][l][0][p] ) ;
		else{
		  Point3df point = mainLinesEchant[i][l][0][p] ;
		  point[0] = -point[0] ;
		  other.push_back(point) ;
		}
	      }
	    for( unsigned int p = 0 ; p < mainLinesEchant[i][l][1].size() 
		   && p < mainLinesEchant[refSubject][0][1].size() ; ++p )
	      if( areaEchant[i][l][1][p] > 0. && areaEchant[refSubject][0][1][p] > 0. ){
		ref.push_back(mainLinesEchant[refSubject][0][1][p]) ;
		if( l == 0 )		
		  other.push_back(mainLinesEchant[i][l][1][p] ) ;
		else{
		  Point3df point = mainLinesEchant[i][l][1][p] ;
		  point[0] = -point[0] ;
		  other.push_back(point) ;
		}
	      }
// 	    for(unsigned int n = 0 ; n < 5 ; ++n ){
// 	      ref.push_back(mainLines[refSubject][0][origins[refSubject][0]]) ;
// 	      other.push_back( mainLines[i][l][origins[i][l]] ) ;
// 	    }
	    
	    AffineLeastSquareEstimation aff( other, ref ) ;
	    motionsToLeftRef[i][l] = *(aff.motion()) ;
	  }
      for(unsigned int i = 0 ; i < mainLines.size() ; ++i )
	motionsToLeftRef[i][1] = motionsToLeftRef[i][1] * lrMotion[1] ;
    }
  
  printAreas( subjects, areaEchant, "areasByRecalAndNorm.txt" ) ;

  Writer< AimsData<float> > wriTangentEnergyField( "tangentsEnergyField.ima") ;
  wriTangentEnergyField.write(tangentsEnergyField) ;
  
  Writer< AimsData<float> > wriPointEnergyField( "pointsEnergyField.ima") ;
  wriPointEnergyField.write(pointsEnergyField) ;

  return scaledCurvAbs ;
}


vector<AimsVector<vector<float>, 2> >
registerAndNorm( const vector<AimsVector<vector<Point3df>, 2> >& mainLines,
		 const vector<AimsVector<vector<float>, 2> >& curvAbs,
		 const vector<AimsVector<vector<float>, 2> >& areasNorm,
		 const vector<AimsVector<vector<float>, 2> >& areas,
		 const vector<AimsVector<vector<float>, 2> >& curvatures, 
		 const vector<AimsVector<vector<float>, 2> >& scales, 
		 const vector<AimsVector<vector<vector<float> >, 2> >& invariants,
		 const vector<AimsVector<int, 2> > origins,
		 vector<Motion >& motionsToRef,
		 const vector<string>& subjects,
		 int refSubject )
{
  float fractionKept = 0.9 ;
  float minFact = 0.5 ; float maxFact = 2. ; float factStep = 0.01 ;
  int maxFactInd = int((maxFact - minFact) / factStep) ;
  
  motionsToRef = vector<Motion>(mainLines.size()) ;
  AimsData<float> tangentsEnergyField( maxFactInd+1, maxFactInd+1, 2, mainLines.size() ) ;
  tangentsEnergyField = 0. ;
  AimsData<float> pointsEnergyField( maxFactInd+1, maxFactInd+1, 2, mainLines.size() ) ;
  pointsEnergyField = 0. ;
  
  int caudFactInd, putFactInd ;
  float caudFact, putFact ;

  AimsVector<Motion, 2> lrMotion ;
  lrMotion[0].setToIdentity() ;
  lrMotion[1].setToIdentity() ;
  lrMotion[1].rotation()(0, 0) = -1. ;
  
  vector<AimsVector<vector<float>, 2> > scaledCurvAbs( mainLines.size() ) ;
  vector<AimsVector<AimsVector<vector<Point3df>, 2>, 2> > mainLinesEchant( mainLines.size() ) ;
  vector<AimsVector<AimsVector<vector<float>, 2>, 2> > areaEchant( mainLines.size() ) ;
  vector<AimsVector<AimsVector<vector<float>, 2>, 2> > areaNormEchant( mainLines.size() ) ;
  vector<AimsVector<AimsVector<vector<float>, 2>, 2> > curvatureEchant( mainLines.size() ) ;
  vector<AimsVector<AimsVector<vector<float>, 2>, 2> > scaleEchant( mainLines.size() ) ;
  vector<AimsVector<AimsVector<vector<vector<float> >, 2>, 2> > invariantEchant( mainLines.size() ) ;

  for( caudFactInd = 0, caudFact = minFact ; caudFactInd < maxFactInd ; 
       ++caudFactInd, caudFact += factStep )
    for( putFactInd = 0, putFact = minFact ; putFactInd < maxFactInd ; 
	 ++putFactInd, putFact += factStep ){
      cout << "Caudate Factor  " << caudFact << "\tPutamen Factor  " << putFact << endl ;
      for(unsigned int i = 0 ; i < mainLines.size() ; ++i )
	for( int l = 0 ; l < 2 ; ++l ){
	  if( int(i) != refSubject || l != 0 )
	    scaledCurvAbs[i][l] = scaleCurvilinearAbscissa( curvAbs[i][l], caudFact, putFact ) ;
	  else
	    scaledCurvAbs[i][l] = curvAbs[i][l] ;
	}
      resample(  mainLines, curvAbs, curvatures, areas, scales, invariants, origins, 0,
		 mainLinesEchant, curvatureEchant,  areaEchant, scaleEchant, invariantEchant ) ;
      resample(  mainLines, curvAbs, curvatures, areasNorm, scales, invariants, origins, 0,
		 mainLinesEchant, curvatureEchant,  areaNormEchant, scaleEchant, invariantEchant ) ;
      for( unsigned int i = 0 ; i < mainLines.size() ; ++i ){
	vector<Point3df> ref, other ;
	if( int(i) == refSubject )
	  motionsToRef[i].setToIdentity() ;
	else{	
	  for( int l = 0 ; l < 2 ; ++l ){
	    for( unsigned int p = 0 ; p < mainLinesEchant[i][l][0].size()
		   && p < mainLinesEchant[refSubject][0][0].size() ; ++p )
	      if( areaEchant[i][l][0][p] > 0. && areaEchant[refSubject][0][0][p] > 0. ){
		ref.push_back(mainLinesEchant[refSubject][0][0][p]) ;
		other.push_back(mainLinesEchant[i][l][0][p] ) ;
	      }
	    for( unsigned int p = 0 ; p < mainLinesEchant[i][l][1].size() 
		   && p < mainLinesEchant[refSubject][0][1].size() ; ++p )
	      if( areaEchant[i][l][1][p] > 0. && areaEchant[refSubject][0][1][p] > 0. ){
		ref.push_back(mainLinesEchant[refSubject][0][1][p]) ;
		other.push_back(mainLinesEchant[i][l][1][p] ) ;
	      }
// 	    for(unsigned int n = 0 ; n < 5 ; ++n ){
// 	      ref.push_back(mainLines[refSubject][0][origins[refSubject][0]]) ;
// 	      other.push_back( mainLines[i][l][origins[i][l]] ) ;
// 	    }
	  }
	  AffineLeastSquareEstimation aff( other, ref ) ;
	  motionsToRef[i] = *(aff.motion()) ;
	}
      }
      vector<AimsVector<vector<Point3df>, 2> > tangents = getTangents( mainLines, motionsToRef ) ;
      
      for( unsigned int i = 0 ; i < mainLines.size() ; ++i )
	for( int l = 0 ; l < 2 ; ++l ){
	  multiset<float> energies;
	  for( int unsigned c = origins[refSubject][0] ; c < curvAbs[refSubject][0].size() ; ++c ){
	    // Pour le sujet de référence, on prend l = 0 pour recaler systématiquement 
	    // sur le striatum gauche 
	    Point3df tang = tangents[refSubject][0][c] ;
	    Point3df interpolatedTangent = vectorInterpolation( curvAbs[i][l], tangents[i][l],
								curvAbs[refSubject][0][c] * caudFact, 
								origins[i][l], true ) ;
	    if( interpolatedTangent != Point3df(0., 0., 0.) ){
	      tang.normalize() ;
	      
	      // Note : la tangente a déjà été tranformée dans le repère du striatum gauche du sujet
	      // de référence.
	      
	      interpolatedTangent.normalize() ;
	      float proj = interpolatedTangent.dot( tang ) ;
	      if( proj > 0. )
		energies.insert( 1. - proj ) ;
	    }
	  }
	  for( int c = 4/*int(origins[refSubject][0] * 1./4.)*/ ; c <= origins[refSubject][0] ; ++c ){
	    Point3df tang = tangents[refSubject][0][c] ;
	    Point3df interpolatedTangent = vectorInterpolation( curvAbs[i][l], tangents[i][l], 
								curvAbs[refSubject][0][c] * putFact, 
								origins[i][l], true ) ;
	    if( interpolatedTangent != Point3df(0., 0., 0.) ){
	      tang.normalize() ;
	      interpolatedTangent.normalize() ;
	      float proj = interpolatedTangent.dot( tang ) ;
	      if( proj > 0. )
		energies.insert( 1. - proj ) ;
	    }
	  }
	  if( !energies.empty() ){
	    int count = 0 ; 
	    for( multiset<float>::iterator it = energies.begin() ; 
		 count < energies.size() * fractionKept ; ++it, ++count )
	      tangentsEnergyField( caudFactInd, putFactInd, l, i ) += *it ;
	    tangentsEnergyField( caudFactInd, putFactInd, l, i ) /= energies.size() ;
	  }
	}
      for( unsigned int i = 0 ; i < mainLines.size() ; ++i )
	for( int l = 0 ; l < 2 ; ++l ){
	  multiset<float> energies;
	  for( int unsigned c = 0 ; c < curvAbs[refSubject][0].size() ; ++c ){
	    Point3df p = vectorInterpolation( scaledCurvAbs[i][l], mainLines[i][l], 
					      curvAbs[refSubject][0][c], 
					      origins[i][l], false ) ;
	    if( p != Point3df(0., 0., 0.) ){
	      p = motionsToRef[i].transform(p) ;
	      energies.insert((p - mainLines[refSubject][0][c]).norm() ) ;
	    }
	  }
	  if( !energies.empty() ){
	    int count = 0 ; 
	    for( multiset<float>::iterator it = energies.begin() ; 
		 count < energies.size() * fractionKept ; ++it, ++count )
	      pointsEnergyField( caudFactInd, putFactInd, l, i ) += *it ;
	    pointsEnergyField( caudFactInd, putFactInd, l, i ) /= energies.size() ;
	  }
	}
    }
  for( unsigned int i = 0 ; i < mainLines.size() ; ++i )
    for(int l = 0 ; l < 2 ; ++l ){
      int minTangCaudEnIndex = 0 ;
      int minTangPutEnIndex = 0 ;
      int minPtsCaudEnIndex = 0 ;
      int minPtsPutEnIndex = 0 ;
      
      for( int x = 0; x < maxFactInd ; ++x )
	for( int y = 0; y < maxFactInd ; ++y ){
	  if( tangentsEnergyField(x, y, l, i) < tangentsEnergyField(minTangCaudEnIndex, minTangPutEnIndex, l, i) ){
	    minTangCaudEnIndex = x ;
	    minTangPutEnIndex = y ;
	  }
	  if( pointsEnergyField(x, y, l, i) < pointsEnergyField(minPtsCaudEnIndex, minPtsPutEnIndex, l, i) ){
	    minPtsCaudEnIndex = x ;
	    minPtsPutEnIndex = y ;
	  }
	}
      float caudateBestFactor = minFact + minTangCaudEnIndex * factStep ;
      float putamenBestFactor = minFact + minTangCaudEnIndex * factStep ;

      cout << "Subject " << i << " " << (l == 0 ? "Left" : "Right" ) 
	   << " factorCaudTang = " <<  minFact + minTangCaudEnIndex * factStep
	   << " factorPutTang = " <<  minFact + minTangPutEnIndex * factStep
	   << " factorCaudPts = " << minFact + minPtsCaudEnIndex * factStep
	   << " factorPutPts = " <<  minFact + minPtsPutEnIndex * factStep
	   << endl ;
      
      
      if( int(i) != refSubject || l != 0 )
	scaledCurvAbs[i][l] = scaleCurvilinearAbscissa( curvAbs[i][l], caudateBestFactor, putamenBestFactor ) ;
      else
	scaledCurvAbs[i][l] = curvAbs[i][l] ;
      resample(  mainLines, curvAbs, curvatures, areas, scales, invariants, origins, 0,
		 mainLinesEchant, curvatureEchant,  areaEchant, scaleEchant, invariantEchant ) ;
      resample(  mainLines, curvAbs, curvatures, areasNorm, scales, invariants, origins, 0,
		 mainLinesEchant, curvatureEchant,  areaNormEchant, scaleEchant, invariantEchant ) ;
      for( unsigned int i = 0 ; i < mainLines.size() ; ++i ){
	vector<Point3df> ref, other ;

	if( int(i) == refSubject )
	  motionsToRef[i].setToIdentity() ;
	else{	
	  for( int l = 0 ; l < 2 ; ++l ){
	    
	    for( unsigned int p = 0 ; p < mainLinesEchant[i][l][0].size()
		   && p < mainLinesEchant[refSubject][0][0].size() ; ++p )
	      if( areaEchant[i][l][0][p] > 0. && areaEchant[refSubject][0][0][p] > 0. ){
		ref.push_back(mainLinesEchant[refSubject][0][0][p]) ;
		if( l == 0 )
		  other.push_back(mainLinesEchant[i][l][0][p] ) ;
		else{
		  Point3df point = mainLinesEchant[i][l][0][p] ;
		  point[0] = -point[0] ;
		  other.push_back(point) ;
		}
	      }
	    for( unsigned int p = 0 ; p < mainLinesEchant[i][l][1].size() 
		   && p < mainLinesEchant[refSubject][0][1].size() ; ++p )
	      if( areaEchant[i][l][1][p] > 0. && areaEchant[refSubject][0][1][p] > 0. ){
		ref.push_back(mainLinesEchant[refSubject][0][1][p]) ;
		if( l == 0 )		
		  other.push_back(mainLinesEchant[i][l][1][p] ) ;
		else{
		  Point3df point = mainLinesEchant[i][l][1][p] ;
		  point[0] = -point[0] ;
		  other.push_back(point) ;
		}
	      }
// 	    for(unsigned int n = 0 ; n < 5 ; ++n ){
// 	      ref.push_back(mainLines[refSubject][0][origins[refSubject][0]]) ;
// 	      other.push_back( mainLines[i][l][origins[i][l]] ) ;
// 	    }
	    
	  }
	}
	AffineLeastSquareEstimation aff( other, ref ) ;
	motionsToRef[i] = *(aff.motion()) ;
      }
    }
  
  printAreas( subjects, areaEchant, "areasByRecalAndNorm.txt" ) ;

  Writer< AimsData<float> > wriTangentEnergyField( "tangentsEnergyField.ima") ;
  wriTangentEnergyField.write(tangentsEnergyField) ;
  
  Writer< AimsData<float> > wriPointEnergyField( "pointsEnergyField.ima") ;
  wriPointEnergyField.write(pointsEnergyField) ;

  return scaledCurvAbs ;
}

// float
// getACPCFractionFromPoint( const Point3df& p, const Point3df& ac, const Point3df& pc )
// {
//   Point3df ap = pc - ac ;
//   float d = ap[0] * p[0] + ap[1] * p[1] + ap[2] * p[2] ;
  
//   Point3df acp(p - ac) ;
//   Point3df acpc(pc - ac) ;
//   float alpha = ap[0] * acp[0] + ap[1] * acp[1] + ap[2] * acp[2] ;
//   alpha /=  ap[0] * acpc[0] + ap[1] * acpc[1] + ap[2] * acpc[2] ;
  
//   return alpha ;
// }


float distanceToPlane( const Point3df& p , const Point3df& planePoint, const Point3df& planeNormal ) 
{
  float d = planeNormal[0] * planePoint[0] 
    + planeNormal[1] * planePoint[1] 
    + planeNormal[2] * planePoint[2] ;
  
  return planeNormal[0] * p[0] 
    + planeNormal[1] * p[1] 
    + planeNormal[2] * p[2] - d ;
}

float
getACPCFractionFromPoint( const Point3df& p, const Point3df& ac, const Point3df& pc )
{
  Point3df ap = pc - ac ;
  //Point3df ap = symYDir ;
  //ap.normalize() ;
  Point3df acp(p - ac) ;
  Point3df acpc(pc - ac) ;
  float alpha = ap[0] * acp[0] + ap[1] * acp[1] + ap[2] * acp[2] ;
  alpha /=  ap[0] * acpc[0] + ap[1] * acpc[1] + ap[2] * acpc[2] ;
  
  return alpha ;
}

Point2df 
getPlaneCurvAbs( const vector<Point3df>& mainLine, 
		 const vector<float>& curvAbs, 
		 int origin, const Point3df& ac, 
		 const Point3df& pc, float alpha, Point2df& planeCurvAbsIndex )
{
  Point3df ap = pc - ac ;
  //Point3df ap = symYDir ;
  Point3df ac2(ac), pc2(pc) ; 
  Point3df mlMin = mainLine[0] ;
  Point3df planeNormal = ap / ap.norm() ;
  Point3df planePoint = ac + alpha * ( pc - ac ) ;
  float originSide = distanceToPlane( mainLine[origin], planePoint, planeNormal ) ;
  Point2df planeCurvAbs;
  int cCurr = origin ;
  while( cCurr >= 0 ){
    if( distanceToPlane( mainLine[cCurr], planePoint, planeNormal ) * originSide < 0 )
      break ;
    --cCurr ;
  }
  if( cCurr < 0 ){
    cout << " putamen is beyond plane !"<< endl ;
    planeCurvAbs[0] = 0. ;
    planeCurvAbsIndex[0] = origin ;
  }else{
    cout  << "Pour le putamen " << curvAbs[cCurr] << " sur " << curvAbs[0] << endl ;
    
    Point3df q(mainLine[cCurr]), p(mainLine[cCurr+1]) ;
    Point3df acp = planePoint - p ;
    Point3df pq = q - p ;
    float weight = planeNormal[0] * acp[0] + planeNormal[1] * acp[1] + planeNormal[2] * acp[2] ;
    weight /=  planeNormal[0] * pq[0] + planeNormal[1] * pq[1] + planeNormal[2] * pq[2] ;
    cout << weight << endl ;
    planeCurvAbs[0] = (1 - weight) * curvAbs[cCurr+1] + weight * curvAbs[cCurr] ;
    planeCurvAbsIndex[0] = cCurr + 1. - weight ;
  }
  cCurr = origin ;
  while( cCurr < int(mainLine.size()) ) {
    if( distanceToPlane( mainLine[cCurr], planePoint, planeNormal ) * originSide < 0 )
      break ;
    ++cCurr ;
  }
  if( cCurr >= int(mainLine.size()) ){
    cout << " caudate is beyond plane !"<< endl ;
    planeCurvAbs[1] = 0. ;
    planeCurvAbsIndex[1] = origin ;
  }else{
    cout << "Pour le caudé " << curvAbs[cCurr] << " sur " 
	 << curvAbs[curvAbs.size()-1] << endl ;
    
    Point3df q(mainLine[cCurr]), p(mainLine[cCurr-1]) ;
    Point3df acp = planePoint - p ;
    Point3df pq = q - p ;
    float weight =  planeNormal[0] * acp[0] + planeNormal[1] * acp[1] + planeNormal[2] * acp[2] ;
    weight /=  planeNormal[0] * pq[0] + planeNormal[1] * pq[1] + planeNormal[2] * pq[2] ;
    cout << weight << endl ;
    planeCurvAbs[1] = (1 - weight) * curvAbs[cCurr-1] + weight * curvAbs[cCurr] ;
    planeCurvAbsIndex[1] = cCurr - 1. + weight ;
  }
  
  return planeCurvAbs ;
}


// vector<AimsVector<vector<float>, 2> >
// normalizeWithCurves( const vector<AimsVector<AimsVector<vector<Point3df>, 2>, 2> >&  mainLines,
// 		     const vector<AimsVector<AimsVector<vector<float>, 2>, 2> >&  areaEchant,
// 		     const vector<AimsVector<AimsVector<vector<float>, 2>, 2> >&  curvatureEchant,
// 		     const vector<AimsVector<int, 2> > origins,
// 		     const vector< AimsVector<Point3df, 3> >& apcs,
// 		     const vector<string>& subjects,
// 		     int refSubject )
// {
  
  
//   for( unsigned int i = 0 ; i < mainLines.size() ; ++i )
//     for( int l = 0 ; l < 2 ; ++l ){
//       // Caudate Part
//       vector<float> caudate( areaEchant[i][l][1].size() * 2 ) ;
//       int beginIndex = areas[i][l].size() / 2 ;
//       Point2d researchArea(-1, -1) ;

//       float maxValue ;
//       for( unsigned int c = 0  ; c < areas[i][l][1].size() ; ++c ){
// 	caudateArea[beginIndex + c] = areas[c] ;
// 	if( )
//       }
      
//       for( unsigned int c = 0  ; c < caudateArea.size() ; ++c ){
// 	if( caudateArea[c] > )
//       }
      
//       vector<float> smoothedCaudate ;
      
//       bool noUniqueMax = true ;
//       float sigma = 0. ;
//       while( noUniqueMax ){
// 	smoothedCaudate = smoothCaudate(caudate, sigma) ;
	
	
	
// 	sigma += 0.1 ;
//       }
      
      
//     }


//   vector<float> putAreaRef( origins[i][l] + 1 ), putCurvAbsRef( origins[i][l] + 1 ) ;
//   vector<float> caudAreaRef( curvAbs[refSubject][0].size() - origins[i][l] ), 
//     caudCurvAbsRef( curvAbs[refSubject][0].size() - origins[i][l] ) ;
//   for( int c = 0 ; c < putAreaRef.size() ; ++c ){
//     putAreaRef[c] = areas[i][l][origins - c] ;
//     putCurvAbsRef = curvAbs[i][l][origins - c] ;
      
//   }
  
//   for( unsigned int i = 0 ; i < mainLines.size() ; ++i )
//     for( int l = 0 ; l < 2 ; ++l ){
//       vector<float> putAreas( ), putCurvAbs()
//     }
// }
  

vector<AimsVector<vector<float>, 2> >
normalizeWithAC( const vector<AimsVector<vector<Point3df>, 2> >& mainLines,
		 const vector<AimsVector<vector<float>, 2> >& curvAbs,
		 const vector<AimsVector<int, 2> > origins,
		 const vector< AimsVector<Point3df, 3> >& apcs,
		 const vector<string>& subjects,
		 int refSubject)
{
  // Normalisation telle que les deux intersections du plan (AC, ACPC) avec la ligne soit la même qq soit 
  // la ligne. Au delà de cette intersection, normalisation pour mettre PC sur PC, ou à défaut le point 
  // max sur le point appartenant au même pla sur le sujet de ref.
  
  // Calcul du alpha max from curve point
  vector<AimsVector<vector<float>, 2> > newCurvAbs(curvAbs.size()) ;
  vector< AimsVector< Point2df, 2 > > maxAlphaByCurve( curvAbs.size() ) ;
  for( unsigned int i = 0 ; i < mainLines.size() ; ++i )
    for( int l = 0 ; l < 2 ; ++l ){
      maxAlphaByCurve[i][l][0] = getACPCFractionFromPoint( mainLines[i][l][5/*0*/], apcs[i][0], 
							   apcs[i][1] ) ;
      float maxAlpha = -10000. ;
      float alpha ;
      for( unsigned int c = origins[i][l] ; c < mainLines[i][l].size() ; ++c ){
	alpha = getACPCFractionFromPoint( mainLines[i][l][c], apcs[i][0], apcs[i][1] ) ;
	if( alpha > maxAlpha )
	  maxAlpha = alpha ;
      }
      maxAlphaByCurve[i][l][1] = maxAlpha ;
    }
  
//   cout << "max getACPCFractionFromPoint done" << endl ;
  
  Point2df refAlpha( getACPCFractionFromPoint( mainLines[refSubject][0][0], 
					       apcs[refSubject][0], apcs[refSubject][1] ),
		     getACPCFractionFromPoint( mainLines[refSubject][0][mainLines[refSubject][0].size()-1], 
					       apcs[refSubject][0], apcs[refSubject][1] ) ) ;
//   cout << "Ref Alpha = " << refAlpha << endl ;
  
  for( unsigned int i = 0 ; i < mainLines.size() ; ++i )
    for( int l = 0 ; l < 2 ; ++l ){
      cout << "Subject = " << subjects[i] << "\t l = " << l << endl ;
      cout << curvAbs[i][l].size() << "\t" << newCurvAbs[i][l].size() << endl ;
      newCurvAbs[i][l] = vector<float>( curvAbs[i][l].size() ) ;
      
//       cerr << "Vector created" << endl ;
      
      if( maxAlphaByCurve[i][l][0] < 0. || maxAlphaByCurve[refSubject][0][0] < 0. ){
	if(maxAlphaByCurve[i][l][0] < 0. ) 
	  cout << "For " << subjects[i] << " " << ( l == 0 ? "Left" : "Right" ) 
	       << "maxAlphaByCurve[i][l][0] < 0." << endl ;
	if( maxAlphaByCurve[refSubject][0][0] < 0. )
	  cout << "For " << subjects[i] << " " << ( l == 0 ? "Left" : "Right" ) 
	       << "maxAlphaByCurve[refSubject][0][0]" << endl ;
      } else {
	
	cout << "before first fact put computed" << endl ;
	//For Origin -> ac proj slice on putamen
	Point2df refACPlaneCurvAbsIndex, planeCurvAbsIndex ;
	float refACPutCurvAbs = getPlaneCurvAbs( mainLines[refSubject][0], 
						 curvAbs[refSubject][0], 
						 origins[refSubject][0], 
						 apcs[refSubject][0], apcs[refSubject][1], 
						 0., refACPlaneCurvAbsIndex )[0] ;
	float subjectACPutCurvAbs = getPlaneCurvAbs( mainLines[i][l], 
						     curvAbs[i][l], 
						     origins[i][l], apcs[i][0], apcs[i][1], 
						     0., planeCurvAbsIndex )[0] ;
	float factorPut = refACPutCurvAbs / subjectACPutCurvAbs ;
	
	cout << "first fact put computed" << endl ;

	for( int c = origins[i][l] ; c > int(refACPlaneCurvAbsIndex[0]) ; --c )
	  newCurvAbs[i][l][c] = factorPut * curvAbs[i][l][c] ;
	
	cout << "first put abs computed" << endl ;
	
	
	//For ac proj slice on putamen -> putamen extremity
	float alphaPut = min( maxAlphaByCurve[i][l][0], maxAlphaByCurve[refSubject][0][0] )*0.99 ;
	float refExtPutCurvAbs = getPlaneCurvAbs( mainLines[refSubject][0], 
						  curvAbs[refSubject][0], 
						  origins[refSubject][0], 
						  apcs[refSubject][0], apcs[refSubject][1], 
						  alphaPut, planeCurvAbsIndex )[0] ;
	float subjectExtPutCurvAbs = getPlaneCurvAbs( mainLines[i][l], 
						      curvAbs[i][l], 
						      origins[i][l], apcs[i][0], apcs[i][1], 
						      alphaPut, planeCurvAbsIndex )[0] ;
	factorPut = (refExtPutCurvAbs - refACPutCurvAbs) / (subjectExtPutCurvAbs - subjectACPutCurvAbs) ;
	cout << "second fact put computed" << endl ;
	
	for( int c = int(refACPlaneCurvAbsIndex[0]) ; c >= 0 ; --c )
	  newCurvAbs[i][l][c] = refACPutCurvAbs + factorPut * ( curvAbs[i][l][c] - subjectACPutCurvAbs ) ;

	cout << "second put abs computed" << endl ;
	
      }
      if( maxAlphaByCurve[i][l][1] < 0. || maxAlphaByCurve[refSubject][0][1] < 0. ){
	if( maxAlphaByCurve[i][l][1] < 0. ) 
	  cout << "For " << subjects[i] << " " << ( l == 0 ? "Left" : "Right" ) 
	       << "maxAlphaByCurve[i][l][1] < 0." << endl ;
	if( maxAlphaByCurve[refSubject][0][1] < 0. )
	  cout << "For " << subjects[i] << " " << ( l == 0 ? "Left" : "Right" ) 
	       << "maxAlphaByCurve[refSubject][0][1]" << endl ;
      } else {
	//For origin -> ac proj slice on caudate
	Point2df planeCurvAbsIndex, refACPlaneCurvAbsIndex ;
	float refACCaudCurvAbs = getPlaneCurvAbs( mainLines[refSubject][0], 
						  curvAbs[refSubject][0], 
						  origins[refSubject][0], 
						  apcs[refSubject][0], apcs[refSubject][1], 
						  0, refACPlaneCurvAbsIndex )[1] ;
	float subjectACCaudCurvAbs = getPlaneCurvAbs( mainLines[i][l], 
						      curvAbs[i][l], 
						      origins[i][l], apcs[i][0], apcs[i][1], 
						      0, planeCurvAbsIndex )[1] ;
	float factorCaud = refACCaudCurvAbs / subjectACCaudCurvAbs ;
	
	cout << "first fact caud computed : index = " << planeCurvAbsIndex[1]  << " sur " << 
	     curvAbs[i][l].size() << endl ;

	for( int c = origins[i][l]+1 ; c <= int(refACPlaneCurvAbsIndex[1]) 
	       && c < int(newCurvAbs[i][l].size()) ; ++c )
	  newCurvAbs[i][l][c] = factorCaud * curvAbs[i][l][c] ;
	
	cout << "first caud abs computed" << endl ;
	// For ac proj slice on caudate -> caudate extremity

	if( maxAlphaByCurve[i][l][1] < 1. || maxAlphaByCurve[refSubject][0][1] < 1. ){
	  // Caudate not crossing PC plane 
	  float alphaCaud = min( maxAlphaByCurve[i][l][1], maxAlphaByCurve[refSubject][0][1] ) * 0.99 ;
	  float refExtCaudCurvAbs = getPlaneCurvAbs( mainLines[refSubject][0], 
						    curvAbs[refSubject][0], 
						    origins[refSubject][0], 
						    apcs[refSubject][0], apcs[refSubject][1], 
						    alphaCaud, planeCurvAbsIndex )[1] ;
	  float subjectExtCaudCurvAbs = getPlaneCurvAbs( mainLines[i][l], 
							 curvAbs[i][l], 
							 origins[i][l], apcs[i][0], apcs[i][1], 
							 alphaCaud, planeCurvAbsIndex )[1] ;
	  factorCaud = (refExtCaudCurvAbs - refACCaudCurvAbs) 
	    / (subjectExtCaudCurvAbs - subjectACCaudCurvAbs) ;
	  
	  cout << "second fact caud computed" << endl ;
	  
	  for(  unsigned int c = int(refACPlaneCurvAbsIndex[1]) ; c <curvAbs[i][l].size()  ; ++c )
	    newCurvAbs[i][l][c] = refACCaudCurvAbs + factorCaud * 
	      ( curvAbs[i][l][c] - subjectACCaudCurvAbs ) ;
	  cout << "second caud abs computed" << endl ;
	} else {
	  // Caudate crossing PC plane 
	  float refExtCaudCurvAbs = getPlaneCurvAbs( mainLines[refSubject][0], 
						    curvAbs[refSubject][0], 
						    origins[refSubject][0], 
						    apcs[refSubject][0], apcs[refSubject][1], 
						    1, planeCurvAbsIndex )[1] ;
	  float subjectExtCaudCurvAbs = getPlaneCurvAbs( mainLines[i][l], 
							 curvAbs[i][l], 
							 origins[i][l], apcs[i][0], apcs[i][1], 
							 1, planeCurvAbsIndex )[1] ;
	  factorCaud = (refExtCaudCurvAbs - refACCaudCurvAbs) 
	    / (subjectExtCaudCurvAbs - subjectACCaudCurvAbs) ;
	  
	  for(  unsigned int c = int(refACPlaneCurvAbsIndex[1]) ; c <curvAbs[i][l].size()  ; ++c )
	    newCurvAbs[i][l][c] = refACCaudCurvAbs 
	      + factorCaud * ( curvAbs[i][l][c] - subjectACCaudCurvAbs ) ;
	}
      }
    }
  for(  unsigned int i = 0 ; i < mainLines.size() ; ++i )
    for( int l = 0 ; l < 2 ; ++l ){
      cout << endl << subjects[i] << " " << ( l == 0 ? "Left" : "Right" ) << endl ;
      for( unsigned int c = 0 ; c < curvAbs[i][l].size()  ; ++c )
	cout << "Old : \t" << curvAbs[i][l][c] << "\tNew : \t" << newCurvAbs[i][l][c] << endl ;
    }
  
  return newCurvAbs ;
}

AimsVector<Point3df, 3>
getBrainMaskPCSideEtremities( const AimsData<short>& brainMask,
			      const AimsVector<Point3df, 3>& acpc,
			      float& acpcByBrainLength )
{
  int x, y, z ;
  float alphaMax = 0. ;
  float alphaMin = 0. ;
  float alpha ;
  ForEach3d( brainMask, x, y, z )
    if( brainMask(x, y, z) ){
      if( y * brainMask.sizeY() > acpc[1][1] ){
	alpha = getACPCFractionFromPoint( Point3df(x * brainMask.sizeX(), y * brainMask.sizeY(),
						   z * brainMask.sizeZ() ), 
					  acpc[0], acpc[1] ) ;
	if( alpha > alphaMax )
	  alphaMax = alpha ;
      }
      if( y * brainMask.sizeY() < acpc[0][1] ){
	alpha = getACPCFractionFromPoint( Point3df(x * brainMask.sizeX(), y * brainMask.sizeY(),
						   z * brainMask.sizeZ() ), 
					  acpc[0], acpc[1] ) ;
	if( alpha < alphaMin )
	  alphaMin = alpha ;
      }
    }
  Point3df frontalExtrem, parietalExtrem ;
  frontalExtrem = acpc[0] + alphaMax * ( acpc[1] - acpc[0] ) ;
  parietalExtrem = acpc[0] + alphaMin * ( acpc[1] - acpc[0] ) ;
  acpcByBrainLength = sqrt( ( acpc[1] - acpc[0] ).dot(acpc[1] - acpc[0]) / 
			    (parietalExtrem - frontalExtrem).dot(parietalExtrem - frontalExtrem) ) ;
  
  cout << "Ratio ACPC / Brain length = " << acpcByBrainLength << endl ;

  AimsVector<Point3df, 3> res(acpc) ;
  res[1] = acpc[0] + alphaMax * ( acpc[1] - acpc[0] ) ;
  cout << res[1] << endl << endl << endl << (acpc[1] - acpc[0]).norm() / (res[1] - acpc[0]).norm() 
       << endl << endl << endl << "alpha = "<< alphaMax << endl << endl ;
  return res ;
}

void 
writeCurves( const vector<AimsVector<AimsVector<vector<float>, 2>, 2> >& areaEchant,
	     const vector<AimsVector<AimsVector<vector<float>, 2>, 2> >& areaNormEchant,
	     const vector<AimsVector<AimsVector<vector<float>, 2>, 2> >& curvatureEchant,
	     const vector<AimsVector<AimsVector<vector<float>, 2>, 2> >& scaleEchant,
	     const vector<AimsVector<AimsVector< vector<vector<float> >, 2>, 2> >& invariantEchant,
	     const vector<string>& subjects,
	     const vector<float>& ages,
	     const vector<float>& disease,
	     const vector<float>& sex,
	     const vector<float>& laterality,
	     const string& outImageNames,
	     const string& outForRName )
{
  unsigned int maxSizeNeg = 0, maxSizePos = 0 ;
  
  for( int unsigned n = 0 ; n < areaEchant.size() ; ++n ){
    if( areaEchant[n][0][0].size() > maxSizeNeg )
      maxSizeNeg = areaEchant[n][0][0].size() ;
    if( areaEchant[n][0][1].size() > maxSizePos )
      maxSizePos = areaEchant[n][0][1].size() ;
    if( areaEchant[n][1][0].size() > maxSizeNeg )
      maxSizeNeg = areaEchant[n][1][0].size() ;
    if( areaEchant[n][1][1].size() > maxSizePos )
      maxSizePos = areaEchant[n][1][1].size() ;
  }
  
  
  ofstream osforR( outForRName.c_str() ) ;
  
  AimsData<float> curvatureImg( curvatureEchant.size(), maxSizeNeg + maxSizePos + 2, 2) ;
  AimsData<float> areaImg( curvatureEchant.size(), maxSizeNeg + maxSizePos + 2, 2) ;
  AimsData<float> areaNormImg( curvatureEchant.size(), maxSizeNeg + maxSizePos + 2, 2) ;
  AimsData<float> curvAbsImg( curvatureEchant.size(), maxSizeNeg + maxSizePos + 2, 2) ;
  AimsData<float> scaleImg( curvatureEchant.size(), maxSizeNeg + maxSizePos + 2, 2) ;
  AimsData<float> invariantImg( curvatureEchant.size(), maxSizeNeg + maxSizePos + 2, 2, 12) ;
  AimsData<float> curvIndexRatioImg( curvatureEchant.size(), maxSizeNeg + maxSizePos + 2, 2) ;
  
  for( unsigned int i = 0 ; i < areaEchant.size()  ; ++i )
    for( unsigned int l = 0 ; l < 2 ; ++l ) {
      osforR << subjects[i] ;
      
      for( int cu = maxSizeNeg - 1 ; cu >=0 ; --cu )
	// 	for( unsigned int pt = 0 ; pt < mainLines[i][l].size() ; ++pt )
	if( cu >= int(areaEchant[i][l][0].size()) )
	  osforR << " " << 0. ;
	else{
	  osforR << " " << areaEchant[i][l][0][cu] ;
	  curvatureImg( i, maxSizeNeg - 1 - cu, l ) = curvatureEchant[i][l][0][cu] ;
	  areaImg( i, maxSizeNeg - 1 - cu, l ) = areaEchant[i][l][0][cu] ;
	  areaNormImg( i, maxSizeNeg - 1 - cu, l ) = areaNormEchant[i][l][0][cu] ;
	  curvAbsImg( i, maxSizeNeg - 1 - cu, l ) = -cu ;
	  scaleImg( i, maxSizeNeg - 1 - cu, l ) = scaleEchant[i][l][0][cu] ;
	  for( int t = 0 ; t < 12 ; ++t )
	    invariantImg( i, maxSizeNeg - 1 - cu, l, t ) = invariantEchant[i][l][0][cu][t] ;
	    
	}
      for( unsigned int cu = 0 ; cu < maxSizePos ; ++cu )
	// 	for( unsigned int pt = 0 ; pt < mainLines[i][l].size() ; ++pt )
	if( cu >= areaEchant[i][l][1].size() )
	  osforR << " " << 0. ;
	else{
	  osforR << " " << areaEchant[i][l][1][cu] ;
	  curvatureImg( i, maxSizeNeg + cu, l ) = curvatureEchant[i][l][1][cu] ;
	  areaImg( i, maxSizeNeg + cu, l ) = areaEchant[i][l][1][cu] ;
	  areaNormImg( i, maxSizeNeg + cu, l ) = areaNormEchant[i][l][1][cu] ;
	  curvAbsImg( i, maxSizeNeg + cu, l ) = cu ;
	  scaleImg( i, maxSizeNeg + cu, l ) = scaleEchant[i][l][1][cu] ;
	  for( int t = 0 ; t < 12 ; ++t )
	    invariantImg( i, maxSizeNeg + cu, l, t ) = invariantEchant[i][l][1][cu][t] ;
	}
      osforR << ( l == 0 ? " Gauche" : " Droit") << " " << disease[i] << " " << ages[i] 
	     << " " << sex[i] << " " << laterality[i] << endl ;
    }      
  osforR.close();
  Writer< AimsData<float> > curvWri(outImageNames + "curvatureImage.ima") ;
  curvWri.write( curvatureImg ) ;
  Writer< AimsData<float> > curvAbsWri(outImageNames + "curvilinearAbscissaImage.ima") ;
  curvAbsWri.write( curvAbsImg ) ;
  Writer< AimsData<float> > areaWri(outImageNames + "areaImage.ima") ;
  areaWri.write( areaImg ) ;
  Writer< AimsData<float> > areaNormWri(outImageNames + "areaNormImage.ima") ;
  areaNormWri.write( areaNormImg ) ;
  Writer< AimsData<float> > invarWri(outImageNames + "invariantImage.ima") ;
  invarWri.write( invariantImg ) ;
  Writer< AimsData<float> > scaleWri(outImageNames + "scaleImage.ima") ;
  scaleWri.write( scaleImg ) ;
}

void 
writeCurvesForR( const vector<AimsVector<AimsVector<vector<float>, 2>, 2> >& areaEchant,
		 const vector<AimsVector<AimsVector<vector<float>, 2>, 2> >& areaNormEchant,
		 const vector<AimsVector<AimsVector<vector<float>, 2>, 2> >& curvatureEchant,
		 const vector<AimsVector<AimsVector<vector<float>, 2>, 2> >& scaleEchant,
		 const vector<AimsVector<AimsVector<vector< vector<float> >, 2>, 2> >& invariantEchant,
		 const vector<string>& subjects,
		 const vector<float>& ages,
		 const vector<float>& disease,
		 const vector<float>& sex,
		 const vector<float>& laterality,
		 const string& outForRName, 
		 int subSampleRate )
{
  int maxSizeNeg = 0, maxSizePos = 0 ;
  
  for( int n = 0 ; n < int(areaEchant.size()) ; ++n ){
    if( int(areaEchant[n][0][0].size()) > maxSizeNeg )
      maxSizeNeg = areaEchant[n][0][0].size() ;
    if( int(areaEchant[n][0][1].size()) > maxSizePos )
      maxSizePos = areaEchant[n][0][1].size() ;
    if( int(areaEchant[n][1][0].size()) > maxSizeNeg )
      maxSizeNeg = areaEchant[n][1][0].size() ;
    if( int(areaEchant[n][1][1].size()) > maxSizePos )
      maxSizePos = areaEchant[n][1][1].size() ;
  }
  
  cout << "-------------- Max Size Neg --------------- " << maxSizeNeg << endl ;
  cout << "-------------- Max Size Pos --------------- " << maxSizePos << endl ;

  ofstream osforR( (outForRName + string(".txt")).c_str() ) ;
  ofstream osforXl( (outForRName + string("Excell.txt")).c_str()) ;
  ofstream osforXlNorm( (outForRName + string("ExcellNormed.txt")).c_str()) ;
  ofstream osforXlMax( (outForRName + string("ExcellMax.txt")).c_str()) ;
  ofstream osforXlNormMax( (outForRName + string("ExcellNormedMax.txt")).c_str()) ;
  osforR << "Subject\tSide\tCurvAbs\tArea\tAreaNorm\tCurvature\tAxisRatio\tI00_2"
	 << "\tI22_2\tI222_3\tI11_3\tI33_3\tI3111_3\tI3131_3\tI3331_3"
	 << "\tI3333_3\tI112_23\tI123_23\tI233_23"  ; 
  osforXl << "Subject\tSide\tDisease\tAge\tSex\tLaterality\tAreas"  ; 
  osforXlNorm << "Subject\tSide\tDisease\tAge\tSex\tLaterality\tAreas" ; 
  osforXlMax << "Subject\tSide\tDisease\tAge\tSex\tLaterality\tAreas" ; 
  osforXlNormMax << "Subject\tSide\tDisease\tAge\tSex\tLaterality\tAreas"  ; 
  
  for( int c = -maxSizeNeg + 1 ; c < maxSizePos ; ++c ){
    // Pour R aussi ?
    //osforR << "\t" << c ;
    osforXl << "\t" << c ;
    osforXlNorm << "\t" << c ;
    osforXlMax << "\t" << c ;
    osforXlNormMax << "\t" << c ;
  }
  
  osforR << endl ;
  osforXl << endl ;
  osforXlNorm << endl ;
  osforXlMax << endl ;
  osforXlNormMax << endl ;

  for( unsigned int i = 0 ; i < areaEchant.size()  ; ++i )
    for( unsigned int l = 0 ; l < 2 ; ++l ) {
      vector<float> areas( maxSizePos + maxSizeNeg + 1) ;
      vector<float> areasNorm( maxSizePos + maxSizeNeg + 1) ;
      vector<float> meanAreas( maxSizePos + maxSizeNeg + 1, 0.) ;
      vector<float> meanAreasNorm( maxSizePos + maxSizeNeg + 1, 0.) ;
      vector<float> curvatures( maxSizePos + maxSizeNeg + 1) ;
      vector<float> scales( maxSizePos + maxSizeNeg + 1) ;
      vector<vector<float> > invariants( maxSizePos + maxSizeNeg + 1, vector<float>(12, 0.) ) ;
      vector<float> meanCurvatures( maxSizePos + maxSizeNeg + 1, 0.) ;
      vector<float> meanScales( maxSizePos + maxSizeNeg + 1, 0.) ;
      vector<vector<float> > meanInvariants( maxSizePos + maxSizeNeg + 1, vector<float>(12, 0.) ) ;
      
      int count = 0 ;
      for( int c = maxSizeNeg - 1 ; c >=0 ; --c, ++count )
	if( c >= int(areaEchant[i][l][0].size() ) ){
	  areas[count] = 0. ;
	  areasNorm[count] = 0. ;
	  curvatures[count] = 0. ;
	  scales[count] = 0. ;
	  invariants[count] = vector<float>(12, 0.) ;
	} else {
	  areas[count] = areaEchant[i][l][0][c] ;
	  areasNorm[count] = areaNormEchant[i][l][0][c] ;
	  curvatures[count] = curvatureEchant[i][l][0][c] ;
	  scales[count] = scaleEchant[i][l][0][c] ;
	  invariants[count] = invariantEchant[i][l][0][c] ;
	}
      int orig = count - 1 ;
      cout << "-------------- Orig --------------- " << orig << endl ;
      
      for( int c = 0 ; c < maxSizePos ; ++c, ++count )
	if( c >= int(areaEchant[i][l][1].size()) ){
	  areas[count] = 0. ;
	  areasNorm[count] = 0. ;
	  curvatures[count] = 0. ;
	  scales[count] = 0. ;
	  invariants[count] = vector<float>(12, 0.) ;
	} else {
	  areas[count] = areaEchant[i][l][1][c] ;
	  areasNorm[count] = areaNormEchant[i][l][1][c] ;
	  curvatures[count] = curvatureEchant[i][l][1][c] ;
	  scales[count] = scaleEchant[i][l][1][c] ;
	  invariants[count] = invariantEchant[i][l][1][c] ;
	}
      
      for( int c = 0 ; c < int(areas.size()) && c < int(areasNorm.size()) ; ++c ){
	float meanArea = 0 ; 
	float meanAreaNorm = 0 ;
	float meanCurvature = 0 ;
	float meanScale = 0 ;
	vector<float> meanInvariant(12, 0.) ;
	
	for( int dc = -subSampleRate ; dc <= subSampleRate ; ++dc ){
	  if( c + dc >= 0 && c + dc < int(areas.size()) ){
	    meanArea += areas[c + dc] ;
	    meanAreaNorm += areasNorm[c + dc] ;
	    meanCurvature += curvatures[c + dc] ;
	    meanScale += scales[c + dc] ;
	    for( int k = 0 ; k < 12 ; ++k )
	      meanInvariant[k] += invariants[c + dc][k] ;
	  }
	}
	meanAreas[c] = meanArea / (2*subSampleRate+1) ;
	meanAreasNorm[c] = meanAreaNorm / (2*subSampleRate+1) ;
	meanCurvatures[c] = meanCurvature / (2*subSampleRate+1) ;
	meanScales[c] = meanScale / (2*subSampleRate+1) ;
	for( int k = 0 ; k < 12 ; ++k )
	  meanInvariants[c][k] = meanInvariant[k] / (2*subSampleRate+1) ;
      }
      
//       for( int k = 0 ; k < areas.size() ; ++k )
// 	cout << k << "\tareas " << areas[k] << "\tareasNorm "<< areasNorm[k] 
// 	     << "\tmeanAreas "<< meanAreas[k] << "\tmeanAreasNorm "<< meanAreasNorm[k] << endl ;
      
//       int k = 0 ; 
//       while( k * (2*subSampleRate+1) <  )

      
      osforXl << subjects[i] << "\t" << (l == 0 ? "Left" : "Right" ) << "\t" << disease[i] << "\t" << ages[i] 
	      << "\t" << sex[i] << "\t" << laterality[i] << "\t" ;
      osforXlNorm << subjects[i] << "\t" << (l == 0 ? "Left" : "Right" ) << "\t" << disease[i] << "\t" << ages[i] 
	      << "\t" << sex[i] << "\t" << laterality[i] << "\t" ;
      osforXlMax << subjects[i] << "\t" << (l == 0 ? "Left" : "Right" ) << "\t" << disease[i] << "\t" << ages[i] 
	      << "\t" << sex[i] << "\t" << laterality[i] << "\t" ;
      osforXlNormMax << subjects[i] << "\t" << (l == 0 ? "Left" : "Right" ) << "\t" << disease[i] << "\t" << ages[i] 
	      << "\t" << sex[i] << "\t" << laterality[i] << "\t" ;
      for( int n = -(2*subSampleRate+1) * int(ceil( float(orig) / (2*subSampleRate+1) )) ; 
	   n <= (2*subSampleRate+1) * int(ceil( float(meanAreas.size() - 1 - orig ) / (2*subSampleRate+1) )) ; 
	   n += (2*subSampleRate+1) )
	if( n + orig >= 0 && n + orig < int(meanAreas.size()) ){
 	  osforR << subjects[i] << "\t" << (l == 0 ? "Left" : "Right" ) << "\t" << n-1 << "\t"
		 << meanAreas[n+orig] << "\t" << meanAreasNorm[n+orig] << "\t" 
		 << meanCurvatures[n+orig] << "\t" << meanScales[n+orig] ;
	  for( int k = 0 ; k < 12 ; ++k )
	    osforR << "\t" << meanInvariants[n+orig][k] ;
	  osforR << endl ;
	  
	  osforXl << "\t" << meanAreas[n+orig] ;
	  osforXlNorm << "\t" << meanAreasNorm[n+orig] ;
	  if( meanAreas[n+orig] >= 1. )
	    osforXlMax << "\t" << meanAreas[n+orig] ;
	  else
	    osforXlMax << "\t" << 1. ;
	  if( meanAreasNorm[n+orig] >= 1. ) 
	    osforXlNormMax << "\t" << meanAreasNorm[n+orig] ;
	  else
	    osforXlNormMax << "\t" << 1. ;
	}
	else{
 	  osforR << subjects[i] << "\t" << (l == 0 ? "Left" : "Right" ) << "\t" << n-1 << "\t"
		 << 0 << "\t" << 0 << "\t" << 0 << "\t" << 0 ;
	  for( int k = 0 ; k < 12 ; ++k )
	    osforR << "\t" << 0 ;
	  osforR << endl ;
	  osforXl << "\t" << 0. ;
	  osforXlNorm << "\t" << 0. ;
	  osforXlMax << "\t" << 1. ;
	  osforXlNormMax << "\t" << 1. ;
	}
      osforXl << endl ;
      osforXlNorm << endl ;
      osforXlMax << endl ;
      osforXlNormMax << endl ;
      
    }
  osforXl.close() ;
  osforXlNorm.close() ;
  osforR.close();
  osforXlMax.close() ;
  osforXlNormMax.close() ;
  ofstream osforR2( (outForRName + string("_Description.txt")).c_str() ) ;
  
  osforR << "Subject\tSiseaseAvt\tAge\tSex\tLaterality" << endl ; 
  for( unsigned int i = 0 ; i < areaEchant.size()  ; ++i )
    osforR2 << subjects[i] << "\t" << disease[i] << "\t" << ages[i] 
	    << "\t" << sex[i] << "\t" << laterality[i] << endl ;

  osforR2.close();
  
  ofstream osforR3( (outForRName + string("_Loss.txt")).c_str() ) ;
  int count = 0 ;
  for( int c = maxSizeNeg - 1 ; c >=0 ; --c, ++count ){
    int nbOfHealthy = 0 ;
    int nbOfPatient = 0 ;
    float areaNormHealthy = 0. ;
    float areaNormPatient = 0. ;
    for( unsigned int i = 0 ; i < areaEchant.size()  ; ++i )
      for( unsigned int l = 0 ; l < 2 ; ++l ) {
	if( c < int(areaNormEchant[i][l][0].size()) )
	  if( disease[i] < 0.5 ){
	    areaNormHealthy += areaNormEchant[i][l][0][c] ;
	    ++nbOfHealthy ;
	  } else {
	    areaNormPatient += areaNormEchant[i][l][0][c] ;
	    ++nbOfPatient ;
	  }
      }
    areaNormHealthy /= float(nbOfHealthy) ;
    areaNormPatient /= float(nbOfPatient) ;
    if( nbOfPatient  == 0 || nbOfHealthy == 0){
      areaNormHealthy = 1. ;
      areaNormPatient = 0. ;
    }
    osforR3 << (areaNormHealthy - areaNormPatient) / areaNormHealthy * 100. << "\t" ;
  }
  for( int c = 0 ; c < maxSizePos ; ++c, ++count ){
    int nbOfHealthy = 0 ;
    int nbOfPatient = 0 ;
    float areaNormHealthy = 0. ;
    float areaNormPatient = 0. ;
    for( unsigned int i = 0 ; i < areaEchant.size()  ; ++i )
      for( unsigned int l = 0 ; l < 2 ; ++l ) {
	if( c < int(areaNormEchant[i][l][1].size()) )
	  if( disease[i] < 0.5 ){
	    areaNormHealthy += areaNormEchant[i][l][1][c] ;
	    ++nbOfHealthy ;
	  } else {
	    areaNormPatient += areaNormEchant[i][l][1][c] ;
	    ++nbOfPatient ;
	  }
      }
    osforR3 << (areaNormHealthy - areaNormPatient) / areaNormHealthy * 100. << "\t" ;
  }
  osforR3.close() ;
}

void
writeCurvAbsImages( const vector<AimsVector<vector<float>, 2> >& curvAbs, 
		    const vector<AimsVector<vector<Point3df>, 2> >& mainLines, 
		    const vector<string>& brainMaskFiles,
		    int refSubject )
{
  AimsData<short> brainMask ;
//   for( unsigned int i = 0 ; i < mainLines.size()  ; ++i ){
  int i = refSubject ;
    Reader< AimsData<short> > reader(brainMaskFiles[i]) ;
    reader.read( brainMask ) ;
    
    AimsData<short> curvAbsImage( brainMask.clone() ) ;
    curvAbsImage = -100 ;
    for( unsigned int l = 0 ; l < 2 ; ++l )
      for( unsigned int c = 0 ; c < mainLines[i][l].size() ; ++c )
	curvAbsImage( int(round(mainLines[i][l][c][0] ) ), 
		      int(round(mainLines[i][l][c][1] ) ), 
		      int(round(mainLines[i][l][c][2] ) ) ) = int( round(curvAbs[i][l][c]) ) ;
    Writer< AimsData<short> > writer( "CurvAbs.ima" ) ;
    writer.write( curvAbsImage ) ;
    cout << "Curv abs image wrote" << endl ;
//   }
}

void
acpcSizeTest(const vector<float>& acpcByBrain, const vector<float>& disease) 
{
  float meanDisease = 0, varDisease = 0, meanHealthy = 0, varHealthy = 0 ;
  int nbHealthy = 0, nbDiseased = 0 ;

  cout << "Diseased" << endl ;
  for( int i = 0 ; i < acpcByBrain.size() ; ++i ){
    if( disease[i]> 0.5 ){
      meanDisease += acpcByBrain[i] ;
      varDisease += acpcByBrain[i]*acpcByBrain[i] ;
      ++nbDiseased ;
      cout << acpcByBrain[i] << endl ;
    }
  }
  cout << endl << "Healthy" << endl ;
  for( int i = 0 ; i < acpcByBrain.size() ; ++i ){
    if( disease[i] < 0.5 ){
      meanHealthy += acpcByBrain[i] ;
      varHealthy += acpcByBrain[i]*acpcByBrain[i] ;
      ++nbHealthy ;
      cout << acpcByBrain[i] << endl ;
    }
  }
  meanHealthy /= nbHealthy ;
  meanDisease /= nbDiseased ;
  varDisease = varDisease / nbDiseased - meanDisease * meanDisease ;
  varHealthy = varHealthy / nbHealthy - meanHealthy * meanHealthy ;
  
  cout << "Healthy : Mean = " << meanHealthy << "\tVar = " << varHealthy << endl
       << "Disease : Mean = " << meanDisease << "\tVar = " << varDisease << endl ;
}


int main( int argc, const char **argv )
{
  // --------------- TEST ----------------------------
 //  vector<float> testCurvAbs( 4 ) ;
//   testCurvAbs[0] = 0. ;
//   testCurvAbs[1] = 5. ;
//   testCurvAbs[2] = 10. ;
//   testCurvAbs[3] = 20. ;
//   vector<float> testScalars( 4 ) ;
//   testScalars[0] = 1. ;
//   testScalars[1] = 2. ;
//   testScalars[2] = 4. ;
//   testScalars[3] = 7. ;
  
//   cout << "Interpolation at 0.5 " << scalarInterpolation(testCurvAbs, testScalars, 0.5 )  
//        << " should be 1.5" << endl ;
//   cout << "Interpolation at 0.8 " << scalarInterpolation(testCurvAbs, testScalars, 0.8 ) 
//        << " should be " << endl ;
//   cout << "Interpolation at 1.2 " << scalarInterpolation(testCurvAbs, testScalars, 1.2 )
//        << " should be " << endl ;
//   cout << "Interpolation at 1.9 " << scalarInterpolation(testCurvAbs, testScalars, 1.9 )
//        << " should be " << endl ;
//   cout << "Interpolation at 2.5 " << scalarInterpolation(testCurvAbs, testScalars, 2.5 ) 
//        << " should be " << endl ;
  
//   return 0 ;

//   vector<AimsVector<vector<Point3df>, 2> > testMainLines(1) ;
//   testMainLines[0][0]


  // --------------- TEST ----------------------------

  enum StriatumPart{ 
    LEFTCAUDATE = 1,
    LEFTPUTAMEN,
    LEFTVENTRALSTRIATUM,
    RIGHTCAUDATE,
    RIGHTPUTAMEN,
    RIGHTVENTRALSTRIATUM,
    LEFTCAUDATEHEAD,
    LEFTCAUDATETAIL,
    RIGHTCAUDATEHEAD,
    RIGHTCAUDATETAIL
  } ;

  vector<string> mlfiles, motionToRef, striatafiles, apcfiles, brainmaskfiles ;
  string fileout ;

  AimsApplication	app( argc, argv, "Striatum main" );

  app.addOptionSeries( mlfiles, "-ml", "mainline files" );
  app.addOptionSeries( striatafiles, "-i", "Striatia to register" ) ;
  app.addOptionSeries( motionToRef, "-m", "Motions NGC based" );
  app.addOptionSeries( apcfiles, "-apc", "AC, PC, IHP file (mm)" ) ;
  app.addOptionSeries( brainmaskfiles, "-b", "brain mask files" ) ;
  app.addOption( fileout, "-o", "output base file name" ) ;
  
  app.initialize();
  
  vector<string> subjects( mlfiles.size() ) ;
  vector<float> ages( mlfiles.size() ) ;
  vector<float> disease( mlfiles.size() ) ;
  vector<float> sex( mlfiles.size() ) ;
  vector<float> laterality( mlfiles.size() ) ;
  vector< AimsVector<Point3df, 3> > apcs( mlfiles.size() ) ;
  vector<AimsVector<vector<Point3df>, 2> > mainLines( mlfiles.size() ) ;
  vector<AimsVector<vector<float>, 2> > curvAbs( mlfiles.size() ) ;
  vector<AimsVector<vector<float>, 2> > normCurvAbs( mlfiles.size() ) ;
  vector<AimsVector<vector<float>, 2> > areas( mlfiles.size() ) ;
  vector<AimsVector<vector<float>, 2> > areasNorm( mlfiles.size() ) ;
  vector<AimsVector<vector<float>, 2> > curvatures( mlfiles.size() ) ;
  vector<AimsVector<vector<float>, 2> > scales( mlfiles.size() ) ;  
  vector<AimsVector<vector<vector<float> >, 2> > invariants( mlfiles.size() ) ;
  vector<AimsVector<int, 2> > origins( mlfiles.size() ) ;
  vector<AimsVector<int, 2> > originsNorm( mlfiles.size() ) ;
  
  int refSubject = -1 ;
  string lr ;
  Point3df p ;
  float curv ;
  float area ;
  float areaUnNorm ;
  float curvature ;
  float smallestByGreatestAxis ;
  float temp ;
  vector<float> invariant(12) ;

  ofstream os("result.txt") ;
  ofstream osL0("leftPoints0.txt") ;
  ofstream osL1("leftPoints1.txt") ;
  ofstream osR0("rightPoints0.txt") ;
  ofstream osR1("rightPoints1.txt") ;
  
  vector<float> voxelSize ;
  for(unsigned int i = 0 ; i < mlfiles.size() ; ++i) 
    {
      cout << i << endl ;
      ifstream is( mlfiles[i].c_str() ) ;
      Graph g ;
      Reader<Graph> gr(striatafiles[i]) ;
      gr.read(g) ;
      g.getProperty("voxel_size", voxelSize ) ;
      is >> subjects[i] >> ages[i] >> disease[i] >> sex[i] >> laterality[i] ;
      cout << subjects[i] << "\t" << ages[i]<< "\t" <<disease[i] 
	   << "\t" <<sex[i] << "\t" <<laterality[i] << endl ;
      os << subjects[i]<< " " << ages[i]<< " " <<disease[i] 
	 << " " <<sex[i]<< " " <<laterality[i] << endl ;
      
      if(subjects[i] == "S08714_002" )
	refSubject = i ;
      while(!is.eof()){
	is >> lr >> curv >> p >> area >> areaUnNorm >> curvature >> smallestByGreatestAxis ;
	//temporary
	for( unsigned int k = 0 ; k < invariant.size() ; ++k )
	  is >> temp ;
	//temporary

	for( unsigned int k = 0 ; k < invariant.size() ; ++k )
	  is >> invariant[k] ;

	cout << lr << "\t" << curv  << "\t" << p << "\t" << area << "\t" << areaUnNorm << endl ;
	os << curv << " " << area << endl ;
	if( i == 0 && lr == "Right" )
// 	  osR0 << Point3df( p[0]*voxelSize[0], p[1]*voxelSize[1], p[2]*voxelSize[2] ) << endl ;
	  osR0 << p[0]*voxelSize[0]<< " " <<  p[1]*voxelSize[1] << " " << p[2]*voxelSize[2]  << endl ;
	else if( i == 1 && lr == "Right")
// 	  osR1 << Point3df( p[0]*voxelSize[0], p[1]*voxelSize[1], p[2]*voxelSize[2] ) << endl ;
	  osR1 << p[0]*voxelSize[0]<< " " <<  p[1]*voxelSize[1] << " " << p[2]*voxelSize[2]  << endl ;
	else if( i == 0 )
// 	  osL0 << Point3df( p[0]*voxelSize[0], p[1]*voxelSize[1], p[2]*voxelSize[2] ) << endl ;
	  osL0 << p[0]*voxelSize[0]<< " " <<  p[1]*voxelSize[1] << " " << p[2]*voxelSize[2]  << endl ;
	else
// 	  osL1 << Point3df( p[0]*voxelSize[0], p[1]*voxelSize[1], p[2]*voxelSize[2] ) << endl ;
	  osL1 << p[0]*voxelSize[0]<< " " <<  p[1]*voxelSize[1] << " " << p[2]*voxelSize[2]  << endl ;
	

	if( mainLines[i][lr == "Right"].empty() ){
	  mainLines[i][lr == "Right"].push_back(Point3df(p[0]*voxelSize[0], p[1]*voxelSize[1], p[2]*voxelSize[2] ) ) ;
	  //mainLines[i][lr == "Right"].push_back(p) ;
	  areas[i][lr == "Right"].push_back(areaUnNorm) ;
	  areasNorm[i][lr == "Right"].push_back(area) ;
	  curvAbs[i][lr == "Right"].push_back(curv) ;
	  curvatures[i][lr == "Right"].push_back(curvature) ;
	  scales[i][lr == "Right"].push_back(smallestByGreatestAxis) ;
	  invariants[i][lr == "Right"].push_back(invariant) ;	  
	} else if( p != mainLines[i][lr == "Right"][mainLines[i][lr == "Right"].size()-1] ){
	  mainLines[i][lr == "Right"].push_back(Point3df(p[0]*voxelSize[0], p[1]*voxelSize[1], p[2]*voxelSize[2] )) ;
	  //mainLines[i][lr == "Right"].push_back(p) ;
	  areasNorm[i][lr == "Right"].push_back(area) ;
	  areas[i][lr == "Right"].push_back(areaUnNorm) ;
	  curvAbs[i][lr == "Right"].push_back(curv) ;
	  curvatures[i][lr == "Right"].push_back(curvature) ;
	  scales[i][lr == "Right"].push_back(smallestByGreatestAxis) ;
	  invariants[i][lr == "Right"].push_back(invariant) ;
	}
      }
    }
  
  os.close() ;
  osL0.close() ;
  osL1.close() ;
  osR0.close() ;
  osR1.close() ;
  
  //read Apc File
    cout << "APC vect size " << apcs.size() << endl ;
  for(unsigned int i = 0 ; i < apcfiles.size() ; ++i) {
    ifstream isapc(apcfiles[i].c_str()) ;
    string val ;
    isapc >> val ;
    while( val != "ACmm:" )
      isapc >> val ;
    isapc >> apcs[i][0][0] >> apcs[i][0][1] >> apcs[i][0][2] 
	  >> val >> apcs[i][1][0] >> apcs[i][1][1] >> apcs[i][1][2] 
	  >> val >> apcs[i][2][0] >> apcs[i][2][1] >> apcs[i][2][2] ;
    isapc.close() ;
    apcs[i][2] = apcs[i][1] ;
    cout << "AC = " << apcs[i][0] << "\tPC = " << apcs[i][1] << "\tPIH = " << apcs[i][2] << endl ;
  }
  
  cout << "APC file read" << endl ;
  
  
  if(refSubject == -1)
    refSubject = 0 ;
  for( unsigned int i = 0 ; i < subjects.size() ; ++i )
    cout << i << " " << subjects[i] << endl ;

  ofstream osforxl( "xlformated.txt" ) ;
  for( unsigned int i = 0 ; i < mainLines.size()  ; ++i )
    for( unsigned int l = 0 ; l < 2 ; ++l ) {
      osforxl << subjects[i] << ( l == 0 ? " Gauche" : " Droit") ;
      for( unsigned int pt = 0 ; pt < mainLines[i][l].size() ; ++pt )
	osforxl << " " << curvAbs[i][l][pt] ;
      osforxl << endl ;
      osforxl << subjects[i] << ( l == 0 ? " Gauche" : " Droit") ;
      for( unsigned int pt = 0 ; pt < mainLines[i][l].size() ; ++pt )
	osforxl << " " << areas[i][l][pt] ;
      osforxl << endl ;
    }      
  osforxl.close();
  
  cout << "Ref subject " << refSubject << endl ;

//   vector<Motion> motionsToRef(mainLines.size()) ;
//   for(unsigned int i = 0 ; i < motionToRef.size() ; ++i ){
//     int lastSlash = motionToRef[i].find_last_of("/") ;
//     string name = motionToRef[i].substr( lastSlash+1, 10 ) ;
//     int found = -1 ;
//     for(unsigned int j = 0 ; j < subjects.size() ; ++j ){
//       if( subjects[j] == name )
// 	found = j ;
//       if( found != -1 ){
// 	MotionReader mr(motionToRef[i]) ;
// 	mr.read(motionsToRef[j]) ;
// 	cout << motionToRef[i]<< endl << endl ;
//       }
//     }
//   }
  vector<Motion> motionsToRef(mainLines.size()) ;
  cout << "MLSize = " << mainLines.size() << "\tMot size = "<< motionToRef.size() << endl ;
  for(unsigned int i = 0 ; i < motionToRef.size() ; ++i ){
    MotionReader mr(motionToRef[i]) ;
    cout << motionToRef[i]<< endl ;
    mr.read(motionsToRef[i]) ;
    cout << motionToRef[i]<< endl << endl ;
  }

  
  
  vector<Point3df> symYDir( mainLines.size() ) ;
  vector<Point3df> symZDir( mainLines.size() ) ;
  for( unsigned int i = 0 ; i < symYDir.size() ; ++i ){
    Motion mot = motionsToRef[i] ;
    mot.translation() = Point3df(0., 0., 0.) ;
    cout << subjects[i] << endl ;
    cout << "Motion to ref : " << motionsToRef[i] << endl ;
    cout << "Mot : " << mot << endl ;
    Point3df y(0., 1., 0.) ;
    symYDir[i] = mot.transform(y) ;
    symYDir[i].normalize() ;
    Point3df z(0., 1., 0.) ;
    symZDir[i] = mot.transform(z) ;
    symZDir[i].normalize() ;
  }

  vector<Graph*> striata(striatafiles.size()) ;
//   for(unsigned int i = 0 ; i < striatafiles.size() ; ++i ){
//     int lastSlash = striatafiles[i].find_last_of("/") ;
//     string name = striatafiles[i].substr( lastSlash+1, 10 ) ;
//     int found = -1 ;
//     for(unsigned int j = 0 ; j < subjects.size() ; ++j ){
//       if( subjects[j] == name )
// 	found = j ;
//       if( found != -1 ){
// 	striata[j] = new Graph ;
// 	Reader<Graph> gr(striatafiles[i]) ;
// 	gr.read(*(striata[j])) ;
// 	//striata[j] = g ;
//       }
//     }
//   }
  
  vector<AimsData<float> > striataMask( striata.size() ) ;
//   for( unsigned int s = 0 ; s < striata.size() ; ++s ){
//     vector<int> bmMin ;
//     vector<int> bmMax ;
//     if( !striata[s]->getAttribute( "boundingbox_min", bmMin ) ) ;
//     if( !striata[s]->getAttribute( "boundingbox_max", bmMax ) ) ;
//     vector<float> voxelSize(4) ;
//     if( !striata[s]->getAttribute( "voxel_size", voxelSize ) ) cerr << "no voxel size" << endl ;
    
    
//     list< pair< string, int> > names ;
//     names.push_back(pair< string, int>("Left_caudate-nucleus(LCd)", LEFTCAUDATE ) ) ;
//     names.push_back(pair< string, int>("Left_caudate-nucleus_head(LCdh)", LEFTCAUDATEHEAD ) ) ;
//     names.push_back(pair< string, int>("Left_caudate-nucleus_tail(LCdt)", LEFTCAUDATETAIL ) ) ;
//     names.push_back(pair< string, int>("Left_putamen-nucleus(LPu)", LEFTPUTAMEN ) ) ;
//     names.push_back(pair< string, int>("Left_ventral-striatum(LVs)", LEFTVENTRALSTRIATUM ) ) ;
//     names.push_back(pair< string, int>("Right_caudate-nucleus(RCd)", RIGHTCAUDATE ) ) ;
//     names.push_back(pair< string, int>("Right_caudate-nucleus_head(RCdh)", RIGHTCAUDATEHEAD ) ) ;
//     names.push_back(pair< string, int>("Right_caudate-nucleus_tail(RCdt)", RIGHTCAUDATETAIL ) ) ;
//     names.push_back(pair< string, int>("Right_putamen-nucleus(RPu)", RIGHTPUTAMEN ) ) ;
//     names.push_back(pair< string, int>("Right_ventral-striatum(RVs)", RIGHTVENTRALSTRIATUM ) ) ;
    
//     striataMask[s] = AimsData<float>( (bmMax[0]-bmMin[0]), (bmMax[1]-bmMin[1]),
// 				      (bmMax[2]-bmMin[2]) ) ;
//     striataMask[s].setSizeXYZT(voxelSize[0], voxelSize[1], voxelSize[2], 1.0) ;
//     striataMask[s] = 0. ;
//     for( list< pair< string, int> >::const_iterator nit = names.begin() ; 
// 	 nit != names.end() ; ++nit ){
//       set<Vertex*> structure = striata[s]->getVerticesWith( "name", nit->first ) ;
      
//       for( set<Vertex*>::iterator it = structure.begin() ; it != structure.end() ; ++it ){
// 	rc_ptr<BucketMap<Void> >		bck ;
// 	(*it)->getAttribute( "aims_roi", bck ) ;
// 	BucketMap<Void>::Bucket::iterator	ibk( (*bck)[0].begin( )), ebk( (*bck)[0].end() ) ;
	
// 	while( ibk != ebk ){
// 	  // On remplit le masque de notre sujet pour une région donnée
// 	  striataMask[s]( (ibk->first)[0], (ibk->first)[1], (ibk->first)[2] ) 
// 	    = (nit->first.substr(0, 4) == "Left" ) ;
// 	  ++ibk ;
// 	}
//       }
//     }
//     Writer< AimsData<float> > stW("striata" + subjects[s] ) ;
//     stW.write(striataMask[s]) ;
//     delete striata[s] ;
//     striataMask[s] = AimsData<float>(0) ;
//   }  

  // Reechantillonnage
  // Find Origin
      
  for(unsigned int i = 0 ; i < curvAbs.size() ; ++i )
    for(unsigned int l = 0 ; l < 2 ; ++l){
      int ind = 0 ;
      while( curvAbs[i][l][ind] < 0. )
	++ind ;
      origins[i][l] = ind ;
    }
  
  originsNorm = origins ;
  normCurvAbs = curvAbs ;
  
  //Correct Origin for area
  for(unsigned int i = 0 ; i < curvAbs.size() ; ++i )
    for(unsigned int l = 0 ; l < 2 ; ++l){
       int minAreaInd = origins[i][l] ;
       int minAreaNormInd = origins[i][l] ;
       for( int corr = -10 ; corr <= 10 ; ++corr ){
	 if( origins[i][l] + corr >= 0 && origins[i][l] + corr < int(curvAbs[i][l].size()) ){
	   if( areas[i][l][origins[i][l] + corr] < areas[i][l][minAreaInd] )
	     minAreaInd = origins[i][l] + corr ;
	   if( areasNorm[i][l][origins[i][l] + corr] < areasNorm[i][l][minAreaNormInd] )
	     minAreaNormInd = origins[i][l] + corr ;
	 }
       }
      float corrAreaAbs = curvAbs[i][l][minAreaInd] ;
      float corrAreaNormAbs = curvAbs[i][l][minAreaNormInd] ;
      for( unsigned int c = 0 ; c < curvAbs[i][l].size() ; ++c){
	normCurvAbs[i][l][c] = curvAbs[i][l][c] - corrAreaNormAbs ;
	curvAbs[i][l][c] = curvAbs[i][l][c] - corrAreaAbs ;
      }
      cout << endl << endl ;
      
      origins[i][l] = minAreaInd ;
      originsNorm[i][l] = minAreaNormInd ;
      cout << subjects[i] << " " << l << "\tOrig = " << origins[i][l] << " and for norm area " 
	   << originsNorm[i][l] << endl ;
      if( areas[i][l][minAreaInd] > areas[i][l][minAreaInd-1] || 
	  areas[i][l][minAreaInd] > areas[i][l][minAreaInd-2] || 
	  areas[i][l][minAreaInd] > areas[i][l][minAreaInd+1] || 
	  areas[i][l][minAreaInd] > areas[i][l][minAreaInd+2] ){
	cerr << "NOT THE AREA MIN !!!!!!!!!!" << endl ;
	return 0 ;
      }
      if( areasNorm[i][l][minAreaNormInd] > areasNorm[i][l][minAreaNormInd-1] || 
	  areasNorm[i][l][minAreaNormInd] > areasNorm[i][l][minAreaNormInd-2] || 
	  areasNorm[i][l][minAreaNormInd] > areasNorm[i][l][minAreaNormInd+1] || 
	  areasNorm[i][l][minAreaNormInd] > areasNorm[i][l][minAreaNormInd+2] ){
	cerr << "NOT THE AREA NORM MIN !!!!!!!!!!" << endl ;
	return 0 ;
      }
    }

  
  

  //Project pc on y symetry axis
  vector< AimsVector<Point3df, 3> > apcsProj( apcs ) ;
  for( unsigned int i = 0 ; i < mainLines.size()  ; ++i )
    apcsProj[i][1] = apcsProj[i][0] + symYDir[i] * (apcsProj[i][1]-apcsProj[i][0]).dot(symYDir[i]) 
      + symZDir[i] * (apcsProj[i][1]-apcsProj[i][0]).dot(symZDir[i]) ;
  
  cout << "APC projected"<< endl ;
  
  vector<AimsVector<vector<float>, 2> > nCuAbs = 
    normalizeWithAC( mainLines, curvAbs, origins,
		     apcsProj, subjects, refSubject) ;
  
  vector<AimsVector<vector<float>, 2> > nCuAbsNorm = 
    normalizeWithAC( mainLines, normCurvAbs, origins,
		     apcsProj, subjects, refSubject) ;

  vector<AimsVector<AimsVector<vector<Point3df>, 2>, 2> > mainLinesEchant( mlfiles.size() ) ;
  vector<AimsVector<AimsVector<vector<float>, 2>, 2> > areaEchant( mlfiles.size() ) ;
  vector<AimsVector<AimsVector<vector<float>, 2>, 2> > areaNormEchant( mlfiles.size() ) ;
  vector<AimsVector<AimsVector<vector<float>, 2>, 2> > curvatureEchant( mlfiles.size() ) ;
  vector<AimsVector<AimsVector<vector<float>, 2>, 2> > scaleEchant( mainLines.size() ) ;
  vector<AimsVector<AimsVector<vector<vector<float> >, 2>, 2> > invariantEchant( mainLines.size() ) ;
  
  resample(  mainLines, nCuAbs, curvatures, areas, scales, invariants, origins, 0,
	     mainLinesEchant, curvatureEchant,  areaEchant, scaleEchant, invariantEchant ) ;
  resample(  mainLines, nCuAbsNorm, curvatures, areasNorm, scales, invariants, originsNorm, 0,
	     mainLinesEchant, curvatureEchant,  areaNormEchant, scaleEchant, invariantEchant ) ;
  
  
//   vector<AimsVector<vector<float>, 2> > nCuAbs = normalizeWithAC( mainLines, curvAbs, 
// 								  origins, apcs, 
// 								  subjects, refSubject ) ;
  
//   for(unsigned int i = 0 ; i < curvAbs.size() ; ++i )
//     for(unsigned int l = 0 ; l < 2 ; ++l){
//       for( int dc = -3 ; dc <= 3 ; ++dc )
// 	cout << "\t" <<  normCurvAbs[i][l][ originsNorm[i][l] + dc ] ;
//       cout << endl ;
//     }
  
//   vector<AimsVector<vector<float>, 2> > nCuAbsNorm = normalizeWithAC( mainLines, normCurvAbs, 
// 								      originsNorm, apcs, 
// 								      subjects, refSubject ) ;
    
//   for(unsigned int i = 0 ; i < curvAbs.size() ; ++i )
//     for(unsigned int l = 0 ; l < 2 ; ++l)
//       {
// 	Point2d maxInd = areaMax( areas[i][l], originsNorm[i][l] ) ;
// 	cout << subjects[i] << "\t" << ( l==0 ? "Left" : "Right" ) 
// 	     << "\tMax In Putamen at" << nCuAbsNorm[i][l][maxInd[0]]
// 	     << "\tMax In Caudate at" << nCuAbsNorm[i][l][maxInd[1]] << endl ;
//       }
  
//   cout << "CurvAbs normalized"<< endl ;
//   resample(  mainLines, nCuAbs, curvatures, areas, scales, invariants, origins, 0,
// 	     mainLinesEchant, curvatureEchant,  areaEchant, scaleEchant, invariantEchant ) ;
//   resample(  mainLines, nCuAbsNorm, curvatures, areasNorm, scales, invariants, originsNorm, 0,
// 	     mainLinesEchant, curvatureEchant,  areaNormEchant, scaleEchant, invariantEchant ) ;

  for(unsigned int i = 0 ; i < curvAbs.size() ; ++i )
    for(unsigned int l = 0 ; l < 2 ; ++l)
      if( areaNormEchant[i][l][1][0] > areaNormEchant[i][l][0][0] || 
	  areaNormEchant[i][l][1][0] > areaNormEchant[i][l][0][1] || 
	  areaNormEchant[i][l][1][0] > areaNormEchant[i][l][1][1] || 
	  areaNormEchant[i][l][1][0] > areaNormEchant[i][l][1][2] ){
	cerr << "NOT THE AREA NORM MIN AFTER RESAMPLE !!!!!!!!!! For " << subjects[i] << " " << (l==0? "Left" : "Right") << endl ;
	return 0 ;
      }
  for(unsigned int i = 0 ; i < curvAbs.size() ; ++i )
    for(unsigned int l = 0 ; l < 2 ; ++l)
      if( areaEchant[i][l][1][0] > areaEchant[i][l][0][0] || 
	  areaEchant[i][l][1][0] > areaEchant[i][l][0][1] || 
	  areaEchant[i][l][1][0] > areaEchant[i][l][1][1] || 
	  areaEchant[i][l][1][0] > areaEchant[i][l][1][2] ){
	cerr << "NOT THE AREA MIN AFTER RESAMPLE !!!!!!!!!! For " << subjects[i] << " " << (l==0? "Left" : "Right") << endl ;
	return 0 ;
      }
	//   int maxCaudateSize = 0 ;
//   for( unsigned int i = 0 ; i < mainLines.size()  ; ++i )
//     for( unsigned int l = 0 ; l < 2 ; ++l )
//       if( areaEchant[i][l][1].size() > maxCaudateSize )
// 	maxCaudateSize = areaEchant[i][l][1].size() ;

//   vector<AimsVector<AimsVector<vector<float>, 2>, 2> > areaNormEchant2 = 
//     correctZeroAreaValue( areaNormEchant, maxCaudateSize ) ;
//   vector<AimsVector<AimsVector<vector<float>, 2>, 2> > areaEchant2 = 
//     correctZeroAreaValue( areaEchant, maxCaudateSize ) ;
  
  
//   for( unsigned int i = 0 ; i < mainLines.size()  ; ++i )
//     for( unsigned int l = 0 ; l < 2 ; ++l )
//       for( int c = 0 ; c < maxCaudateSize ; ++c )
// 	cout << "-----" << ( c < maxCaudateSize ? areaEchant[i][l][1][c] : 0.) 
// 	     << "\t" << areaEchant2[i][l][1][c] << endl ;
  

//   for( unsigned int i = 0 ; i < mainLines.size()  ; ++i )
//     for( unsigned int l = 0 ; l < 2 ; ++l ){
//       cout <<  subjects[i]<< "\t" << l 
// 	   << "\t" << areaNormEchant[i][l][1][areaNormEchant[i][l][1].size()-1]
// 	   << "\t" << areasNorm[i][l][areasNorm[i][l].size()-1] 
// 	   << "\tand " << areaEchant[i][l][1][areaEchant[i][l][1].size()-1] 
// 	   << "\t" << areas[i][l][areas[i][l].size()-1] 
// 	   << endl ;
//     }  
  cout << "CurvAbs resampled"<< endl ;
  for( int subSampleRate = 0 ; subSampleRate < 3 ; subSampleRate += 1 )
    writeCurvesForR( areaEchant, areaNormEchant, curvatureEchant, scaleEchant, invariantEchant,
		     subjects, ages, disease, sex, laterality, 
		     string("normWithAc_Sub") 
		     + toString(subSampleRate), subSampleRate ) ;
  
   
  writeCurves( areaEchant, areaNormEchant, curvatureEchant, scaleEchant, invariantEchant,
	       subjects, ages, disease, sex, laterality, "normWithAc_", "toto.txt" ) ;
  
  vector<AimsVector<Point3df, 3> > newApcs( mainLines.size() );
  vector<float> acpcByBrainLength( mainLines.size() ) ;
  
  for( unsigned int i = 0 ; i < mainLines.size()  ; ++i ){
    Reader< AimsData<short> > reader( brainmaskfiles[i] ) ;
    AimsData<short> mask ;
    reader.read( mask ) ;
    newApcs[i] = getBrainMaskPCSideEtremities( mask, apcsProj[i], acpcByBrainLength[i] ) ;
    newApcs[i][1] = newApcs[i][0] + float(0.5) * ( newApcs[i][1] - newApcs[i][0] ) ;
    cout << apcs[i] << " \tand\t" <<  newApcs[i] << endl ;
  }
  acpcSizeTest( acpcByBrainLength, disease) ;

  vector<AimsVector<vector<float>, 2> > nBrainCuAbs = normalizeWithAC( mainLines, curvAbs, 
								       origins, newApcs, 
								       subjects, refSubject ) ;
  vector<AimsVector<vector<float>, 2> > nBrainCuAbsNorm = normalizeWithAC( mainLines, curvAbs, 
								       origins, newApcs, 
								       subjects, refSubject ) ;
  resample(  mainLines, nBrainCuAbs, curvatures, areas, scales, invariants, origins, 0,
	     mainLinesEchant, curvatureEchant,  areaEchant, scaleEchant, invariantEchant ) ;
  resample(  mainLines, nBrainCuAbsNorm, curvatures, areasNorm, scales, invariants, originsNorm, 0,
	     mainLinesEchant, curvatureEchant,  areaNormEchant, scaleEchant, invariantEchant ) ;

  for( int subSampleRate = 0 ; subSampleRate < 3 ; ++subSampleRate )
    writeCurvesForR( areaEchant, areaNormEchant, curvatureEchant, scaleEchant, invariantEchant,  
		     subjects, ages, disease, sex, laterality, 
		     string("normWithBrainAc_Sub") + toString(subSampleRate), subSampleRate ) ;
  
  
  for( unsigned int i = 0 ; i < mainLines.size()  ; ++i )
    for( unsigned int l = 0 ; l < 2 ; ++l ){
      cout << endl << endl << subjects[i] << " " << ( l == 0 ? "Left" : "Right" ) << endl ;
      for( unsigned int c = 0 ; c < mainLines[i][l].size() ; ++c )
	cout << "With ACPC : \t" << nCuAbs[i][l][c] 
	     << "\tand with brain extremity : " << nBrainCuAbs[i][l][c] << endl ;
    }
  
  writeCurvAbsImages( nCuAbsNorm, mainLines, brainmaskfiles, refSubject ) ;
 
  return 0 ;



















































  cout << "Origins Corrected" << endl ;
  vector<Point3df> ap(apcfiles.size()) ;
  for( unsigned int i = 0 ; i < apcfiles.size() ; ++i ){
    ap[i] = apcs[i][1] - apcs[i][0] ;
    ap[i].normalize() ;
    cout << "AP " << i << " : " << ap[i] << endl ;
  }

  for( unsigned int i = 0 ; i < apcfiles.size() ; ++i ){
    cout<< apcs[i][1] - apcs[i][0] ;
    ap[i].normalize() ;
    cout << "AP " << i << " : " << ap[i] << endl ;
  }
  
  cout << "ACPC computed" << endl ;
  
  vector<AimsVector<AimsVector<float, 2>, 2 > > acPlaneCurvAbs( apcfiles.size() ) ;
  
    
  for( unsigned int i = 0 ; i < mainLines.size()  ; ++i ){
    float d = ap[i][0] * apcs[i][0][0] + ap[i][1] *apcs[i][0][1] + ap[i][2] *apcs[i][0][2] ;
    for( unsigned int l = 0 ; l < 2 ; ++l ){
//       cout << subjects[i] << " " << ( l == 0 ? "Left" : "Right" ) << endl ;
      float originSide = ap[i][0] * mainLines[i][l][origins[i][l]][0] 
	+ ap[i][1] * mainLines[i][l][origins[i][l]][1] 
	+ ap[i][2] * mainLines[i][l][origins[i][l]][2] - d ;
      int cCurr = origins[i][l] ;
      while( cCurr >= 0 ){
// 	cout << ap[i][0] * mainLines[i][l][cCurr][0] 
// 	  + ap[i][1] * mainLines[i][l][cCurr][1] 
// 	  + ap[i][2] * mainLines[i][l][cCurr][2] - d << " against " << originSide 
// 	     << " and for points " << mainLines[i][l][cCurr] << "with ac " << apcs[i][0] << endl ;
	if( ( ap[i][0] * mainLines[i][l][cCurr][0] 
	      + ap[i][1] * mainLines[i][l][cCurr][1] 
	      + ap[i][2] * mainLines[i][l][cCurr][2] - d ) * originSide < 0 )
	  break ;
	--cCurr ;
      }
      if( cCurr < 0 ){
	cout << subjects[i] << " " << ( l == 0 ? "Left" : "Right" ) 
	     << " putamen is beyond ac plane !"<< endl ;
	//CALCUL
      }else{
	cout << subjects[i] << " " << ( l == 0 ? "Left" : "Right" ) << endl 
	     << "Pour le putamen " << curvAbs[i][l][cCurr] << " sur " << curvAbs[i][l][0] << endl ;
	
	Point3df q(mainLines[i][l][cCurr]), p(mainLines[i][l][cCurr+1]) ;
	Point3df acp = apcs[i][0] - p ;
	Point3df pq = q - p ;
	float alpha = ap[i][0] * acp[0] + ap[i][1] * acp[1] + ap[i][2] * acp[2] ;
	alpha /=  ap[i][0] * pq[0] + ap[i][1] * pq[1] + ap[i][2] * pq[2] ;
	cout << alpha << endl ;
	acPlaneCurvAbs[i][l][0] = (1 - alpha) * curvAbs[i][l][cCurr+1] + alpha * curvAbs[i][l][cCurr] ;
      }
      
      cCurr = origins[i][l] ;
      while( cCurr < int(mainLines[i][l].size()) ) {
// 	cout << ap[i][0] * mainLines[i][l][cCurr][0] 
// 	  + ap[i][1] * mainLines[i][l][cCurr][1] 
// 	  + ap[i][2] * mainLines[i][l][cCurr][2] - d << " against " << originSide 
// 	     << " and for points " << mainLines[i][l][cCurr] << "with ac " << apcs[i][0] << endl ;
	if( ( ap[i][0] * mainLines[i][l][cCurr][0] 
	      + ap[i][1] * mainLines[i][l][cCurr][1] 
	      + ap[i][2] * mainLines[i][l][cCurr][2] - d ) * originSide < 0 )
	  break ;
	++cCurr ;
      }
      //BlaBla
      if( cCurr >= int(mainLines[i][l].size()) ){
	//CALCUL
	cout << subjects[i] << " " << ( l == 0 ? "Left" : "Right" ) << " caudate is beyond ac plane !"<< endl ;
      }else{
	cout << subjects[i] << " " << ( l == 0 ? "Left" : "Right" ) << endl 
	     << "Pour le caudé " << curvAbs[i][l][cCurr] << " sur " 
	     << curvAbs[i][l][curvAbs[i][l].size()-1] << endl ;

	Point3df q(mainLines[i][l][cCurr]), p(mainLines[i][l][cCurr-1]) ;
	Point3df acp = apcs[i][0] - p ;
	Point3df pq = q - p ;
	float alpha = ap[i][0] * acp[0] + ap[i][1] * acp[1] + ap[i][2] * acp[2] ;
	alpha /=  ap[i][0] * pq[0] + ap[i][1] * pq[1] + ap[i][2] * pq[2] ;
	cout << alpha << endl ;
	acPlaneCurvAbs[i][l][1] = (1 - alpha) * curvAbs[i][l][cCurr-1] + alpha * curvAbs[i][l][cCurr] ;
      }
    }
  }

  for( unsigned int i = 0 ; i < mainLines.size()  ; ++i )
    for( unsigned int l = 0 ; l < 2 ; ++l ){
      cout << subjects[i] << " " << ( l == 0 ? "Left" : "Right" ) << endl 
	   << "Pour le putamen " << acPlaneCurvAbs[i][l][0] << " sur " << curvAbs[i][l][0] << endl
	   << "Pour le caudé " << acPlaneCurvAbs[i][l][1] << " sur " 
	   << curvAbs[i][l][curvAbs[i][l].size()-1] << endl ;
    }

  
  for( unsigned int i = 0 ; i < mainLines.size()  ; ++i )
    for( unsigned int l = 0 ; l < 2 ; ++l ){
      normCurvAbs[i][l].reserve( curvAbs[i][l].size() ) ;
      for( int c = 0 ; c < int(curvAbs[i][l].size()) ; ++c ){
	normCurvAbs[i][l].push_back( ( curvAbs[i][l][c] * 
				      acPlaneCurvAbs[refSubject][0][ c < origins[refSubject][l] ] /
				      acPlaneCurvAbs[i][l][ c < origins[refSubject][l] ] ) ) ;
      }
    }

  resample(  mainLines, curvAbs, curvatures, areas, scales, invariants, origins, 0,
	     mainLinesEchant, curvatureEchant,  areaEchant, scaleEchant, invariantEchant ) ;
  resample(  mainLines, curvAbs, curvatures, areasNorm, scales, invariants, originsNorm, 0,
	     mainLinesEchant, curvatureEchant,  areaNormEchant, scaleEchant, invariantEchant ) ;
  
  printAreas( subjects, areaEchant, "areasByACNorm.txt" ) ;
  
  return 0 ;
  
  vector<Motion> motionsToLeftRef ;
  registerAndNorm( mainLines, curvAbs, areasNorm, areas, curvatures, scales, invariants, 
		   origins, motionsToLeftRef, subjects, refSubject ) ;
  for( unsigned int i = 0 ; i < mainLines.size() ; ++i ){
    MotionWriter motWri(subjects[i] + string("_TO_") + subjects[refSubject] + string(".trm") ) ;
    motWri.write( motionsToLeftRef[i]) ;
  }
  
//   AimsVector<Motion, 2> lrMotion ;
//   lrMotion[0].setToIdentity() ;
//   lrMotion[1].setToIdentity() ;
//   lrMotion[1].rotation()(0, 0) = -1. ;

//   vector<AimsVector<Motion, 2> > motionsToLeftRef ;
//   //curvAbs = normalizeCurvAbs( areas, curvAbs, origins, refSubject ) ;
//   vector<AimsVector<vector<float>, 2> > newCurvAbs(curvAbs) ;
//   for( int i = 0 ; i < motionsToRef.size() ; ++i ){
//     motionsToLeftRef[i][0] = motionsToRef[i] ;
//     motionsToLeftRef[i][1] = ;
//   }
//   vector<AimsVector<vector<Point3df>, 2> > tangents = getTangents(mainLines, motionsToRef) ;
//   newCurvAbs = normalizeCurvAbsByTangent( tangents, newCurvAbs, origins, refSubject ) ;
//    // boucle de recalage et de normalisation de l'abscisse curviligne
//   for( int iterate = 0 ; iterate < 10 ; ++iterate ){
//     cout << "Iteration " << iterate << endl ;
//     registerML( mainLines, newCurvAbs, areasNorm, areas, curvatures, origins, motionsToLeftRef, refSubject ) ;
//     vector<AimsVector<vector<Point3df>, 2> > tangents = getTangents(mainLines, motionsToLeftRef) ;
//     newCurvAbs = normalizeCurvAbsByTangent( tangents, newCurvAbs, origins, refSubject ) ;
//   }
  
//   resample( mainLines, newCurvAbs, curvatures, areas, origins, 0,
// 	    mainLinesEchant, curvatureEchant,  areaEchant ) ;
  

//   printAreas( subjects, areaEchant, "areaForExcellNormByRefined.txt" ) ;
      
  return 0 ;

  //Rechantillonnage
  for(int orig = 0 ; orig <= 0 ; ++orig ){    
    resample(  mainLines, curvAbs, curvatures, areas, scales, invariants, origins, 0,
	       mainLinesEchant, curvatureEchant,  areaEchant, scaleEchant, invariantEchant ) ;
    resample(  mainLines, curvAbs, curvatures, areasNorm, scales, invariants, origins, 0,
	       mainLinesEchant, curvatureEchant,  areaNormEchant, scaleEchant, invariantEchant ) ;
        
    printAreas( subjects, areaEchant, "areaForExcellNormByMax.txt" ) ;
    
    // -------------------- OUTPUT FOR R -----------------------------------
    unsigned int maxSizeNeg = 0, maxSizePos = 0 ;
    
    for( int unsigned n = 0 ; n < areaEchant.size() ; ++n ){
      if( areaEchant[n][0][0].size() > maxSizeNeg )
	maxSizeNeg = areaEchant[n][0][0].size() ;
      if( areaEchant[n][0][1].size() > maxSizePos )
	maxSizePos = areaEchant[n][0][1].size() ;
      if( areaEchant[n][1][0].size() > maxSizeNeg )
	maxSizeNeg = areaEchant[n][1][0].size() ;
      if( areaEchant[n][1][1].size() > maxSizePos )
	maxSizePos = areaEchant[n][1][1].size() ;
    }
    
    
    ofstream osforR( "xlformated.txt" ) ;
    
    AimsData<float> curvatureImg( curvatures.size(), maxSizeNeg + maxSizePos + 2, 2) ;
    AimsData<float> areaImg( curvatures.size(), maxSizeNeg + maxSizePos + 2, 2) ;
    AimsData<float> curvAbsImg( curvatures.size(), maxSizeNeg + maxSizePos + 2, 2) ;
    AimsData<Point3df> linesImg( curvatures.size(), 2, maxSizeNeg + maxSizePos + 2) ;
    linesImg = Point3df(0., 0., 0.) ;
    AimsData<float> curvIndexRatioImg( curvatures.size(), maxSizeNeg + maxSizePos + 2, 2) ;
        
    for( unsigned int i = 0 ; i < areaEchant.size()  ; ++i )
      for( unsigned int l = 0 ; l < 2 ; ++l ) {
	osforR << subjects[i] ;
	
	for( int cu = maxSizeNeg - 1 ; cu >=0 ; --cu )
	  // 	for( unsigned int pt = 0 ; pt < mainLines[i][l].size() ; ++pt )
	  if( cu >= int(areaEchant[i][l][0].size()) )
	    osforR << " " << 0. ;
	  else{
	    osforR << " " << areaEchant[i][l][0][cu] ;
	    curvatureImg( i, maxSizeNeg - 1 - cu, l ) = curvatureEchant[i][l][0][cu] ;
	    areaImg( i, maxSizeNeg - 1 - cu, l ) = areaEchant[i][l][0][cu] ;
	    curvAbsImg( i, maxSizeNeg - 1 - cu, l ) = -cu ;
	    linesImg( i, l, maxSizeNeg - 1 - cu ) = mainLinesEchant[i][l][0][cu] ;
	  }
	for( unsigned int cu = 0 ; cu < maxSizePos ; ++cu )
	  // 	for( unsigned int pt = 0 ; pt < mainLines[i][l].size() ; ++pt )
	  if( cu >= areaEchant[i][l][1].size() )
	    osforR << " " << 0. ;
	  else{
	    osforR << " " << areaEchant[i][l][1][cu] ;
	    curvatureImg( i, maxSizeNeg + cu, l ) = curvatureEchant[i][l][1][cu] ;
	    areaImg( i, maxSizeNeg + cu, l ) = areaEchant[i][l][1][cu] ;
	    curvAbsImg( i, maxSizeNeg + cu, l ) = cu ;
	    linesImg( i, l, maxSizeNeg + cu ) = mainLinesEchant[i][l][1][cu] ;
	  }
	osforR << ( l == 0 ? " Gauche" : " Droit") << " " << disease[i] << " " << ages[i] 
	       << " " << sex[i] << " " << laterality[i] << endl ;
      }      
    osforR.close();
    Writer< AimsData<float> > curvWri("curvatureImage.ima") ;
    curvWri.write( curvatureImg ) ;
    Writer< AimsData<float> > curvAbsWri("curvilinearAbscissaImage.ima") ;
    curvAbsWri.write( curvAbsImg ) ;
    Writer< AimsData<float> > areaWri("areaImage.ima") ;
    areaWri.write( areaImg ) ;

    cout << "FINISHED FOR R" << endl ;
    
    // -------------------- OUTPUT FOR R -----------------------------------

    return 0 ;
    
    cout << areaEchant[0][0][0].size() << "\t" << areaEchant[1][0][0].size() << "\t" 
	 << areaEchant[0][0][1].size() << "\t" << areaEchant[1][0][1].size() << "\t" 
	 << areaEchant[0][1][0].size() << "\t" << areaEchant[1][1][0].size() << "\t" 
	 << areaEchant[0][1][1].size() << "\t" << areaEchant[1][1][1].size() << endl ; 
    vector<Motion> motions( mlfiles.size() ) ;
    for(unsigned int i = 0 ; i < mlfiles.size() ; ++i )
      if( int (i) == refSubject )
	motions[i].setToIdentity() ;
      else{	
	vector<Point3df> ref, other ;
	for(unsigned int l = 0 ; l < 2 ; ++l )
	  {
	    BucketMap<Void> linkBetween ;
	    linkBetween.setSizeXYZT( voxelSize[0],voxelSize[1], voxelSize[2], 1.0) ;
	    
	    for( unsigned int p = 0 ; p < mainLinesEchant[i][l][0].size()
		   && p < mainLinesEchant[refSubject][l][0].size() ; ++p )
	      if( areaEchant[i][l][0][p] > 0. && areaEchant[refSubject][l][0][p] > 0. ){
		ref.push_back(mainLinesEchant[refSubject][l][0][p]) ;
		other.push_back(mainLinesEchant[i][l][0][p]) ;

		//-------------- TEST -----------------------------
		for( float alpha = 0. ; alpha <= 1. ; alpha += 0.05 ){
		  Point3df point = alpha * mainLinesEchant[refSubject][l][0][p] 
		    + (1 - alpha) * mainLinesEchant[i][l][0][p]  ;
		  Point3d pInt( static_cast<int>(point[0] + 0.5), 
				static_cast<int>(point[1] + 0.5), 
				static_cast<int>(point[2] + 0.5) ) ;
		  linkBetween[p][pInt] ;
		}
		//-------------- TEST -----------------------------
	      }
	    for( unsigned int p = 0 ; p < mainLinesEchant[i][l][1].size() 
		       && p < mainLinesEchant[refSubject][l][1].size() ; ++p )
	      if( areaEchant[i][l][1][p] > 0. && areaEchant[refSubject][l][1][p] > 0. ){
		    ref.push_back(mainLinesEchant[refSubject][l][1][p]) ;
		    other.push_back(mainLinesEchant[i][l][1][p]) ;
		    //-------------- TEST -----------------------------
		    for( float alpha = 0. ; alpha <= 1. ; alpha += 0.05 ){
		      Point3df point = alpha * mainLinesEchant[refSubject][l][1][p]
			+ (1 - alpha) * mainLinesEchant[i][l][1][p]  ;
		      Point3d pInt( static_cast<int>(point[0] + 0.5), 
				    static_cast<int>(point[1] + 0.5), 
				    static_cast<int>(point[2] + 0.5) ) ;
		      linkBetween[p][pInt] ;
		    }
		//-------------- TEST -----------------------------
	      }
	    for(unsigned int n = 0 ; n < 5 ; ++n ){
	      other.push_back(mainLines[i][l][origins[i][l]+orig]) ;
	      ref.push_back(mainLines[refSubject][l][origins[refSubject][l]+orig]) ;
	    }
	    
	    if( int(i) != refSubject ){
	      cout << "Size T = " << linkBetween.size() << endl ;
	      Writer< BucketMap<Void> > wriLink( string("linkNoRecal_") + ( l == 0 ? string("Left.bck") : 
									    string("Right.bck") ) ) ;
	      wriLink.write( linkBetween ) ;
	    }
	  }
	AffineLeastSquareEstimation aff( other, ref ) ;
	motions[i] = *(aff.motion()) ;


	if( 0 ){
/****************************************************************************************************************************/
	//Partie Robustesse
	
	// Le tri des residus pour ref et test  
	multimap<float, Point3df> mt;
	multimap<float, Point3df> mr;
  
	multimap<float, Point3df>::iterator itt;
	multimap<float, Point3df>::iterator itr;
  
	// Diverses variables utiles
	Point3df r;		// point residu courant
	float r2;			// norme caree de r
	unsigned indice;		// indice de boucle multiple (variable locale boucle impossible)
	unsigned sizemoitie = unsigned ( double(other.size())*0.6 );

	// La liste des points ref et test ayant les plus petits residus
	vector<Point3df> othermoitie;
	vector<Point3df> refmoitie;
  
  
	for(int boucle=0;boucle<3;boucle++)
	  {
	    mt.clear();mr.clear();
	    othermoitie.clear();refmoitie.clear();

     
  	    for(unsigned int l=0;l<other.size();l++)  
		{
			Point3df x = other[l];
			Point3df y = ref[l];

	  		r = y - motions[i].transform(x);			//calcul du residu pour chaque point
	  		r2 = r.norm();						// sa norme carrée
	  		mt.insert(pair<float, Point3df>(r2, other[l]));		//insertion du residu et du point test dans map	
	  		mr.insert(pair<float, Point3df>(r2, ref[l]));		//insertion du residu et du point test dans map
		}

      
	      // On remplit les listes avec une partie seulement des meilleurs residus      
	      for (itt = mt.begin(),itr = mr.begin(), indice = 0 ;indice<sizemoitie;++itt,++itr,++indice)
		{
	  		othermoitie.push_back( (*itt).second );
	  		refmoitie.push_back( (*itr).second );
		}


	     // Calcul transfo affine robuste...
  	     AffineLeastSquareEstimation leastSquareEstimation_moitie(othermoitie, refmoitie);
	     motions[i] = *(leastSquareEstimation_moitie.motion());
	     
	  }					// fin boucle robustesse		
/****************************************************************************************************************************/
	  	  
	}
      }    
    for( unsigned int mot = 0 ; mot < motions.size() ; ++mot ){
      MotionWriter motWri(string("MLBased_" + subjects[mot] + "_TO_" + subjects[refSubject] + ".trm")) ; 
      motWri.write(motions[mot]) ;
    }

    cout << "Transfo computed" << endl ;
    vector<AimsVector<vector<float>, 2> > newCurvAbs;
//   vector<AimsVector<vector<float>, 2> > newCurvAbs 
//     = normalizeCurvAbsByTangent( tangents, curvAbs, origins, refSubject ) ;

//   resample(  mainLines, newCurvAbs, curvatures, areas, origins, orig,
// 	     mainLinesEchant, curvatureEchant,  areaEchant ) ;
  
//   saveLinkBuckets( mainLinesEchant, areaEchant, refSubject, 
// 		   Point3df( voxelSize[0], voxelSize[1], voxelSize[2] ) ) ;
    
//   printAreas( subjects, areaEchant, "areaForExcellNormByRecal.txt" ) ;
    
    for( unsigned int i = 0 ; i < subjects.size() ; ++i)
      for( int l = 0 ; l < 2 ; ++l ){
	cout << "\tSubject" << subjects[i] << "\t" << (l ? "Right " : "Left ") 
	     << "\tMin old curv abs = " << curvAbs[i][l][0]  
	     << "\tMax old curv abs = " << curvAbs[i][l][newCurvAbs[i][l].size()-1]  
	     << endl ;
	
      }
    for( unsigned int i = 0 ; i < subjects.size() ; ++i)
      for( int l = 0 ; l < 2 ; ++l ){
	cout << "\tSubject" << subjects[i] << "\t" << (l ? "Right " : "Left ") 
	     << "\tMin new curv abs = " << newCurvAbs[i][l][0]  
	     << "\tMax new curv abs = " << newCurvAbs[i][l][newCurvAbs[i][l].size()-1]  
	     << endl ;
	
      }
    
    return 0 ;


    Point3df ptC, ptRef, diff ;
    float norm, leastNorm ;
    int leastIndex ;
    Point2d refIndMin(-1, -1), refIndMax(-1, -1) ;
    Point3df voxelSize ;

    for( int l = 0 ; l < linesImg.dimY() ; ++l )
      for( int pRef  = 0 ; pRef < linesImg.dimZ() ; ++pRef ){
	//cout << "pRef : " << pRef << "\tlref : " <<linesImg(refSubject, l, pRef) << endl ;
	if( linesImg(refSubject, l, pRef) != Point3df(0., 0., 0.) && refIndMin[l] == -1 )
	  refIndMin[l] = pRef ;
	if( linesImg(refSubject, l, pRef) == Point3df(0., 0., 0.) && refIndMin[l] != -1 
	    && refIndMax[l] == -1 )
	  refIndMax[l] = pRef ;
      }
    cout << "Got Boundaries " << refIndMin << "\t"<< refIndMax <<  endl ;
    cout << "dimZ = " << linesImg.dimZ() << endl ;
    for( int i = 0 ; i < linesImg.dimX()  ; ++i ){
      for( int l = 0 ; l < linesImg.dimY() ; ++l ){
	BucketMap<Void> linkBetween ;
	linkBetween.setSizeXYZT( voxelSize[0],voxelSize[1], voxelSize[2], 1.0) ;
	for( int p  = 0 ; p < linesImg.dimZ() ; ++p ){
	  //cout << "i = " << i << "\t l = " << l << "\tp : " << p << endl ;  ;
	  ptC = linesImg(i, l, p) ;
	  //cout << "ptC" << endl ;
	  diff = motions[i].transform( linesImg(i, l, p) ) - linesImg( refSubject, l, refIndMin[l] ) ;
	  //cout << "diff0" << endl ;
	  leastNorm = sqrt(diff.dot(diff)) ;
	  //cout << "leastNorm0" << endl ;
	  leastIndex = refIndMin[l] ;
	  //cout << "leastIndex0" << endl ;
	  if( ptC != Point3df(0., 0., 0.) ){
	    for( int pRef  = refIndMin[l] ; pRef < refIndMax[l] ; ++pRef ){
	      //cout << "pRef = " << pRef << endl ;
	      diff = motions[i].transform(linesImg(i, l, p)) - linesImg(refSubject, l, pRef) ;
	      //cout << "diffi" << endl ;
	      norm = sqrt(diff.dot(diff)) ;
	      //cout << "normi" << endl ;
	  
	      if( norm < leastNorm ){
		leastNorm = norm ;   
		leastIndex = pRef ;
	      }
	    }
// 	    if( i != refSubject )
// 	    cout << p << "\tCurvAbs = " << curvAbsImg(i, p, l) 
// 		 << "\tConn to " << leastIndex << "\t With "<< leastNorm << endl ;
// 	    if( curvAbsImg( i, p, l ) == 0. )
// 	      curvIndexRatioImg(i, p, l) = 1. ;
// 	    else
// 	      curvIndexRatioImg(i, p, l) = curvAbsImg(refSubject, leastIndex, l ) / curvAbsImg(i, p, l) ;
	      curvIndexRatioImg(i, p, l) = curvAbsImg(i, p, l) - curvAbsImg(refSubject, leastIndex, l ) ;
	    

	    //--------------------------------------- Checking
	    if( i != refSubject ){
	      Point3df linesPt = motions[i].transform(linesImg(i, l, p)) ;
	      for( float alpha = 0. ; alpha <= 1. ; alpha += 0.05 ){
		Point3df point = alpha * linesPt + (1 - alpha) * linesImg(refSubject, l, leastIndex) ;
		Point3d pInt( static_cast<int>(point[0] + 0.5), 
			      static_cast<int>(point[1] + 0.5), 
			      static_cast<int>(point[2] + 0.5) ) ;
		linkBetween[p][pInt] ;
	      }
	    }
	  }
	  //cout << "curvInd" << endl ;
	}
	if( i != refSubject ){
	  cout << "Size T = " << linkBetween.size() << endl ;
	  Writer< BucketMap<Void> > wriLink( string("link_") + ( l == 0 ? string("Left.bck") : 
								 string("Right.bck") ) ) ;
	  wriLink.write( linkBetween ) ;
	}
      }
    }    
    Writer< AimsData<float> > curvIndexRatioImgWri( "curvIndexRatioImg.ima") ;
    curvIndexRatioImgWri.write( curvIndexRatioImg ) ;
    
    
    vector<float> matchNGC( striataMask.size(), 0. ) ;
    vector<float> matchML( striataMask.size(), 0. ) ;
    
    LinearResampler<float> linResamp ;
    for( unsigned int s = 0 ; s < striataMask.size() ; ++s ){
      if( int(s) == refSubject ){
	matchNGC[s] = 1. ;
	matchML[s] = 1. ;
      }else{
	AimsData<float> ref, test ;
	if( disease[s] > disease[refSubject] || ( disease[s] == disease[refSubject] 
						  && ages[s] > ages[refSubject] ) ){
	  test = striataMask[refSubject] ;
	  ref = striataMask[s] ;
// 	  ref = striataMask[refSubject] ;
// 	  test = striataMask[s] ;
	}else{
 	  ref = striataMask[refSubject] ;
 	  test = striataMask[s] ;
// 	  test = striataMask[refSubject] ;
// 	  ref = striataMask[s] ;
	}
	Writer< AimsData<float> > refW0("ref0.ima") ;
	refW0.write(ref) ;
	Writer< AimsData<float> > testW0("test0.ima") ;
	testW0.write(test) ;

 	AimsData<float> transfByNGC = test.clone() ; 
	linResamp.setRef(transfByNGC) ;
// 	linResamp.doit( motionsToRef[s], transfByNGC ) ;
	transfByNGC = linResamp.doit( motionsToRef[s].inverse(), 
				      ref.dimX(), ref.dimY(), ref.dimZ(),
				      Point3df(ref.sizeX(), ref.sizeY(), 
					       ref.sizeZ() ) ) ;
	AimsData<float> transfByML = test.clone() ;
	linResamp.setRef(transfByML) ;
	
	transfByML = linResamp.doit( motions[s].inverse(), ref.dimX(), ref.dimY(), ref.dimZ(),
				     Point3df(ref.sizeX(), ref.sizeY(), 
					      ref.sizeZ() )   ) ;
// 	AimsData<float> transfByML = test.clone() ;
// 	linResamp.doit( motions[s], transfByML ) ;
	int x, y, z ;
	long count = 0 ;
	ForEach3d( ref, x, y, z ){
	  if( ref(x, y, z) > 0.5 ){
	    ++count ;
	    matchNGC[s] += transfByNGC(x, y, z) ;
	    matchML[s] += transfByML(x, y, z) ;
	  }
	}
	Writer< AimsData<float> > refW("ref.ima") ;
	refW.write(ref) ;
	Writer< AimsData<float> > testW("test.ima") ;
	testW.write(test) ;
	Writer< AimsData<float> > MLW("transfByML.ima") ;
	MLW.write(transfByML) ;
	Writer< AimsData<float> > NGCW("transfByNGC.ima") ;
	NGCW.write(transfByNGC) ;
	
	matchNGC[s] /= float(count) ;
	matchML[s] /= float(count) ;
	
      }
    }
    
    for( unsigned int s = 0 ; s < striataMask.size() ; ++s ){
      if(orig == 0){
	MotionWriter mW( "MLBased_" + subjects[s] + "_TO_" + subjects[refSubject] ) ;
	mW.write(motions[s]) ;
      }
      cout << matchNGC[s] << "\t" << orig << " : " << matchML[s] << endl ;
    }
  }
  
  return 0 ;   
}
