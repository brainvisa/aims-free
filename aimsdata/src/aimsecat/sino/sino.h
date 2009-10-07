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

#ifndef AIMS_SINO_SINO_H
#define AIMS_SINO_SINO_H

#include <aims/data/data_g.h>
#include <aims/io/ecatSinoheader.h>
#include <ecat+/io/io.h>
#include <iostream>


namespace aims
{

  /** An attempt to efficiently manage view/sinogram acquisition PET data.
   */
  class ScannerConf
  {
  public:
    // Ring Difference = cros
    ScannerConf( int ring, int numSeg, int ringDiff, int span, int numAngles, int numBin, 
    	         float zResolution, float binResolution, float faceRadius ) ;
    ScannerConf( UnifiedEcatSinoInfo *u );
    ScannerConf( const ScannerConf &s);
    ~ScannerConf() {}
    
    int getSpan() const {return _span ; }
    int getMaxSeg() const{ return _numSeg; }
    int getZ( int segment ) const { return _numZ[ segment ]; }
    int getNumAngles() const {return _numAngles ; }
    int getNumBins() const { return _numBins ;}
    float getZResolution() const {return _zResolution ;}
    float getXResolution() const {return _binResolution ;}
    
    bool operator!=(const ScannerConf& conf) const ;
    
    ScannerConf& operator=(const ScannerConf& ) ;
    //private:
    int  _ring;
    int  _ringDiff;
    int  _span;
    int _numSeg;
    int _numAngles ;
    int _numBins ;
    
    float _zResolution ;
    float _binResolution ;
    std::vector<int> _numZ;
  };
  static ScannerConf ecatHRPlusUsual(63, 5, 22, 9, 0, 0, 0., 0., 0.);


  template <class T>
  class AIMSDATA_API Sinogram 
  {
  public:
    enum Mode { VIEW, SINO };

    Sinogram( const ScannerConf& conf = ecatHRPlusUsual ) :
      _scannerConfig(conf)
    {
      _internalMode = Sinogram<T>::VIEW;
      _displayMode = Sinogram<T>::VIEW;
    }
    Sinogram( const ScannerConf& conf, Sinogram<T>::Mode create_mode );
    
    // To Do !!! Ne pas oublier de donner des sizeX et sizeY aux aimsData en dessous du premier, pour que getPlaneInInternalMode fonctionne.
    void allocate(  int, int );
    
    Mode getInternalMode() const
    { return _internalMode; }
     
    void setInternalMode( const Sinogram<T>::Mode& /*internalMode*/ ) 
    {
      ASSERT( 0 ); //not coded yet. 
    }
    
    Mode displayMode() 
    { return _displayMode; }
    
    void setDisplayMode( Mode displayMode) 
    { _displayMode = displayMode; }
    

    AimsData<AimsData<AimsData< T > > >& start() { return _bin; }

    int  transCodeSegment( int s) const ;
    int  getZ( int segment ) { return _scannerConfig.getZ(segment); };

    ~Sinogram() 
    {}

    Sinogram<T>&  operator = (const Sinogram<T>& other);
    
    inline T& operator() ( int segment, int zOrAlpha, int alphaOrZ, int ro )
    {
      if( _internalMode == _displayMode )
	return _bin( segment )( zOrAlpha )( ro, alphaOrZ );
      else
	return _bin( segment )( alphaOrZ )( ro, zOrAlpha );
    }
    
    inline AimsData< AimsData< T > >& operator() ( int segment )
    {  return _bin( segment ); }

    AimsData<T>  getPlaneInDisplayMode( int, int );
    AimsData<T>& getPlaneInInternalMode( int, int );
    
    AimsData<T>  extractView( int, int );
    AimsData<T>& getView( int, int);
    const AimsData<T>& getView( int, int ) const;

    AimsData<T>  extractSino( int, int  );
    AimsData<T>& getSino( int, int  );
    const AimsData<T>& getSino( int, int  ) const;

    const aims::PythonHeader& header() const { return _header; } ;
    void setHeader( const PythonHeader& hdr ) 
    { 
      _header.copy( hdr ) ;
    }
    
    int getNumAngles() const ;
    int getNumPlanes( int seg ) const ;
    int getNumBins() const ;
    int getNumSeg() const ;
    

    int getRing() const { return _scannerConfig._ring; }
    int getSpan() const { return _scannerConfig._span; }
    int getRingDiff() const { return _scannerConfig._ringDiff; }
 
