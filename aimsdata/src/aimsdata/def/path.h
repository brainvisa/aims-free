/*
 *  Paths required for A.I.M.S.
 */
#ifndef AIMS_DEF_PATH_H
#define AIMS_DEF_PATH_H

#include <aims/config/aimsdata_config.h>
#include <string>


namespace aims
{

  class AIMSDATA_API Path
  {
  public:

    Path();
    ~Path() { }

    const std::string& aims() const { return _aims; }
    const std::string& dependencies() const { return _dependencies; }
    const std::string& memmap() const;
    const std::string& nomenclature() const { return _nomenclature; }
    const std::string& syntax() const { return _syntax; }
    const std::string& hierarchy() const { return _hierarchy; }
    const std::string& shfjShared() const;
    const std::string& globalShared() const;
    const std::string& home() const;

    static const Path& singleton();

  protected:

    std::string _aims;
    std::string _dependencies;
    std::string	_nomenclature;
    std::string	_syntax;
    std::string	_hierarchy;
  };

}

#endif

// mode:C++
