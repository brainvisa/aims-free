
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/signalfilter/gslices.h>

using namespace carto;


void GaussianSlices::doit( rc_ptr<Volume<float> > & data, const GCoef& coef )
{
  initialize( coef );
  doit( data );
}


void GaussianSlices::doit( rc_ptr<Volume<float> > & data )
{
  int dX = data->getSizeX();
  int dY = data->getSizeY();
  int dZ = data->getSizeZ();
  int dT = data->getSizeT();

  float *input = new float[ dZ + 4 ];
  float *output = new float[ dZ + 4 ];
  float *work = new float[ dZ + 4 ];

  float *in = NULL, *out = NULL;

  int i, j, k, t;

  for ( t=0; t<dT; t++ )
  {
    for ( j=0; j<dY; ++j )
      for ( i=0; i<dX; ++i )
      {
        in = input;
        for ( k=0; k<dZ; ++k )
          *in++ = data->at( i, j, k, t );

        recurse( input, output, work, dZ );

        out = output;
        for ( k=0; k<dZ; ++k )
          data->at( i, j, k, t ) = *out++;
      }
  }


  delete[] work;
  delete[] output;
  delete[] input;
}



