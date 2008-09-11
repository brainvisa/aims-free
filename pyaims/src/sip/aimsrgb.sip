

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
  AimsRGB & operator /= ( unsigned );
  AimsRGB & operator *= ( unsigned );
  AimsRGB operator + ( const AimsRGB & );
  AimsRGB operator - ( const AimsRGB & );
  //AimsRGB operator * ( unsigned );
  AimsRGB operator / ( unsigned );

  bool  operator == ( const AimsRGB & );
%MethodCode
  sipRes = ( (*sipCpp) == *a0 );
%End
  bool  operator != ( const AimsRGB & );
%MethodCode
  sipRes = ( (*sipCpp) != *a0 );
%End
  bool  operator == ( const unsigned char & );
%MethodCode
  sipRes = ( (*sipCpp) == a0 );
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
  AimsRGBA & operator /= ( const unsigned char & );
  AimsRGBA & operator *= ( const unsigned char & );
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


