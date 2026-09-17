#ifndef AIMS_DEF_SETTINGS_H
#define AIMS_DEF_SETTINGS_H

#include <cartobase/object/attributed.h>


namespace aims
{

  class Settings : public carto::AttributedObject
  {
  public:
    static Settings & settings();

    carto::AttributedObject & personalSettings() { return _perso; }

  protected:
    Settings();

    carto::AttributedObject	_perso;
  };

}

#endif

