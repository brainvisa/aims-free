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


#ifndef AIMS_VEIN_EXTRACTION_H
#define AIMS_VEIN_EXTRACTION_H

#include "../geodesicminimalpath/geodesicminimalpath.h"

template <typename T>
class VeinExtraction {
public:
  static const int HalfConvolutionSize = 2 ;
  
  VeinExtraction( Volume<T>* image, const Point3d& initialPoint, const Point3d& finalPoint, 
		  float diameter = 5., int roiBorder = 15, bool lowerValueInaccessible = false,
		  bool usingWholeImage = false, bool fillDisk = false ) ;
  ~VeinExtraction() ;
  
  const Volume<short>& getExtractedVein( ) ;
  
  std::list < Point3d > getExtractedPath( ) {
    std::list < Point3d > path = pathExtractor_->getMinimalPath( finalPoint_ - roiOffset_ ) ; 
    std::list < Point3d >::iterator iter( path.begin() ), last( path.end() ) ;
    while( iter != last ){
      (*iter) += roiOffset_ ;
      ++iter ;
    }
    return path ;
  }

  std::list < Point3df > getCenteredPath() {
    if ( recomputePath_ ) 
      findVeinCenter( ) ;
    recomputePath_ = false ;
    return centeredPath_ ;
  }
  
  // degug begin
  bool getConvolutedImage(Volume<float>& conv) 
    {  
    	if(convolutedImage_){
	  conv = *convolutedImage_ ; 
	  return true ;
    }
    return false ;
}

  const Point3d & roiOffset() { return roiOffset_ ; } 
  const Point3d & roiSize() { return roiSize_ ; } 

  // debug end
  void setInitialPoint( const Point3d& initialPoint ) ;
  void setFinalPoint( const Point3d& initialPoint ) ;
  void setImage( Volume<T> * image ) ;
  
private:
  void init( ) ;
  void findVeinCenter( ) ;
  void extractVeinContours( ) {}
  void imageConvolution() ;
  
  GeodesicMinimalPath * pathExtractor_ ;
  bool recomputePath_ ;
  float diameter_ ;
  Point3d roiOffset_ ;
  Point3d roiSize_ ;
  int roiBorder_ ;
  Volume<T> *image_ ;
  Volume<short> *segmentedVein_ ;
  Volume<float> * convolutedImage_ ;
  Point3d initialPoint_ ;
  Point3d finalPoint_ ;

  bool lowerValueInaccessible_ ;
  bool usingWholeImage_ ;
  std::list < Point3df > centeredPath_ ;
  bool fillDisk_ ;
};

#endif
