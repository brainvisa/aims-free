/*    %Z%%M% %I% %E% GE Medical Systems    */
/*
 * Copyright 1991 by General Electric Co.
 */

/*  (ghost) 11-Nov-91  */

#ifndef _GE_IMGHDR_H_
#define _GE_IMGHDR_H_

#define RASPOINT        float  
#define REFCHANTYPE     short int  
#define IMATRIXTYPE     short int  
#define DIMXYTYPE       float
#define PIXSIZETYPE     float  
#define BLOCK           char  
  

typedef struct imghdr{
        int img_magic;          /* image file unique magic number */  
        int img_hdr_length;     /* length of header, also byte displacement to the 'pixel data area' */  
        int img_width;          /* x-axis pixel count (image width, for example: 128, 256, 512) */  
        int img_height;         /* y-axis pixel count (image height) */  
        int img_depth;          /* number of bits in an uncompressed pixel (1, 8, 16, or 24 bits).  
                                Note: NOT magnitude resolution (CT is 16 not 12) */  
        int img_compress;       /* Fomm of compression and packing applied to file, where:  
                                1 = IC_RECT Nor~compressed, normal rectangular image. 
                                2 = IC_PACKED Image is line length map packed. 
                                3 = IC_COMPRESSED Image is compressed via DCPM only
                                4 = IC_COMPACK Image is compressed and packed. */  
        int img_dwindow;        /* default window width (stored image value range) */  
        int img_dievel;         /* default level value (stored image value magnitude) */  
        int img_bgshade;        /* default background shade for non-pixels during unpack */  
        int img_ovrtiow;        /* pixel value to substitute when overtiow occcurs in GIP */  
        int img_undflow;        /* pixel value to substitute when underflow occcurs in GIP */  
        int img_top_offset;     /* number of (blank) lines at the top of the image.*/  
        int img_bot_offset;     /* number of (blank) lines at the bottom of the image */  
        short img_version;      /* version of the header structure IMG_HDR_VERSION */  
        unsigned short img_checksum;    /* 16 bit end_around_carry sum of true image pixels a value of  
                                zero indicates the checksum is not defined for this file */  
        int img_p_id;           /* a byte disp to 'unique image identifier text table' */  
        int img_l_id;           /* byte length of 'unique image identifier text table' */  
        int img_p_unpack;       /* a byte disp to 'unpack control table '*/  
        int img_l_unpack;       /* byte length of 'unpack control table' */  
        int img_p_compress;     /* a byte disp to 'compression seed table' */  
        int img_l_compress;     /* byte length of 'compression seed table' */  
        int img_p_histo;        /* a byte disp to 'histogram control table' */  
        int img_l_histo;        /* byte length of 'histogram control table' */  
        int img_p_text;         /* a byte disp to 'text plane data' */  
        int img_l_text;         /* byte length of 'text plane data' */  
        int img_p_graphics;     /* a byte disp to 'graphics plane data' */  
        int img_l_graphics;     /* byte length of 'graphks plane data' */  
        int img_p_dbHdr;        /* a byte disp to 'data base header data' */  
        int img_l_dbHdr;        /* byte length of 'data base header data' */
        int img_level;          /* value to add to the actual pixel d ata values to get the correct  
                                annotation value. For CT, physical 0 means -1024 Hounsfield number. */  
        int img_p_user;         /* byte displacement to 'user defined data' */  
        int img_l_user;         /* byte length of 'user defined data' */  
        int img_p_suite;        /* byte displacement to 'suite header data' */  
        int img_l_suite;        /* byte length of 'suite header data' */  
        int img_p_exam;         /* byte displacement to 'exam header data' */  
        int img_l_exam;         /* byte length of 'exam' header data' */  
        int img_p_series;       /* byte displacement to 'series header data' */  
        int img_l_series;       /* byte length of 'series header data' */
        int img_p_image;        /* byte displacement to 'image header data' */  
        int img_l_image;                /* byte iength of 'image header data' */  
} ImgHdr;  

#endif

