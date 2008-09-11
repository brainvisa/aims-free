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


#include "segmentation/veinextraction_d.h"
#include <aims/io/process.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/io/finder.h>
#include <aims/io/aimsGraphW.h>
#include <aims/roi/roi.h>
#include <aims/getopt/getopt2.h>
#include <aims/vector/vector.h> 
#include <stdio.h>

using namespace aims;
using namespace std;


template<class T>
static bool veins( Process & p, const string & filename, Finder & f );

class VeinExtractionProcess : public Process
{
public:
  VeinExtractionProcess( const string & fout, const vector<float>& beginPoint, 
			 const vector<float>& endPoint, float diameter, int borderWidth, 
			 bool lowerValueInaccessible,
			 const string& outputType, bool wholeImage, const string& modality );

  template<class T> 
  bool veins_m( AimsData<T> & data, const string & filename, Finder & f );

private:
  string	_fileout;
  vector<float> _beginPoint ;
  vector<float> _endPoint ;
  float _diameter ;
  int _borderWidth ;
  bool _lowerValueInaccessible ;
  string _outputType ;
  bool _wholeImage ;
  string _modality ;
};

VeinExtractionProcess::VeinExtractionProcess( const string & fout, const vector<float>& beginPoint, 
					      const vector<float>& endPoint, float diameter, int borderWidth,
					      bool lowerValueInaccessible,
					      const string& outputType,
					      bool wholeImage, const string& modality )
  : Process(), _fileout( fout ), _beginPoint(beginPoint), _endPoint(endPoint), 
    _diameter(diameter), _borderWidth(borderWidth), _lowerValueInaccessible(lowerValueInaccessible), 
    _outputType(outputType), _wholeImage(wholeImage), _modality(modality)
{
  registerProcessType( "Volume", "U8", &veins<uint8_t> );
  registerProcessType( "Volume", "S8", &veins<int8_t> );
  registerProcessType( "Volume", "U16", &veins<uint16_t> );
  registerProcessType( "Volume", "S16", &veins<int16_t> );
  registerProcessType( "Volume", "S32", &veins<int32_t> );
  registerProcessType( "Volume", "FLOAT", &veins<float> );
  registerProcessType( "Volume", "DOUBLE", &veins<double> );
  // add new types here when morphology operators become templates...
}

template<class T> 
bool veins( Process & p, const string & filename, Finder & f )
{
  VeinExtractionProcess	& dp = (VeinExtractionProcess &) p;
  AimsData<T>		data;
  return( dp.veins_m( data, filename, f ) );
}


