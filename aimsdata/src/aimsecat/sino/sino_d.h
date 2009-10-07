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

#ifndef ECAT_SINO_D_H
#define ECAT_SINO_D_H

#include <aims/sino/sino.h>
#include <cartobase/exception/format.h>
#include <iostream>


template < class T >
int  aims::Sinogram< T >::transCodeSegment( int s) const
{
  int tmp = int( float(s)/2.0 + .5);
  return( s%2 ? tmp : -tmp );
}




template < class T >
aims::Sinogram< T >::Sinogram( const ScannerConf &config, aims::Sinogram<T>::Mode mode)
  : _internalMode( mode ), _displayMode( mode ), _scannerConfig(config)
{
  //cout << "setting res" << endl ;
  setXResolution( config._binResolution ) ;
  setZResolution( config._zResolution ) ;
  
  //cout << "Res set" << endl ;
    
  _bin = AimsData<AimsData<AimsData<T> > >( _scannerConfig.getMaxSeg() );
  
  //cout << "Bin created" << endl ;
  int i, j;
  
  if ( _internalMode == aims::Sinogram<T>::SINO )
    {
      ForEach1d(_bin, i)
	{
	  _bin(i) = AimsData<AimsData<T > >( _scannerConfig.getZ(i) );
	  ForEach1d( _bin(i), j)
	    {
	      _bin(i)(j) = AimsData< T >( _scannerConfig.getNumBins(), _scannerConfig.getNumAngles() );
	    }
	}
    }
  else if ( _internalMode == aims::Sinogram<T>::VIEW )
    {
      ForEach1d(_bin, i)
	{
	  //cout << "segment " << i << endl ;
	  _bin(i) = AimsData<AimsData<T > >( _scannerConfig.getNumAngles() );
	  ForEach1d( _bin(i), j)
	    {
	      ( _bin(i) )(j) = AimsData< T >( _scannerConfig.getNumBins(), _scannerConfig.getZ(i)  );
	    }
	}
    }
  else
    {
      // Mettre un throw
//std::      cerr << "Cannot initialize properly a sinogram" << endl;
    }
  //_header = 0 ;
}

template < class T >
void aims::Sinogram< T >::allocate( int    r, 
			            int alpha)
{

  _bin = AimsData<AimsData<AimsData<T> > >( _scannerConfig.getMaxSeg() );
  
  int i, j;
  
  if ( _internalMode == aims::Sinogram<T>::SINO )
    {
      ForEach1d(_bin, i)
	{
	  _bin(i) = AimsData<AimsData<T > >( _scannerConfig.getZ(i) );
	  ForEach1d( ( _bin(i) ), j)
	    {
	      ( _bin(i) )(j) = AimsData< T >( r, alpha);
	    }
	}
    }
  else if ( _internalMode == aims::Sinogram<T>::VIEW )
    {
            ForEach1d(_bin, i)
	{
	  _bin(i) = AimsData<AimsData<T > >( alpha );
	  ForEach1d( ( _bin(i) ), j)
	    {
	      ( _bin(i) )(j) = AimsData< T >( r, _scannerConfig.getZ(i) );
	    }
	}
    }
  else
    {
      // Mettre un throw
//std::      cerr << "Cannot initialize properly a sinogram" << endl;
    }
}


template < class T >
aims::Sinogram<T>& aims::Sinogram<T>::operator = ( const aims::Sinogram<T>& other )
{
  if ( &other == this )
    return *this;
  
  
  _header.copy(other._header) ;
    
/*   std::cout << "scanner config copy" << std::endl ; */
  _scannerConfig = other._scannerConfig ;
/*   std::cout << "scanner config copied" << std::endl ; */

  _internalMode  = other._internalMode;
  _displayMode   = other._displayMode;
  _bin = other._bin;
/*   std::cout << "copy complete" << std::endl ; */
  return *this;
}

template < class T >
AimsData<T>& aims::Sinogram<T>::getPlaneInInternalMode( int segment, int zOrAlpha )
{
  
  return _bin(segment)(zOrAlpha) ;
}

