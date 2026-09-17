
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
