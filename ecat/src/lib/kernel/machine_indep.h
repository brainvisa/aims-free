#ifndef machine_indep_h
#define machine_indep_h

#include <stdio.h>

#if defined(WIN32) || defined(WIN64)
#include <winsock2.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif
#ifdef linux
/* WHERE IS swab on redhat ??? */
#include <sys/types.h>
void swab(const void *src, void *dest, size_t nbytes);
#endif
void SWAB(const void *from, void *to, int length);
void ftovaxf(float f, unsigned short *bufr);
int file_data_to_host(char *dptr, int nblks, int dtype);
int read_raw_acs_data(char *fname, int strtblk, int nblks, 
					  char *dptr, int dtype);
int read_matrix_data( FILE *fptr, int strtblk, int nblks,
					 char *dptr, int dtype);
int write_matrix_data( FILE *fptr, int strtblk, int nblks,
					 char *dptr, int dtype);
void bufWrite(char* s, char* buf, int* i, int len);
void bufWrite_s(short val, char* buf, int* i);
void bufWrite_i(int val, char* buf, int* i);
void bufWrite_u(unsigned int val, char* buf, int* i);
void bufWrite_f(float val, char* buf, int* i);
void bufRead(char* s, char* buf, int* i, int len);
void bufRead_s(short*, char* buf, int* i);
void bufRead_i(int*, char* buf, int* i);
void bufRead_u(unsigned int*, char* buf, int* i);
void bufRead_f(float*, char* buf, int* i);
#if defined(__cplusplus)
}
#endif /* __cpluplus */
#endif  /* machine_indep_h */