template < class T >
AimsData<T> aims::Sinogram<T>::getPlaneInDisplayMode( int segment, int zOrAlpha )
{
  if( _displayMode == _internalMode ){
//     cout << "Display mode == internal mode" << endl ;
    return _bin(segment)(zOrAlpha).clone() ;
  }
  
  int dimX = _bin(segment)(0).dimX() ;
  int dimY = _bin(segment).dimX() ;
  float sizeX = _bin(segment)(0).sizeX() ;
  float sizeY = _bin(segment).sizeX() ;
  
//std::  cout << "Display mode != Internal mode. dimX = " << dimX << "\tdimY = " << dimY << endl ;
  
  AimsData<T> plane( dimX, dimY ) ;
  plane.setSizeXYZT( sizeX, sizeY ) ;
  for( int alphaOrZ = 0 ; alphaOrZ < dimY ; ++alphaOrZ )
    for( int ro = 0 ; ro < dimX ; ++ro )
      plane(ro, alphaOrZ) = _bin(segment)(alphaOrZ)(ro, zOrAlpha) ;
  return plane ;
}
 



template < class T >
AimsData<T> aims::Sinogram<T>::extractView( int seg, int alpha )
{
  if (_internalMode == aims::Sinogram<T>::VIEW)
    {
      return _bin(seg)(alpha).clone() ;
    }
  else
    {
      int   dimX = _bin(seg)(0).dimX() ;
      int   dimY = _bin(seg).dimX() ;
      float sizeX = _bin(seg)(0).sizeX() ;
      float sizeY = _bin(seg).sizeX() ;
  
//   cout << "Display mode != Internal mode. dimX = "
//        <<  dimX << "\tdimY = " << dimY << endl ;
  
  AimsData<T> plane( dimX, dimY ) ;
  plane.setSizeXYZT( sizeX, sizeY ) ;
  for( int z = 0 ; z < dimY ; ++z )
    for( int ro = 0 ; ro < dimX ; ++ro )
      plane(ro, z) = _bin(seg)(z)(ro, alpha) ;
  return plane ;
    }
}

template < class T >
AimsData<T>& aims::Sinogram<T>::getView( int seg, int alpha ) 
{
 if (_internalMode != aims::Sinogram<T>::VIEW)
    throw carto::format_error("Attempt to reference in VIEW mode when internal is SINO");

 return (_bin(seg))(alpha) ;
 
}

template < class T >
const AimsData<T>& aims::Sinogram<T>::getView( int seg, int alpha ) const
{
 if (_internalMode != aims::Sinogram<T>::VIEW)
    throw carto::format_error("Attempt to reference in VIEW mode when internal is SINO");

 return (_bin(seg))(alpha) ;
 
}

template < class T >
AimsData<T> aims::Sinogram<T>::extractSino( int seg, int z )
{
  if (_internalMode == aims::Sinogram<T>::SINO)
    {
      return _bin(seg)(z).clone() ;
    }
  else
    {
      int   dimX = _bin(seg)(0).dimX() ;
      int   dimY = _bin(seg).dimX() ;
      float sizeX = _bin(seg)(0).sizeX() ;
      float sizeY = _bin(seg).sizeX() ;
  
//   cout << "Display mode != Internal mode. dimX = "
//        <<  dimX << "\tdimY = " << dimY << endl ;
      
      AimsData<T> plane( dimX, dimY ) ;
      plane.setSizeXYZT( sizeX, sizeY ) ;
      for( int alpha = 0 ; alpha < dimY ; ++alpha )
	for( int ro = 0 ; ro < dimX ; ++ro )
	  plane(ro, alpha) = _bin(seg)(alpha)(ro, z) ;
      return plane ;
    }
}

template < class T >
AimsData<T>& aims::Sinogram<T>::getSino( int seg, int z ) 
{
 if (_internalMode != aims::Sinogram<T>::SINO)
    throw
      carto::format_error("Attempt to reference in SINO mode when internal is VIEW");
 
  return (_bin(seg))(z) ;
}
    
template < class T >
const AimsData<T>& aims::Sinogram<T>::getSino( int seg, int z ) const
{
 if (_internalMode != aims::Sinogram<T>::SINO)
    throw
      carto::format_error("Attempt to reference in SINO mode when internal is VIEW");
 
  return (_bin(seg))(z) ;
}
    

