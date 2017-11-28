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


#ifndef AIMS_RESAMPLING_BUCKETMAPSAMPLER_H
#define AIMS_RESAMPLING_BUCKETMAPSAMPLER_H

#include <vector>
#include <cartobase/object/object.h>
#include <aims/vector/vector.h>
#include <aims/bucket/bucketMap.h>
#include <aims/resampling/samplable.h>
#include <aims/resampling/generalsampler.h>

namespace aimsalgo
{
	template <class T,unsigned D>
	class BucketMapSampler : public GeneralSampler<T,D>
	{
	public:
		BucketMapSampler( ) { }
		virtual ~BucketMapSampler() { }

		/** @name Methods */
		//@{
		/** Sample a Samplable object. \\
			@param object Samplable object
			@param start AimsVector<T,D> containing coordinates of the start sampling point
			@param sizes AimsVector<T,D> containing the sizes of the sampling box
			@param resolution AimsVector<T,D> containing the resolution of the sampling box
			@return carto::Object * containing the sampled data
		*/
		virtual carto::Object sample( Samplable<T,D> & object, AimsVector<T,D> & start, AimsVector<T,D> & sizes, AimsVector<T,D> & resolution );
		//@}

		//@{
		/** Sample a Samplable object. \\
			@param object Samplable object
			@param dimensionIndex unsigned containing the current dimension index value contained in [0,D]
			@param partialCoords AimsVector<short,D> * containing the current partial coordinates used to generate points
			@param result BucketMap<Void> * containing the current partial coordinates used to generate points
		*/
		virtual void sample(Samplable<T,D> * data, unsigned dimensionIndex, AimsVector<short,D> & partialCoords, aims::BucketMap<Void> & result);
		//@}

		//@{
		/** Normalize an AimsVector<T,D> by dividing with the resolution. \\
			@param vector AimsVector<T,D> to normalize
			@param resolution AimsVector<T,D> containing the resolution to use
			@return AimsVector<short,D> normalized for the resolution
		*/
		virtual AimsVector<short,D> normalize(AimsVector<T,D> & vector, AimsVector<T,D> & resolution);
		//@}

		//@{
		/** Denormalize an AimsVector<short,D> by multiplying with the resolution. \\
			@param vector AimsVector<short,D> to denormalize
			@param resolution AimsVector<T,D> containing the resolution to use
			@return AimsVector<T,D> denormalized using the resolution
		*/
		virtual AimsVector<T,D> denormalize(AimsVector<short,D> & vector, AimsVector<T,D> & resolution);
		//@}
	private:
		AimsVector<T,D> _start; 
		AimsVector<short,D> _normalizedStart; 
		AimsVector<T,D> _sizes;
		AimsVector<short,D> _normalizedSizes;
		AimsVector<T,D> _resolution;
	};

}

#endif
