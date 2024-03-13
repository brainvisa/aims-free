/* this source builds the C code of gifticlib, with symbols renaming in niftilib
   done in soma-io.
*/

// Have stdlib.h define atoll()
#define _POSIX_C_SOURCE 200112L

#include <aims/io/gifti.h>
#include "gifticlib/gifti_io.c"
#include "gifticlib/gifti_xml.c"

