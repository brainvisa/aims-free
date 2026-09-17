
#ifndef _AIMS_PIX2VOL_H_
#define _AIMS_PIX2VOL_H_

#include <aims/data/data.h>
#include "activPixel.h"

AimsData< short >
AimsActivPixel2Volume( AimsData< short >& vol, AimsActivPixel& pixels,
                       Point3df& trans, short label );

#endif
