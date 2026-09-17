
#ifndef AIMS_SIGNALFILTER_SIGNALFILTER_G_H
#define AIMS_SIGNALFILTER_SIGNALFILTER_G_H

/**@name	signalfilter package
         The signalfilter package provides a set of signal filters (FIR, IIR)
*/
//@{ 
    //@Include: signalfilter/firfilter.h
    //@Include: signalfilter/parity.h
    //@Include: signalfilter/convol.h
    /**@name Gaussian filtering*/
    //@{
          //@Include: signalfilter/gcoef.h
          //@Include: signalfilter/grecurse.h
          //@Include: signalfilter/glines.h
          //@Include: signalfilter/gcolumns.h
          //@Include: signalfilter/gslices.h
          //@Include: signalfilter/g2dsmooth.h
          //@Include: signalfilter/g3dsmooth.h
          //@Include: signalfilter/gderive.h
          //@Include: signalfilter/ghessian.h
    //@}
//@}

# include <aims/signalfilter/firfilter.h>
# include <aims/signalfilter/parity.h>
# include <aims/signalfilter/convol.h>
# include <aims/signalfilter/gcoef.h>
# include <aims/signalfilter/grecurse.h>
# include <aims/signalfilter/glines.h>
# include <aims/signalfilter/gcolumns.h>
# include <aims/signalfilter/gslices.h>
# include <aims/signalfilter/g2dsmooth.h>
# include <aims/signalfilter/g3dsmooth.h>
# include <aims/signalfilter/gjacobian.h>
# include <aims/signalfilter/ghessian.h>
# include <aims/signalfilter/gframes.h>
# include <aims/signalfilter/vfilter.h>

#endif