template < class T > 
int aims::Sinogram<T>::getNumAngles() const 
{
  if  (_internalMode == aims::Sinogram<T>::VIEW)
    return( _bin(0).dimX() );
  else
    return( _bin(0)(0).dimY() );
}

template < class T > 
int aims::Sinogram<T>::getNumPlanes( int seg )  const 
{
  if  (_internalMode == aims::Sinogram<T>::VIEW)
    return( _bin(seg)(0).dimY() );
  else
    return( _bin(seg).dimX() );
}

template < class T > 
int aims::Sinogram<T>::getNumBins() const 
{
    return( _bin(0)(0).dimX() );
}

template < class T > 
int aims::Sinogram<T>::getNumSeg() const 
{
  return _bin.dimX();
}

template < class T > 
std::string aims::Sinogram<T>::getFileType() const
{
  std::string fileType;
  _header.getProperty("file_type", fileType);
  return fileType;
}

template < class T > 
std::string aims::Sinogram<T>::getEcatFileType() const
{
  std::string fileType;
  _header.getProperty("ecat_file_type", fileType);
  return fileType;
}

template < class T > 
short aims::Sinogram<T>::getSystemType() const
{
  short systemType;
  _header.getProperty("ecat_system_type", systemType);
  return systemType;
}

template < class T > 
unsigned int aims::Sinogram<T>::getScanStartTime() const
{
  unsigned int startTime;
  _header.getProperty("ecat_scan_start_time", startTime);
  return startTime;
}

template < class T > 
 int aims::Sinogram<T>::getPrompts() const
{
   int prompts;
  _header.getProperty("ecat_prompts", prompts);
  return prompts;
}

template < class T > 
std::vector<float> aims::Sinogram<T>::getUncorrectedSingles() const
{
  std::vector<float> us;  
  _header.getProperty( "ecat_uncor_singles", us);
  return us ;
}

template < class T > 
 int aims::Sinogram<T>::getDelayed() const
{
  int delayed;
  _header.getProperty("ecat_delayed", delayed);
  return delayed;
}

template < class T > 
 int aims::Sinogram<T>::getMultiples() const
{
   int multiples;
  _header.getProperty("ecat_multiples", multiples);
  return multiples;
}

template < class T > 
 int aims::Sinogram<T>::getTrues() const
{
   int trues;
  _header.getProperty("ecat_net_trues", trues);
  return trues;
}

template < class T > 
float 
aims::Sinogram<T>::getTotalAverageCorrected() const
{
  float tac ;
  _header.getProperty("ecat_tot_avg_cor", tac);
  return tac;
}

template < class T > 
float 
aims::Sinogram<T>::getTotalAverageUncorrected() const
{
  float tau ;
  _header.getProperty("ecat_tot_avg_uncor", tau);
  return tau;
}

template < class T > 
 int 
aims::Sinogram<T>::getTotalCoinRate() const
{
   int tcr ;
  _header.getProperty("ecat_total_coin_rate", tcr);
  return tcr;
}

template < class T > 
float 
aims::Sinogram<T>::getDeadTimeCorrectionFactor() const
{
  float dtcf ;
  _header.getProperty("ecat_loss_correction_fctr", dtcf);
  return dtcf;
}


template < class T > 
unsigned int 
aims::Sinogram<T>::getStartTime() const
{
  int st;
  
  _header.getProperty("ecat_frame_start_time", st);
  return st;

}

template < class T > 
unsigned int 
aims::Sinogram<T>::getDurationTime() const
{
  int dt;
  
  _header.getProperty("ecat_frame_duration", dt);
  return dt;
}

template < class T > 
unsigned int 
aims::Sinogram<T>::getGateDuration() const
{
  int dt;
  
  _header.getProperty("ecat_gate_duration", dt);
  return dt;
}


template < class T > 
std::string aims::Sinogram<T>::getRadioPharmaceutical() const
{
  std::string radiopharmac ;
  
  _header.getProperty("ecat_radiopharmaceutical", radiopharmac);
  return radiopharmac;
}

template < class T > 
std::string aims::Sinogram<T>::getIsotopeName() const
{
  std::string isotopeName;
  
  _header.getProperty("ecat_isotope_name", isotopeName);
  return isotopeName;
}

