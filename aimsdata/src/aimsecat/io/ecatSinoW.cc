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

// we don't want to issue a warning
#ifndef AIMSDATA_CLASS_NO_DEPREC_WARNING
#define AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 *  EACT data writer class
 */
#include <aims/io/ecatSinoheader.h>
#include <aims/data/data.h>
#include <aims/data/pheader.h>
#include <aims/io/ecatSinoW.h>
#include <aims/io/datatypecode.h> 
#include <ecat+/io/io.h>
#include <cartobase/exception/file.h>
#include <string>
#include <vector>
#include <time.h>


using namespace std;
using namespace carto;   

namespace aims {

template< class T >
static void fillEcatSinoHdr( UnifiedEcatSinoInfo *uesi, const vector< Sinogram<T> >& thing )
{
  if ( thing[0].getFileType() =="ECAT")
    {
      // Get Info commune Main Header
      uesi->system_type = thing[0].getSystemType() ;
      uesi->scan_start_time =  thing[0].getScanStartTime() ;
      string s = thing[0].getRadioPharmaceutical() ;
      if ( s.length() )
	strcpy( uesi->radiopharmaceutical, s.c_str() );
      
      s = thing[0].getIsotopeName() ;
      if ( s.length() )
	strcpy( uesi->isotope_code, s.c_str() );
      
      uesi->isotope_halflife = thing[0].getIsotopeHalfLife() ;
      uesi->distance_scanned = thing[0].getDistanceScanned() ;
      uesi->transaxial_fov = thing[0].getTransaxialFOV();
      uesi->angular_compression = thing[0].getAngularCompression() ;
      
      s = thing[0].getStudyType();
      if ( s.length() )
	strcpy( uesi->study_name, s.c_str() );
      
      uesi->patient_orientation = thing[0].getPatientOrientation();
      uesi->plane_separation = thing[0].getPlaneSeparation() ;
      uesi->num_planes = thing[0].getTotalNumPlanes();

      int multiType = thing[0].getMultiFileType() ;
      uesi->num_gates = ( multiType == EcatSinoHeader::MULTIGATE ? thing.size() : 1 ) ;
      uesi->num_frames = ( multiType == EcatSinoHeader::MULTIFRAME ? thing.size() : 1 ) ;
      uesi->num_bed_pos = ( multiType == EcatSinoHeader::MULTIBED ? thing.size() - 1 : 0 ) ;
      uesi->init_bed_position = thing[0].getInitBedPosition();
      uesi->lwr_true_thres = thing[0].getLowerTrueThreshold();
      uesi->upr_true_thres = thing[0].getUpperTrueThreshold();
      uesi->acquisition_mode = thing[0].getAcquisitionMode();
      uesi->acquisition_type = thing[0].getAcquisitionType();
      uesi->bin_size = thing[0].getBinSize();
      uesi->branching_fraction = thing[0].getBranchingFraction();
      uesi->dose_start_time = thing[0].getDoseStartTime();
      uesi->bed_elevation = thing[0].getBedElevation();
      uesi->coin_samp_mode = thing[0].getCoinSampleMode();
      uesi->axial_samp_mode = thing[0].getAxialSampleMode();
      uesi->calibration_factor = thing[0].getCalibrationFactor();
      vector<float> bedOffset  = thing[0].getBedOffset();
      for(int i = 0 ; i < 15 ; ++i )
	uesi->bed_offset[i] = bedOffset[i] ;
      uesi->lwr_sctr_thres = thing[0].getLowerScatterThreshold();
      uesi->dosage = thing[0].getDosage();
      uesi->well_counter_factor = thing[0].getWellCounterFactor();
      uesi->septa_state = thing[0].getSeptaState();
      uesi->x_resolution = thing[0].getXResolution();
      uesi->v_resolution = thing[0].getVResolution();
      uesi->z_resolution = thing[0].getZResolution();
      uesi->w_resolution = thing[0].getWResolution();

      // Allocate uesi Scan3D subheader : chainage peut etre inutile
      scan3DSub *currentUesiSub;
      if (( currentUesiSub = (scan3DSub *)calloc(thing.size(),
                                                 sizeof(scan3DSub))) == NULL)
      {
        EcatSinoFree( uesi );
        throw file_error( uesi->fname );
      }
      int i;
      (currentUesiSub[thing.size() - 1]).next = NULL;
      for ( i = 1 ; i< (int) thing.size() ; ++i)
	{
	      (currentUesiSub[i-1]).next = &(currentUesiSub[i]);
	}
      uesi->subfirst = currentUesiSub;
      uesi->sublast = &(currentUesiSub[thing.size() -1]);
      
      // Get Info partag�es par les subheaders
	  
      // Get Info locales � chaque subheaders
      
      for ( i = 0 ; i<(int) thing.size() ;++i)
	{
	  (currentUesiSub[i]).prompts = thing[i].getPrompts() ;
	  (currentUesiSub[i]).delayed = thing[i].getDelayed() ;
	  (currentUesiSub[i]).multiples = thing[i].getMultiples() ;
	  (currentUesiSub[i]).net_trues = thing[i].getTrues() ;
	  
	  (currentUesiSub[i]).frame_start_time = thing[i].getStartTime() ;
	  (currentUesiSub[i]).frame_duration = thing[i].getDurationTime() ;
	  (currentUesiSub[i]).gate_duration = thing[i].getGateDuration() ;
	  (currentUesiSub[i]).net_trues = thing[i].getTrues();
	  (currentUesiSub[i]).tot_avg_cor = thing[i].getTotalAverageCorrected() ;
	  (currentUesiSub[i]).tot_avg_uncor = thing[i].getTotalAverageUncorrected() ;
	  (currentUesiSub[i]).total_coin_rate = thing[i].getTotalCoinRate() ;
	  (currentUesiSub[i]).loss_correction_fctr = thing[i].getDeadTimeCorrectionFactor() ;
	  
	  vector<float> us = thing[i].getUncorrectedSingles() ;
	  if ( us.size() > 0)
	    for( int j = 0 ; j < 128 ; ++j )
	      (currentUesiSub[i]).uncor_singles[j] = us[j] ;
	  (currentUesiSub[i]).scan_min = thing[i].getScanMin() ;
	  (currentUesiSub[i]).scan_max = thing[i].getScanMax() ;
	}
    }
  else
    {
      EcatSinoFree( uesi );
      throw file_error( uesi->fname );
    }
}

template< class T >
static bool testHeaderValidity( const vector< Sinogram<T> >& thing)
{
  if (thing.size() == 0 ) return false ;
  if (thing.size() == 1 ) return true ;
  
  for( int i = 1 ; i < (int)thing.size() ; ++i )
    {
//       if( thing[i].header() == 0 )
// 	return false ;
      if( thing[i].getInternalMode() != thing[0].getInternalMode() ||
	  thing[i].getNumAngles() != thing[0].getNumAngles() ||
	  thing[i].getNumBins() != thing[0].getNumBins() ||
	  thing[i].getNumSeg() != thing[0].getNumSeg() ||
	  thing[i].getRing() != thing[0].getRing() ||
	  thing[i].getSpan() != thing[0].getSpan() ||
	  thing[i].getRingDiff() != thing[0].getRingDiff() ||
	  thing[i].getFileType() != thing[0].getFileType() ||
	  thing[i].getEcatFileType() != thing[0].getEcatFileType() ||
	  thing[i].getSystemType() != thing[0].getSystemType() ||
	  thing[i].getScanStartTime() != thing[0].getScanStartTime() ||
	  thing[i].getRadioPharmaceutical() != thing[0].getRadioPharmaceutical() ||
	  thing[i].getIsotopeName() != thing[0].getIsotopeName() ||
	  thing[i].getIsotopeHalfLife() != thing[0].getIsotopeHalfLife() ||
	  thing[i].getDistanceScanned() != thing[0].getDistanceScanned() ||
	  thing[i].getTransaxialFOV() != thing[0].getTransaxialFOV() ||
	  thing[i].getAngularCompression() != thing[0].getAngularCompression() ||
	  thing[i].getStudyType() != thing[0].getStudyType() ||
	  thing[i].getPatientOrientation() != thing[0].getPatientOrientation() ||
	  thing[i].getPlaneSeparation() != thing[0].getPlaneSeparation() ||
	  thing[i].getTotalNumPlanes() != thing[0].getTotalNumPlanes() ||
	  thing[i].getMultiFileType() != thing[0].getMultiFileType() ||
	  thing[i].getInitBedPosition() != thing[0].getInitBedPosition() ||
	  thing[i].getLowerTrueThreshold() != thing[0].getLowerTrueThreshold() ||
	  thing[i].getUpperTrueThreshold() != thing[0].getUpperTrueThreshold() ||
	  thing[i].getAcquisitionMode() != thing[0].getAcquisitionMode() ||
	  thing[i].getAcquisitionType() != thing[0].getAcquisitionType() ||
	  thing[i].getBinSize() != thing[0].getBinSize() ||
	  thing[i].getBranchingFraction() != thing[0].getBranchingFraction() ||
	  thing[i].getDoseStartTime() != thing[0].getDoseStartTime() ||
	  thing[i].getBedElevation() != thing[0].getBedElevation() ||
	  thing[i].getCoinSampleMode() != thing[0].getCoinSampleMode() ||
	  thing[i].getAxialSampleMode() != thing[0].getAxialSampleMode() ||
	  thing[i].getCalibrationFactor() != thing[0].getCalibrationFactor() ||
	  thing[i].getBedOffset() != thing[0].getBedOffset() ||
	  thing[i].getLowerScatterThreshold() != thing[0].getLowerScatterThreshold() ||
	  thing[i].getDosage() != thing[0].getDosage() ||
	  thing[i].getWellCounterFactor() != thing[0].getWellCounterFactor() ||
	  thing[i].getSeptaState() != thing[0].getSeptaState() ||
	  thing[i].getXResolution() != thing[0].getXResolution() ||
	  thing[i].getVResolution() != thing[0].getVResolution() ||
	  thing[i].getZResolution() != thing[0].getZResolution() ||
	  thing[i].getWResolution() != thing[0].getWResolution() )
	return false ;
    }
  return true ;
}

template< class T >
void EcatSinoWriter<T>::write( const Sinogram<T>& thing )
{
  vector< Sinogram< T > > vec ;
  vec.push_back( thing ) ;
  write(vec) ;
}

template< class T >
void EcatSinoWriter<T>::write( const vector< Sinogram<T> >& thing)
{
  //string fileName = removeExtension( _name);
  string fileName = _name  + ".S";
  
  //cout << "Writing " << fileName << endl ;
  UnifiedEcatSinoInfo *uesi =  EcatSinoOpen((char*)fileName.c_str(),
                                             const_cast<char*>( "w" ) );
  if (uesi == ECATSHFJ_FAIL)
    throw file_error( fileName );

  // On peut peut ete faire plus simple mais les pb d'ordre d'initialisation
  // sont dus au fait que des informations d'int�r�t g�n�ral sont stock�es 
  // dans des subheader de la structure Ecat.
  // Modus operandi
  // Init du main header
  //   -1 Init des champs tr�s g�n�raux de uesi (tailles...) par m�thodes de Sinogram
  //   -2 Init du champs ecat_file_type
  // Init des subheaders
  //   -3 Init des champs via le header du Sinogram (objet attribu�...)
  //   -4 Init des champs via les m�thodes directes de Sinogram.



  // 111111111
  // Champs initialisable depuis les informations contenues 
  // dans AimsData sans recours au Header de AimsData
  
  if( thing.size() == 0 )
    throw file_error( fileName ) ;
    
  if (thing[0].getInternalMode() ==  Sinogram<T>::VIEW)
    uesi->storage_order = 0;
  else
    uesi->storage_order = 1;

  uesi->num_r_elements = thing[0].getNumBins() ;
  uesi->num_angles =     thing[0].getNumAngles();

  // 2222222222
  // Champ pour le type du sinogramme/
  string eft = thing[0].getEcatFileType() ; 
  DataTypeCode<T>     dt;
  if ( dt.dataType() == "FLOAT")   
    sprintf(uesi->file_type,"Float3dSinogram");
  else if (dt.dataType() == "S16") 
    sprintf(uesi->file_type, "Short3dSinogram");
  else
    {
      EcatSinoFree( uesi );
      throw file_error( fileName );
    }
  
  // 33333333333
  // Champs obtenus via le header
  if( testHeaderValidity( thing ) )
    {
      fillEcatSinoHdr( uesi, thing );
      //      hdr.copy( *ph );
    }
  else
    {
      EcatSinoFree( uesi );
      throw file_error( fileName );
    }

  // 44444444444444
  int s;
  for(s = 0; s < thing[0].getNumSeg(); s+=2)
    {
      if (s)
	(uesi->num_z_elements)[s/2] = 2*thing[0].getNumPlanes(s);
      else 
	(uesi->num_z_elements)[s/2] = thing[0].getNumPlanes(s);
    }
  uesi->axial_compression = thing[0].getSpan();
  uesi->ring_difference = thing[0].getRingDiff();
  if ( thing[0].getNumSeg() > 1)
    uesi->num_dimensions = 4;
  else
    uesi->num_dimensions = 3;      

  // Ecriture des donn�es proprement dites
  // Ecriture segment par segment
  // Normalemement il faudrait remplcer short par T
  if (thing[0].getInternalMode() !=  Sinogram<T>::VIEW ) // temporaire
    throw file_error( fileName );

  int seg;
  int isMultiBed = 0, isMultiGate = 0, isMultiFrame = 0 ;
  if( thing[0].getMultiFileType() == EcatSinoHeader::MULTIBED )
    isMultiBed = 1 ;
  else if ( thing[0].getMultiFileType() == EcatSinoHeader::MULTIFRAME )
    isMultiFrame = 1 ;
  else if ( thing[0].getMultiFileType() == EcatSinoHeader::MULTIGATE )
    isMultiGate = 1 ;
  int fileNum ;
  for( fileNum = 0 ; fileNum < (int) thing.size() ; ++fileNum )
    for( seg = 0; seg < thing[fileNum].getNumSeg(); ++seg )
      {
	int l = thing[fileNum].getNumPlanes(seg)*thing[fileNum].getNumBins();
	int fsize =thing[fileNum].getNumAngles()*
	  thing[fileNum].getNumPlanes(seg)*thing[fileNum].getNumBins();
	T *s_pt,*s_vol = new T[ fsize ];
	s_pt = s_vol;
	
	int angle;
	for(angle=0;angle < thing[fileNum].getNumAngles(); ++angle)
	  {
	    memcpy((void *) s_pt, (void *) (thing[fileNum].getView(seg, angle)).begin(),
		   sizeof(T)*l );
	    s_pt += l;
	  }
	
	//for (int t=0;t< 1;t++) // On ne g�re que un seul lit ou une seul frame 
	
	if( dt.dataType() == "S16" ){
	  //cout << "Writing S16" << endl ;

	  if(EcatWriteSino_S16(uesi, (short *)s_vol,
				   thing[fileNum].transCodeSegment(seg),
				   fileNum*isMultiBed,
				   fileNum*isMultiGate,
				   fileNum*isMultiFrame ) == -1 )
            throw logic_error( "Internal error: EcatWriteSino_S16 failed" );
	}else if( dt.dataType() == "FLOAT" ){
	  //cout << "Writing FLOAT" << endl ;
	  if(EcatWriteSino_FLOAT(uesi, (float *)s_vol,
				     thing[fileNum].transCodeSegment(seg),
				     fileNum*isMultiBed,
				     fileNum*isMultiGate,
				     fileNum*isMultiFrame ) == -1 )
            throw logic_error( "Internal error: EcatWriteSino_FLOAT failed" );
	}
	delete[] s_vol; 
      }
  
  EcatSinoClose(uesi);
} 



template bool testHeaderValidity( const vector< Sinogram<short> >& thing) ;
template bool testHeaderValidity( const vector< Sinogram<float> >& thing) ;

template void fillEcatSinoHdr( UnifiedEcatSinoInfo *uesi, 
			       const vector< Sinogram<short> >& thing ) ;
template void fillEcatSinoHdr( UnifiedEcatSinoInfo *uesi, 
			       const vector< Sinogram<float> >& thing ) ;

template class AIMSDATA_API EcatSinoWriter< short >;
template class AIMSDATA_API EcatSinoWriter< float >;

} // namespace aims
