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

#include <aims/def/general.h>
#include <aims/io/ecatSinoheader.h>
#include <aims/data/data.h>
#include <aims/io/ecatSinoR.h>
#include <ecat+/io/io.h>
#include <cartobase/exception/format.h>
#include <string>
#include <vector>

using namespace aims;
using namespace carto;
using namespace std;


template< class T >
string EcatSinoReader< T >::removeExtension( const string& name )
{
  string res = name;
  string ext="";
  if ( res.length() > 2 )
    ext = res.substr( int(res.length() - 2), 2 );
  if (ext == ".S")
    res = res.substr( 0, res.length() - 2 );
  return res;
}

namespace aims
{

template< >
void EcatSinoReader< short >::read( Sinogram<short>& sino, 
				    int bedDataOrFrame ) //  0 <=bedDataOrFrame <= max
{
  UnifiedEcatSinoInfo    *uesi;
  string             fileName;
  
  // --------------------------------Initialisation du header propri�taire
  EcatSinoHeader *hdr = new EcatSinoHeader( _name ); 
  try 
    { 
      hdr->read(); 
    } 
  catch( exception & e ) 
    { 
      delete hdr; 
      throw( e ); 
    } 
  
  // ----------------------------------------Ouverture du uesi



  fileName = _name;   // .S ECAT sinos
  if ((uesi = EcatSinoOpen( (char*) fileName.c_str(),
    const_cast<char*>( "r" ) )) == ECATSHFJ_FAIL)
    throw format_error( fileName );

  ScannerConf readConf( uesi );

  // --------------------Get and allocate sino object
  Sinogram<short>::Mode mode;
  if (uesi->storage_order == 0)
    mode = Sinogram<short>::VIEW;
  else if  (uesi->storage_order == 1)
    mode = Sinogram<short>::SINO;
  else 
      throw 
	format_error("Unknown acquisition mode (neither 2D nor 3D) ", _name );
  if( mode != sino.getInternalMode() || readConf != sino.getScannerConf() )
    sino = Sinogram<short>( readConf, mode);
  //sino.allocate( uesi->num_r_elements, uesi->num_angles);
  //PythonHeader * subHdr = new PythonHeader( hdr->getSingleBedDataFrame(bedDataOrFrame) ) ;
  sino.setHeader( hdr->getSingleBedDataFrame(bedDataOrFrame) );
  int bed = 0, data = 1, frame = 1 ;
  int multiType ;
  if( sino.header().getProperty( "multi_files_type", multiType ) )
    {
      if ( multiType == EcatSinoHeader::MULTIBED )
	bed = bedDataOrFrame ;
      else if ( multiType == EcatSinoHeader::MULTIGATE )
	data = bedDataOrFrame + 1 ;
      else if ( multiType == EcatSinoHeader::MULTIFRAME )
	frame = bedDataOrFrame + 1 ;
    }
  // ici impl�menter des it�rateurs.
  int s;
  ForEach1d( (sino.start()), s)
    {
      short *buffer = EcatSinoRead_S16( uesi, sino.transCodeSegment(s), 
					bed, data, frame ); 
      short *pt = buffer;
      int    length = sino.getZ( s ) *  uesi->num_r_elements;
      AimsData<AimsData< short > > segment = ( sino.start() )(s);
      int v;
      ForEach1d(segment, v)
	{
	  AimsData< short > vue = segment(v);
	  memcpy(  vue.begin(), pt, sizeof(short)*length);
	  pt += length;
	}
      free( buffer );
    }

  // -----------------------fermeture de l'uei
  EcatSinoClose( uesi );
  delete hdr ;
  // ------------------------Fin.
 
//   cout << "\tTHING : -------------" << endl ;
//   thing.printHeader() ;
}

template< >
void EcatSinoReader< short >::read( vector < Sinogram<short> > & thing )
{
  EcatSinoHeader *hdr = new EcatSinoHeader( _name ); 
  try 
    { 
      hdr->read(); 
    } 
  catch( exception & e ) 
    { 
      delete hdr; 
      throw( e ); 
    } 
  
  short numGates, numFrames, numBedPos ;
  
  hdr->getProperty( "ecat_num_gates", numGates) ; 
  hdr->getProperty( "ecat_num_frames", numFrames) ; 
  hdr->getProperty( "ecat_num_bed_pos", numBedPos) ;
  
//   cout << "ecat_num_bed_pos = " << numBedPos << endl ;
//   cout << "ecat_num_frames = " << numFrames << endl ;
  
  int num_files = 1 ;
  if( numFrames > 1 )
    num_files = numFrames ;
  
  else if( numGates > 1 )
    num_files = numGates ;
  
  else if( numBedPos > 0 )
    num_files = numBedPos + 1 ;
  
  thing.resize( num_files ) ;
  for( int i = 0 ; i < num_files ; ++i ){
//     cout << "Reading frame " << i << endl ;
    read( thing[i], i ) ;
//     cout << "frame " << i << " read" << endl ; 
  }
  delete hdr;
}

template< >
void EcatSinoReader< float >::read( Sinogram<float>& sino, 
				    int bedDataOrFrame ) //  0 <=bedDataOrFrame <= max
{
  //cout << "EcatSinoReader< float >::read bed " << bedDataOrFrame << endl ;
  UnifiedEcatSinoInfo    *uesi;
  string             fileName;
  
  // --------------------------------Initialisation du header propri�taire
  
  //cout << "continue  sino read ?" << endl ;
  //cin >> c ;
  
  EcatSinoHeader *hdr = new EcatSinoHeader( _name ); 
  try 
    { 
      hdr->read(); 
    } 
  catch( exception & e ) 
    { 
      delete hdr; 
      throw( e ); 
    } 
  
  //cout << "continue header read ?" << endl ;
  //cin >> c ;
  //cout << "Header read" << endl ;
  // ----------------------------------------Ouverture du uesi



  //cout << "ecat sino open" << endl ;
  fileName = _name;   // .S ECAT sinos
  if ((uesi = EcatSinoOpen( (char*) fileName.c_str(),
    const_cast<char*>( "r" ) )) == ECATSHFJ_FAIL)
    throw format_error( fileName );

  //cout << "continue sino opened ?" << endl ;
  //cin >> c ;
  //cout << "uesi read" << endl ;

  ScannerConf readConf( uesi );

  // --------------------Get and allocate sino object
  Sinogram<float>::Mode mode;
  if (uesi->storage_order == 0)
    mode = Sinogram<float>::VIEW;
  else if  (uesi->storage_order == 1)
    mode = Sinogram<float>::SINO;
  else 
    throw format_error("Unknown acquisition mode (neither 2D nor 3D) ", _name );
  
  //cout << "scanner conf read" << endl ;
  if( sino.getInternalMode() != mode || sino.getScannerConf() != readConf ){
//     cout << "Sino re-creation !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl ;
//     cout << "sino mode : " << sino.getInternalMode() << " vs mode : " << mode << endl
// 	 << "sino conf diff : " << (sino.getScannerConf() != readConf ) << endl ;
    sino = Sinogram<float>( readConf, mode);
  }
  //cout << "continue creating sino ?" << endl ;
  //cin >> c ;
  
  //sino.allocate( uesi->num_r_elements, uesi->num_angles);
  //PythonHeader * subHdr = new PythonHeader( hdr->getSingleBedDataFrame(bedDataOrFrame) ) ;
  sino.setHeader( hdr->getSingleBedDataFrame(bedDataOrFrame) );
  int bed = 0, data = 1, frame = 1 ;
  int multiType ;
  if( sino.header().getProperty( "multi_files_type", multiType ) )
    {
      if ( multiType == EcatSinoHeader::MULTIBED )
	bed = bedDataOrFrame ;
      else if ( multiType == EcatSinoHeader::MULTIGATE )
	data = bedDataOrFrame + 1 ;
      else if ( multiType == EcatSinoHeader::MULTIFRAME )
	frame = bedDataOrFrame + 1 ;
    }
  //cout << "ecat header set" << endl ;
  
  //cout << "continue subHddr deleted ?" << endl ;
  //cin >> c ;
  // ici impl�menter des it�rateurs.
  int s;
  //cout << "number of seg = " << sino.start().dimX() << endl ;

  ForEach1d( (sino.start()), s)
    {
      //cout << "reading segment " << s << endl ;
      
      float *buffer = EcatSinoRead_FLOAT( uesi, sino.transCodeSegment(s), 
					  bed, data, frame ); 
      //cout << "buffer read" << endl ;
      float *pt = buffer;
      int    length = sino.getZ( s ) *  uesi->num_r_elements;
      AimsData<AimsData< float > >& segment = ( sino.start() )(s);
      int v;
      
//       float maxOnBuf = 0. ;
//       for(int i = 0 ; i < length ; ++i )
// 	if( pt[i] > maxOnBuf )
// 	  maxOnBuf = pt[i] ;
      
//       cout << "seg " << s << " & max on buf = " << maxOnBuf << endl ;
      ForEach1d(segment, v)
	{
	  AimsData< float >& vue = segment(v);
	  memcpy(  vue.begin(), pt, sizeof(float)*length);
	  pt += length;
	}
      //cout << "segment filled" << endl ;
      free( buffer );
    }
  //cout << "continue segment read ?" << endl ;
  //cin >> c ;

  //cout << "closing uesi" <<endl ;
  // -----------------------fermeture de l'uei
  EcatSinoClose( uesi );
  delete hdr ;
  //cout << "continue sino closed ?" << endl ;
  //cin >> c ;
  //cout << "uesi closed" <<endl ;

  // ------------------------Fin.
  
//   cout << "\tTHING : -------------" << endl ;
//   thing.printHeader() ;
}

template< >
void EcatSinoReader< float >::read( vector < Sinogram<float> > & thing )
{
  //cout << "EcatSinoReader< float >::read" << endl ;
  EcatSinoHeader *hdr = new EcatSinoHeader( _name ); 
  try 
    { 
      hdr->read(); 
    } 
  catch( exception & e ) 
    { 
      delete hdr; 
      throw( e ); 
    } 
  
  short numGates, numFrames, numBedPos ;
  
  hdr->getProperty( "ecat_num_gates", numGates) ; 
  hdr->getProperty( "ecat_num_frames", numFrames) ; 
  hdr->getProperty( "ecat_num_bed_pos", numBedPos) ;
  
  unsigned num_files = 1 ;
  if( numFrames > 1 )
    num_files = numFrames ;
  
  else if( numGates > 1 )
    num_files = numGates ;
  
  else if( numBedPos > 0 )
    num_files = numBedPos + 1 ;
  
  if( thing.size() != num_files ){
    thing = vector<Sinogram<float> >(num_files) ;
  }
  for( unsigned i = 0 ; i < num_files ; ++i )
    read( thing[i], i ) ;
  delete hdr; 
}

// instanciation explicite.

template class EcatSinoReader< short >;
template class EcatSinoReader< float >;

} // namespace aims

// compilation of some Volume classes on Aims types
#include <cartodata/volume/volume_d.h>

template class carto::Volume< AimsData<short> >;