template < class T > 
float aims::Sinogram<T>::getIsotopeHalfLife() const
{
  float isotopeHalfLife;
  
   _header.getProperty("ecat_isotope_halflife", isotopeHalfLife);
   return isotopeHalfLife;
}

template < class T > 
float aims::Sinogram<T>::getDistanceScanned() const
{
  float distanceScanned;
  
  _header.getProperty("ecat_distance_scanned", distanceScanned);
  return distanceScanned;
}

template < class T > 
float aims::Sinogram<T>::getTransaxialFOV() const
{
  float transaxialFOV;
    
  _header.getProperty("ecat_transaxial_FOV", transaxialFOV);
  return transaxialFOV;
}

template < class T > 
short aims::Sinogram<T>::getAngularCompression() const
{
  short angularCompression;
    
  _header.getProperty("ecat_angular_compression", angularCompression);
  return angularCompression;
}

template < class T > 
std::string aims::Sinogram<T>::getStudyType() const
{
  std::string studyType;
    
  _header.getProperty("ecat_study_type", studyType);
  return studyType;
}

template < class T > 
short aims::Sinogram<T>::getPatientOrientation() const
{
  short patientOrientation;
    
  _header.getProperty("ecat_patient_orientation", patientOrientation);
  return patientOrientation;
}

template < class T > 
float aims::Sinogram<T>::getPlaneSeparation() const
{
  float planeSeparation ;
    
  _header.getProperty("ecat_plane_separation", planeSeparation);
  return planeSeparation;
}

template < class T > 
short aims::Sinogram<T>::getTotalNumPlanes() const
{
  short numPlanes;
    
  _header.getProperty("ecat_num_planes", numPlanes);
  return numPlanes;
}

template < class T > 
int aims::Sinogram<T>::getMultiFileType() const
{
  int multiFileType;
    
  _header.getProperty("multi_files_type", multiFileType);
  return multiFileType;
}

template < class T > 
float aims::Sinogram<T>::getInitBedPosition() const
{
  float InitBedPosition;
    
  _header.getProperty("ecat_init_bed_position", InitBedPosition);
  return InitBedPosition;
}

template < class T > 
short aims::Sinogram<T>::getLowerTrueThreshold() const
{
  short LowerTrueThreshold;
    
  _header.getProperty("ecat_lwr_true_thres", LowerTrueThreshold);
  return LowerTrueThreshold;
}

template < class T > 
short aims::Sinogram<T>::getUpperTrueThreshold() const
{
  short UpperTrueThreshold;
    
  _header.getProperty("ecat_upr_true_thres", UpperTrueThreshold);
  return UpperTrueThreshold;
}

template < class T > 
short aims::Sinogram<T>::getAcquisitionMode() const
{
  short aquisitionMode;
    
  _header.getProperty("ecat_acquisition_mode", aquisitionMode);
  return aquisitionMode;
}

template < class T > 
short aims::Sinogram<T>::getAcquisitionType() const
{
  short aquisitionType;
    
  _header.getProperty("ecat_acquisition_type", aquisitionType);
  return aquisitionType;
}

template < class T > 
float aims::Sinogram<T>::getBinSize() const
{
  float BinSize;
    
  _header.getProperty("ecat_bin_size", BinSize);
  return BinSize;
}

template < class T > 
float aims::Sinogram<T>::getBranchingFraction() const
{
  float BranchingFraction;
    
  _header.getProperty("ecat_branching_fraction", BranchingFraction);
  return BranchingFraction;
}

template < class T > 
unsigned int aims::Sinogram<T>::getDoseStartTime() const
{
  unsigned int DoseStartTime;
    
  _header.getProperty("ecat_dose_start_time", DoseStartTime);
  return DoseStartTime;
}

template < class T > 
float aims::Sinogram<T>::getBedElevation() const
{
  float bedElevation;
    
  _header.getProperty("ecat_bed_elevation", bedElevation);
  return bedElevation;
}

template < class T > 
short aims::Sinogram<T>::getCoinSampleMode() const
{
  short coinSampleMode;
    
  _header.getProperty("ecat_coin_samp_mode", coinSampleMode);
  return coinSampleMode;
}

