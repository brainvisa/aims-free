#ifndef compress_h
#define compress_h

#if defined(__cplusplus)
extern "C" int compress(char* data, int size, int type, char* z_data);
extern "C" int uncompress(char* z_data, int size, char* data, int type);
#else
extern int compress();
extern int uncompress();
#endif /* __cplusplus */

#endif /* compress_h */
