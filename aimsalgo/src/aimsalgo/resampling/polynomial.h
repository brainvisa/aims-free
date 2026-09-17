
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