template < class T > 
short aims::Sinogram<T>::getAxialSampleMode() const
{
  short axialSampleMode;
    
  _header.getProperty("ecat_axial_samp_mode", axialSampleMode);
  return axialSampleMode;
}

template < class T > 
float aims::Sinogram<T>::getCalibrationFactor() const
{
  float calibrationFactor;
    
  _header.getProperty("ecat_calibration_factor", calibrationFactor);
  return calibrationFactor;
}

template < class T > 
std::vector<float> aims::Sinogram<T>::getBedOffset() const
{
  std::vector<float> bedOffset;  
    
  _header.getProperty( "ecat_bed_offset", bedOffset);
  return bedOffset ;
}

template < class T > 
short aims::Sinogram<T>::getLowerScatterThreshold() const
{
  short lowerScatterThreshold;
    
  _header.getProperty("ecat_lwr_sctr_thres", lowerScatterThreshold);
  return lowerScatterThreshold;
}

template < class T > 
float aims::Sinogram<T>::getDosage() const
{
  float dosage;
    
  _header.getProperty("ecat_dosage", dosage);
  return dosage;
}

template < class T > 
float aims::Sinogram<T>::getWellCounterFactor() const
{
  float wellCounterFactor;
    
  _header.getProperty("ecat_well_counter_factor", wellCounterFactor);
  return wellCounterFactor;
}

template < class T > 
short aims::Sinogram<T>::getSeptaState() const
{
  short septaState;
    
  _header.getProperty("ecat_septa_state", septaState);
  return septaState;
}

template < class T > 
float aims::Sinogram<T>::getXResolution() const
{
  float xRresolution;
    
  _header.getProperty("ecat_x_resolution", xRresolution);
  return xRresolution;
}

template < class T > 
float aims::Sinogram<T>::getVResolution() const
{
  float vRresolution;
    
  _header.getProperty("ecat_v_resolution", vRresolution);
  return vRresolution;
}

template < class T > 
float aims::Sinogram<T>::getZResolution() const
{
  float zRresolution;
    
  _header.getProperty("ecat_z_resolution", zRresolution);
  return zRresolution;
}

template < class T > 
float aims::Sinogram<T>::getWResolution() const
{
  float wRresolution;
    
  _header.getProperty("ecat_w_resolution", wRresolution);
  return wRresolution;
}

template < class T > 
short aims::Sinogram<T>::getScanMin() const
{
  short scanMin;
    
  _header.getProperty("ecat_scan_min", scanMin);
  return scanMin;
}

template < class T > 
short aims::Sinogram<T>::getScanMax() const
{
  short scanMax;
  
  _header.getProperty("ecat_scan_max", scanMax);
  return scanMax;
}




template < class T > 
void aims::Sinogram<T>::setFileType(const std::string& fileType)
{ 
  _header.setProperty("file_type", fileType); 
}

template < class T > 
void aims::Sinogram<T>::setEcatFileType(const std::string& fileType)
{ 
  _header.setProperty("ecat_file_type", fileType); 
}

template < class T > 
void aims::Sinogram<T>::setSystemType(short systemType)
{ 
  _header.setProperty( "ecat_system_type", systemType ) ; 
}

template < class T > 
void aims::Sinogram<T>::setScanStartTime(unsigned int scanStartTime)
{ 
  _header.setProperty( "ecat_scan_start_time", scanStartTime ) ; 
}

template < class T > 
void aims::Sinogram<T>::setPrompts( int prompts)
{ 
  _header.setProperty("ecat_prompts", prompts); 
}

template < class T > 
void aims::Sinogram<T>::setUncorrectedSingles( const std::vector<float>& uncorr )
{ 
  _header.setProperty("ecat_uncor_singles", uncorr);
}

template < class T > 
void aims::Sinogram<T>::setDelayed( int delayed)
{ 
  _header.setProperty("ecat_delayed", delayed); 
}

template < class T > 
void aims::Sinogram<T>::setMultiples( int multiples)
{ 
  _header.setProperty("ecat_multiples", multiples); 
}

template < class T > 
void aims::Sinogram<T>::setTrues( int trues)
{ 
  _header.setProperty("ecat_net_trues", trues); 
}

template < class T > 
void aims::Sinogram<T>::setTotalAverageCorrected(float tac)
{ 
  _header.setProperty("ecat_tot_avg_cor", tac); 
}

