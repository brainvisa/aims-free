/*
 *  Not standard RGB/RGBA norm based operators
 */
#ifndef AIMS_RGB_RGBNORM_H
#define AIMS_RGB_RGBNORM_H

#include <aims/rgb/rgb.h>

double norm (const AimsRGB &aa);
double norm (const AimsRGBA &aa);

int operator < (const AimsRGB &aa, const AimsRGB &bb);
int operator <= (const AimsRGB &aa, const AimsRGB &bb);
int operator > (const AimsRGB &aa, const AimsRGB &bb);
int operator >= (const AimsRGB &aa, const AimsRGB &bb);

int operator < (const AimsRGBA &aa, const AimsRGBA &bb);
int operator <= (const AimsRGBA &aa, const AimsRGBA &bb);
int operator > (const AimsRGBA &aa, const AimsRGBA &bb);
int operator >= (const AimsRGBA &aa, const AimsRGBA &bb);

// AimRGB
inline
double norm (const AimsRGB  &aa )
{ return( sqrt( (int)aa.red() * (int)aa.red() 
                + (int)aa.green() * (int)aa.green()
                + (int)aa.blue() * (int)aa.blue() ) );
}

inline
int operator < (const AimsRGB  &aa,
                const AimsRGB &bb)
{ return( norm(aa) < norm(bb) );
}

inline
int operator <= (const AimsRGB  &aa,
                 const AimsRGB &bb)
{ return( norm(aa) <= norm(bb) );
}

inline
int operator > (const AimsRGB  &aa,
                const AimsRGB &bb)
{ return( norm(aa) > norm(bb) );
}

inline
int operator >= (const AimsRGB  &aa,
                const AimsRGB &bb)
{ return( norm(aa) >= norm(bb) );
}

// AimRGBA
inline
double norm (const AimsRGBA  &aa )
{ return( sqrt( (int)aa.red() * (int)aa.red() 
                + (int)aa.green() * (int)aa.green()
                + (int)aa.blue() * (int)aa.blue()
                + (int)aa.alpha() * (int)aa.alpha() ) );
}

inline
int operator < (const AimsRGBA  &aa,
                const AimsRGBA &bb)
{ return( norm(aa) < norm(bb) );
}

inline
int operator <= (const AimsRGBA  &aa,
                 const AimsRGBA &bb)
{ return( norm(aa) <= norm(bb) );
}

inline
int operator > (const AimsRGBA  &aa,
                const AimsRGBA &bb)
{ return( norm(aa) > norm(bb) );
}

inline
int operator >= (const AimsRGBA  &aa,
                const AimsRGBA &bb)
{ return( norm(aa) >= norm(bb) );
}

#endif
