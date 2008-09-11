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
 *  RGB class
 */
#ifndef AIMS_RGB_RGB_H
#define AIMS_RGB_RGB_H


#include <aims/config/aimsdata_config.h>
#include <aims/def/general.h>
#include <aims/def/assert.h>
#include <cartobase/type/types.h>

class AimsRGB;
class AimsRGBA;


std::ostream& operator << (std::ostream &out, const AimsRGB &aa);
std::ostream& operator << (std::ostream &out, const AimsRGBA &aa);
std::istream& operator >> (std::istream &in , AimsRGB &aa);
std::istream& operator >> (std::istream &in , AimsRGBA &aa);
AimsRGB operator + (const AimsRGB &aa, const AimsRGB &bb);
AimsRGB operator - (const AimsRGB &aa, const AimsRGB &bb);
AimsRGB operator * (const AimsRGB &aa, const byte &bb);
AimsRGB operator * (const byte &aa, const AimsRGB &bb);
AimsRGB operator / (const AimsRGB &aa, const byte &bb);

bool  operator == (const AimsRGB &aa, const AimsRGB &bb);
bool  operator != (const AimsRGB &aa, const AimsRGB &bb);
bool  operator == (const AimsRGB &aa, const byte &bb);

/** The basic class for RGB management
*/
class AimsRGB
{
protected:
  byte _red;
  byte _green;
  byte _blue;

public:
  /// Copy constructor
  AimsRGB(const AimsRGB &other) : _red  (other.red())     , 
				  _green(other.green()) ,
				  _blue (other.blue()) { }
  AimsRGB(const AimsRGBA &other);
  /** The programmer should provide the intensity of each color:
      @param r red intensity [0-255]   [default=0]
      @param g green intensity [0-255] [default=0]
      @param b blue intensity [0-255]  [default=0]
  */
  AimsRGB(const byte &r = 0,
	  const byte &g = 0,
	  const byte &b = 0) : _red(r) , _green(g) , _blue(b) { }
  /// Destructor does nothing
  ~AimsRGB() { }

  /**@name Manipulators and methods*/
  //@{
  AimsRGB& operator  = (const AimsRGB &other)
  { _red   = other.red();
  _green = other.green();
  _blue  = other.blue();
  return(*this);
  }
  AimsRGB& operator  = (const AimsRGBA &other);
  AimsRGB& operator += (const AimsRGB &other)
  { _red   += other.red();
  _green += other.green();
  _blue  += other.blue();
  return(*this);
  }
  AimsRGB& operator += (const AimsRGBA &other);
  AimsRGB& operator  = (const byte &val)
  { _red   = val;
  _green = val;
  _blue  = val;
  return(*this);
  }
  AimsRGB& operator /= (const byte &val)
  {
    ASSERT(val != 0);
    _red   /= val;
    _green /= val;
    _blue  /= val;
    return(*this); 
  }
  AimsRGB& operator *= (const byte &val)
  { _red   *= val; 
  _green *= val; 
  _blue  *= val;
  return(*this); 
  }
  //@}

  /**@name Accessors*/
  //@{
  /// Get reference to red component
  const byte& red  () const { return _red; }
  /// Get reference to green component
  const byte& green() const { return _green; }
  /// Get reference to blue component
  const byte& blue () const { return _blue; }

  /// Get const reference to red component
  byte& red  () { return _red; }
  /// Get const reference to green component
  byte& green() { return _green; }
  /// Get const reference to blue component
  byte& blue () { return _blue; }

  /// Fake vector-like operators
  const byte & operator [] ( unsigned x ) const;
  /// Fake vector-like operators
  byte & operator [] ( unsigned x );
  //@}

  /**@name Friend functions*/
  //@{
  friend AimsRGB operator + (const AimsRGB &aa,
			     const AimsRGB &bb);
  friend AimsRGB operator - (const AimsRGB &aa,
			     const AimsRGB &bb);
  friend AimsRGB operator * (const AimsRGB &aa,
			     const byte &bb);
  friend AimsRGB operator * (const byte &aa,
			     const AimsRGB &bb);
  friend AimsRGB operator * (const AimsRGB &aa,
			     const double &bb);
  friend AimsRGB operator * (const double &aa,
			     const AimsRGB &bb);
  friend AimsRGB operator / (const AimsRGB &aa,
			     const byte &bb);
  friend AimsRGB operator + (const AimsRGB &aa);