template < class T > 
void aims::Sinogram<T>::setTotalAverageUncorrected(float tau)
{ 
  _header.setProperty("ecat_tot_avg_uncor", tau); 
}

template < class T > 
void aims::Sinogram<T>::setTotalCoinRate( int tcr)
{ 
  _header.setProperty("ecat_total_coin_rate", tcr); 
}

template < class T > 
void aims::Sinogram<T>::setDeadTimeCorrectionFactor(float dtcf)
{ 
  _header.setProperty("ecat_loss_correction_fctr", dtcf); 
}

template < class T > 
void aims::Sinogram<T>::setStartTime(unsigned int st)
{ 
  _header.setProperty("ecat_frame_start_time", st); 
}

template < class T > 
void aims::Sinogram<T>::setDurationTime(unsigned int dt)
{ 
  _header.setProperty("ecat_frame_duration", dt); 
}

template < class T > 
void aims::Sinogram<T>::setGateDuration(unsigned int dt)
{ 
  _header.setProperty("ecat_gate_duration", dt); 
}

template < class T > 
void aims::Sinogram<T>::setRadioPharmaceutical(const std::string& radiopharmaceutical)
{ 
  _header.setProperty("ecat_radiopharmaceutical", radiopharmaceutical); 
}

template < class T > 
void aims::Sinogram<T>::setIsotopeName(const std::string& isotopeName)
{ 
  _header.setProperty("ecat_isotope_code", isotopeName); 
}

template < class T > 
void aims::Sinogram<T>::setIsotopeHalfLife(float isotopeHalfLife)
{ 
  _header.setProperty("ecat_isotope_halflife", isotopeHalfLife); 
}

template < class T > 
void aims::Sinogram<T>::setDistanceScanned(float DistanceScanned)
{ 
  _header.setProperty("ecat_distance_scanned", DistanceScanned); 
}

template < class T > 
void aims::Sinogram<T>::setTransaxialFOV(float TransaxialFOV)
{ 
  _header.setProperty("ecat_transaxial_FOV", TransaxialFOV); 
}

template < class T > 
void aims::Sinogram<T>::setAngularCompression(short AngularCompression)
{ 
  _header.setProperty("ecat_angular_compression", AngularCompression); 
}

template < class T > 
void aims::Sinogram<T>::setStudyType(const std::string& studyType)
{ 
  _header.setProperty("ecat_study_type", studyType); 
}

template < class T > 
void aims::Sinogram<T>::setPatientOrientation(short PatientOrientation)
{ 
  _header.setProperty("ecat_patient_orientation", PatientOrientation); 
}

template < class T > 
void aims::Sinogram<T>::setPlaneSeparation(float planeSep)
{ 
  _header.setProperty("ecat_plane_separation", planeSep); 
}

template < class T > 
void aims::Sinogram<T>::setTotalNumPlanes( short numPlanes )
{ 
  _header.setProperty("ecat_num_planes", numPlanes); 
}

template < class T > 
void aims::Sinogram<T>::setMultiFileType( int multiFileType )
{ 
  _header.setProperty("multi_files_type", multiFileType); 
}

template < class T > 
void aims::Sinogram<T>::setInitBedPosition(float InitBedPosition)
{ 
  _header.setProperty("ecat_init_bed_position", InitBedPosition); 
}

template < class T > 
void aims::Sinogram<T>::setLowerTrueThreshold(short LowerTrueThreshold)
{ 
  _header.setProperty("ecat_lwr_true_thres", LowerTrueThreshold); 
}

template < class T > 
void aims::Sinogram<T>::setUpperTrueThreshold(short UpperTrueThreshold)
{ 
  _header.setProperty("ecat_upr_true_thres", UpperTrueThreshold); 
}  

template < class T > 
void aims::Sinogram<T>::setAcquisitionMode(short AquisitionMode)
{ 
  _header.setProperty("ecat_acquisition_mode", AquisitionMode); 
}

template < class T > 
void aims::Sinogram<T>::setAcquisitionType( short AquisitionType )
{ 
  _header.setProperty( "ecat_acquisition_type", AquisitionType ); 
}

