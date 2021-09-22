/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
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
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

/*
 *  Linear combination beetween two datas.
 */
#ifndef AIMS_UTILITY_LINEARCOMB_H
#define AIMS_UTILITY_LINEARCOMB_H

#include <aims/config/aimsdata_config.h>
#include <cartodata/volume/volume.h>


/** The template class to make linear combinations.
    This class is useful to make a linear combination 
    num1*data1/den1+num2*data2/den2 because it goes through
    a float casting.
*/
template <class T>
class AimsLinearCombination
{ protected:
    /// multiplicator for the first data
    float _num1;
    /// dividor for the first data
    float _den1;
    /// multiplicator for the second data
    float _num2;
    /// dividor for the second data
    float _den2;

  public :
    /** The programmer should provide at least 2 parameters to the constructor:
        @param num1 multiplicator for the first data
        @param den1 dividor for the first data
        @param num2 multiplicator for the second data [default=0.0]
        @param den2 dividor for the second data [default=1.0]
    */
    AimsLinearCombination(float num1,float den1,
                          float num2=0.0,float den2=1.0)
    { ASSERT(den1!=0 && den2!=0);
      _num1 = num1;
      _den1 = den1;
      _num2 = num2;
      _den2 = den2;
    }
    /// Destructor does nothing
    virtual ~AimsLinearCombination() { }

    /// Return the combination of data1 and data2
    carto::VolumeRef<T> operator () (
      const carto::rc_ptr<carto::Volume<T> > &data1,
      const carto::rc_ptr<carto::Volume<T> > &data2 );
    /// Return the combination of data1
    carto::VolumeRef<T> operator () (
      const carto::rc_ptr<carto::Volume<T> > &data1);
};


template <class T> inline
carto::VolumeRef<T> AimsLinearCombination<T>::operator () (
  const carto::rc_ptr<carto::Volume<T> > &data1,
  const carto::rc_ptr<carto::Volume<T> > &data2 )
{
  if( data1->getSize() != data2->getSize() )
    AimsError("VolumeRef<T> AimsLinearCombination(const rc_ptr<Volume<T> > &,\
               float,float,const rc_ptr<Volume<T> > &,float,float) : \
               sizes must be the same");

  carto::VolumeRef<T> comb( data1->getSize(), data1->getBorders() );
  comb.setVoxelSize( data1->getVoxelSize() );
  typename carto::Volume<T>::iterator       it;
  typename carto::Volume<T>::const_iterator it1, it2;
  for( it=comb->begin(), it1=data1->begin(), it2=data2->begin();
       it!=comb->end(); it++, it1++, it2++ )
    *it = (T)((double)*it1 * (double)_num1 / (double)_den1 +
              (double)*it2 * (double)_num2 / (double)_den2 );
  return comb;
}


template <class T> inline
carto::VolumeRef<T> AimsLinearCombination<T>::operator () (
  const carto::rc_ptr<carto::Volume<T> > &data1 )
{
  carto::VolumeRef<T> comb( data1->getSize(), data1->getBorders() );
  comb.setVoxelSize( data1->getVoxelSize() );
  typename carto::Volume<T>::iterator       it;
  typename carto::Volume<T>::const_iterator it1;
  for( it=comb->begin(), it1=data1->begin(); it!=comb->end(); it++, it1++ )
       *it = (T)((double)*it1 * (double)_num1 / (double)_den1);
  return comb;
}


#endif
