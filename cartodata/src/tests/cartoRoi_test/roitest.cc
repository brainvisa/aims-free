/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info". 
 * 
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability. 
 * 
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or 
 * data to be ensured and,  more generally, to use and operate it in the 
 * same conditions as regards security. 
 * 
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#include <cartodata/volume/volumeview.h>
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
    ( new VolumeView<int16_t>( vol1, 
                               VolumeView<int16_t>::Position4Di( 2, 2, 2 ), 
                               VolumeView<int16_t>::Position4Di( 6, 6, 6, 1 ) 
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