template < class T > 
void aims::Sinogram<T>::setBinSize(float BinSize)
{ 
  _header.setProperty("ecat_bin_size", BinSize); 
}

template < class T > 
void aims::Sinogram<T>::setBranchingFraction(float BranchingFraction)
{ 
  _header.setProperty("ecat_branching_fraction", BranchingFraction); 
}

template < class T > 
void aims::Sinogram<T>::setDoseStartTime(unsigned int DoseStartTime)
{ 
  _header.setProperty("ecat_dose_start_time", DoseStartTime); 
}

template < class T > 
void aims::Sinogram<T>::setBedElevation(float bedElevation)
{ 
  _header.setProperty("ecat_bed_elevation", bedElevation); 
}

template < class T > 
void aims::Sinogram<T>::setCoinSampleMode( short coinSampleMode )
{ 
  _header.setProperty( "ecat_coin_sample_mode", coinSampleMode ); 
}

template < class T > 
void aims::Sinogram<T>::setAxialSampleMode( short axialSampleMode )
{ 
  _header.setProperty( "ecat_axial_sample_mode", axialSampleMode ); 
}

template < class T > 
void aims::Sinogram<T>::setCalibrationFactor(float calibrationFactor)
{ 
  _header.setProperty("ecat_calibration_factor", calibrationFactor); 
}

template < class T > 
void aims::Sinogram<T>::setBedOffset(const std::vector<float>& bedOffset)
{ 
  _header.setProperty("ecat_bed_offset", bedOffset); 
}

template < class T > 
void aims::Sinogram<T>::setLowerScatterThreshold( short lowerScatterThreshold )
{ 
  _header.setProperty( "ecat_lwr_sctr_thres", lowerScatterThreshold ); 
}

template < class T > 
void aims::Sinogram<T>::setDosage(float dosage)
{ 
  _header.setProperty("ecat_dosage", dosage); 
}

template < class T > 
void aims::Sinogram<T>::setWellCounterFactor(float wellCounterFactor)
{ 
  _header.setProperty("ecat_well_counter_factor", wellCounterFactor); 
}

template < class T > 
void aims::Sinogram<T>::setSeptaState( short septaState )
{ 
  _header.setProperty( "ecat_septa_state", septaState ); 
}

template < class T > 
void aims::Sinogram<T>::setXResolution(float xResolution)
{ 
  _header.setProperty("ecat_x_resolution", xResolution); 
}

template < class T > 
void aims::Sinogram<T>::setVResolution(float vResolution)
{ 
  _header.setProperty("ecat_v_resolution", vResolution); 
}

template < class T > 
void aims::Sinogram<T>::setZResolution(float zResolution)
{ 
  _header.setProperty("ecat_z_resolution", zResolution); 
}

template < class T > 
void aims::Sinogram<T>::setWResolution(float wResolution)
{ 
  _header.setProperty("ecat_w_resolution", wResolution); 
}

template < class T > 
void aims::Sinogram<T>::setScanMin(short scanMin)
{ 
  _header.setProperty("ecat_scan_min", scanMin); 
}

template < class T > 
void aims::Sinogram<T>::setScanMax(short scanMax)
{ 
  _header.setProperty("ecat_scan_max", scanMax); 
}

template < class T > 
aims::Sinogram<T> aims::Sinogram<T>::clone() const
{
  //cout << "cloning" << endl ;
  //cout << "scanner config" << &getScannerConf() << endl ;
  this->getInternalMode() ;
  //cout << "1" << endl ;
  
  //cout << "2" << endl ;
  aims::Sinogram<T> copyOf(getScannerConf(), getInternalMode() ) ;
  
  //std::cout << "copy sino created " << std::endl ;

/*   std::cout << "setting header in clone" << std::endl ; */
  copyOf.setHeader( _header ) ;
/*   std::cout << "header set in clone" << std::endl ; */
  //std::cout << "header set" << std::endl ;
  AimsData< AimsData<AimsData<T> > > copyBin = copyOf.start() ;
  copyBin = AimsData< AimsData<AimsData<T> > >(_bin.dimX()) ;
  for( int seg = 0 ; seg < _bin.dimX() ; ++seg ){
    //std::cout << "seg = " << seg << std::endl ;
    copyBin(seg) = AimsData<AimsData<T> >(_bin(seg).dimX()) ;
    for( int thetaOrZ = 0 ; thetaOrZ < _bin(seg).dimX()  ; ++thetaOrZ ){
      copyBin(seg)(thetaOrZ) = AimsData<T>(_bin(seg)(thetaOrZ).dimX(), _bin(seg)(thetaOrZ).dimY()) ;
      for( int zOrTheta = 0 ; zOrTheta < _bin(seg)(thetaOrZ).dimY() ; ++zOrTheta )
	for( int x = 0 ; x < _bin(seg)(thetaOrZ).dimX() ; ++x )
	  copyOf(seg, thetaOrZ, zOrTheta, x ) = _bin(seg)(thetaOrZ)( x, zOrTheta ) ;
    }
  }
  return copyOf ;
}

