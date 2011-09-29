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
 *  RGB class
 */
#ifndef AIMS_HSV_HSV_H
#define AIMS_HSV_HSV_H


#include <aims/config/aimsdata_config.h>
#include <aims/def/general.h>
#include <aims/def/assert.h>
#include <cartobase/type/types.h>

class AimsHSV;


std::ostream& operator << (std::ostream &out, const AimsHSV &aa);
std::istream& operator >> (std::istream &in , AimsHSV &aa);
AimsHSV operator + (const AimsHSV &aa, const AimsHSV &bb);
AimsHSV operator - (const AimsHSV &aa, const AimsHSV &bb);
AimsHSV operator * (const AimsHSV &aa, const double &bb);
AimsHSV operator * (const AimsHSV &aa, const byte &bb);
AimsHSV operator * (const AimsHSV &aa, const long &bb);
AimsHSV operator * (const double &aa, const AimsHSV &bb);
AimsHSV operator * (const byte &aa, const AimsHSV &bb);
AimsHSV operator * (const long &aa, const AimsHSV &bb);
AimsHSV operator / (const AimsHSV &aa, const byte &bb);
AimsHSV operator / (const AimsHSV &aa, const long &bb);
AimsHSV operator / (const AimsHSV &aa, const double &bb);

bool  operator == (const AimsHSV &aa, const AimsHSV &bb);
bool  operator != (const AimsHSV &aa, const AimsHSV &bb);
bool  operator == (const AimsHSV &aa, const byte &bb);

/** The basic class for HSV management
*/
class AimsHSV
{
protected:
  byte _hue;
  byte _saturation;
  byte _value;

public:
  /// Copy constructor
  AimsHSV(const AimsHSV &other) : _hue(other.hue())     , 
				  _saturation(other.saturation()) ,
				  _value (other.value()) { }
  /** The programmer should provide the intensity of each color:
      @param h hue [0-65535]   [default=0]
      @param s saturation intensity [0-65535] [default=0]
      @param v value intensity [0-65535]  [default=0]
  */
  AimsHSV(const byte &h = 0,
	  const byte &s = 0,
	  const byte &v = 0) : _hue(h) , _saturation(s) , _value(v) { }
  /// Destructor does nothing
  ~AimsHSV() { }

  /**@name Manipulators and methods*/
  //@{
  AimsHSV& operator  = (const AimsHSV &other)
  { _hue      = other.hue();
  _saturation = other.saturation();
  _value      = other.value();
  return(*this);
  }
  AimsHSV& operator += (const AimsHSV &other)
  { _hue      += other.hue();
  _saturation += other.saturation();
  _value      += other.value();
  return(*this);
  }
  AimsHSV& operator  = (const byte &val)
  { _hue      = val;
  _saturation = val;
  _value      = val;
  return(*this);
  }
  AimsHSV& operator /= (const byte &val)
  {
    ASSERT(val != 0);
    _hue        /= val;
    _saturation /= val;
    _value      /= val;
    return(*this); 
  }
  AimsHSV& operator /= (const long &val)
  {
    ASSERT(val != 0);
    _hue        /= val;
    _saturation /= val;
    _value      /= val;
    return(*this);
  }
  AimsHSV& operator /= (const float &val)
  {
    ASSERT(val != 0);
    _hue        /= val;
    _saturation /= val;
    _value      /= val;
    return(*this);
  }
  AimsHSV& operator *= (const byte &val)
  { _hue        *= val;
    _saturation *= val;
    _value      *= val;
  return(*this); 
  }
  AimsHSV& operator *= (const long &val)
  { _hue        *= val;
    _saturation *= val;
    _value      *= val;
  return(*this);
  }
  AimsHSV& operator *= (const double &val)
  { _hue        *= val;
    _saturation *= val;
    _value      *= val;
  return(*this);
  }
  //@}

  /**@name Accessors*/
  //@{
  /// Get reference to hue component
  const byte& hue  () const { return _hue; }
  /// Get reference to saturation component
  const byte& saturation() const { return _saturation; }
  /// Get reference to value component
  const byte& value () const { return _value; }

  /// Get const reference to hue component
  byte& hue  () { return _hue; }
  /// Get const reference to saturation component
  byte& saturation() { return _saturation; }
  /// Get const reference to value component
  byte& value () { return _value; }

  /// Fake vector-like operators
  const byte & operator [] ( unsigned x ) const;
  /// Fake vector-like operators
  byte & operator [] ( unsigned x );
  //@}

  /**@name Friend functions*/
  //@{
  friend AimsHSV operator + (const AimsHSV &aa,
			     const AimsHSV &bb);
  friend AimsHSV operator - (const AimsHSV &aa,
			     const AimsHSV &bb);
  friend AimsHSV operator * (const AimsHSV &aa,
			     const byte &bb);
  friend AimsHSV operator * (const AimsHSV &aa,
                             const long &bb);
  friend AimsHSV operator * (const byte &aa,
			     const AimsHSV &bb);
  friend AimsHSV operator * (const long &aa,
                             const AimsHSV &bb);
  friend AimsHSV operator * (const AimsHSV &aa,
			     const double &bb);
  friend AimsHSV operator * (const double &aa,
			     const AimsHSV &bb);
  friend AimsHSV operator / (const AimsHSV &aa,
			     const byte &bb);
  friend AimsHSV operator + (const AimsHSV &aa);

