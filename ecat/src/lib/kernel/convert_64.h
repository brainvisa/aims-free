#ifndef convert_64_h
#define convert_64_h

#include <ecat/kernel/matrix_64.h>

#if defined(__cplusplus)
extern "C" {
/*
 * high level user functions
 */
#endif

void mh64_convert(Main_header_64 *h_64, Main_header * h);
void sh64_convert(Scan_subheader_64* h_64, Scan_subheader* h, Main_header* mh);
void ih64_convert(Image_subheader_64* h_64, Image_subheader* h, Main_header* mh);
void nh64_convert(Norm_subheader_64* h_64, Norm_subheader* h, Main_header* mh);
void ah64_convert(Attn_subheader_64* h_64, Attn_subheader* h, Main_header* mh);

#if defined(__cplusplus)
}
#endif

#endif
