/*
 *  Merging operators
 */
#ifndef AIMS_UTILITY_MERGE_H
#define AIMS_UTILITY_MERGE_H

#include <aims/config/aimsdata_config.h>
#include <cartodata/volume/volume.h>

enum merge_t
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
class AimsMerge
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
    inline carto::VolumeRef<T> operator () (
      const carto::rc_ptr<carto::Volume<T> > & data,
      const carto::rc_ptr<carto::Volume<U> > & mask);
    //@}

  protected :
    /// Merging type
    merge_t _type;
    /// Merging value
    T    _value;
    /// Label to consider
    U    _label;
};


template <class T,class U>
AimsMerge<T,U>::AimsMerge(merge_t type,T value,U label)
{
  _type   = type;
  _value  = value;
  _label  = label;

}


template <class T,class U> inline
carto::VolumeRef<T> AimsMerge<T,U>::operator () (
  const carto::rc_ptr<carto::Volume<T> > & data,
  const carto::rc_ptr<carto::Volume<U> > & mask )
{
  ASSERT(data->getSizeX() == mask->getSizeX() &&
         data->getSizeY() == mask->getSizeY() &&
         data->getSizeZ() == mask->getSizeZ() &&
         data->getSizeT() == mask->getSizeT()  );

  carto::VolumeRef<T> res = carto::VolumeRef<T>( data ).deepcopy();

  typename carto::Volume<T>::iterator       it1;
  typename carto::Volume<U>::const_iterator it2;

  switch (_type)
  { case AIMS_MERGE_SAME_VALUES : 
      for (it1=res.begin(),it2=mask->begin();it1!=res.end();it1++,it2++)
        if (*it2) *it1 = (T)*it2;
      break;
    case AIMS_MERGE_ONE_TO_ONE : 
      for (it1=res.begin(),it2=mask->begin();it1!=res.end();it1++,it2++)
        if (*it2==_label) *it1 = (T)_value;
      break;
    case AIMS_MERGE_ALL_TO_ONE : 
      for (it1=res.begin(),it2=mask->begin();it1!=res.end();it1++,it2++)
        if (*it2) *it1 = (T)_value;
      break;
    case AIMS_MERGE_ONE_TO_MAXP1 : 
      _value = res.max() + 1;
      for (it1=res.begin(),it2=mask->begin();it1!=res.end();it1++,it2++)
        if (*it2==_label) *it1 = (T)_value;
      break;
    case AIMS_MERGE_ALL_TO_MAXP1 : 
      _value = res.max() + 1;
      for (it1=res.begin(),it2=mask->begin();it1!=res.end();it1++,it2++)
        if (*it2) *it1 = (T)_value;
      break;
  }
  return(res);
}


#endif