  friend bool  operator == (const AimsHSV &aa,
			    const AimsHSV &bb);
  friend bool  operator != (const AimsHSV &aa,
			    const AimsHSV &bb);
  friend bool  operator == (const AimsHSV &aa,
			    const byte &bb);
  //@}
};



#ifndef DOXYGEN_HIDE_INTERNAL_CLASSES

namespace carto
{

  template<> inline std::string DataTypeCode<AimsHSV>::dataType()
  {
    return "HSV";
  }

}

#endif // DOXYGEN_HIDE_INTERNAL_CLASSES


inline
std::ostream& operator << (std::ostream &out,const AimsHSV &aa)
{ return( out << '(' << (int)aa.hue()   << ',' 
                     << (int)aa.saturation() << ',' 
                     << (int)aa.value()  << ')');
}

inline
std::istream& operator >> (std::istream& is, AimsHSV& thing)
{
  int h=0,s=0,v=0;
  char ch = 0;

  //std::istream::sentry s( is ); // to use in the future...
  while( is && ( is.peek() == ' ' || is.peek() == '\t' || is.peek() == '\n' ) )
    is >> ch;
  if (is.peek () == '(')
    is >> ch;
  is >> h;
  if (ch == '(')
    { is >> ch;
    if (ch == ',')
      is >> s >> ch;
    if (ch == ',')
      is >> v >> ch;
    }

  if (ch != 0 && ch != ')')
    is.setstate (std::ios::failbit);
  else if (is.good())
    thing = AimsHSV((byte)h,(byte)s,(byte)v);

  return is;
}


inline
AimsHSV operator + (const AimsHSV &aa,
                    const AimsHSV &bb)
{ return(AimsHSV(aa.hue()        + bb.hue(),
                 aa.saturation() + bb.saturation(),
                 aa.value()      + bb.value()));
}

inline
AimsHSV operator - (const AimsHSV &aa,
                    const AimsHSV &bb)
{ return(AimsHSV(aa.hue()        - bb.hue(),
                 aa.saturation() - bb.saturation(),
                 aa.value()      - bb.value()));
}

inline
AimsHSV operator * (const AimsHSV  &aa,
                    const byte &bb)
{ return(AimsHSV(aa.value()      * bb,
                 aa.saturation() * bb,
                 aa.value()      * bb));
}

inline
AimsHSV operator * (const AimsHSV  &aa,
                    const long &bb)
{ return(AimsHSV(aa.value()      * bb,
                 aa.saturation() * bb,
                 aa.value()      * bb));
}

inline
AimsHSV operator * (const AimsHSV  &aa,
                    const double &bb)
{ return(AimsHSV((byte) (aa.hue()        * bb),
                 (byte) (aa.saturation() * bb),
                 (byte) (aa.value()      * bb)));
}

inline
AimsHSV operator * (const byte &aa,
                    const AimsHSV  &bb)
{ return(AimsHSV(bb.hue()        * aa,
                 bb.saturation() * aa,
                 bb.value()      * aa));
}

inline
AimsHSV operator * (const long &aa,
                    const AimsHSV  &bb)
{ return(AimsHSV(bb.hue()        * aa,
                 bb.saturation() * aa,
                 bb.value()      * aa));
}


inline
AimsHSV operator * (const double &aa,
                    const AimsHSV  &bb)
{ return(AimsHSV((byte) (bb.hue()        * aa),
                 (byte) (bb.saturation() * aa),
                 (byte) (bb.value()      * aa)));
}

inline
AimsHSV operator / (const AimsHSV  &aa,
                    const byte &bb)
{ ASSERT(bb != 0);
  return(AimsHSV(aa.hue()        / bb,
                 aa.saturation() / bb,
                 aa.value()      / bb));
}

inline
AimsHSV operator / (const AimsHSV  &aa,
                    const long &bb)
{ ASSERT(bb != 0);
  return(AimsHSV(aa.hue()        / bb,
                 aa.saturation() / bb,
                 aa.value()      / bb));
}

inline
AimsHSV operator / (const AimsHSV  &aa,
                    const double &bb)
{ ASSERT(bb != 0);
  return(AimsHSV((byte) (aa.hue()        / bb),
                 (byte) (aa.saturation() / bb),
                 (byte) (aa.value()      / bb) ));
}



inline
bool operator == (const AimsHSV &aa,
                  const AimsHSV &bb)
{ return((aa.hue()        == bb.hue())   && 
         (aa.saturation() == bb.saturation()) &&
         (aa.value()      == bb.value())   );
}


inline
bool operator != (const AimsHSV &aa,
                  const AimsHSV &bb)
{ return((aa.hue()        != bb.hue())   ||
         (aa.saturation() != bb.saturation()) ||
         (aa.value()      != bb.value())   );
}


inline
bool operator == (const AimsHSV  &aa,
                  const byte &bb)
{ return((aa.hue()        == bb)  && 
         (aa.saturation() == bb)  &&
         (aa.value()      == bb)   );
}


inline const byte & AimsHSV::operator [] ( unsigned x ) const
{
  switch( x )
    {
    case 0:
      return _hue;
    case 1:
      return _saturation;
    case 2:
      return _value;
    default:
      return _hue;
    }
}


inline byte & AimsHSV::operator [] ( unsigned x )
{
  switch( x )
    {
    case 0:
      return _hue;
    case 1:
      return _saturation;
    case 2:
      return _value;
    default:
      return _hue;
    }
}

#endif


