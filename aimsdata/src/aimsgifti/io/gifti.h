#ifndef AIMS_IO_GIFTI_H
#define AIMS_IO_GIFTI_H

/* This is the header which must be included in Aims/SomaIO for GIFTI IO.
   It handles symbols renaming in niftilib for SomaIO.

   You should NOT include <gifti_io.h> directly.
*/

#include <nifti2_io.h> // should always be included before <gifti_io.h>
#ifdef __cplusplus
extern "C"
{
#endif
#include <gifti_io.h>
#ifdef __cplusplus
}
#endif

#endif

