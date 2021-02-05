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

#include <cstdlib>
#include <aims/resampling/polynomial.h>
#include <cmath>
#include <sstream>

/** The virtual template class fo which samplable classes must inherit from.
    The first template argument provides the type of vector stored items and
    the second template argument provides the size of vector.
*/
using namespace aimsalgo;

template <class T,unsigned D>
Polynomial<T,D>::Polynomial(std::vector<T> * coefficients, T orderStep)
{
	this->_coefficients = NULL;
	this->_dimensionOrders = NULL;
	setOrderStep(orderStep);
	setCoefficients(coefficients);
}

template <class T,unsigned D>
Polynomial<T,D>::~Polynomial()
{
	delete this->_dimensionOrders;
}

template <class T,unsigned D>
void Polynomial<T,D>::setCoefficients(std::vector<T> * coefficients)
{
	unsigned order;

	// Evaluate the order rounded value to check if coefficients are missing
	order = (unsigned)round(pow(coefficients->size(), 1.0 / D));
	this->_order = (((coefficients->size() - (unsigned)pow(order, D)) >= 1) ? order - 1 : order);
	this->_coefficientsTheoricSize = (unsigned)(pow(this->_order, D));
	this->_coefficients = coefficients;
	this->resetDimensionOrders();
}

template <class T,unsigned D>
std::vector<T> * Polynomial<T,D>::getCoefficients()
{
	return this->_coefficients;
}

template <class T,unsigned D>
void Polynomial<T,D>::setOrderStep(T orderStep)
{
	this->_orderStep = orderStep;
}

template <class T,unsigned D>
T Polynomial<T,D>::getOrderStep()
{
	return this->_orderStep;
}

template <class T,unsigned D>
bool Polynomial<T,D>::contains(AimsVector<T,D> & vector)
{
	return (this->resolve(vector) <= 0);
}

template <class T,unsigned D>
T Polynomial<T,D>::resolve(AimsVector<T,D> & vector)
{
	T coefficient, result;
	unsigned index;

	/* Go through all the coefficients */
	result = 0;

	for (index = 0; index < this->_coefficientsTheoricSize; index++)
	{
		coefficient = (*this->_coefficients)[index];
		
		if (coefficient != 0)
		{
			result += this->getValue(vector, (*this->_dimensionOrders)[index], coefficient);
		}
	}

	return (result);
}

template <class T,unsigned D>
void Polynomial<T,D>::resetDimensionOrders()
{
	unsigned index;

	/* Reset the preprocessed dimension orders */
	delete this->_dimensionOrders;
	this->_dimensionOrders = new std::vector<AimsVector<T,D> >(this->_coefficientsTheoricSize);

	for (index = 0; index < this->_coefficientsTheoricSize; index++)
	{
		(*this->_dimensionOrders)[index] = this->getDimensionOrders(index);
	}
}

template <class T,unsigned D>
AimsVector<T,D> Polynomial<T,D>::getDimensionOrders(unsigned index)
{
	unsigned orderIndex;
	int dimensionIndex;
	float size;
	AimsVector<T,D> dimensionOrders((T)0);

	for(dimensionIndex = (D - 1); dimensionIndex >= 0; dimensionIndex--)
	{
		/* Get the integer part of the division */
		size = pow(this->_order, dimensionIndex);
		orderIndex = (unsigned)floor(index / size);
		dimensionOrders[dimensionIndex] = orderIndex * this->_orderStep;
		index -= (orderIndex * (unsigned)size);
	}
	
	return dimensionOrders;
}

template <class T,unsigned D>
T Polynomial<T,D>::getValue(AimsVector<T,D> & vector, AimsVector<T,D> & dimensionOrders, T coefficient)
{
	T result;
	unsigned dimensionIndex;

	result = coefficient;
	if (coefficient != 0)
	{
		for(dimensionIndex = 0; dimensionIndex < D; dimensionIndex++)
		{
			result *= pow(vector[dimensionIndex], dimensionOrders[dimensionIndex]);
		}
	}

	return result;
}

template <class T,unsigned D>
void Polynomial<T,D>::displayEquation()
{
	T coefficient, order;
	unsigned index, dimensionIndex;
	AimsVector<T,D> dimensionOrders;
	std::string value;

	for (index = 0; index < this->_coefficientsTheoricSize; index++)
	{
		std::ostringstream stream1;
		coefficient = (*this->_coefficients)[index];
		
		if (coefficient != 0)
		{
			dimensionOrders = (*this->_dimensionOrders)[index];
			value = "";

			if ((coefficient > 0) && (index > 0))
			{
				value += " + ";
			}
			else if (coefficient < 0)
			{
				value += " - ";
			}

			stream1 << fabs(coefficient);
			value += stream1.str();
			for(dimensionIndex = 0; dimensionIndex < D; dimensionIndex++)
			{
				std::ostringstream stream2;
				order = dimensionOrders[dimensionIndex];

				if (order != 0)
				{
					stream2 << "x";
					stream2 << dimensionIndex;
				
					if (order != 1)
					{
						stream2 << "**";
						stream2 << order;
					}
		
					value += stream2.str();
				}
			}
			std::cout << value;
		}
	}
	
	std::cout << std::endl;
}

/** Declare the existing templated instanciations */
template class aimsalgo::Polynomial<float,3>;
