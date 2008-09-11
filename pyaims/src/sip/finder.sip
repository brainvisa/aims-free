namespace aims
{

class Finder
{
%TypeHeaderCode
#include <aims/data/pheader.h>
#include <aims/io/finder.h>
%End

public:
  Finder();
  virtual ~Finder();
  bool check( const std::string & ) /ReleaseGIL/;
  std::string objectType() const;
  std::string dataType() const;
  std::string format() const;
  carto::Object headerObject() const /PyName=header, ReleaseGIL/;
};

};
