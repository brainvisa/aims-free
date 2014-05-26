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
