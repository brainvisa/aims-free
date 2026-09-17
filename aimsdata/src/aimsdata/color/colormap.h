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