template<class T> 
bool VeinExtractionProcess::veins_m( AimsData<T> & data, const string & filename, Finder & f )
{
  Reader<AimsData<T> >	reader( filename );
  string	format = f.format();
  if( !reader.read( data, 1, &format ) )
    return( false );

  cout << "Volume Read ! " << endl ;
  
  Point3d size( data.dimX(), data.dimY(), data.dimZ() ) ;
  Point3df voxelSize( data.sizeX(), data.sizeY(), data.sizeZ() ) ;
  Volume<T> image( size, 0, voxelSize ) ; 
  
//   cout << "Volume size " << size ;
//   cout << "\tvoxel size " << voxelSize ;
  
  typename AimsData<T>::iterator  iter( data.begin( ) ) ;
  for ( int z = 0  ; z < data.dimZ() ; z++ )
    for ( int y = 0 ; y < data.dimY() ; y++ )
      for ( int x = 0 ; x < data.dimX() ; x++, iter++ ){
	image.setVoxel( x, y, z, data(x, y, z) );
      } 
  
  if( _beginPoint.size() < 3 && _endPoint.size() < 3 )
    return 0 ;
  
  Point3d beginPoint( (short) _beginPoint[0], (short)_beginPoint[1], (short)_beginPoint[2] ) ;
  Point3d endPoint( (short)_endPoint[0],  (short)_endPoint[1],  (short)_endPoint[2] ) ;
  
  bool fillDisk = ( _modality != "MRI" ) ;
  
  VeinExtraction<T> veinExtractor( &image, beginPoint, endPoint, _diameter, _borderWidth, 
  				   _lowerValueInaccessible, _wholeImage, fillDisk ) ; 

  // ------------------TMP---------------------------
//    cout << "Before extracted path" << endl ;
//    list< Point3d > extractedPath( veinExtractor.getExtractedPath() ) ;
//    cout << "After extracted path" << endl ;
//    BucketMap<Void> bck ;
//    bck.setSizeXYZT( data.sizeX(), data.sizeY(), data.sizeZ(), 1.0) ;
//    list< Point3d >::iterator it(extractedPath.begin()), lt(extractedPath.end()) ;
//    while( it != lt ){
//      bck[0][*it] ;
    
//      ++it ;
//    }
//    cout << "Writing extracted" << endl ;
//    Writer< BucketMap<Void> > wr( _fileout + "_extracted.bck" ) ;
//    wr.write(bck) ;
//    cout << "Writed extracted" << endl ;
  // ------------------TMP---------------------------
  
  list< Point3df > minimalPath( veinExtractor.getCenteredPath( ) ) ;

  // ------------------TMP---------------------------
//   list< Point3df >::iterator it2(minimalPath.begin()), lt2(minimalPath.end()) ;
//   BucketMap<Void> bck2 ;
//   bck2.setSizeXYZT( data.sizeX(), data.sizeY(), data.sizeZ(), 1.0) ;
//   while( it2 != lt2 ){
//     bck2[0][Point3d((int)(*it2)[0],(int)(*it2)[1],(int)(*it2)[2])] ;
    
//     ++it2 ;
//   }
//   Writer< BucketMap<Void> > wr2( "/volatile/temp/centered.bck" ) ;
//   wr2.write(bck2) ;
  // ------------------TMP---------------------------
  
  Volume<int16_t> segmentationResult( veinExtractor.getExtractedVein( ) ) ; 
  AimsData<int16_t> res(data.dimX(), data.dimY(), data.dimZ() ) ;
  res.setSizeXYZT(data.sizeX(), data.sizeY(), data.sizeZ() ) ;
  for ( int z = 0  ; z < data.dimZ() ; z++ )
    for ( int y = 0 ; y < data.dimY() ; y++ )
      for ( int x = 0 ; x < data.dimX() ; x++, iter++ )
	res(x, y, z) = segmentationResult( x, y, z ) ;
  
  
  if( _outputType == "image" ){
    Writer<AimsData<int16_t> > writer( _fileout ) ;
    writer.write( res ) ;
  } else {
    AimsRoi roi ;
    roi.setLabel(res) ;
    roi.data2bucket() ;
    roi.setBucketFilename( static_cast<string>( "SegmentedVein" ) ) ;
    if( _fileout.find(".arg") == string::npos ) 
      _fileout += ".arg" ;
    AimsGraphWriter roiWriter( _fileout ) ;
    roiWriter.writeElements( roi ) ;
  }
  return( 1 );
}


int main( int argc, const char **argv )
{
  try
    {
      string filein, fileout, outputType = "arg", modality = "MRI" ;
      vector<float> beginPoint, endPoint ;
      float diameter = 5. ;
      int borderWidth = 15 ;
      bool lowerInacc = false ;
      bool wholeImage = false ;

      AimsApplication app(argc, argv, "Semi-automatic vein extraction using global minimum energy path for active contours with geodesics" ) ;
  
      app.addOption( filein, "-i", "Input image" ) ;
      app.addOption( fileout, "-o", "Output segmented image" ) ;
      app.addOption( modality, "-m", "Input image modality (MRI, Other)", true ) ;
      app.addOptionSeries( beginPoint, "-b", "First point on vein") ;
      app.addOptionSeries( endPoint, "-e", "Last Point on vein") ;
      app.addOption(diameter, "-d", "vein diameter (default : 5 mm)", true) ;
      app.addOption(borderWidth, "-w", "Border with for twisted veins (default : 15 voxels )", true ) ;
      app.addOption(lowerInacc, "-li", "lowervalue of image is inaccessible (default : false)", true ) ;
      app.addOption(outputType, "-t", "Voi type : arg, image (default : arg )", true ) ;
      app.addOption(wholeImage, "-wi", "using whole image (default : no )", true ) ;
      app.initialize() ;

      cout << "Lower Inacc : " << lowerInacc << endl ;

      VeinExtractionProcess	proc( fileout, beginPoint, endPoint, diameter, borderWidth, lowerInacc, outputType,
                                      wholeImage, modality );
      if( !proc.execute( filein ) )
        {
          cerr << "Couldn't process\n";
          return( 1 );
        }
    }
  catch( carto::user_interruption & )
    {
    }
  catch( exception & e )
    {
      cerr << e.what() << endl;
      return EXIT_FAILURE;
    }

  return( EXIT_SUCCESS );
}
