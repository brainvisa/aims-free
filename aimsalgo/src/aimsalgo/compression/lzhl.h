
#ifndef AIMS_COMPRESSION_LZHL_H
#define AIMS_COMPRESSION_LZHL_H

#define ROTL( x, y ) ( ( (x) << (y) ) | ( (x) >> ( 32 - (y) ) ) )

#define LZMIN 4

//
// user tunable parameters
//

// Affect format
// LZBUFBITS is a log2(LZBUFSIZE) and must be in range 10 - 16
#define LZBUFBITS 16

// NOT affect format
#define LZMATCH 5

#define LZSLOWHASH
// LZTABLEBITS is a log2(LZTABLESIZE) and should be in range 9 - 17
#define LZTABLEBITS 15

#define LZOVERLAP
#define LZBACKWARDMATCH
#define LZLAZYMATCH
#define LZSKIPHASH 1024

// HUFFRECALCLEN should be <= 16384
#define HUFFRECALCLEN 4096

#define LZTABLESIZE (1<<(LZTABLEBITS))
#define LZBUFSIZE (1<<(LZBUFBITS))


#endif
