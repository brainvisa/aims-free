#include <cartodata/wip/roi/siteIterator.h>
#include <vector>
#include <iostream>
#include <string>

using namespace std;

namespace carto
{
  
Site::~Site() {};

Site& Site::operator = ( const Site &other )
{
  if ( this != &other ) {
    for( int i = 0; i < size(); ++i ) {
      set( i, other.at( i ) );
    }
  }
  return *this;
}
  
  
bool Site::operator == (const Site &s1)
{
  int i = 0;

  if ( this != &s1 ) {
    if ( size() == s1.size() ) {
      while ( i < size() ) {
	if ( s1.at(i) == at(i) ) i++;
	else return false; // different element
      }
    }else {
      return false;    // different size
    }
  }
  
  return true;
}

bool Site::operator != (const Site &s1)
{
  int i = 0;

  if ( this != &s1 ) {
    if ( size() == s1.size() ) {
      while ( i < size() ) {
	if ( s1.at(i) == at(i) ) i++;
	else return true; // different element
      }
    }else {
      return true;    // different size
    }
  }  
  return false;
}

void
Site::write()
{
  for(int i=0; i<size(); i++)
    cout << at(i) << " " ;
}


SiteIterator::~SiteIterator() {}


} //namespace carto
