/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */


#include "postscript.h"

using namespace std;

static void psColorImage( FILE* fp )
{
  /* spits out code that checks if the PostScript device in question
     knows about the 'colorimage' operator.  If it doesn't, it defines
     'colorimage' in terms of image (ie, generates a greyscale image from
     RGB data) */


  fprintf(fp,"%% define 'colorimage' if it isn't defined\n");
  fprintf(fp,"%%   ('colortogray' and 'mergeprocs' come from xwd2ps\n");
  fprintf(fp,"%%     via xgrab)\n");
  fprintf(fp,"/colorimage where   %% do we know about 'colorimage'?\n");
  fprintf(fp,"  { pop }           %% yes: pop off the 'dict' returned\n");
  fprintf(fp,"  {                 %% no:  define one\n");
  fprintf(fp,"    /colortogray {  %% define an RGB->I function\n");
  fprintf(fp,"      /rgbdata exch store    %% call input 'rgbdata'\n");
  fprintf(fp,"      rgbdata length 3 idiv\n");
  fprintf(fp,"      /npixls exch store\n");
  fprintf(fp,"      /rgbindx 0 store\n");
  fprintf(fp,"      0 1 npixls 1 sub {\n");
  fprintf(fp,"        grays exch\n");
  fprintf(fp,"        rgbdata rgbindx       get 20 mul    %% Red\n");
  fprintf(fp,"        rgbdata rgbindx 1 add get 32 mul    %% Green\n");
  fprintf(fp,"        rgbdata rgbindx 2 add get 12 mul    %% Blue\n");
  fprintf(fp,"        add add 64 idiv      %% I = .5G + .31R + .18B\n");
  fprintf(fp,"        put\n");
  fprintf(fp,"        /rgbindx rgbindx 3 add store\n");
  fprintf(fp,"      } for\n");
  fprintf(fp,"      grays 0 npixls getinterval\n");
  fprintf(fp,"    } bind def\n\n");

  fprintf(fp,"    %% Utility procedure for colorimage operator.\n");
  fprintf(fp,"    %% This procedure takes two procedures off the\n");
  fprintf(fp,"    %% stack and merges them into a single procedure.\n\n");
  
  fprintf(fp,"    /mergeprocs { %% def\n");
  fprintf(fp,"      dup length\n");
  fprintf(fp,"      3 -1 roll\n");
  fprintf(fp,"      dup\n");
  fprintf(fp,"      length\n");
  fprintf(fp,"      dup\n");
  fprintf(fp,"      5 1 roll\n");
  fprintf(fp,"      3 -1 roll\n");
  fprintf(fp,"      add\n");
  fprintf(fp,"      array cvx\n");
  fprintf(fp,"      dup\n");
  fprintf(fp,"      3 -1 roll\n");
  fprintf(fp,"      0 exch\n");
  fprintf(fp,"      putinterval\n");
  fprintf(fp,"      dup\n");
  fprintf(fp,"      4 2 roll\n");
  fprintf(fp,"      putinterval\n");
  fprintf(fp,"    } bind def\n\n");

  fprintf(fp,"    /colorimage { %% def\n");
  fprintf(fp,"      pop pop     %% remove 'false 3' operands\n");
  fprintf(fp,"      {colortogray} mergeprocs\n");
  fprintf(fp,"      image\n");
  fprintf(fp,"    } bind def\n");
  fprintf(fp,"  } ifelse          %% end of 'false' case\n");
  fprintf(fp,"\n\n\n");
}


void writePostscript( byte *pixels, int dimx, int dimy, int offx, int offy,
                      bool fullcolor, const string& filename )
{
  int   i, j, err, rpix, gpix, bpix;
  int   slen, lwidth, bits, colorps;
  byte *inpix;

  slen = bits = colorps = 0;


  FILE *fp = fopen( filename.c_str(), "w" );
    

  /*** write PostScript header ***/


  fprintf(fp,"%%!PS-Adobe-2.0 EPSF-2.0\n");
  fprintf(fp,"%%%%Title: %s\n",filename.c_str());
  fprintf(fp,"%%%%Creator: AIMS Visu 3d\n");
  fprintf(fp,"%%%%BoundingBox: %d %d %d %d\n", offx, offy, 
                                               offx+dimx, offy+dimy);

  fprintf(fp,"%%%%Pages: 1\n");
  fprintf(fp,"%%%%DocumentFonts:\n");
  fprintf(fp,"%%%%EndComments\n");


  if ( fullcolor )
  {
    slen = dimx*3;bits = 8;colorps = 1;
  }
  else
  {
    slen = dimx;  bits = 8;colorps = 0;
  }

  fprintf(fp,"%%%%EndProlog\n\n");

  fprintf(fp,"%%%%Page: 1 1\n\n");

  fprintf(fp,"%% remember original state\n");
  fprintf(fp,"/origstate save def\n\n");

  fprintf(fp,"%% build a temporary dictionary\n");
  fprintf(fp,"20 dict begin\n\n");


  fprintf(fp,"%% define string to hold a scanline's worth of data\n");
  fprintf(fp,"/pix %d string def\n\n", slen);

  fprintf(fp,"%% define space for color conversions\n");
  fprintf(fp,"/grays %d string def  %% space for gray scale line\n", dimx);
  fprintf(fp,"/npixls 0 def\n");
  fprintf(fp,"/rgbindx 0 def\n\n");


  fprintf(fp,"%% lower left corner\n");
  fprintf(fp,"%d %d translate\n\n",offx,offy);

  fprintf(fp,"%% size of image (on paper, in 1/72inch coords)\n");
  fprintf(fp,"%d %d scale\n\n",dimx,dimy);


  /* if we're using color, make sure 'colorimage' is defined */
  if (colorps) psColorImage(fp);

  fprintf(fp,"%d %d %d\t\t\t%% dimensions of data\n",dimx,dimy,bits);
  fprintf(fp,"[%d 0 0 %d 0 0]\t\t%% mapping matrix\n", dimx, dimy );

  fprintf(fp,"{currentfile pix readhexstring pop}\n");
  if (colorps) fprintf(fp,"false 3 colorimage\n"); 
  else fprintf(fp,"image\n");

  /* dump the image data to the file */
  err = 0;

  inpix = pixels;

  for (i=0; i<dimy && err != EOF; i++)
  {
    lwidth = 0;
    putc('\n',fp);

	 for (j=0; j<dimx && err != EOF; j++)
    {
      rpix = inpix[0];
	   gpix = inpix[1];
	   bpix = inpix[2];
	  
	   if (colorps)
      { 
	     err = fprintf(fp,"%02x%02x%02x",rpix,gpix,bpix);
	     lwidth+=6;
	   }  
	   else 
      { /* greyscale */
        err = fprintf(fp,"%02x", MONO(rpix,gpix,bpix));
	     lwidth+=2;
	   }

	   if (lwidth>70)
      { putc('\n',fp);
        lwidth = 0;
      }

	   inpix += 3;
	 }
  }

  fprintf(fp,"\n\nshowpage\n\n");
  fprintf(fp,"%% stop using temporary dictionary\n");
  fprintf(fp,"end\n\n");

  fprintf(fp,"%% restore original state\n");
  fprintf(fp,"origstate restore\n\n");
  fprintf(fp,"%%%%Trailer\n");


  fclose(fp);
}

	    

