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



#ifndef AIMS_PERFUSION_PERF_PARAM_H
#define AIMS_PERFUSION_PERF_PARAM_H

#include <stdlib.h>

namespace aims
{

  class PerfusionParameters
  {
  public:

    PerfusionParameters();
    virtual ~PerfusionParameters() { }

    float tr() { return _tr; }
    float te() { return _te; }
    bool hasVFilter() { return _doVFilter; }
    float bThres() { return _bThres; }
    float lvThres() { return _lvThres; }
    float skipThres() { return _skipThres; }
    int skip() { return _skip; }
    int nAif() { return _nAif; }
    float aifThreshold() { return _aifThres; }
    int preInj() { return _preInj; }
    int aifType() { return _aifType; }
    int nAvg() { return _nAvg; }
    int fitType() { return _fitType; }
    bool hasCorrection() { return _hasCorrection; }
    int svdType() { return _svdType; }
    float svdThreshold() { return _svdThres; }
    float dose() { return _dose; }
    float phiGd() { return _phiGd; }

    void setTr( float tr )  { _tr = tr; }
    void setTe( float te )  { _te = te; }
    void setVFilter( bool onOff ) { _doVFilter = onOff; }
    void setBThres( float t ) { _bThres = t; }
    void setLVThres( float t ) { _lvThres = t; }
    void setSkipThres( float t ) { _skipThres = t; }
    void setSkip( int s ) { _skip = s; }
    void setnAif( int n ) { _nAif = n; }
    void setAifThreshold( float t ) { _aifThres = t; }
    void setPreInj( int p ) { _preInj = p; }
    void setAifType( int t ) { _aifType = t; }
    void setnAvg( int n ) { _nAvg = n; }
    void setFitType( int t ) { _fitType = t; }
    void setCorrection( bool onOff ) { _hasCorrection = onOff; }
    void setSVDType( int t ) { _svdType = t; }
    void setSVDThres( float t ) { _svdThres = t; }
    void setDose( float d ) { _dose = d; }
    void setPhiGd( float p ) { _phiGd = p; }

  private:

    // Image acquisition informations
    float _tr;
    float _te;

    // Brain mask
    bool _doVFilter;
    float _bThres;
    float _lvThres;

    // Signal stabilization
    float _skipThres;
    int _skip;

    // Number of Aif possible point to detect
    int _nAif;
    float _aifThres;

    // Aif bolus start time
    int _preInj;

    // Number of selected points for Aif computation
    int _aifType;
    int _nAvg;

    // Fit
    int _fitType;

    // Convolution matrix linearity correction
    bool _hasCorrection;

    // SVD deconvolution
    int _svdType;
    float _svdThres;
  
    // Maps correction coefficient
    float _dose;
    float _phiGd;
  };

}

#endif
