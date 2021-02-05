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


#include <aims/getopt/getopt2.h>
#include <aims/mesh/texture.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char** argv )
{
  Reader<TimeTexture<float> >	texF;
  Writer<TimeTexture<float> >	texW;
  bool binary = 0, maxFlag=0, minFlag=0;

  AimsApplication app( argc, argv,
                       "Compute the average of a time-series texture" );
  app.addOption( texF, "-f", "input time-series texture" );
  app.addOption( texW, "-o", "output mean texture" );
  app.addOption( binary, "-b", "binarize the input texture", true );
  app.addOption( maxFlag,
                 "--max", "compute the maximum instead of the mean input "
                 "texture", true );
  app.addOption( minFlag,
                 "--min", "compute the minimum instead of the mean input "
                 "texture", true );
  app.alias( "--float_texture", "-f" );
  app.alias( "-i", "-f" );
  app.alias( "--output", "-o" );
  app.alias( "--binary", "-b" );

  try
  {
    app.initialize();
  
    if (maxFlag && minFlag)
      {
        cerr << "You must choose either max or min flag\n";
        ASSERT (0);
      }

    //	read texture
    TimeTexture<float>	tex;
    unsigned             nnodes, ntimes;
    unsigned            j, i ;

    cout << "reading texture " << texF.fileName() << endl;
    texF.read( tex );

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
    texW.write( outTex );
    cout << "done " << endl;

    return EXIT_SUCCESS;
  }
  catch( user_interruption & )
  {
  }
  catch( exception & e )
  {
    cerr << e.what() << endl;
  }
  return EXIT_FAILURE;
}