  friend bool  operator == (const AimsRGB &aa,
			    const AimsRGB &bb);
  friend bool  operator != (const AimsRGB &aa,
			    const AimsRGB &bb);
  friend bool  operator == (const AimsRGB &aa,
			    const byte &bb);
  //@}
};


class AimsRGBA
{
public:
  AimsRGBA( const AimsRGBA & other )
  { *(unsigned *) this = *(unsigned *) &other; } // fast'n'ugly copy...
  AimsRGBA( const AimsRGB & other )
    : _red ( other.red() ), _green( other.green() ), _blue( other.blue() ), 
      _alpha( 255 ) { }
  AimsRGBA( const byte r = 0, const byte g = 0, const byte b = 0, 
	    const byte a = 0 ) : _red(r) , _green(g) , _blue(b), _alpha(a) { }
  ~AimsRGBA() { }

  AimsRGBA & operator = ( const AimsRGBA & other )
  {
    *(unsigned *) this = *(unsigned *) &other;
    return( *this );
  }

  AimsRGBA & operator = ( const AimsRGB & other )
  {
    _red   = other.red();
    _green = other.green();
    _blue  = other.blue();
    _alpha = 255; return(*this);
  }

  AimsRGBA & operator += ( const AimsRGBA & other )
  {
    if( other._alpha == 0 )
      if( _alpha == 0 )
        {
          _red = 0;
          _green = 0;
          _blue = 0;
        }
      else
        {
          // do nothing: keep this
        }
    else if( _alpha == 0 )
      {
        _red   = other._red;
        _green = other._green;
        _blue  = other._blue;
        _alpha = other._alpha;
      }
    else
      {
        _red   += other.red();
        _green += other.green();
        _blue  += other.blue();
        // alpha ?
        if( _alpha < other._alpha )
          _alpha = other._alpha;
      }
    return(*this);
  }

  AimsRGBA & operator += (const AimsRGB & other)
  {
    if( _alpha == 0 )
      {
        _red   = other.red();
        _green = other.green();
        _blue  = other.blue();
      }
    else
      {
        _red   += other.red();
        _green += other.green();
        _blue  += other.blue();
      }
    _alpha = 255;
    return(*this);
  }

  AimsRGBA & operator = ( const byte & val )
  {
    _red   = val;
    _green = val;
    _blue  = val;
    _alpha = 255;	// can be discussed...
    return(*this);
  }

  AimsRGBA & operator /= ( const byte &val )
  {
    ASSERT(val != 0);
    _red   /= val;
    _green /= val;
    _blue  /= val;
    return(*this); 
  }

  AimsRGBA & operator *= (const byte &val)
  {
    _red   *= val; 
    _green *= val; 
    _blue  *= val;
    return(*this); 
  }

  /**@name Accessors*/
  //@{
  /// returns reference: not optimal...
  const byte& red  () const { return _red; }
  const byte& green() const { return _green; }
  const byte& blue () const { return _blue; }
  const byte& alpha() const { return _alpha; }

  byte& red  () { return _red; }
  byte& green() { return _green; }
  byte& blue () { return _blue; }
  byte& alpha() { return _alpha; }

  /// Fake vector-like operators
  const byte & operator [] ( unsigned x ) const;
  /// Fake vector-like operators
  byte & operator [] ( unsigned x );
  //@}

  friend AimsRGB operator * (const AimsRGB &aa,
			     const double &bb);
  friend AimsRGB operator * (const double &aa,
			     const AimsRGB &bb);

protected:
  byte	_red;
  byte	_green;
  byte	_blue;
  byte	_alpha;
};


#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

namespace carto
{

  template<> inline std::string DataTypeCode<AimsRGB>::dataType()
  {
    return "RGB";
  }

  template<> inline std::string DataTypeCode<AimsRGBA>::dataType()
  {
    return "RGBA";
  }

}

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES


