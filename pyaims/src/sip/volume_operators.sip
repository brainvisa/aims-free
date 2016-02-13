
%ModuleCode
#include <pyaims/data/volumeoperators.h>
#include <aims/vector/vector.h>
%End

// U8

// U8 + U8

Volume_U8* __add__( Volume_U8 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U8__/;

Volume_U8 & __iadd__( Volume_U8 &, Volume_U8 & )
  /ReleaseGIL, PyName=__iadd_Volume_U8__/;

Volume_U8* __sub__( Volume_U8 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U8__/;

Volume_U8 & __isub__( Volume_U8 &, Volume_U8 & )
  /ReleaseGIL, PyName=__isub_Volume_U8__/;

Volume_U8* __mul__( Volume_U8 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U8__/;

Volume_U8 & __imul__( Volume_U8 &, Volume_U8 & )
  /ReleaseGIL, PyName=__imul_Volume_U8__/;

Volume_U8* __div__( Volume_U8 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_U8__/;

Volume_U8 & __idiv__( Volume_U8 &, Volume_U8 & )
  /ReleaseGIL, PyName=__idiv_Volume_U8__/;

Volume_U8* __mod__( Volume_U8 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_U8__/;

Volume_U8 & __imod__( Volume_U8 &, Volume_U8 & )
  /ReleaseGIL, PyName=__imod_Volume_U8__/;

Volume_U8* __and__( Volume_U8 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_U8__/;

Volume_U8 & __iand__( Volume_U8 &, Volume_U8 & )
  /ReleaseGIL, PyName=__iand_Volume_U8__/;

Volume_U8* __or__( Volume_U8 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_U8__/;

Volume_U8 & __ior__( Volume_U8 &, Volume_U8 & )
  /ReleaseGIL, PyName=__ior_Volume_U8__/;

// U8 + S16

Volume_S16* __add__( Volume_U8 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U8__/;

Volume_U8 & __iadd__( Volume_U8 &, Volume_S16 & )
  /ReleaseGIL, PyName=__iadd_Volume_U8__/;

Volume_S16* __sub__( Volume_U8 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U8__/;

Volume_U8 & __isub__( Volume_U8 &, Volume_S16 & )
  /ReleaseGIL, PyName=__isub_Volume_U8__/;

Volume_S16* __mul__( Volume_U8 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U8__/;

Volume_U8 & __imul__( Volume_U8 &, Volume_S16 & )
  /ReleaseGIL, PyName=__imul_Volume_U8__/;

Volume_S16* __div__( Volume_U8 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_U8__/;

Volume_U8 & __idiv__( Volume_U8 &, Volume_S16 & )
  /ReleaseGIL, PyName=__idiv_Volume_U8__/;

Volume_S16* __mod__( Volume_U8 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_U8__/;

Volume_U8 & __imod__( Volume_U8 &, Volume_S16 & )
  /ReleaseGIL, PyName=__imod_Volume_U8__/;

Volume_S16* __and__( Volume_U8 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_U8__/;

Volume_U8 & __iand__( Volume_U8 &, Volume_S16 & )
  /ReleaseGIL, PyName=__iand_Volume_U8__/;

Volume_S16* __or__( Volume_U8 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_U8__/;

Volume_U8 & __ior__( Volume_U8 &, Volume_S16 & )
  /ReleaseGIL, PyName=__ior_Volume_U8__/;

// U8 + U16

Volume_U16* __add__( Volume_U8 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U8__/;

Volume_U8 & __iadd__( Volume_U8 &, Volume_U16 & )
  /ReleaseGIL, PyName=__iadd_Volume_U8__/;

Volume_U16* __sub__( Volume_U8 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U8__/;

Volume_U8 & __isub__( Volume_U8 &, Volume_U16 & )
  /ReleaseGIL, PyName=__isub_Volume_U8__/;

Volume_U16* __mul__( Volume_U8 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U8__/;

Volume_U8 & __imul__( Volume_U8 &, Volume_U16 & )
  /ReleaseGIL, PyName=__imul_Volume_U8__/;

Volume_U16* __div__( Volume_U8 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_U8__/;

Volume_U8 & __idiv__( Volume_U8 &, Volume_U16 & )
  /ReleaseGIL, PyName=__idiv_Volume_U8__/;

Volume_U16* __mod__( Volume_U8 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_U8__/;

Volume_U8 & __imod__( Volume_U8 &, Volume_U16 & )
  /ReleaseGIL, PyName=__imod_Volume_U8__/;

Volume_U16* __and__( Volume_U8 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_U8__/;

Volume_U8 & __iand__( Volume_U8 &, Volume_U16 & )
  /ReleaseGIL, PyName=__iand_Volume_U8__/;

Volume_U16* __or__( Volume_U8 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_U8__/;

Volume_U8 & __ior__( Volume_U8 &, Volume_U16 & )
  /ReleaseGIL, PyName=__ior_Volume_U8__/;

// U8 + S32

Volume_S32* __add__( Volume_U8 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U8__/;

Volume_U8 & __iadd__( Volume_U8 &, Volume_S32 & )
  /ReleaseGIL, PyName=__iadd_Volume_U8__/;

Volume_S32* __sub__( Volume_U8 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U8__/;

Volume_U8 & __isub__( Volume_U8 &, Volume_S32 & )
  /ReleaseGIL, PyName=__isub_Volume_U8__/;

Volume_S32* __mul__( Volume_U8 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U8__/;

Volume_U8 & __imul__( Volume_U8 &, Volume_S32 & )
  /ReleaseGIL, PyName=__imul_Volume_U8__/;

Volume_S32* __div__( Volume_U8 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_U8__/;

Volume_U8 & __idiv__( Volume_U8 &, Volume_S32 & )
  /ReleaseGIL, PyName=__idiv_Volume_U8__/;

Volume_S32* __mod__( Volume_U8 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_U8__/;

Volume_U8 & __imod__( Volume_U8 &, Volume_S32 & )
  /ReleaseGIL, PyName=__imod_Volume_U8__/;

Volume_S32* __and__( Volume_U8 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_U8__/;

Volume_U8 & __iand__( Volume_U8 &, Volume_S32 & )
  /ReleaseGIL, PyName=__iand_Volume_U8__/;

Volume_S32* __or__( Volume_U8 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_U8__/;

Volume_U8 & __ior__( Volume_U8 &, Volume_S32 & )
  /ReleaseGIL, PyName=__ior_Volume_U8__/;

// U8 + U32

Volume_U32* __add__( Volume_U8 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U8__/;

Volume_U8 & __iadd__( Volume_U8 &, Volume_U32 & )
  /ReleaseGIL, PyName=__iadd_Volume_U8__/;

Volume_U32* __sub__( Volume_U8 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U8__/;

Volume_U8 & __isub__( Volume_U8 &, Volume_U32 & )
  /ReleaseGIL, PyName=__isub_Volume_U8__/;

Volume_U32* __mul__( Volume_U8 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U8__/;

Volume_U8 & __imul__( Volume_U8 &, Volume_U32 & )
  /ReleaseGIL, PyName=__imul_Volume_U8__/;

Volume_U32* __div__( Volume_U8 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_U8__/;

Volume_U8 & __idiv__( Volume_U8 &, Volume_U32 & )
  /ReleaseGIL, PyName=__idiv_Volume_U8__/;

Volume_U32* __mod__( Volume_U8 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_U8__/;

Volume_U8 & __imod__( Volume_U8 &, Volume_U32 & )
  /ReleaseGIL, PyName=__imod_Volume_U8__/;

Volume_U32* __and__( Volume_U8 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_U8__/;

Volume_U8 & __iand__( Volume_U8 &, Volume_U32 & )
  /ReleaseGIL, PyName=__iand_Volume_U8__/;

Volume_U32* __or__( Volume_U8 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_U8__/;

Volume_U8 & __ior__( Volume_U8 &, Volume_U32 & )
  /ReleaseGIL, PyName=__ior_Volume_U8__/;

// U8 + FLOAT

Volume_FLOAT* __add__( Volume_U8 &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U8__/;

Volume_U8 & __iadd__( Volume_U8 &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__iadd_Volume_U8__/;

Volume_FLOAT* __sub__( Volume_U8 &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U8__/;

Volume_U8 & __isub__( Volume_U8 &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__isub_Volume_U8__/;

Volume_FLOAT* __mul__( Volume_U8 &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U8__/;

Volume_U8 & __imul__( Volume_U8 &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__imul_Volume_U8__/;

Volume_FLOAT* __div__( Volume_U8 &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__div_Volume_U8__/;

Volume_U8 & __idiv__( Volume_U8 &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__idiv_Volume_U8__/;

// U8 + DOUBLE

Volume_DOUBLE* __add__( Volume_U8 &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U8__/;

Volume_U8 & __iadd__( Volume_U8 &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__iadd_Volume_U8__/;

Volume_DOUBLE* __sub__( Volume_U8 &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U8__/;

Volume_U8 & __isub__( Volume_U8 &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__isub_Volume_U8__/;

Volume_DOUBLE* __mul__( Volume_U8 &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U8__/;

Volume_U8 & __imul__( Volume_U8 &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__imul_Volume_U8__/;

Volume_DOUBLE* __div__( Volume_U8 &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__div_Volume_U8__/;

Volume_U8 & __idiv__( Volume_U8 &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__idiv_Volume_U8__/;

// U8 + RGB/RGBA

Volume_RGB* __add__( Volume_U8 &, Volume_RGB & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U8__/;

Volume_RGB* __sub__( Volume_U8 &, Volume_RGB & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U8__/;

Volume_RGB* __mul__( Volume_U8 &, Volume_RGB & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U8__/;

Volume_RGBA* __add__( Volume_U8 &, Volume_RGBA & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U8__/;

Volume_RGBA* __sub__( Volume_U8 &, Volume_RGBA & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U8__/;

Volume_RGBA* __mul__( Volume_U8 &, Volume_RGBA & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U8__/;

// U8 other

Volume_U8* __add__( Volume_U8 &, long )
  /Factory, ReleaseGIL, PyName=__add_Volume_U8__/;
%MethodCode
  unsigned char p = (unsigned char) a1;
  sipRes = __add__( *a0, p );
%End
Volume_U8* __add__( Volume_U8 &, long )
  /Factory, ReleaseGIL, PyName=__radd_Volume_U8__/;
%MethodCode
  unsigned char p = (unsigned char) a1;
  sipRes = __add__( *a0, p );
%End
/*
// sip apparently does not distinguish between int and double:
// this function is never called.
Volume_DOUBLE* __add__( Volume_U8 &, double )
  /Factory, ReleaseGIL, PyName=__radd_Volume_U8__/;
Volume_DOUBLE* __add__( Volume_U8 &, double )
  /Factory, ReleaseGIL, PyName=__add_Volume_U8__/;
*/
Volume_U8 & __iadd__( Volume_U8 &, double )
  /ReleaseGIL, PyName=__iadd_Volume_U8__/;

Volume_U8* __sub__( Volume_U8 &, long )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U8__/;
%MethodCode
  unsigned char p = (unsigned char) a1;
  sipRes = __sub__( *a0, p );
%End
Volume_U8* __sub__( Volume_U8 &, long )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_U8__/;
%MethodCode
  unsigned char p = (unsigned char) a1;
  sipRes = __sub__( p, *a0 );
%End
/*
Volume_DOUBLE* __sub__( Volume_U8 &, double )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_U8__/;
%MethodCode
  sipRes = __sub__( a1, *a0 );
%End
Volume_DOUBLE* __sub__( Volume_U8 &, double )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U8__/;
*/
Volume_U8 & __isub__( Volume_U8 &, double )
  /ReleaseGIL, PyName=__isub_Volume_U8__/;

Volume_U8* __mul__( Volume_U8 &, long )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U8__/;
%MethodCode
  unsigned char p = (unsigned char) a1;
  sipRes = __mul__( *a0, p );
%End
Volume_U8* __mul__( Volume_U8 &, long )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_U8__/;
%MethodCode
  unsigned char p = (unsigned char) a1;
  sipRes = __mul__( *a0, p );
%End
/*
Volume_DOUBLE* __mul__( Volume_U8 &, double )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_U8__/;
Volume_DOUBLE* __mul__( Volume_U8 &, double )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U8__/;
*/
Volume_U8 & __imul__( Volume_U8 &, double )
  /ReleaseGIL, PyName=__imul_Volume_U8__/;

Volume_U8* __div__( Volume_U8 &, long )
  /Factory, ReleaseGIL, PyName=__div_Volume_U8__/;
%MethodCode
  unsigned char p = (unsigned char) a1;
  sipRes = __div__( *a0, p );
%End
Volume_U8* __div__( Volume_U8 &, long )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_U8__/;
%MethodCode
  unsigned char p = (unsigned char) a1;
  sipRes = __div__( p, *a0 );
%End
/*
Volume_DOUBLE* __div__( Volume_U8 &, double )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_U8__/;
%MethodCode
  sipRes = __div__( a1, *a0 );
%End
Volume_DOUBLE* __div__( Volume_U8 &, double )
  /Factory, ReleaseGIL, PyName=__div_Volume_U8__/;
*/
Volume_U8 & __idiv__( Volume_U8 &, double )
  /ReleaseGIL, PyName=__idiv_Volume_U8__/;

Volume_U8* __mod__( Volume_U8 &, long )
  /Factory, ReleaseGIL, PyName=__mod_Volume_U8__/;
%MethodCode
  unsigned char p = (unsigned char) a1;
  sipRes = __mod__( *a0, p );
%End
/*
Volume_U8* __mod__( Volume_U8 &, long )
  /Factory, ReleaseGIL, PyName=__rmod_Volume_U8__/;
%MethodCode
  unsigned char p = (unsigned char) a1;
  sipRes = __mod__( p, *a0 );
%End
*/
Volume_U8 & __imod__( Volume_U8 &, long )
  /ReleaseGIL, PyName=__imod_Volume_U8__/;

Volume_U8* __and__( Volume_U8 &, long )
  /Factory, ReleaseGIL, PyName=__and_Volume_U8__/;
%MethodCode
  unsigned char p = (unsigned char) a1;
  sipRes = __and__( *a0, p );
%End
Volume_U8* __and__( Volume_U8 &, long )
  /Factory, ReleaseGIL, PyName=__rand_Volume_U8__/;
%MethodCode
  unsigned char p = (unsigned char) a1;
  sipRes = __and__( *a0, p );
%End
Volume_U8 & __iand__( Volume_U8 &, long )
  /ReleaseGIL, PyName=__iand_Volume_U8__/;

Volume_U8* __or__( Volume_U8 &, long )
  /Factory, ReleaseGIL, PyName=__or_Volume_U8__/;
%MethodCode
  unsigned char p = (unsigned char) a1;
  sipRes = __or__( *a0, p );
%End
Volume_U8* __or__( Volume_U8 &, long )
  /Factory, ReleaseGIL, PyName=__ror_Volume_U8__/;
%MethodCode
  unsigned char p = (unsigned char) a1;
  sipRes = __or__( *a0, p );
%End
Volume_U8 & __ior__( Volume_U8 &, long )
  /ReleaseGIL, PyName=__ior_Volume_U8__/;

Volume_U8* __xor__( Volume_U8 &, long )
  /Factory, ReleaseGIL, PyName=__xor_Volume_U8__/;
%MethodCode
  unsigned char p = (unsigned char) a1;
  sipRes = __xor__( *a0, p );
%End
Volume_U8* __xor__( Volume_U8 &, long )
  /Factory, ReleaseGIL, PyName=__rxor_Volume_U8__/;
%MethodCode
  unsigned char p = (unsigned char) a1;
  sipRes = __xor__( *a0, p );
%End
Volume_U8 & __ixor__( Volume_U8 &, long )
  /ReleaseGIL, PyName=__ixor_Volume_U8__/;

Volume_RGB* __add__( Volume_U8 &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__radd_Volume_U8__/;
Volume_RGB* __add__( Volume_U8 &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U8__/;
Volume_RGBA* __add__( Volume_U8 &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__radd_Volume_U8__/;
Volume_RGBA* __add__( Volume_U8 &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U8__/;
Volume_RGB* __sub__( Volume_U8 &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U8__/;
Volume_RGB* __sub__( Volume_U8 &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_U8__/;
%MethodCode
  sipRes = __sub__( *a1, *a0 );
%End
Volume_RGBA* __sub__( Volume_U8 &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U8__/;
Volume_RGBA* __sub__( Volume_U8 &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_U8__/;
%MethodCode
  sipRes = __sub__( *a1, *a0 );
%End
Volume_RGB* __mul__( Volume_U8 &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_U8__/;
Volume_RGB* __mul__( Volume_U8 &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U8__/;
Volume_RGBA* __mul__( Volume_U8 &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_U8__/;
Volume_RGBA* __mul__( Volume_U8 &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U8__/;
Volume_RGB* __div__( Volume_U8 &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_U8__/;
%MethodCode
  sipRes = __div__( *a1, *a0 );
%End
Volume_RGBA* __div__( Volume_U8 &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_U8__/;
%MethodCode
  sipRes = __div__( *a1, *a0 );
%End


// --- S16

// S16 + U8

Volume_S16* __add__( Volume_S16 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_S16__/;

Volume_S16 & __iadd__( Volume_S16 &, Volume_U8 & )
  /ReleaseGIL, PyName=__iadd_Volume_S16__/;

Volume_S16* __sub__( Volume_S16 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_S16__/;

Volume_S16 & __isub__( Volume_S16 &, Volume_U8 & )
  /ReleaseGIL, PyName=__isub_Volume_S16__/;

Volume_S16* __mul__( Volume_S16 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_S16__/;

Volume_S16 & __imul__( Volume_S16 &, Volume_U8 & )
  /ReleaseGIL, PyName=__imul_Volume_S16__/;

Volume_S16* __div__( Volume_S16 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_S16__/;

Volume_S16 & __idiv__( Volume_S16 &, Volume_U8 & )
  /ReleaseGIL, PyName=__idiv_Volume_S16__/;

Volume_S16* __mod__( Volume_S16 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_S16__/;

Volume_S16 & __imod__( Volume_S16 &, Volume_U8 & )
  /ReleaseGIL, PyName=__imod_Volume_S16__/;

Volume_S16* __and__( Volume_S16 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_S16__/;

Volume_S16 & __iand__( Volume_S16 &, Volume_U8 & )
  /ReleaseGIL, PyName=__iand_Volume_S16__/;

Volume_S16* __or__( Volume_S16 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_S16__/;

Volume_S16 & __ior__( Volume_S16 &, Volume_U8 & )
  /ReleaseGIL, PyName=__ior_Volume_S16__/;

// S16 + S16

Volume_S16* __add__( Volume_S16 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_S16__/;

Volume_S16 & __iadd__( Volume_S16 &, Volume_S16 & )
  /ReleaseGIL, PyName=__iadd_Volume_S16__/;

Volume_S16* __sub__( Volume_S16 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_S16__/;

Volume_S16 & __isub__( Volume_S16 &, Volume_S16 & )
  /ReleaseGIL, PyName=__isub_Volume_S16__/;

Volume_S16* __mul__( Volume_S16 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_S16__/;

Volume_S16 & __imul__( Volume_S16 &, Volume_S16 & )
  /ReleaseGIL, PyName=__imul_Volume_S16__/;

Volume_S16* __div__( Volume_S16 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_S16__/;

Volume_S16 & __idiv__( Volume_S16 &, Volume_S16 & )
  /ReleaseGIL, PyName=__idiv_Volume_S16__/;

Volume_S16* __mod__( Volume_S16 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_S16__/;

Volume_S16 & __imod__( Volume_S16 &, Volume_S16 & )
  /ReleaseGIL, PyName=__imod_Volume_S16__/;

Volume_S16* __and__( Volume_S16 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_S16__/;

Volume_S16 & __iand__( Volume_S16 &, Volume_S16 & )
  /ReleaseGIL, PyName=__iand_Volume_S16__/;

Volume_S16* __or__( Volume_S16 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_S16__/;

Volume_S16 & __ior__( Volume_S16 &, Volume_S16 & )
  /ReleaseGIL, PyName=__ior_Volume_S16__/;

// S16 + U16

Volume_U16* __add__( Volume_S16 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_S16__/;

Volume_S16 & __iadd__( Volume_S16 &, Volume_U16 & )
  /ReleaseGIL, PyName=__iadd_Volume_S16__/;

Volume_U16* __sub__( Volume_S16 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_S16__/;

Volume_S16 & __isub__( Volume_S16 &, Volume_U16 & )
  /ReleaseGIL, PyName=__isub_Volume_S16__/;

Volume_U16* __mul__( Volume_S16 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_S16__/;

Volume_S16 & __imul__( Volume_S16 &, Volume_U16 & )
  /ReleaseGIL, PyName=__imul_Volume_S16__/;

Volume_U16* __div__( Volume_S16 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_S16__/;

Volume_S16 & __idiv__( Volume_S16 &, Volume_U16 & )
  /ReleaseGIL, PyName=__idiv_Volume_S16__/;

Volume_U16* __mod__( Volume_S16 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_S16__/;

Volume_S16 & __imod__( Volume_S16 &, Volume_U16 & )
  /ReleaseGIL, PyName=__imod_Volume_S16__/;

Volume_U16* __and__( Volume_S16 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_S16__/;

Volume_S16 & __iand__( Volume_S16 &, Volume_U16 & )
  /ReleaseGIL, PyName=__iand_Volume_S16__/;

Volume_U16* __or__( Volume_S16 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_S16__/;

Volume_S16 & __ior__( Volume_S16 &, Volume_U16 & )
  /ReleaseGIL, PyName=__ior_Volume_S16__/;

// S16 + S32

Volume_S32* __add__( Volume_S16 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_S16__/;

Volume_S16 & __iadd__( Volume_S16 &, Volume_S32 & )
  /ReleaseGIL, PyName=__iadd_Volume_S16__/;

Volume_S32* __sub__( Volume_S16 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_S16__/;

Volume_S16 & __isub__( Volume_S16 &, Volume_S32 & )
  /ReleaseGIL, PyName=__isub_Volume_S16__/;

Volume_S32* __mul__( Volume_S16 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_S16__/;

Volume_S16 & __imul__( Volume_S16 &, Volume_S32 & )
  /ReleaseGIL, PyName=__imul_Volume_S16__/;

Volume_S32* __div__( Volume_S16 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_S16__/;

Volume_S16 & __idiv__( Volume_S16 &, Volume_S32 & )
  /ReleaseGIL, PyName=__idiv_Volume_S16__/;

Volume_S32* __mod__( Volume_S16 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_S16__/;

Volume_S16 & __imod__( Volume_S16 &, Volume_S32 & )
  /ReleaseGIL, PyName=__imod_Volume_S16__/;

Volume_S32* __and__( Volume_S16 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_S16__/;

Volume_S16 & __iand__( Volume_S16 &, Volume_S32 & )
  /ReleaseGIL, PyName=__iand_Volume_S16__/;

Volume_S32* __or__( Volume_S16 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_S16__/;

Volume_S16 & __ior__( Volume_S16 &, Volume_S32 & )
  /ReleaseGIL, PyName=__ior_Volume_S16__/;

// S16 + U32

Volume_U32* __add__( Volume_S16 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_S16__/;

Volume_S16 & __iadd__( Volume_S16 &, Volume_U32 & )
  /ReleaseGIL, PyName=__iadd_Volume_S16__/;

Volume_U32* __sub__( Volume_S16 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_S16__/;

Volume_S16 & __isub__( Volume_S16 &, Volume_U32 & )
  /ReleaseGIL, PyName=__isub_Volume_S16__/;

Volume_U32* __mul__( Volume_S16 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_S16__/;

Volume_S16 & __imul__( Volume_S16 &, Volume_U32 & )
  /ReleaseGIL, PyName=__imul_Volume_S16__/;

Volume_U32* __div__( Volume_S16 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_S16__/;

Volume_S16 & __idiv__( Volume_S16 &, Volume_U32 & )
  /ReleaseGIL, PyName=__idiv_Volume_S16__/;

Volume_U32* __mod__( Volume_S16 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_S16__/;

Volume_S16 & __imod__( Volume_S16 &, Volume_U32 & )
  /ReleaseGIL, PyName=__imod_Volume_S16__/;

Volume_U32* __and__( Volume_S16 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_S16__/;

Volume_S16 & __iand__( Volume_S16 &, Volume_U32 & )
  /ReleaseGIL, PyName=__iand_Volume_S16__/;

Volume_U32* __or__( Volume_S16 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_S16__/;

Volume_S16 & __ior__( Volume_S16 &, Volume_U32 & )
  /ReleaseGIL, PyName=__ior_Volume_S16__/;

// S16 + FLOAT

Volume_FLOAT* __add__( Volume_S16 &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__add_Volume_S16__/;

Volume_S16 & __iadd__( Volume_S16 &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__iadd_Volume_S16__/;

Volume_FLOAT* __sub__( Volume_S16 &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_S16__/;

Volume_S16 & __isub__( Volume_S16 &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__isub_Volume_S16__/;

Volume_FLOAT* __mul__( Volume_S16 &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_S16__/;

Volume_S16 & __imul__( Volume_S16 &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__imul_Volume_S16__/;

Volume_FLOAT* __div__( Volume_S16 &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__div_Volume_S16__/;

Volume_S16 & __idiv__( Volume_S16 &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__idiv_Volume_S16__/;

// S16 + DOUBLE

Volume_DOUBLE* __add__( Volume_S16 &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__add_Volume_S16__/;

Volume_S16 & __iadd__( Volume_S16 &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__iadd_Volume_S16__/;

Volume_DOUBLE* __sub__( Volume_S16 &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_S16__/;

Volume_S16 & __isub__( Volume_S16 &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__isub_Volume_S16__/;

Volume_DOUBLE* __mul__( Volume_S16 &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_S16__/;

Volume_S16 & __imul__( Volume_S16 &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__imul_Volume_S16__/;

Volume_DOUBLE* __div__( Volume_S16 &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__div_Volume_S16__/;

Volume_S16 & __idiv__( Volume_S16 &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__idiv_Volume_S16__/;

// S16 other

Volume_S16* __add__( Volume_S16 &, long )
  /Factory, ReleaseGIL, PyName=__add_Volume_S16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __add__( *a0, p );
%End
Volume_S16* __add__( Volume_S16 &, long )
  /Factory, ReleaseGIL, PyName=__radd_Volume_S16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __add__( *a0, p );
%End
/*
Volume_DOUBLE* __add__( Volume_S16 &, double )
  /Factory, ReleaseGIL, PyName=__radd_Volume_S16__/;
Volume_DOUBLE* __add__( Volume_S16 &, double )
  /Factory, ReleaseGIL, PyName=__add_Volume_S16__/;
*/
Volume_S16 & __iadd__( Volume_S16 &, double )
  /ReleaseGIL, PyName=__iadd_Volume_S16__/;

Volume_S16* __sub__( Volume_S16 &, long )
  /Factory, ReleaseGIL, PyName=__sub_Volume_S16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __sub__( *a0, p );
%End
Volume_S16* __sub__( Volume_S16 &, long )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_S16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __sub__( p, *a0 );
%End
/*
Volume_DOUBLE* __sub__( Volume_S16 &, double )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_S16__/;
%MethodCode
  sipRes = __sub__( a1, *a0 );
%End
Volume_DOUBLE* __sub__( Volume_S16 &, double )
  /Factory, ReleaseGIL, PyName=__sub_Volume_S16__/;
*/
Volume_S16 & __isub__( Volume_S16 &, double )
  /ReleaseGIL, PyName=__isub_Volume_S16__/;

Volume_S16* __mul__( Volume_S16 &, long )
  /Factory, ReleaseGIL, PyName=__mul_Volume_S16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __mul__( *a0, p );
%End
Volume_S16* __mul__( Volume_S16 &, long )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_S16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __mul__( *a0, p );
%End
/*
Volume_DOUBLE* __mul__( Volume_S16 &, double )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_S16__/;
Volume_DOUBLE* __mul__( Volume_S16 &, double )
  /Factory, ReleaseGIL, PyName=__mul_Volume_S16__/;
*/
Volume_S16 & __imul__( Volume_S16 &, double )
  /ReleaseGIL, PyName=__imul_Volume_S16__/;

Volume_S16* __div__( Volume_S16 &, long )
  /Factory, ReleaseGIL, PyName=__div_Volume_S16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __div__( *a0, p );
%End
Volume_S16* __div__( Volume_S16 &, long )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_S16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __div__( p, *a0 );
%End
/*
Volume_DOUBLE* __div__( Volume_S16 &, double )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_S16__/;
%MethodCode
  sipRes = __div__( a1, *a0 );
%End
Volume_DOUBLE* __div__( Volume_S16 &, double )
  /Factory, ReleaseGIL, PyName=__div_Volume_S16__/;
*/
Volume_S16 & __idiv__( Volume_S16 &, double )
  /ReleaseGIL, PyName=__idiv_Volume_S16__/;

Volume_S16* __mod__( Volume_S16 &, long )
  /Factory, ReleaseGIL, PyName=__mod_Volume_S16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __mod__( *a0, p );
%End
/*
Volume_S16* __mod__( Volume_S16 &, long )
  /Factory, ReleaseGIL, PyName=__rmod_Volume_S16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __mod__( p, *a0 );
%End
*/
Volume_S16 & __imod__( Volume_S16 &, long )
  /ReleaseGIL, PyName=__imod_Volume_S16__/;

Volume_S16* __and__( Volume_S16 &, long )
  /Factory, ReleaseGIL, PyName=__and_Volume_S16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __and__( *a0, p );
%End
Volume_S16* __and__( Volume_S16 &, long )
  /Factory, ReleaseGIL, PyName=__rand_Volume_S16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __and__( *a0, p );
%End
Volume_S16 & __iand__( Volume_S16 &, long )
  /ReleaseGIL, PyName=__iand_Volume_S16__/;

Volume_S16* __or__( Volume_S16 &, long )
  /Factory, ReleaseGIL, PyName=__or_Volume_S16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __or__( *a0, p );
%End
Volume_S16* __or__( Volume_S16 &, long )
  /Factory, ReleaseGIL, PyName=__ror_Volume_S16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __or__( *a0, p );
%End
Volume_S16 & __ior__( Volume_S16 &, long )
  /ReleaseGIL, PyName=__ior_Volume_S16__/;

Volume_S16* __xor__( Volume_S16 &, long )
  /Factory, ReleaseGIL, PyName=__xor_Volume_S16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __xor__( *a0, p );
%End
Volume_S16* __xor__( Volume_S16 &, long )
  /Factory, ReleaseGIL, PyName=__rxor_Volume_S16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __xor__( *a0, p );
%End
Volume_S16 & __ixor__( Volume_S16 &, long )
  /ReleaseGIL, PyName=__ixor_Volume_S16__/;


// ------ U16

// U16 + U8

Volume_U16* __add__( Volume_U16 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U16__/;

Volume_U16 & __iadd__( Volume_U16 &, Volume_U8 & )
  /ReleaseGIL, PyName=__iadd_Volume_U16__/;

Volume_U16* __sub__( Volume_U16 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U16__/;

Volume_U16 & __isub__( Volume_U16 &, Volume_U8 & )
  /ReleaseGIL, PyName=__isub_Volume_U16__/;

Volume_U16* __mul__( Volume_U16 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U16__/;

Volume_U16 & __imul__( Volume_U16 &, Volume_U8 & )
  /ReleaseGIL, PyName=__imul_Volume_U16__/;

Volume_U16* __div__( Volume_U16 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_U16__/;

Volume_U16 & __idiv__( Volume_U16 &, Volume_U8 & )
  /ReleaseGIL, PyName=__idiv_Volume_U16__/;

Volume_U16* __mod__( Volume_U16 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_U16__/;

Volume_U16 & __imod__( Volume_U16 &, Volume_U8 & )
  /ReleaseGIL, PyName=__imod_Volume_U16__/;

Volume_U16* __and__( Volume_U16 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_U16__/;

Volume_U16 & __iand__( Volume_U16 &, Volume_U8 & )
  /ReleaseGIL, PyName=__iand_Volume_U16__/;

Volume_U16* __or__( Volume_U16 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_U16__/;

Volume_U16 & __ior__( Volume_U16 &, Volume_U8 & )
  /ReleaseGIL, PyName=__ior_Volume_U16__/;

// U16 + S16

Volume_U16* __add__( Volume_U16 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U16__/;

Volume_U16 & __iadd__( Volume_U16 &, Volume_S16 & )
  /ReleaseGIL, PyName=__iadd_Volume_U16__/;

Volume_U16* __sub__( Volume_U16 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U16__/;

Volume_U16 & __isub__( Volume_U16 &, Volume_S16 & )
  /ReleaseGIL, PyName=__isub_Volume_U16__/;

Volume_U16* __mul__( Volume_U16 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U16__/;

Volume_U16 & __imul__( Volume_U16 &, Volume_S16 & )
  /ReleaseGIL, PyName=__imul_Volume_U16__/;

Volume_U16* __div__( Volume_U16 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_U16__/;

Volume_U16 & __idiv__( Volume_U16 &, Volume_S16 & )
  /ReleaseGIL, PyName=__idiv_Volume_U16__/;

Volume_U16* __mod__( Volume_U16 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_U16__/;

Volume_U16 & __imod__( Volume_U16 &, Volume_S16 & )
  /ReleaseGIL, PyName=__imod_Volume_U16__/;

Volume_U16* __and__( Volume_U16 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_U16__/;

Volume_U16 & __iand__( Volume_U16 &, Volume_S16 & )
  /ReleaseGIL, PyName=__iand_Volume_U16__/;

Volume_U16* __or__( Volume_U16 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_U16__/;

Volume_U16 & __ior__( Volume_U16 &, Volume_S16 & )
  /ReleaseGIL, PyName=__ior_Volume_U16__/;

// U16 + U16

Volume_U16* __add__( Volume_U16 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U16__/;

Volume_U16 & __iadd__( Volume_U16 &, Volume_U16 & )
  /ReleaseGIL, PyName=__iadd_Volume_U16__/;

Volume_U16* __sub__( Volume_U16 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U16__/;

Volume_U16 & __isub__( Volume_U16 &, Volume_U16 & )
  /ReleaseGIL, PyName=__isub_Volume_U16__/;

Volume_U16* __mul__( Volume_U16 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U16__/;

Volume_U16 & __imul__( Volume_U16 &, Volume_U16 & )
  /ReleaseGIL, PyName=__imul_Volume_U16__/;

Volume_U16* __div__( Volume_U16 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_U16__/;

Volume_U16 & __idiv__( Volume_U16 &, Volume_U16 & )
  /ReleaseGIL, PyName=__idiv_Volume_U16__/;

Volume_U16* __mod__( Volume_U16 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_U16__/;

Volume_U16 & __imod__( Volume_U16 &, Volume_U16 & )
  /ReleaseGIL, PyName=__imod_Volume_U16__/;

Volume_U16* __and__( Volume_U16 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_U16__/;

Volume_U16 & __iand__( Volume_U16 &, Volume_U16 & )
  /ReleaseGIL, PyName=__iand_Volume_U16__/;

Volume_U16* __or__( Volume_U16 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_U16__/;

Volume_U16 & __ior__( Volume_U16 &, Volume_U16 & )
  /ReleaseGIL, PyName=__ior_Volume_U16__/;

// U16 + S32

Volume_S32* __add__( Volume_U16 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U16__/;

Volume_U16 & __iadd__( Volume_U16 &, Volume_S32 & )
  /ReleaseGIL, PyName=__iadd_Volume_U16__/;

Volume_S32* __sub__( Volume_U16 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U16__/;

Volume_U16 & __isub__( Volume_U16 &, Volume_S32 & )
  /ReleaseGIL, PyName=__isub_Volume_U16__/;

Volume_S32* __mul__( Volume_U16 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U16__/;

Volume_U16 & __imul__( Volume_U16 &, Volume_S32 & )
  /ReleaseGIL, PyName=__imul_Volume_U16__/;

Volume_S32* __div__( Volume_U16 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_U16__/;

Volume_U16 & __idiv__( Volume_U16 &, Volume_S32 & )
  /ReleaseGIL, PyName=__idiv_Volume_U16__/;

Volume_S32* __mod__( Volume_U16 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_U16__/;

Volume_U16 & __imod__( Volume_U16 &, Volume_S32 & )
  /ReleaseGIL, PyName=__imod_Volume_U16__/;

Volume_S32* __and__( Volume_U16 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_U16__/;

Volume_U16 & __iand__( Volume_U16 &, Volume_S32 & )
  /ReleaseGIL, PyName=__iand_Volume_U16__/;

Volume_S32* __or__( Volume_U16 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_U16__/;

Volume_U16 & __ior__( Volume_U16 &, Volume_S32 & )
  /ReleaseGIL, PyName=__ior_Volume_U16__/;

// U16 + U32

Volume_U32* __add__( Volume_U16 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U16__/;

Volume_U16 & __iadd__( Volume_U16 &, Volume_U32 & )
  /ReleaseGIL, PyName=__iadd_Volume_U16__/;

Volume_U32* __sub__( Volume_U16 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U16__/;

Volume_U16 & __isub__( Volume_U16 &, Volume_U32 & )
  /ReleaseGIL, PyName=__isub_Volume_U16__/;

Volume_U32* __mul__( Volume_U16 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U16__/;

Volume_U16 & __imul__( Volume_U16 &, Volume_U32 & )
  /ReleaseGIL, PyName=__imul_Volume_U16__/;

Volume_U32* __div__( Volume_U16 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_U16__/;

Volume_U16 & __idiv__( Volume_U16 &, Volume_U32 & )
  /ReleaseGIL, PyName=__idiv_Volume_U16__/;

Volume_U32* __mod__( Volume_U16 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_U16__/;

Volume_U16 & __imod__( Volume_U16 &, Volume_U32 & )
  /ReleaseGIL, PyName=__imod_Volume_U16__/;

Volume_U32* __and__( Volume_U16 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_U16__/;

Volume_U16 & __iand__( Volume_U16 &, Volume_U32 & )
  /ReleaseGIL, PyName=__iand_Volume_U16__/;

Volume_U32* __or__( Volume_U16 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_U16__/;

Volume_U16 & __ior__( Volume_U16 &, Volume_U32 & )
  /ReleaseGIL, PyName=__ior_Volume_U16__/;

// U16 + FLOAT

Volume_FLOAT* __add__( Volume_U16 &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U16__/;

Volume_U16 & __iadd__( Volume_U16 &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__iadd_Volume_U16__/;

Volume_FLOAT* __sub__( Volume_U16 &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U16__/;

Volume_U16 & __isub__( Volume_U16 &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__isub_Volume_U16__/;

Volume_FLOAT* __mul__( Volume_U16 &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U16__/;

Volume_U16 & __imul__( Volume_U16 &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__imul_Volume_U16__/;

Volume_FLOAT* __div__( Volume_U16 &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__div_Volume_U16__/;

Volume_U16 & __idiv__( Volume_U16 &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__idiv_Volume_U16__/;

// U16 + DOUBLE

Volume_DOUBLE* __add__( Volume_U16 &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U16__/;

Volume_U16 & __iadd__( Volume_U16 &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__iadd_Volume_U16__/;

Volume_DOUBLE* __sub__( Volume_U16 &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U16__/;

Volume_U16 & __isub__( Volume_U16 &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__isub_Volume_U16__/;

Volume_DOUBLE* __mul__( Volume_U16 &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U16__/;

Volume_U16 & __imul__( Volume_U16 &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__imul_Volume_U16__/;

Volume_DOUBLE* __div__( Volume_U16 &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__div_Volume_U16__/;

Volume_U16 & __idiv__( Volume_U16 &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__idiv_Volume_U16__/;

// U16 + RGB/RGBA

Volume_RGB* __add__( Volume_U16 &, Volume_RGB & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U16__/;

Volume_RGB* __sub__( Volume_U16 &, Volume_RGB & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U16__/;

Volume_RGB* __mul__( Volume_U16 &, Volume_RGB & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U16__/;

Volume_RGBA* __add__( Volume_U16 &, Volume_RGBA & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U16__/;

Volume_RGBA* __sub__( Volume_U16 &, Volume_RGBA & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U16__/;

Volume_RGBA* __mul__( Volume_U16 &, Volume_RGBA & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U16__/;

// U16 other

Volume_U16* __add__( Volume_U16 &, long )
  /Factory, ReleaseGIL, PyName=__add_Volume_U16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __add__( *a0, p );
%End
Volume_U16* __add__( Volume_U16 &, long )
  /Factory, ReleaseGIL, PyName=__radd_Volume_U16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __add__( *a0, p );
%End
/*
Volume_DOUBLE* __add__( Volume_U16 &, double )
  /Factory, ReleaseGIL, PyName=__radd_Volume_U16__/;
Volume_DOUBLE* __add__( Volume_U16 &, double )
  /Factory, ReleaseGIL, PyName=__add_Volume_U16__/;
*/
Volume_U16 & __iadd__( Volume_U16 &, double )
  /ReleaseGIL, PyName=__iadd_Volume_U16__/;

Volume_U16* __sub__( Volume_U16 &, long )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __sub__( *a0, p );
%End
Volume_U16* __sub__( Volume_U16 &, long )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_U16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __sub__( p, *a0 );
%End
/*
Volume_DOUBLE* __sub__( Volume_U16 &, double )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_U16__/;
%MethodCode
  sipRes = __sub__( a1, *a0 );
%End
Volume_DOUBLE* __sub__( Volume_U16 &, double )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U16__/;
*/
Volume_U16 & __isub__( Volume_U16 &, double )
  /ReleaseGIL, PyName=__isub_Volume_U16__/;

Volume_U16* __mul__( Volume_U16 &, long )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __mul__( *a0, p );
%End
Volume_U16* __mul__( Volume_U16 &, long )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_U16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __mul__( *a0, p );
%End
/*
Volume_DOUBLE* __mul__( Volume_U16 &, double )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_U16__/;
Volume_DOUBLE* __mul__( Volume_U16 &, double )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U16__/;
*/
Volume_U16 & __imul__( Volume_U16 &, double )
  /ReleaseGIL, PyName=__imul_Volume_U16__/;

Volume_U16* __div__( Volume_U16 &, long )
  /Factory, ReleaseGIL, PyName=__div_Volume_U16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __div__( *a0, p );
%End
Volume_U16* __div__( Volume_U16 &, long )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_U16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __div__( p, *a0 );
%End
/*
Volume_DOUBLE* __div__( Volume_U16 &, double )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_U16__/;
%MethodCode
  sipRes = __div__( a1, *a0 );
%End
Volume_DOUBLE* __div__( Volume_U16 &, double )
  /Factory, ReleaseGIL, PyName=__div_Volume_U16__/;
*/
Volume_U16 & __idiv__( Volume_U16 &, double )
  /ReleaseGIL, PyName=__idiv_Volume_U16__/;

Volume_U16* __mod__( Volume_U16 &, long )
  /Factory, ReleaseGIL, PyName=__mod_Volume_U16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __mod__( *a0, p );
%End
/*
Volume_U16* __mod__( Volume_U16 &, long )
  /Factory, ReleaseGIL, PyName=__rmod_Volume_U16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __mod__( p, *a0 );
%End
*/
Volume_U16 & __imod__( Volume_U16 &, long )
  /ReleaseGIL, PyName=__imod_Volume_U16__/;

Volume_U16* __and__( Volume_U16 &, long )
  /Factory, ReleaseGIL, PyName=__and_Volume_U16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __and__( *a0, p );
%End
Volume_U16* __and__( Volume_U16 &, long )
  /Factory, ReleaseGIL, PyName=__rand_Volume_U16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __and__( *a0, p );
%End
Volume_U16 & __iand__( Volume_U16 &, long )
  /ReleaseGIL, PyName=__iand_Volume_U16__/;

Volume_U16* __or__( Volume_U16 &, long )
  /Factory, ReleaseGIL, PyName=__or_Volume_U16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __or__( *a0, p );
%End
Volume_U16* __or__( Volume_U16 &, long )
  /Factory, ReleaseGIL, PyName=__ror_Volume_U16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __or__( *a0, p );
%End
Volume_U16 & __ior__( Volume_U16 &, long )
  /ReleaseGIL, PyName=__ior_Volume_U16__/;

Volume_U16* __xor__( Volume_U16 &, long )
  /Factory, ReleaseGIL, PyName=__xor_Volume_U16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __xor__( *a0, p );
%End
Volume_U16* __xor__( Volume_U16 &, long )
  /Factory, ReleaseGIL, PyName=__rxor_Volume_U16__/;
%MethodCode
  short p = (short) a1;
  sipRes = __xor__( *a0, p );
%End
Volume_U16 & __ixor__( Volume_U16 &, long )
  /ReleaseGIL, PyName=__ixor_Volume_U16__/;

Volume_RGB* __add__( Volume_U16 &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__radd_Volume_U16__/;
Volume_RGB* __add__( Volume_U16 &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U16__/;
Volume_RGBA* __add__( Volume_U16 &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__radd_Volume_U16__/;
Volume_RGBA* __add__( Volume_U16 &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U16__/;
Volume_RGB* __sub__( Volume_U16 &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U16__/;
Volume_RGB* __sub__( Volume_U16 &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_U16__/;
%MethodCode
  sipRes = __sub__( *a1, *a0 );
%End
Volume_RGBA* __sub__( Volume_U16 &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U16__/;
Volume_RGBA* __sub__( Volume_U16 &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_U16__/;
%MethodCode
  sipRes = __sub__( *a1, *a0 );
%End
Volume_RGB* __mul__( Volume_U16 &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_U16__/;
Volume_RGB* __mul__( Volume_U16 &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U16__/;
Volume_RGBA* __mul__( Volume_U16 &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_U16__/;
Volume_RGBA* __mul__( Volume_U16 &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U16__/;
Volume_RGB* __div__( Volume_U16 &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_U16__/;
%MethodCode
  sipRes = __div__( *a1, *a0 );
%End
Volume_RGBA* __div__( Volume_U16 &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_U16__/;
%MethodCode
  sipRes = __div__( *a1, *a0 );
%End


// --- S32

// S32 + U8

Volume_S32* __add__( Volume_S32 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_S32__/;

Volume_S32 & __iadd__( Volume_S32 &, Volume_U8 & )
  /ReleaseGIL, PyName=__iadd_Volume_S32__/;

Volume_S32* __sub__( Volume_S32 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_S32__/;

Volume_S32 & __isub__( Volume_S32 &, Volume_U8 & )
  /ReleaseGIL, PyName=__isub_Volume_S32__/;

Volume_S32* __mul__( Volume_S32 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_S32__/;

Volume_S32 & __imul__( Volume_S32 &, Volume_U8 & )
  /ReleaseGIL, PyName=__imul_Volume_S32__/;

Volume_S32* __div__( Volume_S32 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_S32__/;

Volume_S32 & __idiv__( Volume_S32 &, Volume_U8 & )
  /ReleaseGIL, PyName=__idiv_Volume_S32__/;

Volume_S32* __mod__( Volume_S32 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_S32__/;

Volume_S32 & __imod__( Volume_S32 &, Volume_U8 & )
  /ReleaseGIL, PyName=__imod_Volume_S32__/;

Volume_S32* __and__( Volume_S32 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_S32__/;

Volume_S32 & __iand__( Volume_S32 &, Volume_U8 & )
  /ReleaseGIL, PyName=__iand_Volume_S32__/;

Volume_S32* __or__( Volume_S32 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_S32__/;

Volume_S32 & __ior__( Volume_S32 &, Volume_U8 & )
  /ReleaseGIL, PyName=__ior_Volume_S32__/;

// S32 + S16

Volume_S32* __add__( Volume_S32 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_S32__/;

Volume_S32 & __iadd__( Volume_S32 &, Volume_S16 & )
  /ReleaseGIL, PyName=__iadd_Volume_S32__/;

Volume_S32* __sub__( Volume_S32 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_S32__/;

Volume_S32 & __isub__( Volume_S32 &, Volume_S16 & )
  /ReleaseGIL, PyName=__isub_Volume_S32__/;

Volume_S32* __mul__( Volume_S32 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_S32__/;

Volume_S32 & __imul__( Volume_S32 &, Volume_S16 & )
  /ReleaseGIL, PyName=__imul_Volume_S32__/;

Volume_S32* __div__( Volume_S32 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_S32__/;

Volume_S32 & __idiv__( Volume_S32 &, Volume_S16 & )
  /ReleaseGIL, PyName=__idiv_Volume_S32__/;

Volume_S32* __mod__( Volume_S32 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_S32__/;

Volume_S32 & __imod__( Volume_S32 &, Volume_S16 & )
  /ReleaseGIL, PyName=__imod_Volume_S32__/;

Volume_S32* __and__( Volume_S32 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_S32__/;

Volume_S32 & __iand__( Volume_S32 &, Volume_S16 & )
  /ReleaseGIL, PyName=__iand_Volume_S32__/;

Volume_S32* __or__( Volume_S32 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_S32__/;

Volume_S32 & __ior__( Volume_S32 &, Volume_S16 & )
  /ReleaseGIL, PyName=__ior_Volume_S32__/;

// S32 + U16

Volume_S32* __add__( Volume_S32 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_S32__/;

Volume_S32 & __iadd__( Volume_S32 &, Volume_U16 & )
  /ReleaseGIL, PyName=__iadd_Volume_S32__/;

Volume_S32* __sub__( Volume_S32 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_S32__/;

Volume_S32 & __isub__( Volume_S32 &, Volume_U16 & )
  /ReleaseGIL, PyName=__isub_Volume_S32__/;

Volume_S32* __mul__( Volume_S32 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_S32__/;

Volume_S32 & __imul__( Volume_S32 &, Volume_U16 & )
  /ReleaseGIL, PyName=__imul_Volume_S32__/;

Volume_S32* __div__( Volume_S32 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_S32__/;

Volume_S32 & __idiv__( Volume_S32 &, Volume_U16 & )
  /ReleaseGIL, PyName=__idiv_Volume_S32__/;

Volume_S32* __mod__( Volume_S32 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_S32__/;

Volume_S32 & __imod__( Volume_S32 &, Volume_U16 & )
  /ReleaseGIL, PyName=__imod_Volume_S32__/;

Volume_S32* __and__( Volume_S32 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_S32__/;

Volume_S32 & __iand__( Volume_S32 &, Volume_U16 & )
  /ReleaseGIL, PyName=__iand_Volume_S32__/;

Volume_S32* __or__( Volume_S32 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_S32__/;

Volume_S32 & __ior__( Volume_S32 &, Volume_U16 & )
  /ReleaseGIL, PyName=__ior_Volume_S32__/;

// S32 + S32

Volume_S32* __add__( Volume_S32 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_S32__/;

Volume_S32 & __iadd__( Volume_S32 &, Volume_S32 & )
  /ReleaseGIL, PyName=__iadd_Volume_S32__/;

Volume_S32* __sub__( Volume_S32 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_S32__/;

Volume_S32 & __isub__( Volume_S32 &, Volume_S32 & )
  /ReleaseGIL, PyName=__isub_Volume_S32__/;

Volume_S32* __mul__( Volume_S32 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_S32__/;

Volume_S32 & __imul__( Volume_S32 &, Volume_S32 & )
  /ReleaseGIL, PyName=__imul_Volume_S32__/;

Volume_S32* __div__( Volume_S32 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_S32__/;

Volume_S32 & __idiv__( Volume_S32 &, Volume_S32 & )
  /ReleaseGIL, PyName=__idiv_Volume_S32__/;

Volume_S32* __mod__( Volume_S32 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_S32__/;

Volume_S32 & __imod__( Volume_S32 &, Volume_S32 & )
  /ReleaseGIL, PyName=__imod_Volume_S32__/;

Volume_S32* __and__( Volume_S32 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_S32__/;

Volume_S32 & __iand__( Volume_S32 &, Volume_S32 & )
  /ReleaseGIL, PyName=__iand_Volume_S32__/;

Volume_S32* __or__( Volume_S32 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_S32__/;

Volume_S32 & __ior__( Volume_S32 &, Volume_S32 & )
  /ReleaseGIL, PyName=__ior_Volume_S32__/;

// S32 + U32

Volume_U32* __add__( Volume_S32 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_S32__/;

Volume_S32 & __iadd__( Volume_S32 &, Volume_U32 & )
  /ReleaseGIL, PyName=__iadd_Volume_S32__/;

Volume_U32* __sub__( Volume_S32 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_S32__/;

Volume_S32 & __isub__( Volume_S32 &, Volume_U32 & )
  /ReleaseGIL, PyName=__isub_Volume_S32__/;

Volume_U32* __mul__( Volume_S32 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_S32__/;

Volume_S32 & __imul__( Volume_S32 &, Volume_U32 & )
  /ReleaseGIL, PyName=__imul_Volume_S32__/;

Volume_U32* __div__( Volume_S32 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_S32__/;

Volume_S32 & __idiv__( Volume_S32 &, Volume_U32 & )
  /ReleaseGIL, PyName=__idiv_Volume_S32__/;

Volume_U32* __mod__( Volume_S32 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_S32__/;

Volume_S32 & __imod__( Volume_S32 &, Volume_U32 & )
  /ReleaseGIL, PyName=__imod_Volume_S32__/;

Volume_U32* __and__( Volume_S32 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_S32__/;

Volume_S32 & __iand__( Volume_S32 &, Volume_U32 & )
  /ReleaseGIL, PyName=__iand_Volume_S32__/;

Volume_U32* __or__( Volume_S32 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_S32__/;

Volume_S32 & __ior__( Volume_S32 &, Volume_U32 & )
  /ReleaseGIL, PyName=__ior_Volume_S32__/;

// S32 + FLOAT

Volume_FLOAT* __add__( Volume_S32 &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__add_Volume_S32__/;

Volume_S32 & __iadd__( Volume_S32 &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__iadd_Volume_S32__/;

Volume_FLOAT* __sub__( Volume_S32 &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_S32__/;

Volume_S32 & __isub__( Volume_S32 &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__isub_Volume_S32__/;

Volume_FLOAT* __mul__( Volume_S32 &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_S32__/;

Volume_S32 & __imul__( Volume_S32 &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__imul_Volume_S32__/;

Volume_FLOAT* __div__( Volume_S32 &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__div_Volume_S32__/;

Volume_S32 & __idiv__( Volume_S32 &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__idiv_Volume_S32__/;

// S32 + DOUBLE

Volume_DOUBLE* __add__( Volume_S32 &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__add_Volume_S32__/;

Volume_S32 & __iadd__( Volume_S32 &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__iadd_Volume_S32__/;

Volume_DOUBLE* __sub__( Volume_S32 &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_S32__/;

Volume_S32 & __isub__( Volume_S32 &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__isub_Volume_S32__/;

Volume_DOUBLE* __mul__( Volume_S32 &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_S32__/;

Volume_S32 & __imul__( Volume_S32 &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__imul_Volume_S32__/;

Volume_DOUBLE* __div__( Volume_S32 &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__div_Volume_S32__/;

Volume_S32 & __idiv__( Volume_S32 &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__idiv_Volume_S32__/;

// S32 other

Volume_S32* __add__( Volume_S32 &, long )
  /Factory, ReleaseGIL, PyName=__add_Volume_S32__/;
%MethodCode
  int p = (int) a1;
  sipRes = __add__( *a0, p );
%End
Volume_S32* __add__( Volume_S32 &, long )
  /Factory, ReleaseGIL, PyName=__radd_Volume_S32__/;
%MethodCode
  int p = (int) a1;
  sipRes = __add__( *a0, p );
%End
/*
Volume_DOUBLE* __add__( Volume_S32 &, double )
  /Factory, ReleaseGIL, PyName=__radd_Volume_S32__/;
Volume_DOUBLE* __add__( Volume_S32 &, double )
  /Factory, ReleaseGIL, PyName=__add_Volume_S32__/;
*/
Volume_S32 & __iadd__( Volume_S32 &, double )
  /ReleaseGIL, PyName=__iadd_Volume_S32__/;

Volume_S32* __sub__( Volume_S32 &, long )
  /Factory, ReleaseGIL, PyName=__sub_Volume_S32__/;
%MethodCode
  int p = (int) a1;
  sipRes = __sub__( *a0, p );
%End
Volume_S32* __sub__( Volume_S32 &, long )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_S32__/;
%MethodCode
  int p = (int) a1;
  sipRes = __sub__( p, *a0 );
%End
/*
Volume_DOUBLE* __sub__( Volume_S32 &, double )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_S32__/;
%MethodCode
  sipRes = __sub__( a1, *a0 );
%End
Volume_DOUBLE* __sub__( Volume_S32 &, double )
  /Factory, ReleaseGIL, PyName=__sub_Volume_S32__/;
*/
Volume_S32 & __isub__( Volume_S32 &, double )
  /ReleaseGIL, PyName=__isub_Volume_S32__/;

Volume_S32* __mul__( Volume_S32 &, long )
  /Factory, ReleaseGIL, PyName=__mul_Volume_S32__/;
%MethodCode
  int p = (int) a1;
  sipRes = __mul__( *a0, p );
%End
Volume_S32* __mul__( Volume_S32 &, long )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_S32__/;
%MethodCode
  int p = (int) a1;
  sipRes = __mul__( *a0, p );
%End
/*
Volume_DOUBLE* __mul__( Volume_S32 &, double )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_S32__/;
Volume_DOUBLE* __mul__( Volume_S32 &, double )
  /Factory, ReleaseGIL, PyName=__mul_Volume_S32__/;
*/
Volume_S32 & __imul__( Volume_S32 &, double )
  /ReleaseGIL, PyName=__imul_Volume_S32__/;

Volume_S32* __div__( Volume_S32 &, long )
  /Factory, ReleaseGIL, PyName=__div_Volume_S32__/;
%MethodCode
  int p = (int) a1;
  sipRes = __div__( *a0, p );
%End
Volume_S32* __div__( Volume_S32 &, long )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_S32__/;
%MethodCode
  int p = (int) a1;
  sipRes = __div__( p, *a0 );
%End
/*
Volume_DOUBLE* __div__( Volume_S32 &, double )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_S32__/;
%MethodCode
  sipRes = __div__( a1, *a0 );
%End
Volume_DOUBLE* __div__( Volume_S32 &, double )
  /Factory, ReleaseGIL, PyName=__div_Volume_S32__/;
*/
Volume_S32 & __idiv__( Volume_S32 &, double )
  /ReleaseGIL, PyName=__idiv_Volume_S32__/;

Volume_S32* __mod__( Volume_S32 &, long )
  /Factory, ReleaseGIL, PyName=__mod_Volume_S32__/;
%MethodCode
  int p = (int) a1;
  sipRes = __mod__( *a0, p );
%End
/*
Volume_S32* __mod__( Volume_S32 &, long )
  /Factory, ReleaseGIL, PyName=__rmod_Volume_S32__/;
%MethodCode
  int p = (int) a1;
  sipRes = __mod__( p, *a0 );
%End
*/
Volume_S32 & __imod__( Volume_S32 &, long )
  /ReleaseGIL, PyName=__imod_Volume_S32__/;

Volume_S32* __and__( Volume_S32 &, long )
  /Factory, ReleaseGIL, PyName=__and_Volume_S32__/;
%MethodCode
  int p = (int) a1;
  sipRes = __and__( *a0, p );
%End
Volume_S32* __and__( Volume_S32 &, long )
  /Factory, ReleaseGIL, PyName=__rand_Volume_S32__/;
%MethodCode
  int p = (int) a1;
  sipRes = __and__( *a0, p );
%End
Volume_S32 & __iand__( Volume_S32 &, long )
  /ReleaseGIL, PyName=__iand_Volume_S32__/;

Volume_S32* __or__( Volume_S32 &, long )
  /Factory, ReleaseGIL, PyName=__or_Volume_S32__/;
%MethodCode
  int p = (int) a1;
  sipRes = __or__( *a0, p );
%End
Volume_S32* __or__( Volume_S32 &, long )
  /Factory, ReleaseGIL, PyName=__ror_Volume_S32__/;
%MethodCode
  int p = (int) a1;
  sipRes = __or__( *a0, p );
%End
Volume_S32 & __ior__( Volume_S32 &, long )
  /ReleaseGIL, PyName=__ior_Volume_S32__/;

Volume_S32* __xor__( Volume_S32 &, long )
  /Factory, ReleaseGIL, PyName=__xor_Volume_S32__/;
%MethodCode
  int p = (int) a1;
  sipRes = __xor__( *a0, p );
%End
Volume_S32* __xor__( Volume_S32 &, long )
  /Factory, ReleaseGIL, PyName=__rxor_Volume_S32__/;
%MethodCode
  int p = (int) a1;
  sipRes = __xor__( *a0, p );
%End
Volume_S32 & __ixor__( Volume_S32 &, long )
  /ReleaseGIL, PyName=__ixor_Volume_S32__/;


// --- U32

// U32 + U8

Volume_U32* __add__( Volume_U32 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U32__/;

Volume_U32 & __iadd__( Volume_U32 &, Volume_U8 & )
  /ReleaseGIL, PyName=__iadd_Volume_U32__/;

Volume_U32* __sub__( Volume_U32 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U32__/;

Volume_U32 & __isub__( Volume_U32 &, Volume_U8 & )
  /ReleaseGIL, PyName=__isub_Volume_U32__/;

Volume_U32* __mul__( Volume_U32 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U32__/;

Volume_U32 & __imul__( Volume_U32 &, Volume_U8 & )
  /ReleaseGIL, PyName=__imul_Volume_U32__/;

Volume_U32* __div__( Volume_U32 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_U32__/;

Volume_U32 & __idiv__( Volume_U32 &, Volume_U8 & )
  /ReleaseGIL, PyName=__idiv_Volume_U32__/;

Volume_U32* __mod__( Volume_U32 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_U32__/;

Volume_U32 & __imod__( Volume_U32 &, Volume_U8 & )
  /ReleaseGIL, PyName=__imod_Volume_U32__/;

Volume_U32* __and__( Volume_U32 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_U32__/;

Volume_U32 & __iand__( Volume_U32 &, Volume_U8 & )
  /ReleaseGIL, PyName=__iand_Volume_U32__/;

Volume_U32* __or__( Volume_U32 &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_U32__/;

Volume_U32 & __ior__( Volume_U32 &, Volume_U8 & )
  /ReleaseGIL, PyName=__ior_Volume_U32__/;

// U32 + S16

Volume_U32* __add__( Volume_U32 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U32__/;

Volume_U32 & __iadd__( Volume_U32 &, Volume_S16 & )
  /ReleaseGIL, PyName=__iadd_Volume_U32__/;

Volume_U32* __sub__( Volume_U32 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U32__/;

Volume_U32 & __isub__( Volume_U32 &, Volume_S16 & )
  /ReleaseGIL, PyName=__isub_Volume_U32__/;

Volume_U32* __mul__( Volume_U32 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U32__/;

Volume_U32 & __imul__( Volume_U32 &, Volume_S16 & )
  /ReleaseGIL, PyName=__imul_Volume_U32__/;

Volume_U32* __div__( Volume_U32 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_U32__/;

Volume_U32 & __idiv__( Volume_U32 &, Volume_S16 & )
  /ReleaseGIL, PyName=__idiv_Volume_U32__/;

Volume_U32* __mod__( Volume_U32 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_U32__/;

Volume_U32 & __imod__( Volume_U32 &, Volume_S16 & )
  /ReleaseGIL, PyName=__imod_Volume_U32__/;

Volume_U32* __and__( Volume_U32 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_U32__/;

Volume_U32 & __iand__( Volume_U32 &, Volume_S16 & )
  /ReleaseGIL, PyName=__iand_Volume_U32__/;

Volume_U32* __or__( Volume_U32 &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_U32__/;

Volume_U32 & __ior__( Volume_U32 &, Volume_S16 & )
  /ReleaseGIL, PyName=__ior_Volume_U32__/;

// S32 + U16

Volume_U32* __add__( Volume_U32 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U32__/;

Volume_U32 & __iadd__( Volume_U32 &, Volume_U16 & )
  /ReleaseGIL, PyName=__iadd_Volume_U32__/;

Volume_U32* __sub__( Volume_U32 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U32__/;

Volume_U32 & __isub__( Volume_U32 &, Volume_U16 & )
  /ReleaseGIL, PyName=__isub_Volume_U32__/;

Volume_U32* __mul__( Volume_U32 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U32__/;

Volume_U32 & __imul__( Volume_U32 &, Volume_U16 & )
  /ReleaseGIL, PyName=__imul_Volume_U32__/;

Volume_U32* __div__( Volume_U32 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_U32__/;

Volume_U32 & __idiv__( Volume_U32 &, Volume_U16 & )
  /Factory, PyName=__idiv_Volume_U32__/;

Volume_U32* __mod__( Volume_U32 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_U32__/;

Volume_U32 & __imod__( Volume_U32 &, Volume_U16 & )
  /ReleaseGIL, PyName=__imod_Volume_U32__/;

Volume_U32* __and__( Volume_U32 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_U32__/;

Volume_U32 & __iand__( Volume_U32 &, Volume_U16 & )
  /ReleaseGIL, PyName=__iand_Volume_U32__/;

Volume_U32* __or__( Volume_U32 &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_U32__/;

Volume_U32 & __ior__( Volume_U32 &, Volume_U16 & )
  /ReleaseGIL, PyName=__ior_Volume_U32__/;

// U32 + S32

Volume_U32* __add__( Volume_U32 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U32__/;

Volume_U32 & __iadd__( Volume_U32 &, Volume_S32 & )
  /ReleaseGIL, PyName=__iadd_Volume_U32__/;

Volume_U32* __sub__( Volume_U32 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U32__/;

Volume_U32 & __isub__( Volume_U32 &, Volume_S32 & )
  /ReleaseGIL, PyName=__isub_Volume_U32__/;

Volume_U32* __mul__( Volume_U32 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U32__/;

Volume_U32 & __imul__( Volume_U32 &, Volume_S32 & )
  /ReleaseGIL, PyName=__imul_Volume_U32__/;

Volume_U32* __div__( Volume_U32 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_U32__/;

Volume_U32 & __idiv__( Volume_U32 &, Volume_S32 & )
  /ReleaseGIL, PyName=__idiv_Volume_U32__/;

Volume_U32* __mod__( Volume_U32 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_U32__/;

Volume_U32 & __imod__( Volume_U32 &, Volume_S32 & )
  /ReleaseGIL, PyName=__imod_Volume_U32__/;

Volume_U32* __and__( Volume_U32 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_U32__/;

Volume_U32 & __iand__( Volume_U32 &, Volume_S32 & )
  /ReleaseGIL, PyName=__iand_Volume_U32__/;

Volume_U32* __or__( Volume_U32 &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_U32__/;

Volume_U32 & __ior__( Volume_U32 &, Volume_S32 & )
  /ReleaseGIL, PyName=__ior_Volume_U32__/;

// U32 + U32

Volume_U32* __add__( Volume_U32 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U32__/;

Volume_U32 & __iadd__( Volume_U32 &, Volume_U32 & )
  /ReleaseGIL, PyName=__iadd_Volume_U32__/;

Volume_U32* __sub__( Volume_U32 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U32__/;

Volume_U32 & __isub__( Volume_U32 &, Volume_U32 & )
  /ReleaseGIL, PyName=__isub_Volume_U32__/;

Volume_U32* __mul__( Volume_U32 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U32__/;

Volume_U32 & __imul__( Volume_U32 &, Volume_U32 & )
  /ReleaseGIL, PyName=__imul_Volume_U32__/;

Volume_U32* __div__( Volume_U32 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_U32__/;

Volume_U32 & __idiv__( Volume_U32 &, Volume_U32 & )
  /ReleaseGIL, PyName=__idiv_Volume_U32__/;

Volume_U32* __mod__( Volume_U32 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_U32__/;

Volume_U32 & __imod__( Volume_U32 &, Volume_U32 & )
  /ReleaseGIL, PyName=__imod_Volume_U32__/;

Volume_U32* __and__( Volume_U32 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__and_Volume_U32__/;

Volume_U32 & __iand__( Volume_U32 &, Volume_U32 & )
  /ReleaseGIL, PyName=__iand_Volume_U32__/;

Volume_U32* __or__( Volume_U32 &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__or_Volume_U32__/;

Volume_U32 & __ior__( Volume_U32 &, Volume_U32 & )
  /ReleaseGIL, PyName=__ior_Volume_U32__/;

// U32 + FLOAT

Volume_FLOAT* __add__( Volume_U32 &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U32__/;

Volume_U32 & __iadd__( Volume_U32 &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__iadd_Volume_U32__/;

Volume_FLOAT* __sub__( Volume_U32 &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U32__/;

Volume_U32 & __isub__( Volume_U32 &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__isub_Volume_U32__/;

Volume_FLOAT* __mul__( Volume_U32 &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U32__/;

Volume_U32 & __imul__( Volume_U32 &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__imul_Volume_U32__/;

Volume_FLOAT* __div__( Volume_U32 &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__div_Volume_U32__/;

Volume_U32 & __idiv__( Volume_U32 &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__idiv_Volume_U32__/;

// U32 + DOUBLE

Volume_DOUBLE* __add__( Volume_U32 &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U32__/;

Volume_U32 & __iadd__( Volume_U32 &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__iadd_Volume_U32__/;

Volume_DOUBLE* __sub__( Volume_U32 &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U32__/;

Volume_U32 & __isub__( Volume_U32 &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__isub_Volume_U32__/;

Volume_DOUBLE* __mul__( Volume_U32 &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U32__/;

Volume_U32 & __imul__( Volume_U32 &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__imul_Volume_U32__/;

Volume_DOUBLE* __div__( Volume_U32 &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__div_Volume_U32__/;

Volume_U32 & __idiv__( Volume_U32 &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__idiv_Volume_U32__/;

// U32 + RGB/RGBA

Volume_RGB* __add__( Volume_U32 &, Volume_RGB & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U32__/;

Volume_RGB* __sub__( Volume_U32 &, Volume_RGB & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U32__/;

Volume_RGB* __mul__( Volume_U32 &, Volume_RGB & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U32__/;

Volume_RGBA* __add__( Volume_U32 &, Volume_RGBA & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U32__/;

Volume_RGBA* __sub__( Volume_U32 &, Volume_RGBA & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U32__/;

Volume_RGBA* __mul__( Volume_U32 &, Volume_RGBA & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U32__/;

// U32 other

Volume_U32* __add__( Volume_U32 &, long )
  /Factory, ReleaseGIL, PyName=__add_Volume_U32__/;
%MethodCode
  unsigned p = (unsigned) a1;
  sipRes = __add__( *a0, p );
%End
Volume_U32* __add__( Volume_U32 &, long )
  /Factory, ReleaseGIL, PyName=__radd_Volume_U32__/;
%MethodCode
  unsigned p = (unsigned) a1;
  sipRes = __add__( *a0, p );
%End
/*
Volume_DOUBLE* __add__( Volume_U32 &, double )
  /Factory, ReleaseGIL, PyName=__radd_Volume_U32__/;
Volume_DOUBLE* __add__( Volume_U32 &, double )
  /Factory, ReleaseGIL, PyName=__add_Volume_U32__/;
*/
Volume_U32 & __iadd__( Volume_U32 &, double )
  /ReleaseGIL, PyName=__iadd_Volume_U32__/;

Volume_U32* __sub__( Volume_U32 &, long )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U32__/;
%MethodCode
  unsigned p = (unsigned) a1;
  sipRes = __sub__( *a0, p );
%End
Volume_U32* __sub__( Volume_U32 &, long )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_U32__/;
%MethodCode
  unsigned p = (unsigned) a1;
  sipRes = __sub__( p, *a0 );
%End
/*
Volume_DOUBLE* __sub__( Volume_U32 &, double )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_U32__/;
%MethodCode
  sipRes = __sub__( a1, *a0 );
%End
Volume_DOUBLE* __sub__( Volume_U32 &, double )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U32__/;
*/
Volume_U32 & __isub__( Volume_U32 &, double )
  /ReleaseGIL, PyName=__isub_Volume_U32__/;

Volume_U32* __mul__( Volume_U32 &, long )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U32__/;
%MethodCode
  unsigned p = (unsigned) a1;
  sipRes = __mul__( *a0, p );
%End
Volume_U32* __mul__( Volume_U32 &, long )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_U32__/;
%MethodCode
  unsigned p = (unsigned) a1;
  sipRes = __mul__( *a0, p );
%End
/*
Volume_DOUBLE* __mul__( Volume_U32 &, double )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_U32__/;
Volume_DOUBLE* __mul__( Volume_U32 &, double )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U32__/;
*/
Volume_U32 & __imul__( Volume_U32 &, double )
  /ReleaseGIL, PyName=__imul_Volume_U32__/;

Volume_U32* __div__( Volume_U32 &, long )
  /Factory, ReleaseGIL, PyName=__div_Volume_U32__/;
%MethodCode
  unsigned p = (unsigned) a1;
  sipRes = __div__( *a0, p );
%End
Volume_U32* __div__( Volume_U32 &, long )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_U32__/;
%MethodCode
  unsigned p = (unsigned) a1;
  sipRes = __div__( p, *a0 );
%End
/*
Volume_DOUBLE* __div__( Volume_U32 &, double )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_U32__/;
%MethodCode
  sipRes = __div__( a1, *a0 );
%End
Volume_DOUBLE* __div__( Volume_U32 &, double )
  /Factory, ReleaseGIL, PyName=__div_Volume_U32__/;
*/
Volume_U32 & __idiv__( Volume_U32 &, double )
  /ReleaseGIL, PyName=__idiv_Volume_U32__/;

Volume_U32* __mod__( Volume_U32 &, long )
  /Factory, ReleaseGIL, PyName=__mod_Volume_U32__/;
%MethodCode
  unsigned p = (unsigned) a1;
  sipRes = __mod__( *a0, p );
%End
/*
Volume_U32* __mod__( Volume_U32 &, long )
  /Factory, ReleaseGIL, PyName=__rmod_Volume_U32__/;
%MethodCode
  unsigned p = (unsigned) a1;
  sipRes = __mod__( p, *a0 );
%End
*/
Volume_U32 & __imod__( Volume_U32 &, long )
  /ReleaseGIL, PyName=__imod_Volume_U32__/;

Volume_U32* __and__( Volume_U32 &, long )
  /Factory, ReleaseGIL, PyName=__and_Volume_U32__/;
%MethodCode
  unsigned p = (unsigned) a1;
  sipRes = __and__( *a0, p );
%End
Volume_U32* __and__( Volume_U32 &, long )
  /Factory, ReleaseGIL, PyName=__rand_Volume_U32__/;
%MethodCode
  unsigned p = (unsigned) a1;
  sipRes = __and__( *a0, p );
%End
Volume_U32 & __iand__( Volume_U32 &, long )
  /ReleaseGIL, PyName=__iand_Volume_U32__/;

Volume_U32* __or__( Volume_U32 &, long )
  /Factory, ReleaseGIL, PyName=__or_Volume_U32__/;
%MethodCode
  unsigned p = (unsigned) a1;
  sipRes = __or__( *a0, p );
%End
Volume_U32* __or__( Volume_U32 &, long )
  /Factory, ReleaseGIL, PyName=__ror_Volume_U32__/;
%MethodCode
  unsigned p = (unsigned) a1;
  sipRes = __or__( *a0, p );
%End
Volume_U32 & __ior__( Volume_U32 &, long )
  /ReleaseGIL, PyName=__ior_Volume_U32__/;

Volume_U32* __xor__( Volume_U32 &, long )
  /Factory, ReleaseGIL, PyName=__xor_Volume_U32__/;
%MethodCode
  unsigned p = (unsigned) a1;
  sipRes = __xor__( *a0, p );
%End
Volume_U32* __xor__( Volume_U32 &, long )
  /Factory, ReleaseGIL, PyName=__rxor_Volume_U32__/;
%MethodCode
  unsigned p = (unsigned) a1;
  sipRes = __xor__( *a0, p );
%End
Volume_U32 & __ixor__( Volume_U32 &, long )
  /ReleaseGIL, PyName=__ixor_Volume_U32__/;

Volume_RGB* __add__( Volume_U32 &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__radd_Volume_U32__/;
Volume_RGB* __add__( Volume_U32 &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U32__/;
Volume_RGBA* __add__( Volume_U32 &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__radd_Volume_U32__/;
Volume_RGBA* __add__( Volume_U32 &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__add_Volume_U32__/;
Volume_RGB* __sub__( Volume_U32 &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U32__/;
Volume_RGB* __sub__( Volume_U32 &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_U32__/;
%MethodCode
  sipRes = __sub__( *a1, *a0 );
%End
Volume_RGBA* __sub__( Volume_U32 &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_U32__/;
Volume_RGBA* __sub__( Volume_U32 &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_U32__/;
%MethodCode
  sipRes = __sub__( *a1, *a0 );
%End
Volume_RGB* __mul__( Volume_U32 &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_U32__/;
Volume_RGB* __mul__( Volume_U32 &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U32__/;
Volume_RGBA* __mul__( Volume_U32 &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_U32__/;
Volume_RGBA* __mul__( Volume_U32 &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_U32__/;
Volume_RGB* __div__( Volume_U32 &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_U32__/;
%MethodCode
  sipRes = __div__( *a1, *a0 );
%End
Volume_RGBA* __div__( Volume_U32 &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_U32__/;
%MethodCode
  sipRes = __div__( *a1, *a0 );
%End


// --- FLOAT

// FLOAT + U8

Volume_FLOAT* __add__( Volume_FLOAT &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_FLOAT__/;

Volume_FLOAT & __iadd__( Volume_FLOAT &, Volume_U8 & )
  /ReleaseGIL, PyName=__iadd_Volume_FLOAT__/;

Volume_FLOAT* __sub__( Volume_FLOAT &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_FLOAT__/;

Volume_FLOAT & __isub__( Volume_FLOAT &, Volume_U8 & )
  /ReleaseGIL, PyName=__isub_Volume_FLOAT__/;

Volume_FLOAT* __mul__( Volume_FLOAT &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_FLOAT__/;

Volume_FLOAT & __imul__( Volume_FLOAT &, Volume_U8 & )
  /ReleaseGIL, PyName=__imul_Volume_FLOAT__/;

Volume_FLOAT* __div__( Volume_FLOAT &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_FLOAT__/;

Volume_FLOAT & __idiv__( Volume_FLOAT &, Volume_U8 & )
  /ReleaseGIL, PyName=__idiv_Volume_FLOAT__/;

/*
Volume_FLOAT* __mod__( Volume_FLOAT &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_FLOAT__/;

Volume_FLOAT & __imod__( Volume_FLOAT &, Volume_U8 & )
  /ReleaseGIL, PyName=__imod_Volume_FLOAT__/;
*/

// FLOAT + S16

Volume_FLOAT* __add__( Volume_FLOAT &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_FLOAT__/;

Volume_FLOAT & __iadd__( Volume_FLOAT &, Volume_S16 & )
  /ReleaseGIL, PyName=__iadd_Volume_FLOAT__/;

Volume_FLOAT* __sub__( Volume_FLOAT &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_FLOAT__/;

Volume_FLOAT & __isub__( Volume_FLOAT &, Volume_S16 & )
  /ReleaseGIL, PyName=__isub_Volume_FLOAT__/;

Volume_FLOAT* __mul__( Volume_FLOAT &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_FLOAT__/;

Volume_FLOAT & __imul__( Volume_FLOAT &, Volume_S16 & )
  /ReleaseGIL, PyName=__imul_Volume_FLOAT__/;

Volume_FLOAT* __div__( Volume_FLOAT &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_FLOAT__/;

Volume_FLOAT & __idiv__( Volume_FLOAT &, Volume_S16 & )
  /ReleaseGIL, PyName=__idiv_Volume_FLOAT__/;

/*
Volume_FLOAT* __mod__( Volume_FLOAT &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_FLOAT__/;

Volume_FLOAT & __imod__( Volume_FLOAT &, Volume_S16 & )
  /ReleaseGIL, PyName=__imod_Volume_FLOAT__/;
*/

// FLOAT + U16

Volume_FLOAT* __add__( Volume_FLOAT &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_FLOAT__/;

Volume_FLOAT & __iadd__( Volume_FLOAT &, Volume_U16 & )
  /ReleaseGIL, PyName=__iadd_Volume_FLOAT__/;

Volume_FLOAT* __sub__( Volume_FLOAT &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_FLOAT__/;

Volume_FLOAT & __isub__( Volume_FLOAT &, Volume_U16 & )
  /ReleaseGIL, PyName=__isub_Volume_FLOAT__/;

Volume_FLOAT* __mul__( Volume_FLOAT &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_FLOAT__/;

Volume_FLOAT & __imul__( Volume_FLOAT &, Volume_U16 & )
  /ReleaseGIL, PyName=__imul_Volume_FLOAT__/;

Volume_FLOAT* __div__( Volume_FLOAT &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_FLOAT__/;

Volume_FLOAT & __idiv__( Volume_FLOAT &, Volume_U16 & )
  /Factory, PyName=__idiv_Volume_FLOAT__/;

/*
Volume_FLOAT* __mod__( Volume_FLOAT &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_FLOAT__/;

Volume_FLOAT & __imod__( Volume_FLOAT &, Volume_U16 & )
  /ReleaseGIL, PyName=__imod_Volume_FLOAT__/;
*/

// FLOAT + S32

Volume_FLOAT* __add__( Volume_FLOAT &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_FLOAT__/;

Volume_FLOAT & __iadd__( Volume_FLOAT &, Volume_S32 & )
  /ReleaseGIL, PyName=__iadd_Volume_FLOAT__/;

Volume_FLOAT* __sub__( Volume_FLOAT &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_FLOAT__/;

Volume_FLOAT & __isub__( Volume_FLOAT &, Volume_S32 & )
  /ReleaseGIL, PyName=__isub_Volume_FLOAT__/;

Volume_FLOAT* __mul__( Volume_FLOAT &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_FLOAT__/;

Volume_FLOAT & __imul__( Volume_FLOAT &, Volume_S32 & )
  /ReleaseGIL, PyName=__imul_Volume_FLOAT__/;

Volume_FLOAT* __div__( Volume_FLOAT &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_FLOAT__/;

Volume_FLOAT & __idiv__( Volume_FLOAT &, Volume_S32 & )
  /ReleaseGIL, PyName=__idiv_Volume_FLOAT__/;

/*
Volume_FLOAT* __mod__( Volume_FLOAT &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_FLOAT__/;

Volume_FLOAT & __imod__( Volume_FLOAT &, Volume_S32 & )
  /ReleaseGIL, PyName=__imod_Volume_FLOAT__/;
*/

// FLOAT + U32

Volume_FLOAT* __add__( Volume_FLOAT &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_FLOAT__/;

Volume_FLOAT & __iadd__( Volume_FLOAT &, Volume_U32 & )
  /ReleaseGIL, PyName=__iadd_Volume_FLOAT__/;

Volume_FLOAT* __sub__( Volume_FLOAT &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_FLOAT__/;

Volume_FLOAT & __isub__( Volume_FLOAT &, Volume_U32 & )
  /ReleaseGIL, PyName=__isub_Volume_FLOAT__/;

Volume_FLOAT* __mul__( Volume_FLOAT &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_FLOAT__/;

Volume_FLOAT & __imul__( Volume_FLOAT &, Volume_U32 & )
  /ReleaseGIL, PyName=__imul_Volume_FLOAT__/;

Volume_FLOAT* __div__( Volume_FLOAT &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_FLOAT__/;

Volume_FLOAT & __idiv__( Volume_FLOAT &, Volume_U32 & )
  /ReleaseGIL, PyName=__idiv_Volume_FLOAT__/;

/*
Volume_FLOAT* __mod__( Volume_FLOAT &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__mod_Volume_FLOAT__/;

Volume_FLOAT & __imod__( Volume_FLOAT &, Volume_U32 & )
  /ReleaseGIL, PyName=__imod_Volume_FLOAT__/;
*/

// FLOAT + FLOAT

Volume_FLOAT* __add__( Volume_FLOAT &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__add_Volume_FLOAT__/;

Volume_FLOAT & __iadd__( Volume_FLOAT &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__iadd_Volume_FLOAT__/;

Volume_FLOAT* __sub__( Volume_FLOAT &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_FLOAT__/;

Volume_FLOAT & __isub__( Volume_FLOAT &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__isub_Volume_FLOAT__/;

Volume_FLOAT* __mul__( Volume_FLOAT &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_FLOAT__/;

Volume_FLOAT & __imul__( Volume_FLOAT &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__imul_Volume_FLOAT__/;

Volume_FLOAT* __div__( Volume_FLOAT &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__div_Volume_FLOAT__/;

Volume_FLOAT & __idiv__( Volume_FLOAT &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__idiv_Volume_FLOAT__/;

// FLOAT + DOUBLE

Volume_DOUBLE* __add__( Volume_FLOAT &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__add_Volume_FLOAT__/;

Volume_FLOAT & __iadd__( Volume_FLOAT &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__iadd_Volume_FLOAT__/;

Volume_DOUBLE* __sub__( Volume_FLOAT &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_FLOAT__/;

Volume_FLOAT & __isub__( Volume_FLOAT &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__isub_Volume_FLOAT__/;

Volume_DOUBLE* __mul__( Volume_FLOAT &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_FLOAT__/;

Volume_FLOAT & __imul__( Volume_FLOAT &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__imul_Volume_FLOAT__/;

Volume_DOUBLE* __div__( Volume_FLOAT &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__div_Volume_FLOAT__/;

Volume_FLOAT & __idiv__( Volume_FLOAT &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__idiv_Volume_FLOAT__/;

// FLOAT + RGB/RGBA

Volume_RGB* __add__( Volume_FLOAT &, Volume_RGB & )
  /Factory, ReleaseGIL, PyName=__add_Volume_FLOAT__/;

Volume_RGB* __sub__( Volume_FLOAT &, Volume_RGB & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_FLOAT__/;

Volume_RGB* __mul__( Volume_FLOAT &, Volume_RGB & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_FLOAT__/;

Volume_RGBA* __add__( Volume_FLOAT &, Volume_RGBA & )
  /Factory, ReleaseGIL, PyName=__add_Volume_FLOAT__/;

Volume_RGBA* __sub__( Volume_FLOAT &, Volume_RGBA & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_FLOAT__/;

Volume_RGBA* __mul__( Volume_FLOAT &, Volume_RGBA & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_FLOAT__/;

// FLOAT other

Volume_FLOAT* __add__( Volume_FLOAT &, float )
  /Factory, ReleaseGIL, PyName=__add_Volume_FLOAT__/;
%MethodCode
  float p = (float) a1;
  sipRes = __add__( *a0, p );
%End
Volume_FLOAT* __add__( Volume_FLOAT &, float )
  /Factory, ReleaseGIL, PyName=__radd_Volume_FLOAT__/;
%MethodCode
  float p = (float) a1;
  sipRes = __add__( *a0, p );
%End
/*
Volume_DOUBLE* __add__( Volume_FLOAT &, double )
  /Factory, ReleaseGIL, PyName=__radd_Volume_FLOAT__/;
Volume_DOUBLE* __add__( Volume_FLOAT &, double )
  /Factory, ReleaseGIL, PyName=__add_Volume_FLOAT__/;
*/
Volume_FLOAT & __iadd__( Volume_FLOAT &, double )
  /ReleaseGIL, PyName=__iadd_Volume_FLOAT__/;

Volume_FLOAT* __sub__( Volume_FLOAT &, float )
  /Factory, ReleaseGIL, PyName=__sub_Volume_FLOAT__/;
%MethodCode
  float p = (float) a1;
  sipRes = __sub__( *a0, p );
%End
Volume_FLOAT* __sub__( Volume_FLOAT &, float )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_FLOAT__/;
%MethodCode
  float p = (float) a1;
  sipRes = __sub__( p, *a0 );
%End
/*
Volume_DOUBLE* __sub__( Volume_FLOAT &, double )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_FLOAT__/;
%MethodCode
  sipRes = __sub__( a1, *a0 );
%End
Volume_DOUBLE* __sub__( Volume_FLOAT &, double )
  /Factory, ReleaseGIL, PyName=__sub_Volume_FLOAT__/;
*/
Volume_FLOAT & __isub__( Volume_FLOAT &, double )
  /ReleaseGIL, PyName=__isub_Volume_FLOAT__/;

Volume_FLOAT* __mul__( Volume_FLOAT &, float )
  /Factory, ReleaseGIL, PyName=__mul_Volume_FLOAT__/;
%MethodCode
  float p = (float) a1;
  sipRes = __mul__( *a0, p );
%End
Volume_FLOAT* __mul__( Volume_FLOAT &, float )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_FLOAT__/;
%MethodCode
  float p = (float) a1;
  sipRes = __mul__( *a0, p );
%End
/*
Volume_DOUBLE* __mul__( Volume_FLOAT &, double )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_FLOAT__/;
Volume_DOUBLE* __mul__( Volume_FLOAT &, double )
  /Factory, ReleaseGIL, PyName=__mul_Volume_FLOAT__/;
*/
Volume_FLOAT & __imul__( Volume_FLOAT &, double )
  /ReleaseGIL, PyName=__imul_Volume_FLOAT__/;

Volume_FLOAT* __div__( Volume_FLOAT &, float )
  /Factory, ReleaseGIL, PyName=__div_Volume_FLOAT__/;
%MethodCode
  float p = (float) a1;
  sipRes = __div__( *a0, p );
%End
Volume_FLOAT* __div__( Volume_FLOAT &, float )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_FLOAT__/;
%MethodCode
  float p = (float) a1;
  sipRes = __div__( p, *a0 );
%End
/*
Volume_DOUBLE* __div__( Volume_FLOAT &, double )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_FLOAT__/;
%MethodCode
  sipRes = __div__( a1, *a0 );
%End
Volume_DOUBLE* __div__( Volume_FLOAT &, double )
  /Factory, ReleaseGIL, PyName=__div_Volume_FLOAT__/;
*/
Volume_FLOAT & __idiv__( Volume_FLOAT &, double )
  /ReleaseGIL, PyName=__idiv_Volume_FLOAT__/;

/* mod exists in python on float, but is not implemented on Volume un C++
Volume_FLOAT* __mod__( Volume_FLOAT &, float )
  /Factory, ReleaseGIL, PyName=__mod_Volume_FLOAT__/;
%MethodCode
  float p = (float) a1;
  sipRes = __mod__( *a0, p );
%End
Volume_FLOAT* __mod__( Volume_FLOAT &, float )
  /Factory, ReleaseGIL, PyName=__rmod_Volume_FLOAT__/;
%MethodCode
  float p = (float) a1;
  sipRes = __mod__( p, *a0 );
%End
Volume_FLOAT & __imod__( Volume_FLOAT &, float )
  /ReleaseGIL, PyName=__imod_Volume_FLOAT__/;
*/

Volume_RGB* __add__( Volume_FLOAT &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__radd_Volume_FLOAT__/;
Volume_RGB* __add__( Volume_FLOAT &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__add_Volume_FLOAT__/;
Volume_RGBA* __add__( Volume_FLOAT &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__radd_Volume_FLOAT__/;
Volume_RGBA* __add__( Volume_FLOAT &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__add_Volume_FLOAT__/;
Volume_RGB* __sub__( Volume_FLOAT &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_FLOAT__/;
Volume_RGB* __sub__( Volume_FLOAT &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_FLOAT__/;
%MethodCode
  sipRes = __sub__( *a1, *a0 );
%End
Volume_RGBA* __sub__( Volume_FLOAT &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_FLOAT__/;
Volume_RGBA* __sub__( Volume_FLOAT &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_FLOAT__/;
%MethodCode
  sipRes = __sub__( *a1, *a0 );
%End
Volume_RGB* __mul__( Volume_FLOAT &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_FLOAT__/;
Volume_RGB* __mul__( Volume_FLOAT &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_FLOAT__/;
Volume_RGBA* __mul__( Volume_FLOAT &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_FLOAT__/;
Volume_RGBA* __mul__( Volume_FLOAT &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_FLOAT__/;
Volume_RGB* __div__( Volume_FLOAT &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_FLOAT__/;
%MethodCode
  sipRes = __div__( *a1, *a0 );
%End
Volume_RGBA* __div__( Volume_FLOAT &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_FLOAT__/;
%MethodCode
  sipRes = __div__( *a1, *a0 );
%End


// --- DOUBLE

// DOUBLE + U8

Volume_DOUBLE* __add__( Volume_DOUBLE &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_DOUBLE__/;

Volume_DOUBLE & __iadd__( Volume_DOUBLE &, Volume_U8 & )
  /ReleaseGIL, PyName=__iadd_Volume_DOUBLE__/;

Volume_DOUBLE* __sub__( Volume_DOUBLE &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_DOUBLE__/;

Volume_DOUBLE & __isub__( Volume_DOUBLE &, Volume_U8 & )
  /ReleaseGIL, PyName=__isub_Volume_DOUBLE__/;

Volume_DOUBLE* __mul__( Volume_DOUBLE &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_DOUBLE__/;

Volume_DOUBLE & __imul__( Volume_DOUBLE &, Volume_U8 & )
  /ReleaseGIL, PyName=__imul_Volume_DOUBLE__/;

Volume_DOUBLE* __div__( Volume_DOUBLE &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_DOUBLE__/;

Volume_DOUBLE & __idiv__( Volume_DOUBLE &, Volume_U8 & )
  /ReleaseGIL, PyName=__idiv_Volume_DOUBLE__/;

// DOUBLE + S16

Volume_DOUBLE* __add__( Volume_DOUBLE &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_DOUBLE__/;

Volume_DOUBLE & __iadd__( Volume_DOUBLE &, Volume_S16 & )
  /ReleaseGIL, PyName=__iadd_Volume_DOUBLE__/;

Volume_DOUBLE* __sub__( Volume_DOUBLE &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_DOUBLE__/;

Volume_DOUBLE & __isub__( Volume_DOUBLE &, Volume_S16 & )
  /ReleaseGIL, PyName=__isub_Volume_DOUBLE__/;

Volume_DOUBLE* __mul__( Volume_DOUBLE &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_DOUBLE__/;

Volume_DOUBLE & __imul__( Volume_DOUBLE &, Volume_S16 & )
  /ReleaseGIL, PyName=__imul_Volume_DOUBLE__/;

Volume_DOUBLE* __div__( Volume_DOUBLE &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_DOUBLE__/;

Volume_DOUBLE & __idiv__( Volume_DOUBLE &, Volume_S16 & )
  /ReleaseGIL, PyName=__idiv_Volume_DOUBLE__/;

// DOUBLE + U16

Volume_DOUBLE* __add__( Volume_DOUBLE &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_DOUBLE__/;

Volume_DOUBLE & __iadd__( Volume_DOUBLE &, Volume_U16 & )
  /ReleaseGIL, PyName=__iadd_Volume_DOUBLE__/;

Volume_DOUBLE* __sub__( Volume_DOUBLE &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_DOUBLE__/;

Volume_DOUBLE & __isub__( Volume_DOUBLE &, Volume_U16 & )
  /ReleaseGIL, PyName=__isub_Volume_DOUBLE__/;

Volume_DOUBLE* __mul__( Volume_DOUBLE &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_DOUBLE__/;

Volume_DOUBLE & __imul__( Volume_DOUBLE &, Volume_U16 & )
  /ReleaseGIL, PyName=__imul_Volume_DOUBLE__/;

Volume_DOUBLE* __div__( Volume_DOUBLE &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_DOUBLE__/;

Volume_DOUBLE & __idiv__( Volume_DOUBLE &, Volume_U16 & )
  /Factory, PyName=__idiv_Volume_DOUBLE__/;

// DOUBLE + S32

Volume_DOUBLE* __add__( Volume_DOUBLE &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_DOUBLE__/;

Volume_DOUBLE & __iadd__( Volume_DOUBLE &, Volume_S32 & )
  /ReleaseGIL, PyName=__iadd_Volume_DOUBLE__/;

Volume_DOUBLE* __sub__( Volume_DOUBLE &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_DOUBLE__/;

Volume_DOUBLE & __isub__( Volume_DOUBLE &, Volume_S32 & )
  /ReleaseGIL, PyName=__isub_Volume_DOUBLE__/;

Volume_DOUBLE* __mul__( Volume_DOUBLE &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_DOUBLE__/;

Volume_DOUBLE & __imul__( Volume_DOUBLE &, Volume_S32 & )
  /ReleaseGIL, PyName=__imul_Volume_DOUBLE__/;

Volume_DOUBLE* __div__( Volume_DOUBLE &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_DOUBLE__/;

Volume_DOUBLE & __idiv__( Volume_DOUBLE &, Volume_S32 & )
  /ReleaseGIL, PyName=__idiv_Volume_DOUBLE__/;

// DOUBLE + U32

Volume_DOUBLE* __add__( Volume_DOUBLE &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_DOUBLE__/;

Volume_DOUBLE & __iadd__( Volume_DOUBLE &, Volume_U32 & )
  /ReleaseGIL, PyName=__iadd_Volume_DOUBLE__/;

Volume_DOUBLE* __sub__( Volume_DOUBLE &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_DOUBLE__/;

Volume_DOUBLE & __isub__( Volume_DOUBLE &, Volume_U32 & )
  /ReleaseGIL, PyName=__isub_Volume_DOUBLE__/;

Volume_DOUBLE* __mul__( Volume_DOUBLE &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_DOUBLE__/;

Volume_DOUBLE & __imul__( Volume_DOUBLE &, Volume_U32 & )
  /ReleaseGIL, PyName=__imul_Volume_DOUBLE__/;

Volume_DOUBLE* __div__( Volume_DOUBLE &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_DOUBLE__/;

Volume_DOUBLE & __idiv__( Volume_DOUBLE &, Volume_U32 & )
  /ReleaseGIL, PyName=__idiv_Volume_DOUBLE__/;

// DOUBLE + FLOAT

Volume_DOUBLE* __add__( Volume_DOUBLE &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__add_Volume_DOUBLE__/;

Volume_DOUBLE & __iadd__( Volume_DOUBLE &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__iadd_Volume_DOUBLE__/;

Volume_DOUBLE* __sub__( Volume_DOUBLE &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_DOUBLE__/;

Volume_DOUBLE & __isub__( Volume_DOUBLE &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__isub_Volume_DOUBLE__/;

Volume_DOUBLE* __mul__( Volume_DOUBLE &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_DOUBLE__/;

Volume_DOUBLE & __imul__( Volume_DOUBLE &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__imul_Volume_DOUBLE__/;

Volume_DOUBLE* __div__( Volume_DOUBLE &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__div_Volume_DOUBLE__/;

Volume_DOUBLE & __idiv__( Volume_DOUBLE &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__idiv_Volume_DOUBLE__/;

// DOUBLE + DOUBLE

Volume_DOUBLE* __add__( Volume_DOUBLE &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__add_Volume_DOUBLE__/;

Volume_DOUBLE & __iadd__( Volume_DOUBLE &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__iadd_Volume_DOUBLE__/;

Volume_DOUBLE* __sub__( Volume_DOUBLE &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_DOUBLE__/;

Volume_DOUBLE & __isub__( Volume_DOUBLE &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__isub_Volume_DOUBLE__/;

Volume_DOUBLE* __mul__( Volume_DOUBLE &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_DOUBLE__/;

Volume_DOUBLE & __imul__( Volume_DOUBLE &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__imul_Volume_DOUBLE__/;

Volume_DOUBLE* __div__( Volume_DOUBLE &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__div_Volume_DOUBLE__/;

Volume_DOUBLE & __idiv__( Volume_DOUBLE &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__idiv_Volume_DOUBLE__/;

// DOUBLE + RGB/RGBA

Volume_RGB* __add__( Volume_DOUBLE &, Volume_RGB & )
  /Factory, ReleaseGIL, PyName=__add_Volume_DOUBLE__/;

Volume_RGB* __sub__( Volume_DOUBLE &, Volume_RGB & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_DOUBLE__/;

Volume_RGB* __mul__( Volume_DOUBLE &, Volume_RGB & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_DOUBLE__/;

Volume_RGBA* __add__( Volume_DOUBLE &, Volume_RGBA & )
  /Factory, ReleaseGIL, PyName=__add_Volume_DOUBLE__/;

Volume_RGBA* __sub__( Volume_DOUBLE &, Volume_RGBA & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_DOUBLE__/;

Volume_RGBA* __mul__( Volume_DOUBLE &, Volume_RGBA & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_DOUBLE__/;

// DOUBLE other

Volume_DOUBLE* __add__( Volume_DOUBLE &, double )
  /Factory, ReleaseGIL, PyName=__radd_Volume_DOUBLE__/;
Volume_DOUBLE* __add__( Volume_DOUBLE &, double )
  /Factory, ReleaseGIL, PyName=__add_Volume_DOUBLE__/;
Volume_DOUBLE & __iadd__( Volume_DOUBLE &, double )
  /ReleaseGIL, PyName=__iadd_Volume_DOUBLE__/;

Volume_DOUBLE* __sub__( Volume_DOUBLE &, double )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_DOUBLE__/;
%MethodCode
  sipRes = __sub__( a1, *a0 );
%End
Volume_DOUBLE* __sub__( Volume_DOUBLE &, double )
  /Factory, ReleaseGIL, PyName=__sub_Volume_DOUBLE__/;
Volume_DOUBLE & __isub__( Volume_DOUBLE &, double )
  /ReleaseGIL, PyName=__isub_Volume_DOUBLE__/;

Volume_DOUBLE* __mul__( Volume_DOUBLE &, double )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_DOUBLE__/;
Volume_DOUBLE* __mul__( Volume_DOUBLE &, double )
  /Factory, ReleaseGIL, PyName=__mul_Volume_DOUBLE__/;
Volume_DOUBLE & __imul__( Volume_DOUBLE &, double )
  /ReleaseGIL, PyName=__imul_Volume_DOUBLE__/;

Volume_DOUBLE* __div__( Volume_DOUBLE &, double )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_DOUBLE__/;
%MethodCode
  sipRes = __div__( a1, *a0 );
%End
Volume_DOUBLE* __div__( Volume_DOUBLE &, double )
  /Factory, ReleaseGIL, PyName=__div_Volume_DOUBLE__/;
Volume_DOUBLE & __idiv__( Volume_DOUBLE &, double )
  /ReleaseGIL, PyName=__idiv_Volume_DOUBLE__/;

Volume_RGB* __add__( Volume_DOUBLE &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__radd_Volume_DOUBLE__/;
Volume_RGB* __add__( Volume_DOUBLE &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__add_Volume_DOUBLE__/;
Volume_RGBA* __add__( Volume_DOUBLE &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__radd_Volume_DOUBLE__/;
Volume_RGBA* __add__( Volume_DOUBLE &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__add_Volume_DOUBLE__/;
Volume_RGB* __sub__( Volume_DOUBLE &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_DOUBLE__/;
Volume_RGB* __sub__( Volume_DOUBLE &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_DOUBLE__/;
%MethodCode
  sipRes = __sub__( *a1, *a0 );
%End
Volume_RGBA* __sub__( Volume_DOUBLE &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_DOUBLE__/;
Volume_RGBA* __sub__( Volume_DOUBLE &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_DOUBLE__/;
%MethodCode
  sipRes = __sub__( *a1, *a0 );
%End
Volume_RGB* __mul__( Volume_DOUBLE &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_DOUBLE__/;
Volume_RGB* __mul__( Volume_DOUBLE &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_DOUBLE__/;
Volume_RGBA* __mul__( Volume_DOUBLE &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_DOUBLE__/;
Volume_RGBA* __mul__( Volume_DOUBLE &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_DOUBLE__/;
Volume_RGB* __div__( Volume_DOUBLE &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_DOUBLE__/;
%MethodCode
  sipRes = __div__( *a1, *a0 );
%End
Volume_RGBA* __div__( Volume_DOUBLE &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_DOUBLE__/;
%MethodCode
  sipRes = __div__( *a1, *a0 );
%End


// --- POINT3DF

// POINT3DF + U8

Volume_POINT3DF* __mul__( Volume_POINT3DF &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_POINT3DF__/;

Volume_POINT3DF & __imul__( Volume_POINT3DF &, Volume_U8 & )
  /ReleaseGIL, PyName=__imul_Volume_POINT3DF__/;

Volume_POINT3DF* __mul__( Volume_POINT3DF &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_POINT3DF__/;

Volume_POINT3DF* __div__( Volume_POINT3DF &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_POINT3DF__/;

Volume_POINT3DF & __idiv__( Volume_POINT3DF &, Volume_U8 & )
  /ReleaseGIL, PyName=__idiv_Volume_POINT3DF__/;

// POINT3DF + S16

Volume_POINT3DF* __mul__( Volume_POINT3DF &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_POINT3DF__/;

Volume_POINT3DF & __imul__( Volume_POINT3DF &, Volume_S16 & )
  /ReleaseGIL, PyName=__imul_Volume_POINT3DF__/;

Volume_POINT3DF* __mul__( Volume_POINT3DF &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_POINT3DF__/;

Volume_POINT3DF* __div__( Volume_POINT3DF &, Volume_S16 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_POINT3DF__/;

Volume_POINT3DF & __idiv__( Volume_POINT3DF &, Volume_S16 & )
  /ReleaseGIL, PyName=__idiv_Volume_POINT3DF__/;

// POINT3DF + U16

Volume_POINT3DF* __mul__( Volume_POINT3DF &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_POINT3DF__/;

Volume_POINT3DF & __imul__( Volume_POINT3DF &, Volume_U16 & )
  /ReleaseGIL, PyName=__imul_Volume_POINT3DF__/;

Volume_POINT3DF* __mul__( Volume_POINT3DF &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_POINT3DF__/;

Volume_POINT3DF* __div__( Volume_POINT3DF &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_POINT3DF__/;

Volume_POINT3DF & __idiv__( Volume_POINT3DF &, Volume_U16 & )
  /ReleaseGIL, PyName=__idiv_Volume_POINT3DF__/;

// POINT3DF + S32

Volume_POINT3DF* __mul__( Volume_POINT3DF &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_POINT3DF__/;

Volume_POINT3DF & __imul__( Volume_POINT3DF &, Volume_S32 & )
  /ReleaseGIL, PyName=__imul_Volume_POINT3DF__/;

Volume_POINT3DF* __mul__( Volume_POINT3DF &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_POINT3DF__/;

Volume_POINT3DF* __div__( Volume_POINT3DF &, Volume_S32 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_POINT3DF__/;

Volume_POINT3DF & __idiv__( Volume_POINT3DF &, Volume_S32 & )
  /ReleaseGIL, PyName=__idiv_Volume_POINT3DF__/;

// POINT3DF + U32

Volume_POINT3DF* __mul__( Volume_POINT3DF &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_POINT3DF__/;

Volume_POINT3DF & __imul__( Volume_POINT3DF &, Volume_U32 & )
  /ReleaseGIL, PyName=__imul_Volume_POINT3DF__/;

Volume_POINT3DF* __mul__( Volume_POINT3DF &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_POINT3DF__/;

Volume_POINT3DF* __div__( Volume_POINT3DF &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_POINT3DF__/;

Volume_POINT3DF & __idiv__( Volume_POINT3DF &, Volume_U32 & )
  /ReleaseGIL, PyName=__idiv_Volume_POINT3DF__/;

// POINT3DF + FLOAT

Volume_POINT3DF* __mul__( Volume_POINT3DF &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_POINT3DF__/;

Volume_POINT3DF & __imul__( Volume_POINT3DF &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__imul_Volume_POINT3DF__/;

Volume_POINT3DF* __mul__( Volume_POINT3DF &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_POINT3DF__/;

Volume_POINT3DF* __div__( Volume_POINT3DF &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__div_Volume_POINT3DF__/;

Volume_POINT3DF & __idiv__( Volume_POINT3DF &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__idiv_Volume_POINT3DF__/;

// POINT3DF + DOUBLE

Volume_POINT3DF* __mul__( Volume_POINT3DF &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_POINT3DF__/;

Volume_POINT3DF & __imul__( Volume_POINT3DF &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__imul_Volume_POINT3DF__/;

Volume_POINT3DF* __mul__( Volume_POINT3DF &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_POINT3DF__/;

Volume_POINT3DF* __div__( Volume_POINT3DF &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__div_Volume_POINT3DF__/;

Volume_POINT3DF & __idiv__( Volume_POINT3DF &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__idiv_Volume_POINT3DF__/;

// POINT3DF + POINT3DF

Volume_POINT3DF* __add__( Volume_POINT3DF &, Volume_POINT3DF & )
  /Factory, ReleaseGIL, PyName=__add_Volume_POINT3DF__/;

Volume_POINT3DF & __iadd__( Volume_POINT3DF &, Volume_POINT3DF & )
  /ReleaseGIL, PyName=__iadd_Volume_POINT3DF__/;

Volume_POINT3DF* __sub__( Volume_POINT3DF &, Volume_POINT3DF & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_POINT3DF__/;

Volume_POINT3DF & __isub__( Volume_POINT3DF &, Volume_POINT3DF & )
  /ReleaseGIL, PyName=__isub_Volume_POINT3DF__/;

// POINT3DF other

Volume_POINT3DF* __add__( Volume_POINT3DF &, Point3df & )
  /Factory, ReleaseGIL, PyName=__radd_Volume_POINT3DF__/;
Volume_POINT3DF* __add__( Volume_POINT3DF &, Point3df & )
  /Factory, ReleaseGIL, PyName=__add_Volume_POINT3DF__/;
Volume_POINT3DF & __iadd__( Volume_POINT3DF &, Point3df & )
  /ReleaseGIL, PyName=__iadd_Volume_POINT3DF__/;

Volume_POINT3DF* __sub__( Volume_POINT3DF &, Point3df & )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_POINT3DF__/;
%MethodCode
  sipRes = __sub__( *a1, *a0 );
%End
Volume_POINT3DF* __sub__( Volume_POINT3DF &, Point3df & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_POINT3DF__/;
Volume_POINT3DF & __isub__( Volume_POINT3DF &, Point3df & )
  /ReleaseGIL, PyName=__isub_Volume_POINT3DF__/;

Volume_POINT3DF* __mul__( Volume_POINT3DF &, float )
  /Factory, ReleaseGIL, PyName=__mul_Volume_POINT3DF__/;
Volume_POINT3DF & __imul__( Volume_POINT3DF &, float )
  /Factory, ReleaseGIL, PyName=__imul_Volume_POINT3DF__/;
Volume_POINT3DF* __mul__( Volume_POINT3DF &, float )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_POINT3DF__/;

Volume_POINT3DF* __div__( Volume_POINT3DF &, float )
  /Factory, ReleaseGIL, PyName=__div_Volume_POINT3DF__/;
Volume_POINT3DF & __idiv__( Volume_POINT3DF &, float )
  /Factory, ReleaseGIL, PyName=__idiv_Volume_POINT3DF__/;
/*
Volume_POINT3DF* __div__( Volume_POINT3DF &, float )
  /Factory, ReleaseGIL, PyName=__rdiv_Volume_POINT3DF__/;
%MethodCode
  sipRes = __div__( a1, *a0 );
%End
*/

// --- RGB

// RGB + U8

Volume_RGB* __add__( Volume_RGB &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_RGB__/;

Volume_RGB & __iadd__( Volume_RGB &, Volume_U8 & )
  /ReleaseGIL, PyName=__iadd_Volume_RGB__/;

Volume_RGB* __sub__( Volume_RGB &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_RGB__/;

Volume_RGB & __isub__( Volume_RGB &, Volume_U8 & )
  /ReleaseGIL, PyName=__isub_Volume_RGB__/;

Volume_RGB* __mul__( Volume_RGB &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_RGB__/;

Volume_RGB & __imul__( Volume_RGB &, Volume_U8 & )
  /ReleaseGIL, PyName=__imul_Volume_RGB__/;

Volume_RGB* __div__( Volume_RGB &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_RGB__/;

Volume_RGB & __idiv__( Volume_RGB &, Volume_U8 & )
  /ReleaseGIL, PyName=__idiv_Volume_RGB__/;

// RGB + U16

Volume_RGB* __add__( Volume_RGB &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_RGB__/;

Volume_RGB & __iadd__( Volume_RGB &, Volume_U16 & )
  /ReleaseGIL, PyName=__iadd_Volume_RGB__/;

Volume_RGB* __sub__( Volume_RGB &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_RGB__/;

Volume_RGB & __isub__( Volume_RGB &, Volume_U16 & )
  /ReleaseGIL, PyName=__isub_Volume_RGB__/;

Volume_RGB* __mul__( Volume_RGB &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_RGB__/;

Volume_RGB & __imul__( Volume_RGB &, Volume_U16 & )
  /ReleaseGIL, PyName=__imul_Volume_RGB__/;

Volume_RGB* __div__( Volume_RGB &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_RGB__/;

Volume_RGB & __idiv__( Volume_RGB &, Volume_U16 & )
  /Factory, PyName=__idiv_Volume_RGB__/;

// RGB + U32

Volume_RGB* __add__( Volume_RGB &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_RGB__/;

Volume_RGB & __iadd__( Volume_RGB &, Volume_U32 & )
  /ReleaseGIL, PyName=__iadd_Volume_RGB__/;

Volume_RGB* __sub__( Volume_RGB &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_RGB__/;

Volume_RGB & __isub__( Volume_RGB, Volume_U32 & )
  /ReleaseGIL, PyName=__isub_Volume_RGB__/;

Volume_RGB* __mul__( Volume_RGB &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_RGB__/;

Volume_RGB & __imul__( Volume_RGB &, Volume_U32 & )
  /ReleaseGIL, PyName=__imul_Volume_RGB__/;

Volume_RGB* __div__( Volume_RGB &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_RGB__/;

Volume_RGB & __idiv__( Volume_RGB &, Volume_U32 & )
  /ReleaseGIL, PyName=__idiv_Volume_RGB__/;

// RGB + FLOAT

Volume_RGB* __add__( Volume_RGB &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__add_Volume_RGB__/;

Volume_RGB & __iadd__( Volume_RGB &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__iadd_Volume_RGB__/;

Volume_RGB* __sub__( Volume_RGB &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_RGB__/;

Volume_RGB & __isub__( Volume_RGB &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__isub_Volume_RGB__/;

Volume_RGB* __mul__( Volume_RGB &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_RGB__/;

Volume_RGB & __imul__( Volume_RGB &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__imul_Volume_RGB__/;

Volume_RGB* __div__( Volume_RGB &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__div_Volume_RGB__/;

Volume_RGB & __idiv__( Volume_RGB &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__idiv_Volume_RGB__/;

// RGB + DOUBLE

Volume_RGB* __add__( Volume_RGB &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__add_Volume_RGB__/;

Volume_RGB & __iadd__( Volume_RGB &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__iadd_Volume_RGB__/;

Volume_RGB* __sub__( Volume_RGB &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_RGB__/;

Volume_RGB & __isub__( Volume_RGB &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__isub_Volume_RGB__/;

Volume_RGB* __mul__( Volume_RGB &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_RGB__/;

Volume_RGB & __imul__( Volume_RGB &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__imul_Volume_RGB__/;

Volume_RGB* __div__( Volume_RGB &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__div_Volume_RGB__/;

Volume_RGB & __idiv__( Volume_RGB &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__idiv_Volume_RGB__/;

// RGB + RGB

Volume_RGB* __add__( Volume_RGB &, Volume_RGB & )
  /Factory, ReleaseGIL, PyName=__add_Volume_RGB__/;

Volume_RGB & __iadd__( Volume_RGB &, Volume_RGB & )
  /ReleaseGIL, PyName=__iadd_Volume_RGB__/;

Volume_RGB* __sub__( Volume_RGB &, Volume_RGB & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_RGB__/;

Volume_RGB & __isub__( Volume_RGB &, Volume_RGB & )
  /ReleaseGIL, PyName=__isub_Volume_RGB__/;

// RGB other

Volume_RGB* __add__( Volume_RGB &, double )
  /Factory, ReleaseGIL, PyName=__add_Volume_RGB__/;
Volume_RGB* __add__( Volume_RGB &, double )
  /Factory, ReleaseGIL, PyName=__radd_Volume_RGB__/;
Volume_RGB & __iadd__( Volume_RGB &, double )
  /ReleaseGIL, PyName=__iadd_Volume_RGB__/;

Volume_RGB* __add__( Volume_RGB &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__add_Volume_RGB__/;
Volume_RGB* __add__( Volume_RGB &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__radd_Volume_RGB__/;
Volume_RGB & __iadd__( Volume_RGB &, AimsRGB & )
  /ReleaseGIL, PyName=__iadd_Volume_RGB__/;

Volume_RGB* __sub__( Volume_RGB &, double )
  /Factory, ReleaseGIL, PyName=__sub_Volume_RGB__/;
Volume_RGB* __sub__( Volume_RGB &, double )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_RGB__/;
%MethodCode
  sipRes = __sub__( a1, *a0 );
%End
Volume_RGB & __isub__( Volume_RGB &, double )
  /ReleaseGIL, PyName=__isub_Volume_RGB__/;

Volume_RGB* __sub__( Volume_RGB &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_RGB__/;
Volume_RGB* __sub__( Volume_RGB &, AimsRGB & )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_RGB__/;
%MethodCode
  sipRes = __sub__( *a1, *a0 );
%End
Volume_RGB & __isub__( Volume_RGB &, AimsRGB & )
  /ReleaseGIL, PyName=__isub_Volume_RGB__/;

Volume_RGB* __mul__( Volume_RGB &, double )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_RGB__/;
Volume_RGB* __mul__( Volume_RGB &, double )
  /Factory, ReleaseGIL, PyName=__mul_Volume_RGB__/;
Volume_RGB & __imul__( Volume_RGB &, double )
  /ReleaseGIL, PyName=__imul_Volume_RGB__/;

Volume_RGB* __div__( Volume_RGB &, double )
  /Factory, ReleaseGIL, PyName=__div_Volume_RGB__/;
Volume_RGB & __idiv__( Volume_RGB &, double )
  /ReleaseGIL, PyName=__idiv_Volume_RGB__/;


// --- RGBA

// RGBA + U8

Volume_RGBA* __add__( Volume_RGBA &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_RGBA__/;

Volume_RGBA & __iadd__( Volume_RGBA &, Volume_U8 & )
  /ReleaseGIL, PyName=__iadd_Volume_RGBA__/;

Volume_RGBA* __sub__( Volume_RGBA &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_RGBA__/;

Volume_RGBA & __isub__( Volume_RGBA &, Volume_U8 & )
  /ReleaseGIL, PyName=__isub_Volume_RGBA__/;

Volume_RGBA* __mul__( Volume_RGBA &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_RGBA__/;

Volume_RGBA & __imul__( Volume_RGBA &, Volume_U8 & )
  /ReleaseGIL, PyName=__imul_Volume_RGBA__/;

Volume_RGBA* __div__( Volume_RGBA &, Volume_U8 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_RGBA__/;

Volume_RGBA & __idiv__( Volume_RGBA &, Volume_U8 & )
  /ReleaseGIL, PyName=__idiv_Volume_RGBA__/;

// RGBA + U16

Volume_RGBA* __add__( Volume_RGBA &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_RGBA__/;

Volume_RGBA & __iadd__( Volume_RGBA &, Volume_U16 & )
  /ReleaseGIL, PyName=__iadd_Volume_RGBA__/;

Volume_RGBA* __sub__( Volume_RGBA &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_RGBA__/;

Volume_RGBA & __isub__( Volume_RGBA &, Volume_U16 & )
  /ReleaseGIL, PyName=__isub_Volume_RGBA__/;

Volume_RGBA* __mul__( Volume_RGBA &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_RGBA__/;

Volume_RGBA & __imul__( Volume_RGBA &, Volume_U16 & )
  /ReleaseGIL, PyName=__imul_Volume_RGBA__/;

Volume_RGBA* __div__( Volume_RGBA &, Volume_U16 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_RGBA__/;

Volume_RGBA & __idiv__( Volume_RGBA &, Volume_U16 & )
  /Factory, PyName=__idiv_Volume_RGBA__/;

// RGBA + U32

Volume_RGBA* __add__( Volume_RGBA &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__add_Volume_RGBA__/;

Volume_RGBA & __iadd__( Volume_RGBA &, Volume_U32 & )
  /ReleaseGIL, PyName=__iadd_Volume_RGBA__/;

Volume_RGBA* __sub__( Volume_RGBA &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_RGBA__/;

Volume_RGBA & __isub__( Volume_RGBA, Volume_U32 & )
  /ReleaseGIL, PyName=__isub_Volume_RGBA__/;

Volume_RGBA* __mul__( Volume_RGBA &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_RGBA__/;

Volume_RGBA & __imul__( Volume_RGBA &, Volume_U32 & )
  /ReleaseGIL, PyName=__imul_Volume_RGBA__/;

Volume_RGBA* __div__( Volume_RGBA &, Volume_U32 & )
  /Factory, ReleaseGIL, PyName=__div_Volume_RGBA__/;

Volume_RGBA & __idiv__( Volume_RGBA &, Volume_U32 & )
  /ReleaseGIL, PyName=__idiv_Volume_RGBA__/;

// RGBA + FLOAT

Volume_RGBA* __add__( Volume_RGBA &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__add_Volume_RGBA__/;

Volume_RGBA & __iadd__( Volume_RGBA &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__iadd_Volume_RGBA__/;

Volume_RGBA* __sub__( Volume_RGBA &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_RGBA__/;

Volume_RGBA & __isub__( Volume_RGBA &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__isub_Volume_RGBA__/;

Volume_RGBA* __mul__( Volume_RGBA &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_RGBA__/;

Volume_RGBA & __imul__( Volume_RGBA &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__imul_Volume_RGBA__/;

Volume_RGBA* __div__( Volume_RGBA &, Volume_FLOAT & )
  /Factory, ReleaseGIL, PyName=__div_Volume_RGBA__/;

Volume_RGBA & __idiv__( Volume_RGBA &, Volume_FLOAT & )
  /ReleaseGIL, PyName=__idiv_Volume_RGBA__/;

// RGBA + DOUBLE

Volume_RGBA* __add__( Volume_RGBA &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__add_Volume_RGBA__/;

Volume_RGBA & __iadd__( Volume_RGBA &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__iadd_Volume_RGBA__/;

Volume_RGBA* __sub__( Volume_RGBA &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_RGBA__/;

Volume_RGBA & __isub__( Volume_RGBA &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__isub_Volume_RGBA__/;

Volume_RGBA* __mul__( Volume_RGBA &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__mul_Volume_RGBA__/;

Volume_RGBA & __imul__( Volume_RGBA &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__imul_Volume_RGAB__/;

Volume_RGBA* __div__( Volume_RGBA &, Volume_DOUBLE & )
  /Factory, ReleaseGIL, PyName=__div_Volume_RGBA__/;

Volume_RGBA & __idiv__( Volume_RGBA &, Volume_DOUBLE & )
  /ReleaseGIL, PyName=__idiv_Volume_RGBA__/;

// RGBA + RGBA

Volume_RGBA* __add__( Volume_RGBA &, Volume_RGBA & )
  /Factory, ReleaseGIL, PyName=__add_Volume_RGBA__/;

Volume_RGBA & __iadd__( Volume_RGBA &, Volume_RGBA & )
  /ReleaseGIL, PyName=__iadd_Volume_RGBA__/;

Volume_RGBA* __sub__( Volume_RGBA &, Volume_RGBA & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_RGBA__/;

Volume_RGBA & __isub__( Volume_RGBA &, Volume_RGBA & )
  /ReleaseGIL, PyName=__isub_Volume_RGBA__/;

// RGBA other

Volume_RGBA* __add__( Volume_RGBA &, double )
  /Factory, ReleaseGIL, PyName=__add_Volume_RGBA__/;
Volume_RGBA* __add__( Volume_RGBA &, double )
  /Factory, ReleaseGIL, PyName=__radd_Volume_RGBA__/;
Volume_RGBA & __iadd__( Volume_RGBA &, double )
  /ReleaseGIL, PyName=__iadd_Volume_RGBA__/;

Volume_RGBA* __add__( Volume_RGBA &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__add_Volume_RGBA__/;
Volume_RGBA* __add__( Volume_RGBA &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__radd_Volume_RGBA__/;
Volume_RGBA & __iadd__( Volume_RGBA &, AimsRGBA & )
  /ReleaseGIL, PyName=__iadd_Volume_RGBA__/;

Volume_RGBA* __sub__( Volume_RGBA &, double )
  /Factory, ReleaseGIL, PyName=__sub_Volume_RGBA__/;
Volume_RGBA* __sub__( Volume_RGBA &, double )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_RGBA__/;
%MethodCode
  sipRes = __sub__( a1, *a0 );
%End
Volume_RGBA & __isub__( Volume_RGBA &, double )
  /ReleaseGIL, PyName=__isub_Volume_RGBA__/;

Volume_RGBA* __sub__( Volume_RGBA &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__sub_Volume_RGBA__/;
Volume_RGBA* __sub__( Volume_RGBA &, AimsRGBA & )
  /Factory, ReleaseGIL, PyName=__rsub_Volume_RGBA__/;
%MethodCode
  sipRes = __sub__( *a1, *a0 );
%End
Volume_RGBA & __isub__( Volume_RGBA &, AimsRGBA & )
  /ReleaseGIL, PyName=__isub_Volume_RGBA__/;

Volume_RGBA* __mul__( Volume_RGBA &, double )
  /Factory, ReleaseGIL, PyName=__rmul_Volume_RGBA__/;
Volume_RGBA* __mul__( Volume_RGBA &, double )
  /Factory, ReleaseGIL, PyName=__mul_Volume_RGBA__/;
Volume_RGBA & __imul__( Volume_RGBA &, double )
  /ReleaseGIL, PyName=__imul_Volume_RGBA__/;

Volume_RGBA* __div__( Volume_RGBA &, double )
  /Factory, ReleaseGIL, PyName=__div_Volume_RGBA__/;
Volume_RGBA & __idiv__( Volume_RGBA &, double )
  /ReleaseGIL, PyName=__idiv_Volume_RGBA__/;

