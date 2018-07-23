#ifndef ulvc_h
#define ulvc_h

extern void putin();
extern void putout();
extern void itrl();
extern void prefm();
extern void atrl();
extern void posmsb();
extern void uvlc();
extern void uvld();

void pcpet(int skipdpcm, int Long, char* input_filename, char* output_filename, int ncol, int nline);
void pdpet(int skipdpcm, int Long, char* input_filename, char* output_filename, int ncol, int nline);

#endif
