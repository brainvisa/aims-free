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


#ifndef AIMS_PRIMALSKETCH_PERONAMALIKSMOOTHER_H
#define AIMS_PRIMALSKETCH_PERONAMALIKSMOOTHER_H

#include <cstdlib>
#include <aims/primalsketch/imageSmoother.h>

namespace aims {

  template<class T> class PeronaMalikSmoother : public Smoother<AimsData<T>, AimsData<T> >
  {

  private:

  	float _K;				// control of the gradient - proportion  of gradients which are NOT edges - generally around 0.98
	float _gradK;			// the actual gradient bound, computed from K and the image to be processed
	float _sigma; 		// regularisation parameter (i.e. smoothing of the gradient)
	float _dt;

	int _conductance;  // vaut 1, ou 2, ou autrre si on rajoute des fonctions de conductance

	inline float conductance(float x) {switch (_conductance) {	case 1: return(conductance1(x));
																								case 2: return(conductance2(x));
																								default : exit(EXIT_FAILURE);}}

	inline float conductance1(float x) {return (1.0/float(1.0+(x*x)/(_gradK*_gradK)));}
	inline float conductance2(float x) {return (exp(-(x*x)/(_gradK*_gradK)));}

	void SetDt(float dt) {if (dt<=0.25) _dt=dt; else {cerr << "Diffusion Smoother : dt must be <= 0.25" << endl; exit(EXIT_FAILURE);}}

  public:

	PeronaMalikSmoother(float dt, float K, float sigma, int cond)
		: _K(K), _sigma(sigma), _conductance(cond) {SetDt(dt);}

    AimsData<T> doSmoothing( const AimsData<T> & ima, int maxiter, bool verbose=false);

	float dt() {return _dt;}
    bool optimal() {return true;}

  };

}

#endif
