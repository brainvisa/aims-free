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


#ifndef AIMS_SIGNALFILTER_FIRFILTER_H
#define AIMS_SIGNALFILTER_FIRFILTER_H

#include <aims/config/aimsalgo_config.h>
#include <aims/def/general.h>

template <class T> class AimsData;


/** The 4 dimensions.
    Useful enum to specify one direction.
*/
AIMSDATA_API enum AimsDirectionAxis
{
  AIMS_X_DIRECTION,
  AIMS_Y_DIRECTION,
  AIMS_Z_DIRECTION,
  AIMS_T_DIRECTION
};


/** The different types of finite impulse response filters
*/
enum AimsFirFilterType
{
  ///
  AIMS_FIR_LOWPASS,
  ///
  AIMS_FIR_HIGHPASS,
  ///
  AIMS_FIR_BANDPASS,
  ///
  AIMS_FIR_BANDSTOP
};


/** The class of Finite Impulse Response filters
*/
class AIMSALGO_API AimsFIRFilter
{ protected:
    /**@name Data*/
    //@{
    /// Sampling frequency
    float _fSampling;
    /// Cut frequency
    float _fCut;
    /// Low cut frequency
    float _fLow;
    /// High cut frequency
    float _fHigh;
    /// Frequency resolution
    float _deltaF;
    /// Residual ondulation in pass-band
    float _deltaP;
    /// Residual ondulation in stop-band
    float _deltaS;
    /// Beta parameter
    float _beta;
    /// Type of the filter
    int _type;
    /// Number of coefficients
    int _nCoef;
    /// Impulse response
    AimsData<float> *_impulseResponse;
    //@}

    /**@name Methods*/
    //@{
    /// Process the number of coefficients
    void numberOfFIRFilterCoef();
    /// Return a Kaiser window
    AimsData<float> kaiserWindow();
    /// Return the ideal impulse response of the F.I.R. filter
    AimsData<float> idealImpulseResponse();
    /// Return the modified impulse response of the F.I.R. filter
    AimsData<float> impulseResponse();
    /// send the process which evaluate the coefficients
    void initialize();
  //@}

  public:
    /**@name Constructor and Destructor*/
    //@{
    /** Constructor dedicated to lowpass filters or highpass filters.
        @param fs sampling frequency
        @param fc cutting frequency
        @param deltaf frequency resolution or step
        @param deltap residual ripple in pass band
        @param deltas residual ripple in stop band
        @param type type of the FIR filter (AIMS_FIR_LOWPASS,AIMS_FIR_HIGHPASS)
    */
    AimsFIRFilter(float fs, float fc, float deltaf, float deltap, float deltas,
                  int type);
    /** Constructor dedicated to stopband filters or passband filters.
        @param fs sampling frequency
        @param fl lower cutting frequency
        @param fh higher cutting frequency
        @param deltaf frequency resolution or step
        @param deltap residual ripple in pass band
        @param deltas residual ripple in stop band
        @param type type of the FIR filter (AIMS_FIR_BANDPASS,AIMS_FIR_BANDSTOP)
    */
    AimsFIRFilter(float fs, float fl, float fh, float deltaf, float deltap,
                  float deltas, int type);
    /** Destructor free the impulse response*/
    virtual ~AimsFIRFilter();    
    //@}

    /**@name Material and methods*/
    //@{
    /// Return the sampling frequency
    float fSampling() const;
    /// Return the cut frequency (highpass or lowpass)
    float fCut()      const;
    /// Return the low cut frequency (bandpass or bandstop)
    float fLow()      const;
    /// Return the high cut frequency (bandpass or bandstop)
    float fHigh()     const;
    /// Return the frequency step
    float deltaF()    const;
    /// Return the passband ripple
    float deltaP()    const;
    /// Return the stopband ripple
    float deltaS()    const;
    /// Return the type of the filter
    int  type()      const;
  
    /** Send a filter processing on a data according to a direction.
        @param data 1D/2D/3D or 4D data const reference to process
        @param dir direction of filtering (AIMS_X_DIRECTION, AIMS_Y_DIRECTION,.)
    */
    AimsData<float> 
    process(const AimsData<float> &data,
            AimsDirectionAxis dir = AIMS_X_DIRECTION);
    //@}
};


#endif
