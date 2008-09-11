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
 *  Merging operators
 */
#ifndef AIMS_UTILITY_MERGE_H
#define AIMS_UTILITY_MERGE_H

#include <aims/config/aimsdata_config.h>
#include <aims/data/data.h>

AIMSDATA_API enum merge_t
{
  AIMS_MERGE_SAME_VALUES,
  AIMS_MERGE_ONE_TO_ONE,
  AIMS_MERGE_ALL_TO_ONE,
  AIMS_MERGE_ONE_TO_MAXP1,
  AIMS_MERGE_ALL_TO_MAXP1
};


/** The template class to merge data and byte label data.
    There are 5 different types of merging :\\
    \\- AIMS_MERGE_SAME_VALUES
    \\- AIMS_MERGE_ONE_TO_ONE
    \\- AIMS_MERGE_ALL_TO_ONE
    \\- AIMS_MERGE_ONE_TO_MAXP1
    \\- AIMS_MERGE_ALL_TO_MAXP1
*/ 
template <class T,class U>
class AIMSDATA_API AimsMerge
{
  public :
    /**@name Constructor and destructor*/
    //@{
    /** The programmer must provide a type of merging.
        @param type type of merging (AIMS_MERGE_SAME_VALUES,...)
        @param value value that will replace points verifying the label
        @param label label to merge
    */
    AimsMerge(merge_t type,T value=0,U label=0);
    /// Destructor does nothing
    virtual ~AimsMerge() {}
    //@}

    /**@name Methods*/
    //@{
    /// Return the result of the merge of a data and a byte label data
    inline AimsData<T> operator () (const AimsData<T> &data,
                                    const AimsData<U> &mask);
    //@}

  protected :
    /**@name Data*/
    //@{
    /// Merging type
    merge_t _type;
    /// Merging value
    T    _value;
    /// Label to consider
    U    _label;
    //@}
};


template <class T,class U>
AimsMerge<T,U>::AimsMerge(merge_t type,T value,U label)
{
  _type   = type;
  _value  = value;
  _label  = label;

}


template <class T,class U> inline
AimsData<T> AimsMerge<T,U>::operator () (const AimsData<T> &data,
                                         const AimsData<U> &mask)
{ ASSERT(data.dimX() == mask.dimX() &&
         data.dimY() == mask.dimY() &&
         data.dimZ() == mask.dimZ() &&
         data.dimT() == mask.dimT()  );

  AimsData<T> res = data.clone();

  typename AimsData<T>::iterator       it1;
  typename AimsData<U>::const_iterator it2;

  switch (_type)
  { case AIMS_MERGE_SAME_VALUES : 
      for (it1=res.begin(),it2=mask.begin();it1<res.end();it1++,it2++)
        if (*it2) *it1 = (T)*it2;
      break;
    case AIMS_MERGE_ONE_TO_ONE : 
      for (it1=res.begin(),it2=mask.begin();it1<res.end();it1++,it2++)
        if (*it2==_label) *it1 = (T)_value;
      break;
    case AIMS_MERGE_ALL_TO_ONE : 
      for (it1=res.begin(),it2=mask.begin();it1<res.end();it1++,it2++)
        if (*it2) *it1 = (T)_value;
      break;
    case AIMS_MERGE_ONE_TO_MAXP1 : 
      _value = res.maximum() + 1;
      for (it1=res.begin(),it2=mask.begin();it1<res.end();it1++,it2++)
        if (*it2==_label) *it1 = (T)_value;
      break;
    case AIMS_MERGE_ALL_TO_MAXP1 : 
      _value = res.maximum() + 1;
      for (it1=res.begin(),it2=mask.begin();it1<res.end();it1++,it2++)
        if (*it2) *it1 = (T)_value;
      break;
  }
  return(res);
}


#endif
