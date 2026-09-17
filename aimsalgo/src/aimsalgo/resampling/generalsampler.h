
#ifndef AIMS_RESAMPLING_GENERALSAMPLER_H
#define AIMS_RESAMPLING_GENERALSAMPLER_H

#include <cartobase/smart/rcptr.h>
#include <cartobase/object/object.h>
#include <aims/vector/vector.h>
#include <aims/resampling/samplable.h>

namespace aimsalgo
{
	template <class T,unsigned D>
	class GeneralSampler
	{
	public:
			
		GeneralSampler( ) { }
		virtual ~GeneralSampler() { }

		/** @name Methods */
		//@{
		/** Sample a Samplable object. \\
			@param object Samplable object
			@param start AimsVector<T,D> containing coordinates of the start sampling point
			@param sizes AimsVector<T,D> containing the sizes of the sampling box
			@param resolution AimsVector<T,D> containing the resolution of the sampling box
			@return carto::RCObject * containing the sampled data
		*/
		virtual carto::Object sample( Samplable<T,D> & object, AimsVector<T,D> & start,
		AimsVector<T,D> & sizes, AimsVector<T,D> & resolution ) = 0;
		//@}

	};

}

#endif
