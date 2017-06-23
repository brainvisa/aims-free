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


#ifndef AIMS_RESAMPLING_POLYNOMIAL_H
#define AIMS_RESAMPLING_POLYNOMIAL_H

#include <cartobase/object/object.h>
#include <aims/vector/vector.h>
#include <aims/resampling/samplable.h>

/** The template class used to generate Polynomial objects.
*/
namespace aimsalgo
{
	template <class T,unsigned D>
	class Polynomial : public Samplable<T,D>
	{
		public:
			Polynomial(std::vector<T> * coefficients, T orderStep = 1);
			virtual ~Polynomial();

			virtual void setCoefficients(std::vector<T> * coefficients);
			virtual std::vector<T> * getCoefficients();
			virtual void setOrderStep(T orderStep);
			virtual T getOrderStep();
			virtual bool contains(AimsVector<T,D> & vector);
			virtual T resolve(AimsVector<T,D> & vector);
			virtual void resetDimensionOrders();
			virtual AimsVector<T,D> getDimensionOrders(unsigned index);
			virtual T getValue(AimsVector<T,D> & vector, AimsVector<T,D> & dimensionOrders, T coefficient);
			virtual void displayEquation();

		private:
			unsigned _order;
			unsigned _coefficientsTheoricSize;
			T _orderStep;
			std::vector<T> * _coefficients;
			std::vector<AimsVector<T,D> > * _dimensionOrders;
	};
	
}

#endif
