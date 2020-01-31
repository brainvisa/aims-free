

class AimsRGB
{
%TypeHeaderCode
#include <aims/rgb/rgb.h>
%End

public:
  AimsRGB( const AimsRGB & );
  AimsRGB( const AimsRGBA & );
  AimsRGB( unsigned =0, unsigned =0, unsigned =0);
  ~AimsRGB();

  AimsRGB & operator += ( const AimsRGB & );
  AimsRGB & operator += ( const AimsRGBA & );
  AimsRGB & operator /= ( long );
  AimsRGB & operator *= ( long );
  AimsRGB operator + ( const AimsRGB & );
  AimsRGB operator - ( const AimsRGB & );
  AimsRGB operator * ( long );
  AimsRGB operator / ( long );

  bool  operator == ( const AimsRGB & ) const;
%MethodCode
  sipRes = ( (*sipCpp) == *a0 );
%End
  bool  operator != ( const AimsRGB & ) const;
%MethodCode
  sipRes = ( (*sipCpp) != *a0 );
%End
  bool  operator == ( const unsigned char & ) const;
%MethodCode
  sipRes = ( (*sipCpp) == a0 );
%End
  bool  operator != ( const unsigned char & ) const;
%MethodCode
  sipRes = ( (*sipCpp) != a0 );
%End

  int __len__() const;
%MethodCode
  sipRes = 3;
%End

  unsigned red () const;
  unsigned green () const;
  unsigned blue () const;
  unsigned __getitem__( unsigned ) const;
%MethodCode
  if( a0 >= 3 )
    {
      sipIsErr = 1;
      sipRes = 0;
      //PyErr_SetString( sip  Self, "list index out of range" );
    }
  else
    sipRes = (*sipCpp)[a0];
%End

  void __setitem__( unsigned, unsigned );
%MethodCode
  if( a0 >= 3 )
    {
      sipIsErr = 1;
      //PyErr_SetString( sip__Self, "list index out of range" );
    }
  else
    (*sipCpp)[ a0 ] = (unsigned char) a1;
%End
};


class AimsRGBA
{

public:
  AimsRGBA( const AimsRGBA & );
  AimsRGBA( const AimsRGB & );
  AimsRGBA( unsigned = 0, unsigned = 0, unsigned = 0, unsigned = 0 );
  ~AimsRGBA();

  AimsRGBA & operator += ( const AimsRGBA & );
  AimsRGBA & operator += (const AimsRGB & );
  AimsRGBA & operator /= ( const long & );
  AimsRGBA & operator *= ( const long & );
  bool  operator == ( const AimsRGBA & );
%MethodCode
  sipRes = ( (*sipCpp) == *a0 );
%End
  bool  operator != ( const AimsRGBA & );
%MethodCode
  sipRes = ( (*sipCpp) != *a0 );
%End
  bool  operator == ( const unsigned char & );
%MethodCode
  sipRes = ( (*sipCpp) == a0 );
%End
  bool  operator != ( const unsigned char & );
%MethodCode
  sipRes = ( (*sipCpp) != a0 );
%End

  int __len__() const;
%MethodCode
  sipRes = 4;
%End

  unsigned red  () const;
  unsigned green() const;
  unsigned blue () const;
  unsigned alpha() const;
  unsigned __getitem__( unsigned ) const;
%MethodCode
  if( a0 >= 4 )
    {
      sipIsErr = 1;
      sipRes = 0;
      //PyErr_SetString( sip Self, "list index out of range" );
    }
  else
    sipRes = (*sipCpp)[a0];
%End

  void __setitem__( unsigned, unsigned );
%MethodCode
  if( a0 >= 4 )
    {
      sipIsErr = 1;
      //PyErr_SetString( sip Self, "list index out of range" );
    }
  else
    (*sipCpp)[ a0 ] = (unsigned char) a1;
%End
};


class AimsHSV
{
%TypeHeaderCode
#include <aims/hsv/hsv.h>
%End

public:
  AimsHSV( const AimsHSV & );
  AimsHSV( unsigned =0, unsigned =0, unsigned =0);
  ~AimsHSV();

  AimsHSV & operator += ( const AimsHSV & );
  AimsHSV & operator /= ( long );
  AimsHSV & operator *= ( long );
  AimsHSV operator + ( const AimsHSV & );
  AimsHSV operator - ( const AimsHSV & );
  AimsHSV operator * ( long );
  AimsHSV operator / ( long );

  bool  operator == ( const AimsHSV & );
%MethodCode
  sipRes = ( (*sipCpp) == *a0 );
%End
  bool  operator != ( const AimsHSV & );
%MethodCode
  sipRes = ( (*sipCpp) != *a0 );
%End
  bool  operator == ( const unsigned char & );
%MethodCode
  sipRes = ( (*sipCpp) == a0 );
%End
  bool  operator != ( const unsigned char & );
%MethodCode
  sipRes = ( (*sipCpp) != a0 );
%End

  int __len__() const;
%MethodCode
  sipRes = 3;
%End

  unsigned hue () const;
  unsigned saturation () const;
  unsigned value () const;
  unsigned __getitem__( unsigned ) const;
%MethodCode
  if( a0 >= 3 )
    {
      sipIsErr = 1;
      sipRes = 0;
      //PyErr_SetString( sip  Self, "list index out of range" );
    }
  else
    sipRes = (*sipCpp)[a0];
%End

  void __setitem__( unsigned, unsigned );
%MethodCode
  if( a0 >= 3 )
    {
      sipIsErr = 1;
      //PyErr_SetString( sip__Self, "list index out of range" );
    }
  else
    (*sipCpp)[ a0 ] = (unsigned char) a1;
%End
};