inline
std::ostream& operator << (std::ostream &out,const AimsRGB &aa)
{ return( out << '(' << (int)aa.red()   << ',' 
                     << (int)aa.green() << ',' 
                     << (int)aa.blue()  << ')');
}


inline
std::ostream& operator << ( std::ostream &out,const AimsRGBA &aa )
{
  return( out << '(' << (int)aa.red()   << ',' 
	  << (int)aa.green() << ',' 
	  << (int)aa.blue()  << ',' 
	  << (int)aa.alpha() << ')');
}


inline
std::istream& operator >> (std::istream& is, AimsRGB& thing)
{
  int r=0,g=0,b=0;
  char ch = 0;

  //std::istream::sentry s( is ); // to use in the future...
  while( is && ( is.peek() == ' ' || is.peek() == '\t' || is.peek() == '\n' ) )
    is >> ch;
  if (is.peek () == '(')
    is >> ch;
  is >> r;
  if (ch == '(')
    { is >> ch;
    if (ch == ',')
      is >> g >> ch;
    if (ch == ',')
      is >> b >> ch;
    }

  if (ch != 0 && ch != ')')
    is.setstate (std::ios::failbit);
  else if (is.good())
    thing = AimsRGB((byte)r,(byte)g,(byte)b);

  return is;
}


inline
std::istream& operator >> ( std::istream& is, AimsRGBA & thing )
{
  int r = 0, g = 0, b = 0, a = 0;
  char ch = 0;

  while( is && ( is.peek() == ' ' || is.peek() == '\t' || is.peek() == '\n' ) )
    is >> ch;
  if (is.peek () == '(')
    is >> ch;
  is >> r;
  is >> ch;
  if (ch == ',')
    is >> g >> ch;
  if (ch == ',')
    is >> b >> ch;
  if (ch == ',')
    is >> a >> ch;

  if (ch != 0 && ch != ')')
    is.setstate (std::ios::failbit);
  else if (is.good())
    thing = AimsRGBA( (byte) r, (byte) g, (byte) b, (byte) a );

  return is;
}


inline
AimsRGB operator + (const AimsRGB &aa,
                    const AimsRGB &bb)
{ return(AimsRGB(aa.red()   + bb.red(),
                 aa.green() + bb.green(),
                 aa.blue()  + bb.blue()));
}


inline
AimsRGBA operator + (const AimsRGBA &aa,
                     const AimsRGBA &bb)
{
  if( bb.alpha() == 0 )
    if( aa.alpha() == 0 )
      return AimsRGBA( 0, 0, 0, 0 );
    else
      return aa;
  else if( aa.alpha() == 0 )
    return bb;
  else
    return AimsRGBA( aa.red()   + bb.red(),
                     aa.green() + bb.green(),
                     aa.blue()  + bb.blue(), 
                     aa.alpha() >= bb.alpha() ? aa.alpha() : bb.alpha() );
}


inline
AimsRGB operator - (const AimsRGB &aa,
                    const AimsRGB &bb)
{ return(AimsRGB(aa.red()   - bb.red(),
                 aa.green() - bb.green(),
                 aa.blue()  - bb.blue()));
}


inline
AimsRGBA operator - (const AimsRGBA &aa,
                     const AimsRGBA &bb)
{
  return AimsRGBA( aa.red()   - bb.red(),
                   aa.green() - bb.green(),
                   aa.blue()  - bb.blue(), 
                   aa.alpha() >= bb.alpha() ? aa.alpha() : bb.alpha() );
}


inline
AimsRGB operator * (const AimsRGB  &aa,
                    const byte &bb)
{ return(AimsRGB(aa.red()   * bb,
                 aa.green() * bb,
                 aa.blue()  * bb));
}

inline
AimsRGB operator * (const AimsRGB  &aa,
                    const double &bb)
{ return(AimsRGB((byte) (aa.red()   * bb),
                 (byte) (aa.green() * bb),
                 (byte) (aa.blue()  * bb)));
}



inline
AimsRGBA operator * (const AimsRGBA  &aa,
		     const double &bb)
{ return(AimsRGBA((byte) (aa.red()   * bb),
                 (byte) (aa.green() * bb),
                 (byte) (aa.blue()  * bb), 
                  aa.alpha()));
}



