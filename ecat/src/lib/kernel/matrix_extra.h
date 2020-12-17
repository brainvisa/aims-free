#ifndef matrix_extra_h
#define matrix_extra_h

#include <ecat/kernel/matrix.h>

#if defined(__cplusplus)
extern "C" {
/*
 * high level user functions
 */
#endif

int read_host_data(MatrixFile* mptr, int matnum, MatrixData* data, int dtype); 
int write_host_data(MatrixFile* mptr, int matnum, MatrixData* data);

#if defined(__cplusplus)
}
#endif

#endif
