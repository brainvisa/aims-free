
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/signalfilter/gframes.h>

using namespace carto;


void GaussianFrames::doit( rc_ptr<Volume<float> > & data, const GCoef& coef )
{
  initialize( coef );
  doit( data );
}


void GaussianFrames::doit( rc_ptr<Volume<float> > & data )
{
  int dX = data->getSizeX();
  int dY = data->getSizeY();
  int dZ = data->getSizeZ();
  int dT = data->getSizeT();

  float *input = new float[ dT ];
  float *output = new float[ dT ];
  float *work = new float[ dT ];

  float *in = NULL, *out = NULL;

  int i, j, k, t;

  for ( k=0; k<dZ; ++k )
    for ( j=0; j<dY; ++j)
      for ( i=0; i<dX; ++i )
      {
        in = input;
        for ( t=0; t<dT; ++t )
          *in++ = data->at( i, j, k, t );

        recurse( input, output, work, dT );

        out = output;
        for ( t=0; t<dT; ++t )
          data->at( i, j, k, t ) = *out++;
      }

  delete[] work;
  delete[] output;
  delete[] input;
}