template < class T > 
void aims::Sinogram<T>::printHeader() const
{
/*std::  cout << "File Type : " << getFileType() << endl
       << "Ecat File Type : " << getEcatFileType() << endl 
       << "System Type : " << getSystemType() << endl 
       << "Scan Start Time : " << getScanStartTime() << endl 
       << "Radio Pharmaceutical : " << getRadioPharmaceutical() << endl 
       << "Isotope Name : " << getIsotopeName() << endl 
       << "Isotope HalfLife : " << getIsotopeHalfLife() << endl 
       << "Distance Scanned : " << getDistanceScanned() << endl 
       << "Transaxial FOV : " << getTransaxialFOV() << endl 
       << "Angular Compression : " << getAngularCompression() << endl 
       << "Study Type : " << getStudyType() << endl 
       << "Patient Orientation : " << getPatientOrientation() << endl 
       << "Plane Separation : " << getPlaneSeparation() << endl 
       << "Total Num Planes : " << getTotalNumPlanes() << endl 
       << "Multi File Type : " << getMultiFileType() << endl 
       << "Init Bed Position : " << getInitBedPosition() << endl 
       << "Lower True Threshold : " << getLowerTrueThreshold() << endl 
       << "Upper True Threshold : " << getUpperTrueThreshold() << endl 
       << "Aquisition Mode : " << getAcquisitionMode() << endl 
       << "Aquisition Type : " << getAcquisitionType() << endl 
       << "Bin Size : " << getBinSize() << endl 
       << "Branching Fraction : " << getBranchingFraction() << endl 
       << "Dose Start Time : " << getDoseStartTime() << endl 
       << "Prompts : " << getPrompts() << endl 
       << "First Uncorrected Singles : " << getUncorrectedSingles()[0] << endl 
       << "Fourth Uncorrected Singles : " << getUncorrectedSingles()[3] << endl 
       << "Delayed : " << getDelayed() << endl 
       << "Multiples : " << getMultiples() << endl 
       << "Trues : " << getTrues() << endl 
       << "Total Average Corrected : " << getTotalAverageCorrected() << endl 
       << "Total Average Uncorrected : " << getTotalAverageUncorrected() << endl 
       << "Total Coin Rate : " << getTotalCoinRate() << endl 
       << "Dead Time Correction Factor : " << getDeadTimeCorrectionFactor() << endl 
       << "Start Time : " << getStartTime() << endl 
       << "Duration Time : " << getDurationTime() << endl
       << "Bed Elevation : " << getBedElevation()<< endl 
       << "Coin Sample Mode : " << getCoinSampleMode()<< endl 
       << "Axial Sample Mode : " << getAxialSampleMode()<< endl 
       << "Calibration Factor : " << getCalibrationFactor()<< endl 
       << "Bed Offset : " << getBedOffset()[0]<< endl 
       << "Lower Scatter Threshold : " << getLowerScatterThreshold()<< endl 
       << "Dosage : " << getDosage()<< endl 
       << "Well Counter Factor : " << getWellCounterFactor()<< endl 
       << "X Resolution : " << getXResolution()<< endl 
       << "V Resolution : " << getVResolution()<< endl 
       << "Z Resolution : " << getZResolution()<< endl 
       << "W Resolution : " << getWResolution()<< endl 
       << "Scan Min : " << getScanMin()<< endl 
       << "Scan Max : " << getScanMax()<< endl ;    
*/ }

#endif