    std::string getFileType() const ;
    std::string getEcatFileType()  const ;
    short getSystemType()  const ;
    unsigned int getScanStartTime()  const ;
    std::string getRadioPharmaceutical()  const ;
    std::string getIsotopeName() const ;
    float  getIsotopeHalfLife() const ;
    float  getDistanceScanned() const ;
    float  getTransaxialFOV() const ;
    short    getAngularCompression() const ;
    std::string getStudyType() const ;
    short    getPatientOrientation() const ;
    float  getPlaneSeparation()  const ;
    short    getTotalNumPlanes()  const ;
    int    getMultiFileType() const ; // Is it a multi bed, multi frame or multi gate
    float  getInitBedPosition() const ;
    short    getLowerTrueThreshold() const ;
    short    getUpperTrueThreshold() const ;
    short    getAcquisitionMode() const ;
    short    getAcquisitionType() const ;
    float  getBinSize() const ;
    float  getBranchingFraction() const ;
    unsigned int  getDoseStartTime() const ;
    float getBedElevation() const ;
    short getCoinSampleMode() const ;
    short getAxialSampleMode() const ;
    float getCalibrationFactor() const ;
    std::vector<float> getBedOffset() const ;
    short getLowerScatterThreshold() const ;
    float getDosage() const ;
    float getWellCounterFactor() const ;
    short getSeptaState() const ;
    float getXResolution() const ;
    float getVResolution() const ;
    float getZResolution() const ;
    float getWResolution() const ;
    short getScanMin() const ;
    short getScanMax() const ;
    
    int getPrompts() const ;
    std::vector<float> getUncorrectedSingles( )  const ;
    int getDelayed() const ;
    int getMultiples() const ;
    int getTrues() const ;
    float getTotalAverageCorrected()  const ;
    float getTotalAverageUncorrected()  const ;
    int getTotalCoinRate()  const ;
    float getDeadTimeCorrectionFactor()  const ;
    unsigned int getStartTime()  const ;
    unsigned int getDurationTime()  const ;
    unsigned int getGateDuration() const ;

    void setFileType(const std::string& fileType);
    void setEcatFileType( const std::string& fileType ) ;
    void setSystemType( short systemType) ;
    void setScanStartTime( unsigned int scanStartTime) ;

    void setPrompts( int prompts);
    void setUncorrectedSingles( const std::vector<float>& uncorrSingles ) ;
    void setDelayed( int delayed);
    void setMultiples( int multiples);
    void setTrues( int trues);

    void setTotalAverageCorrected( float ) ;
    void setTotalAverageUncorrected( float) ;
    void setTotalCoinRate( int) ;
    void setDeadTimeCorrectionFactor(float) ;
    void setStartTime(unsigned int) ;
    void setDurationTime(unsigned int) ;
    void setGateDuration(unsigned int) ;

    void setRadioPharmaceutical(const std::string& radiopharmaceutical) ;
    void setIsotopeName(const std::string& isotopeName);
    void setIsotopeHalfLife(float isotopeHalfLife);
    void setDistanceScanned(float DistanceScanned);
    void setTransaxialFOV(float TransaxialFOV);
    void setAngularCompression(short AngularCompression);
    void setStudyType(const std::string& StudyType);
    void setPatientOrientation(short PatientOrientation);
    void setPlaneSeparation(float planeSep) ;
    void setTotalNumPlanes(short NumPlanes);
    void setMultiFileType( int MultiFileType );
    void setInitBedPosition(float InitBedPosition);
    void setLowerTrueThreshold(short LowerTrueThreshold);
    void setUpperTrueThreshold(short UpperTrueThreshold);
    void setAcquisitionMode(short AquisitionMode);
    void setAcquisitionType(short AquisitionType);
    void setBinSize(float BinSize);
    void setBranchingFraction(float BranchingFraction);
    void setDoseStartTime(unsigned int DoseStartTime);    
    void setBedElevation (float) ;
    void setCoinSampleMode (short) ;
    void setAxialSampleMode (short) ;
    void setCalibrationFactor (float) ;
    void setBedOffset (const std::vector<float>&) ;
    void setLowerScatterThreshold (short) ;
    void setDosage (float) ;
    void setWellCounterFactor (float) ;
    void setSeptaState (short) ;
    void setXResolution (float) ;
    void setVResolution (float) ;
    void setZResolution (float) ;
    void setWResolution (float) ;    
    void setScanMin(short scanMin) ;
    void setScanMax(short scanMax) ;

    void printHeader() const ;
    Sinogram<T> clone() const ;

    const ScannerConf& getScannerConf() const { return _scannerConfig ; } 
  private:
    PythonHeader  			_header ;
    AimsData<AimsData<AimsData< T > > > _bin;
    Mode				_internalMode;
    Mode				_displayMode;    
    ScannerConf 			_scannerConfig;
  };
}

#endif
