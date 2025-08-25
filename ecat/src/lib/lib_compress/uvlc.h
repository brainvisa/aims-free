#ifndef ulvc_h
#define ulvc_h

#include <stdio.h>

extern void putin();
extern void putout();
extern void itrl();
extern void prefm();
extern void atrl();
extern void posmsb();
extern void uvlc(int *pp, int Long, FILE *output, int *pout, int *rout);
extern void uvld(int *pp, int Long, FILE *input, int *pin, int *rin);

void pcpet(int skipdpcm, int Long, char* input_filename, char* output_filename, int ncol, int nline);
void pdpet(int skipdpcm, int Long, char* input_filename, char* output_filename, int ncol, int nline);

#endif
