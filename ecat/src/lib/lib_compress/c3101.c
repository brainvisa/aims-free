/******************************************************************************
*                                                                             *
*          ######          ######         Copyright (C) 1991                  *
*         ##########    ##########        Advanced Hardware Architectures     *
*        ###########    ###########       P.O. 9669 Moscow, Idaho 83843       *
*       ##### ################ #####      208-883-8000                        *
*      #####   ##############   #####                                         *
*     ################################    Copyright (C) 1991                  *
*    ###############    ###############   Hewlett Packard Co.                 *
*   #####       ####    ####       #####                                      *
*  #####        ####    ####        #####                                     *
*                                                                             *
*******************************************************************************
*                                                                             *
*******************************************************************************
*                                                                             *
* This code is the property of Advanced Hardware Architectures, and may       * 
* not be copied in whole or in part without prior written permission of       *
* Advanced Hardware Architectures.                                            *
*                                                                             *
*******************************************************************************/

/*  Compile with Define STAND_ALONE to make c3101.exe as follows        */
/*    for MS C6.0 :       cl /AL /D STAND_ALONE c3101.c                 */

/* Speed Hacks */

#ifdef __GNUC__
#define INLINE extern inline
#else
#define INLINE			/* Not available for non-GNU C Compilers */
#endif

/* #define STAND_ALONE */
/* Header Files */

#include <stdio.h>
#if defined(unix) || defined(__osf__) || defined(__APPLE__)
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#else
#include <stdlib.h>
#include <dos.h>
#include <malloc.h>
#include <io.h>
#include <fcntl.h>
#endif

/* Macros */

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/* Global Variables */

static unsigned short hash[16384];
static unsigned short dict[3832][2];

static          out_len = 0;
static          out_code_len = 9;
static long     out_code_remainder = 0;

static long            inp_byte_cnt;
static long            out_byte_cnt;

static int             blk_inp_byte_cnt = 0;
static int             blk_out_byte_cnt = 0;
static int             nac = 0;
static int             reset_flag = 0;
static int             thresh_in_cnt = 0;
static int             comp_diff = 64;
static int             period_mask;
static int             period = 512;             /*  default period  */
static int             threshold = 1;            /* default threshold */
extern FILE *infile, *outfile;

/******************************************************************************
*   Routine:      get_ubyte: Get an uncompressed byte from stdin,             *
*                 accumulate statistics for deciding reset                    *
******************************************************************************/

INLINE int
get_ubyte()
{
    int             c;

    blk_inp_byte_cnt++;
    if ((blk_inp_byte_cnt & period_mask) == 0) {

	thresh_in_cnt++;
	if (thresh_in_cnt >= 64) {
	    if (nac > 2047 && (comp_diff >= 64)) {
		if ((comp_diff % 64) < threshold) {
		    reset_flag = TRUE;
		}
	    } else if (comp_diff < 64)
		reset_flag = TRUE;
	    inp_byte_cnt += blk_inp_byte_cnt;
	    out_byte_cnt += blk_out_byte_cnt;
	    /* comp_diff -= 64; */
	    blk_inp_byte_cnt = blk_out_byte_cnt = 0;
	    thresh_in_cnt = 0;
	    comp_diff = 63;
	}
	comp_diff += 1;
    }
	return (fgetc(infile));
}

#ifdef STAND_ALONE
/******************************************************************************
*     Routine:  usage                                                         *
*                   p is the period in bytes for checking the compression     *
*                         ratio (default is 512 )                             *
*                   t is the threshold for dictionary reset (default is 1 )   *
******************************************************************************/
usage()
{
    fprintf(stderr, "Usage: c3101 [-p period] [-t threshold] < in > out\n");
    exit(-1);
}
#endif

/******************************************************************************
*      Hash function (not necessarily the same as in the AHA3101 chip)        *
******************************************************************************/
#define HASH_F(chr, code) (((chr << 6) ^ code) & 0x3fff)

/*****************************************************************************
*   Routine ptcode : put out 9,10,11, or 12 bit code on stdout               *
*****************************************************************************/
INLINE void
ptcode(code)
    unsigned short  code;
{
	out_code_remainder |= (((long) code) << out_len);
    out_len += out_code_len;
    while (out_len >= 8) {
	out_len -= 8;
	fputc(out_code_remainder & 0xff, outfile);
	blk_out_byte_cnt++;
	if ((blk_out_byte_cnt & period_mask) == 0)
	    comp_diff -= 1;
	out_code_remainder /= 256;
    }
}

/*****************************************************************************
*   Routine ptrnd : rounds up to byte boundary                               *
*****************************************************************************/
INLINE void
ptrnd()
{
    if (out_len > 0) {
	fputc(out_code_remainder & 0xff, outfile);
	blk_out_byte_cnt++;
	if ((blk_out_byte_cnt & period_mask) == 0)
	    comp_diff -= 1;
	out_len = 0;
    }
    out_code_remainder = 0;
}

