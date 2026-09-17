
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
