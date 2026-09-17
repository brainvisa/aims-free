#ifndef AIMSGUI_QTMISC_INITQAPPLICATION_H
#define AIMSGUI_QTMISC_INITQAPPLICATION_H

#include <aims/config/aimsdata_config.h>
#include <QApplication>

namespace aims
{

  /** Instantiate, reuse, or replace the QApplication or QCoreApplication
      as requested, so that GUI can take place even after a QCoreApplication
      has been used. */
  QCoreApplication* initializeQApplication( int argc, char** argv,
                                            bool use_gui=true );

}


#endif
