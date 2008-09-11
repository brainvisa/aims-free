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


#include <math.h>
#include <cmath>
#include <aims/resampling/bucketmapsampler.h>

using namespace aims;
using namespace aimsalgo;
using namespace carto;

/**
	Get the sampling bucket map for the object
*/
template <class T,unsigned D>
Object BucketMapSampler<T,D>::sample( Samplable<T,D> & object, AimsVector<T,D> & start, AimsVector<T,D> & sizes, AimsVector<T,D> & resolution )
{
	carto::rc_ptr<BucketMap<Void> > bucketMap( new BucketMap<Void>() );
	Object	result = Object::value( bucketMap );
	AimsVector<short,D> partialCoords((short)0);

	// Set result bucket map sizes
	if (D > 0)
	{
		bucketMap->setSizeX(static_cast<float>(resolution[0]));
	}
	if (D > 1)
	{
		bucketMap->setSizeY(static_cast<float>(resolution[1]));
	}
	if(D > 2)
	{
		bucketMap->setSizeZ(static_cast<float>(resolution[2]));
	}
	if(D > 3)
	{
		bucketMap->setSizeT(static_cast<float>(resolution[3]));
	}

	// Local storage of values in object in order to not process these during sampling.
	this->_start = start;
	this->_sizes = sizes;
	this->_resolution = resolution;
	this->_normalizedStart = this->normalize(start, resolution);
	this->_normalizedSizes = this->normalize(sizes, resolution);

	// Process sampling on object
	this->sample(&object, 0, partialCoords, (*bucketMap));
        std::cout << "OK, " << bucketMap->size() << ", " << (*bucketMap)[0].size() << "\n";
	return result;
}

/**
Recursively samples by generating a grid and checking that each point is contained in the sampled object.
*/
template <class T,unsigned D>
void BucketMapSampler<T,D>::sample(Samplable<T,D> * data, unsigned dimensionIndex, AimsVector<short,D> & partialCoords, aims::BucketMap<Void> & result)
{
	T size;

	if (dimensionIndex < D )
	{
		// Reset the new dimension coordinate
		partialCoords[dimensionIndex] = 0;
		size = this->_normalizedSizes[dimensionIndex];

		while (partialCoords[dimensionIndex] <= size)
		{
			// Recursive call to get the next dimension values
			this->sample(data, dimensionIndex + 1, partialCoords, result);

			// Increment dimension values
			partialCoords[dimensionIndex] += 1;
		}
	}
	else
	{
		bool contains;
		AimsVector<T,D> coordinates((T)0);
		
		if ( data != 0 )
		{
			// Process denormalized coordinates and check is they are contained in object
			coordinates = this->_start + this->denormalize(partialCoords, this->_resolution);
			contains = data->contains(coordinates);
		}
		else
		{
			contains = true;
		}

		if (contains)
		{
			// Add the generated normalized point to result
			result.insert(this->_normalizedStart + partialCoords, (Void)0);
		}
	}

	return;
}

/**
Normalizes an AimsVector<T,D> using the resolution.
*/
template <class T,unsigned D>
AimsVector<short,D> BucketMapSampler<T,D>::normalize(AimsVector<T,D> & vector, AimsVector<T,D> & resolution)
{
	T value1, value2;
	AimsVector<short,D> normalized((short)0);

	// Processes the normalized values
	for (unsigned index=0; index < D; index++)
	{
		value1 = ((T)(vector[index]));
		value2 = ((T)(resolution[index]));
		normalized[index]=((short) ceil(value1 / value2));
	}

	return normalized;
}

/**
Denormalizes an AimsVector<short,D> using the resolution.
*/
template <class T,unsigned D>
AimsVector<T,D> BucketMapSampler<T,D>::denormalize(AimsVector<short,D> & vector, AimsVector<T,D> & resolution)
{
	AimsVector<T,D> denormalized((T)0);

	for (unsigned index=0; index < D; index++)
	{
		denormalized[index] = resolution[index] * vector[index];
	}

	return denormalized;
}

/** Declare the existing templated instanciations */
template class aimsalgo::BucketMapSampler<float,3>;
