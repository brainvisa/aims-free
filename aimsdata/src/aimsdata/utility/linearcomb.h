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

/*
 *  Linear combination beetween two datas.
 */
#ifndef AIMS_UTILITY_LINEARCOMB_H
#define AIMS_UTILITY_LINEARCOMB_H

#include <aims/config/aimsdata_config.h>
#include <aims/data/data.h>


/** The template class to make linear combinations.
    This class is useful to make a linear combination 
    num1*data1/den1+num2*data2/den2 because it goes through
    a float casting.
*/
template <class T>
class AIMSDATA_API AimsLinearCombination
{ protected:
    /**@name Data*/
    //@{
    /// multiplicator for the first data
    float _num1;
    /// dividor for the first data
    float _den1;
    /// multiplicator for the second data
    float _num2;
    /// dividor for the second data
    float _den2;
    //@}

  public :
    /**@name Constructor and destructor*/
    //@{
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
    //@}

    /**@name Methods*/
    //@{
    /// Return the combination of data1 and data2
    AimsData<T> operator () (const AimsData<T> &data1,
                             const AimsData<T> &data2);
    /// Return the combination of data1
    AimsData<T> operator () (const AimsData<T> &data1);
    //@}
};


template <class T> inline
AimsData<T> AimsLinearCombination<T>::operator () (const AimsData<T> &data1,
                                                   const AimsData<T> &data2)
{ if (data1.dimX() != data2.dimX() ||
      data1.dimY() != data2.dimY() ||
      data1.dimZ() != data2.dimZ() ||
      data1.dimT() != data2.dimT()   )
    AimsError("AimsData<T> AimsLinearCombination(const AimsData<T> &,\
               float,float,const AimsData<T> &,float,float) : \
               sizes must be the same");

  AimsData<T> comb(data1.dimX(),data1.dimY(),data1.dimZ(),data1.dimT(),
                     data1.borderWidth());
  comb.setSizeXYZT(data1);
  typename AimsData<T>::iterator       it;
  typename AimsData<T>::const_iterator it1,it2;
  for (it=comb.begin(),it1=data1.begin(),it2=data2.begin();
       it<comb.end();it++,it1++,it2++)
    *it = (T)((double)*it1 * (double)_num1 / (double)_den1 +
              (double)*it2 * (double)_num2 / (double)_den2 );
  return(comb);
}


template <class T> inline
AimsData<T> AimsLinearCombination<T>::operator () (const AimsData<T> &data1)
{ AimsData<T> comb(data1.dimX(),data1.dimY(),data1.dimZ(),data1.dimT(),
                     data1.borderWidth());
  comb.setSizeXYZT(data1);
  typename AimsData<T>::iterator       it;
  typename AimsData<T>::const_iterator it1;
  for (it=comb.begin(),it1=data1.begin();it<comb.end();it++,it1++)
    *it = (T)((double)*it1 * (double)_num1 / (double)_den1);
  return(comb);
}


#endif
