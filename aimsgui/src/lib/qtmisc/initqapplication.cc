#include <aims/qtmisc/initqapplication.h>

namespace aims
{

  QCoreApplication* initializeQApplication( int argc, char** argv,
                                            bool use_gui )
  {
    if( use_gui )
    {
      // need a QApplication
      if( QCoreApplication::instance()
          && dynamic_cast<QApplication *>( QCoreApplication::instance() ) )
      {
        // it is already a QApplication: OK just use this one
        return QCoreApplication::instance();
      }
      else if( QCoreApplication::instance() )
      {
        // it is a QCoreApplication: need to delete it.
        delete QCoreApplication::instance();
      }
      // now instantiate a QApplication
      return new QApplication( argc, argv );
    }
    else
    {
      // need a QCoreApplication, and can use an existing QApplication
      if( QCoreApplication::instance() )
        return QCoreApplication::instance(); // OK use the existng one
      return new QCoreApplication( argc, argv );
    }
  }

}

