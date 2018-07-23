#ifndef convert_70_h
#define convert_70_h

#include <ecat/kernel/matrix_64.h>

#if defined(__cplusplus)
extern "C" {
/*
 * high level user functions
 */
#endif

void mh_convert(Main_header *h, Main_header_64 *h_64);
void sh_convert(Scan_subheader *h, Scan_subheader_64 *h_64, Main_header *mh);
void ih_convert(Image_subheader *h, Image_subheader_64 *h_64, Main_header *mh);
void nh_convert(Norm_subheader *h, Norm_subheader_64 *h_64, Main_header *mh);
void ah_convert(Attn_subheader *h, Attn_subheader_64 *h_64, Main_header *mh);

#if defined(__cplusplus)
}
#endif

#endif
