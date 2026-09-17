
#ifndef AIMS_RESAMPLING_SAMPLABLE_H
#define AIMS_RESAMPLING_SAMPLABLE_H

#include <cartobase/object/object.h>
#include <aims/vector/vector.h>

/** The virtual template class fo which samplable classes must inherit from.
    The first template argument provides the type of vector stored items and
    the second template argument provides the size of vector.
*/
namespace aimsalgo
{
	template <class T,unsigned D>
	class Samplable
	{
		public:
			//---------------------------------------------------------------------
			/**	@name Constructors, Destructor*/
			//---------------------------------------------------------------------
			//@{
			/**	The programmer should not call the constructor of an
				abstract base class.
			*/
			Samplable(){}
			virtual ~Samplable(){}
			//@}
			
			virtual bool contains(AimsVector<T,D> & vector) = 0;
	};
}

#endif