/*****************************************************************************
*   Macro :  RESET                                                           *
*****************************************************************************/
#define RESET \
{ nac = 0x108; ocode = 0x200; mxln = 0; \
      reset_flag = 0; inp_byte_cnt += blk_inp_byte_cnt; \
	  out_byte_cnt += blk_out_byte_cnt; period_mask = 0xf; \
	      thresh_in_cnt = 0; comp_diff = 64; \
		  blk_inp_byte_cnt = blk_out_byte_cnt = 0; \
		      thresh_in_cnt = 0; comp_diff = 64; \
			  ptcode(1); ptrnd(); out_code_len = 9; \
			      for (dindx = 0; dindx < 16384; dindx++) \
				  hash[dindx] = 0x8000; \
 }

/*****************************************************************************
*  Routine :  cmpr   to compress stdin to stdout                             *
*****************************************************************************/
extern void
cmpr()
{
    int             code;
    unsigned short  dindx;
    unsigned short  ocode;
    unsigned short  mxln;
    int             chr;
    unsigned short  hindx;

    if ((chr = get_ubyte()) == EOF)
	return;
    chr += 8;

    RESET;
    code = chr;

    while ((chr = get_ubyte()) != EOF) {

	chr += 8;
	if (mxln != 127) {

	    /* Search dictionary to see if char/code entry exists */
	    hindx = HASH_F(chr, code);

	    while (hash[hindx] != 0x8000) {
		dindx = hash[hindx];
		if (chr == dict[dindx][0] && code == dict[dindx][1]) {
		    code = dindx + 0x108;
		    mxln += 1;
		    break;
		}
		hindx = 0x1fff & (hindx + 127);
	    }

	    if (hash[hindx] != 0x8000)
		continue;

	    if (nac < 0x1000) {
		hash[hindx] = nac - 0x108;
		dict[nac - 0x108][0] = chr;
		dict[nac - 0x108][1] = code;
		nac += 1;
		if (nac == 2048) {
		    inp_byte_cnt += blk_inp_byte_cnt;
		    out_byte_cnt += blk_out_byte_cnt;
		    blk_inp_byte_cnt = blk_out_byte_cnt = 0;
		    thresh_in_cnt = 0;
		    comp_diff = 64;
		    switch (period) {
		    case 512:
			period_mask = 0x7;
			break;
		    case 1024:
			period_mask = 0xf;
			break;
		    case 2048:
			period_mask = 0x1f;
			break;
		    case 4096:
			period_mask = 0x3f;
			break;
		    }
		}
	    }
	}
	mxln = 0;
	while (code >= (int)ocode) {
	    ptcode(2);
	    out_code_len++;
	    ocode <<= 1;
	}

	ptcode(code);

	if (reset_flag)
	    RESET;

	code = chr;
    }

    while (code >= (int)ocode) {
	ptcode(2);
	out_code_len++;
	ocode <<= 1;
    }

    ptcode(3);
    ptrnd();
    ptcode(code);
    ptrnd();

    out_byte_cnt += blk_out_byte_cnt;
    inp_byte_cnt += blk_inp_byte_cnt;
    inp_byte_cnt--;			/* Correct for counting EOF */
}
 
/******************************************************************************
*                                                                             *
*   Main program...: main   calls cmpr()                                      *
*                                                                             *
******************************************************************************/
#ifdef STAND_ALONE
main(argc, argv)
    int             argc;
    char          **argv;
{
    double          ratio;

#ifdef MSDOS
    setmode(0, O_BINARY);
    setmode(1, O_BINARY);
#endif

    			/* Parse arguments for -p or -t flag */
    argc--;		/*  call usage if values are invalid */
    while (*++argv) {
	if (*argv[0] != '-')
	    usage();
	else {
	    if ((*argv)[1] == 'p') {
		if ((*argv)[2] != '\0' || (*(argv + 1)) == 0)
		    usage();
		else
		    period = atoi(*++argv);
		switch (period) {
		case 512:
		case 1024:
		case 2048:
		case 4096:
		    break;
		default:
		    fprintf(stderr,
			    "Period must be 512, 1024, 2048 or 4096!\n");
		    usage();
		}
		continue;
	    } else if ((*argv)[1] == 't') {
		if ((*argv)[2] != '\0' || (*(argv + 1)) == 0)
		    usage();
		else
		    threshold = atoi(*++argv);
		if (threshold < 0 || threshold > 63) {
		    fprintf(stderr, "Threshold must be between 0 and 63!\n");
		    usage();
		}
	    } else {
		fprintf(stderr, "Unrecognized option!\n");
		usage();
	    }
	}
    }

    fprintf(stderr, "c3101 - AHA 3101 DCLZ Compression Demonstration, Copyright 1991.\n");
     
    cmpr();		/* call cmpr to compress stdin to stdout  */

    ratio = out_byte_cnt ? (double) inp_byte_cnt / out_byte_cnt : 0.0;
    fprintf(stderr, "In, Out, Ratio: %10lu%10lu%12.3f\n",
	    inp_byte_cnt, out_byte_cnt, ratio);
    exit(0);
}
#endif
