#ifndef compress_h
#define compress_h

#include <ecat/kernel/matrix.h>

#if defined(__cplusplus)
extern "C"
{
#endif
  int compress(MatrixData *data, int data_type, char* z_data, int method);
  int uncompress(char* z_data, int size, MatrixData* matrix_data, int type);
#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* compress_h */
