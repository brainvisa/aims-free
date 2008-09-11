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
 *  ColorMap class
 */
#ifndef AIMS_COLOR_COLORMAP_H
#define AIMS_COLOR_COLORMAP_H

#include <aims/config/aimsdata_config.h>
#include <aims/rgb/rgb.h>
#include <iostream>
#include <string>
#include <vector>


namespace aims {


class AIMSDATA_API ColorMap
{
  public:

    ColorMap();
    ~ColorMap();

    size_t nItem() const;

    const AimsRGB& item( int n ) const;
    AimsRGB& item( int n );

    void reserve ( size_t size );
    void push_back( const AimsRGB& item );

    void erase();

    const std::string& name() const;
    void setName( const std::string& name );

    friend
    std::ostream& operator << ( std::ostream& out, const ColorMap& thing );

  protected:

    std::string _name;
    std::vector< AimsRGB > _data;

};


inline
ColorMap::ColorMap()
{
}


inline
ColorMap::~ColorMap()
{ 
    _data.clear();
}


inline
size_t ColorMap::nItem() const
{
    return _data.size();
}


inline
const AimsRGB& ColorMap::item( int n ) const
{
    return _data[ n ];
}


inline
AimsRGB& ColorMap::item( int n )
{
    return _data[ n ];
}

inline
void ColorMap::reserve( size_t size )
{
    _data.reserve( size );
}


inline
void ColorMap::push_back( const AimsRGB& item )
{
    _data.push_back( item );
}


inline
void ColorMap::erase()
{
    _data.clear();
}


inline
const std::string& ColorMap::name() const
{
    return _name;
}


inline
void ColorMap::setName( const std::string& name )
{
    _name = name;
}


}


#endif
