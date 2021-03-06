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



#ifndef _AIMSCORRECTIONEPI_CONTEXTEPI_H_
#define _AIMSCORRECTIONEPI_CONTEXTEPI_H_


#include <aims/data/data.h>
#include <aims/vector/vector.h>
#include <cartobase/thread/loopContext.h>


class ContextEpi : public carto::LoopContext
{

  public:

    enum SmoothingType
    {

      DericheGaussian,
      DiscreteGaussian

    };

    ContextEpi( int nOrientation,
                int size, int sizeDivider, float inPlaneResolution,
                int levels, double ftol,
		const AimsVector< float, 3 >& deltaP,
                AimsData< float >& S,
                AimsData< float >& T0,
                AimsData< float >& T1,
		bool verbose = false );
    ContextEpi( int nOrientation,
                int size, int sizeDivider, float inPlaneResolution,
                int levels, double ftol,
		const AimsVector< float, 3 >& deltaP,
                AimsData< float >& S,
                AimsData< float >& T0,
                AimsData< float >& T1,
                carto::ThreadGauge* gauge, carto::ThreadCancel* cancel,
		bool verbose = false );
    ~ContextEpi();

    void setDericheGaussianSmoothing( float sigma );
    void setDiscreteGaussianSmoothing( int sizeMask );

    void doIt( int startIndex, int countIndex );

    void setLocation( short* t2Image, short** dwImages,
                      short** outImages, int location );

    bool verbose;

  protected:

    void init();

    int _nOrientation;

    int _realSize;
    float _realRes;
    int _procSize;
    float _procRes;

    int _sizeDivider;
    int _levels;
    double _ftol;
    AimsVector< float, 3 > _deltaP;

    AimsData< float >& _Sss;	//  _S seems to be a macro on solaris...
    AimsData< float >& _T0;
    AimsData< float >& _T1;

    short* _t2Image;
    short** _dwImages;
    short** _outImages;

    short* _ref;
    short** _dis;
    short** _out;

    float** _p1;
    float** _p2;
    float** _p12linear;
    float*** _p12;

    SmoothingType _smoothingType;
    float _sigma;
    int _sizeMask;

    int _location;

};


#endif
