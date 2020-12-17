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
*  This code is the property of Advanced Hardware Architectures and may       *
*  not be copied in whole or in part without prior written permission of      *
*  Advanced Hardware Architectures.                                           *
*                                                                             *
******************************************************************************/

/* Speed Hacks */

#ifdef __GNUC__
#define INLINE inline
#else
#define INLINE			/* Not available for non-GNU C Compilers */
#endif

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

static long            inp_byte_cnt;
static long            out_byte_cnt;
static int             period,threshold;
/*****************************************************************************
*   Routine:  usage                                                          *
*****************************************************************************/
void usage()
{
    fprintf(stderr, "Usage: dclz [infile] [outfile] [c|d]\n");
    exit(-1);
}

void            cmpr();
void            dcmpr();

FILE *infile=NULL, *outfile=NULL;

/*****************************************************************************
*   Main program                                                             *
*****************************************************************************/
int dclz(in_filename, out_filename, comp_mode)
char *in_filename, *out_filename;
int comp_mode;
{
	inp_byte_cnt = 0;
	out_byte_cnt = 0;

	if ( (infile = fopen(in_filename, "r")) == NULL) {
	    perror(in_filename);
	    exit(EXIT_FAILURE);
	}

	if ( (outfile = fopen(out_filename, "w")) == NULL) {
	    perror(out_filename);
	    exit(EXIT_FAILURE);
	}

	/* Hard-code period and threshold - different than chip*/
	period = 2048;
	threshold = 32;

	/* Now check for valid mode */

	if (!(comp_mode == 'c' || comp_mode == 'C' || comp_mode == 'd' ||
	    comp_mode == 'D')) {
	    fprintf(stderr, "*** Please specify [c] to compress or [d] to decompress ***\n");
	    exit(EXIT_FAILURE);
	}

	if (comp_mode == 'c' || comp_mode == 'C')
	    cmpr();
	
	if (comp_mode == 'd' || comp_mode == 'D')
	    dcmpr();

	fclose(infile);
	fclose(outfile);
	return 0;
}