inline
AimsRGB operator * (const byte &aa,
                    const AimsRGB  &bb)
{ return(AimsRGB(bb.red()   * aa,
                 bb.green() * aa,
                 bb.blue()  * aa));
}


inline
AimsRGB operator * (const double &aa,
                    const AimsRGB  &bb)
{ return(AimsRGB((byte) (bb.red()   * aa),
                 (byte) (bb.green() * aa),
                 (byte) (bb.blue()  * aa)));
}


inline
AimsRGBA operator * (const double &aa,
		     const AimsRGBA  &bb)
{ return(AimsRGBA((byte) (bb.red()   * aa),
		  (byte) (bb.green() * aa),
		  (byte) (bb.blue()  * aa), 
                  bb.alpha()));
}


inline
AimsRGB operator / (const AimsRGB  &aa,
                    const byte &bb)
{ ASSERT(bb != 0);
  return(AimsRGB(aa.red()   / bb,
                 aa.green() / bb,
                 aa.blue()  / bb));
}



inline
bool operator == (const AimsRGB &aa,
                  const AimsRGB &bb)
{ return((aa.red()   == bb.red())   && 
         (aa.green() == bb.green()) &&
         (aa.blue()  == bb.blue())   );
}


inline
bool operator != (const AimsRGB &aa,
                  const AimsRGB &bb)
{ return((aa.red()   != bb.red())   ||
         (aa.green() != bb.green()) ||
         (aa.blue()  != bb.blue())   );
}


inline
bool operator == (const AimsRGB  &aa,
                  const byte &bb)
{ return((aa.red()   == bb)  && 
         (aa.green() == bb)  &&
         (aa.blue()  == bb)   );
}


inline
bool operator == (const AimsRGBA &aa,
                  const AimsRGBA &bb)
{
  return (aa.red() == bb.red()) 
    && (aa.green() == bb.green()) 
    && (aa.blue()  == bb.blue()) 
    && (aa.alpha() == bb.alpha());
}


inline
bool operator != (const AimsRGBA &aa,
                  const AimsRGBA &bb)
{
  return (aa.red() != bb.red()) 
    || (aa.green() != bb.green()) 
    || (aa.blue()  != bb.blue()) 
    || (aa.alpha() != bb.alpha());
}


inline
bool operator == (const AimsRGBA  &aa,
                  const byte &bb)
{
  return (aa.red() == bb) 
    && (aa.green() == bb) 
    && (aa.blue()  == bb);
}


inline AimsRGB::AimsRGB( const AimsRGBA & other )
  : _red( other.red() ), _green( other.green() ), _blue( other.blue() )
{
}


inline AimsRGB& AimsRGB::operator = ( const AimsRGBA & other )
{
  _red   = other.red();
  _green = other.green();
  _blue  = other.blue();
  return(*this);
}

inline AimsRGB& AimsRGB::operator += ( const AimsRGBA & other )
{
  _red   += other.red();
  _green += other.green();
  _blue  += other.blue();
  return(*this);
}


inline const byte & AimsRGB::operator [] ( unsigned x ) const
{
  switch( x )
    {
    case 0:
      return _red;
    case 1:
      return _green;
    case 2:
      return _blue;
    default:
      return _red;
    }
}


inline byte & AimsRGB::operator [] ( unsigned x )
{
  switch( x )
    {
    case 0:
      return _red;
    case 1:
      return _green;
    case 2:
      return _blue;
    default:
      return _red;
    }
}


inline const byte & AimsRGBA::operator [] ( unsigned x ) const
{
  switch( x )
    {
    case 0:
      return _red;
    case 1:
      return _green;
    case 2:
      return _blue;
    case 3:
      return _alpha;
    default:
      return _red;
    }
}


inline byte & AimsRGBA::operator [] ( unsigned x )
{
  switch( x )
    {
    case 0:
      return _red;
    case 1:
      return _green;
    case 2:
      return _blue;
    case 3:
      return _alpha;
    default:
      return _red;
    }
}

#endif


