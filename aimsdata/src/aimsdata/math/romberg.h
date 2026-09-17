/*
 *  Romberg's numerical integration.
 */
#ifndef AIMS_MATH_ROMBERG_H
#define AIMS_MATH_ROMBERG_H

/**@name Romberg's numerical integration.
    This algorithm returns the true value for polynomial functions
    using an adaptive trapeziodal integration.
*/
//@{
///
void AimsTrapezeIntegration(float (*func)(float),float a,float b,
                            int n,float *s);
///
float AimsRombergIntegration(float (*func)(float),float a,float b,
                             float eps=1e-6,int jmax=20,int k=5);
//@}

#endif
