#include <cstdlib>
#include <cartodata/volume/volume.h>
#include <cartodata/roi/siteIterator.h>
#include <iostream>
#include <vector>

using namespace carto;
using namespace std;

int main( int /*argc*/, char** /*argv*/ )
{
  int	result = EXIT_SUCCESS;

  cout << "-- Test 1: regular volume: vol1 ( 10, 10, 10 ) --" << endl;
  VolumeRef<int16_t>	vol1( new Volume<int16_t>( 10, 10, 10 ) );
  vol1->fill(0);

  cout << "-- Test 2: volume view: vol2 ( 6, 6, 6 ) in vol1 --" << endl;
  VolumeRef<int16_t>	vol2
    ( new Volume<int16_t>( vol1,
                           Volume<int16_t>::Position4Di( 2, 2, 2 ),
                           Volume<int16_t>::Position4Di( 6, 6, 6, 1 )
                          ) );
  vol2->fill( 1 );
 
  SiteIteratorOf< Volume<int16_t> >  it(vol1);

  cout << "-- read Sites1 --"<<endl;
  cout << "de 2 a 7"<<endl;
  while( it.isValid())
    {
      it.writeSite(); 
      it.next();
    }

  (*vol1)(3, 3, 3) = 2;
  (*vol1)(1,1,1) = 3;
  cout <<endl<<endl;
  
  cout << "-- read Sites2 --"<<endl;
  cout << "de 2 a 7 et 1 1 1"<<endl;
  cout << (*vol1)(3,3,3)<<" "<<(*vol1)(1,1,1)<<endl;

  for (it.restart(); it.isValid(); it.next()) {
    it.writeSite(); 
  }
  cout <<endl<<endl;
  
  cout << "-- read Sites3 --"<<endl;
  cout << "3 3 3 et 1 1 1"<<endl;
  SiteIteratorOf< Volume<int16_t> > it2(vol1, 3); 
  
  for (it2.restart(); it2.isValid(); it2.next())
    {
      it2.writeSite(); 
    }

  return result;
}

