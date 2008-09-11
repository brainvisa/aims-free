/* Copyright (c) 1995-2007 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */


#include <aims/io/io_g.h>
#include <aims/getopt/getopt.h>
#include <aims/mesh/texture.h>
#include <aims/distancemap/meshvoronoi.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>

using namespace aims;
using namespace std;


BEGIN_USAGE(usage)
  "-------------------------------------------------------------------------",
  "AimsTextureAverage -f[loat_texture] <input_float_texture>]              ",
  "                   -o[utput <output_mesh>]                               ",
  "                   --binary                                              ",
  "                   --max                                              ",
  "                   --min                                              ",
  "                  [-h[elp]]                                              ",
  "-------------------------------------------------------------------------",
  " Compute the average of a time-serie texture                             ",
  "-------------------------------------------------------------------------",
  "     input_float_texture : input time-serie texture                  ",
  "     output_texture      : output mean texture                           ",
  "     binary              : binarise the input texture                    ",
  "     max                 : compute the maximum instead of the mean input texture                    ",
  "     min                 : compute the minimum instead of the mean input texture           ",
  "-------------------------------------------------------------------------",
END_USAGE


//
// Usage
//
void Usage( void )
{
  AimsUsage( usage );
}


int main( int argc, char** argv )
{
  char	*floattexfile = 0, *outtexfile = 0;
  int  binary = 0, maxFlag=0, minFlag=0;

  //
  // Parser of options
  //
  AimsOption opt[] = {
    { 'h',"help"         ,AIMS_OPT_FLAG  ,( void* )Usage  ,AIMS_OPT_CALLFUNC,0},
    { 'f',"float_texture",AIMS_OPT_STRING,&floattexfile   ,0                ,0},
    { 'o',"out_texture"  ,AIMS_OPT_STRING,&outtexfile     ,0                ,0},
    { ' ',"binary"       ,AIMS_OPT_FLAG  ,&binary         ,0                ,0},
    { ' ',"max"          ,AIMS_OPT_FLAG  ,&maxFlag        ,0                ,0},
    { ' ',"min"          ,AIMS_OPT_FLAG  ,&minFlag        ,0                ,0},
    { 0  ,0              ,AIMS_OPT_END   ,0               ,0                ,0}};

  AimsParseOptions( &argc, argv, opt, usage );
  
  if (*floattexfile == 0)
    {
      cerr << "You must choose an input texture \n";
      ASSERT (0);
    }
  
  if (maxFlag && minFlag)  
    {
      cerr << "You must choose either max or min flag\n";
      ASSERT (0);
    }

  //	read texture
  TimeTexture<float>	tex;
  unsigned             nnodes, ntimes;
  unsigned            j, i ;

 
  cout << "reading texture " << floattexfile << endl;
  Reader<TimeTexture<float> >	texF( floattexfile );
  texF >> tex ;
  
  nnodes = tex[0].nItem(); ntimes = tex.size();
  TimeTexture<float>	itex(ntimes,nnodes);
  
   if (binary != 0)
     {
       cout << "Texture binarization.\n";
       for (j=0;j<nnodes;++j)
         for (i=0;i<ntimes;++i)
           if (tex[i].item(j) != 0)
             itex[i].item(j) = 1;
           else
             itex[i].item(j) = 0;
     }
   else
     for (j=0;j<nnodes;++j)
      for (i=0;i<ntimes;++i)
	itex[i].item(j) = tex[i].item(j);
  

   float               m = 0;
   //float s = 0;
  TimeTexture<float>  outTex(1,nnodes);

  cout << ntimes << " time series, " << nnodes << " nodes" <<endl;

  if (!minFlag && !maxFlag)
    for (j=0;j<nnodes;++j)
      {
        m = 0;
        for (i=0;i<ntimes;++i)
          m += itex[i].item(j);
        m =  m / ntimes;

        outTex[0].item(j)= m ;

        /* 
        s = 0;   
        for (i=0;i<ntimes;++i)
          s += (itex[i].item(j) - m) * (itex[i].item(j) - m);
        s = sqrt(s) / ntimes;

        outTex[1].item(j) = (float)s ;
        */ 
      }
  
  if (minFlag)
    {
      cout << "Compute minimum texture (instead of mean texture).\n" ; 
      for (j=0;j<nnodes;++j)
        {
          m = 0;
          for (i=0;i<ntimes;++i)
            if (itex[i].item(j) < m ) 
              m = itex[i].item(j);
        
          outTex[0].item(j)= m ;
        }
    }

  if (maxFlag)
    {
      cout << "Compute maximum texture (instead of mean texture).\n" ; 
      for (j=0;j<nnodes;++j)
      {
        m = 0;
        for (i=0;i<ntimes;++i)
          if (itex[i].item(j) > m ) 
            m = itex[i].item(j);
        
        outTex[0].item(j)= m ;
      }
    }



  cout << "writing texture : " << endl;
  Writer<TimeTexture<float> >	texW( outtexfile );
  texW << outTex ;
  cout << "done " << endl;

  
  return 0;
}
