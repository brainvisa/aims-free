
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/signalfilter/gcolumns.h>

using namespace carto;


void GaussianColumns::doit( rc_ptr<Volume<float> > & data, const GCoef& coef )
{
  initialize( coef );
  doit( data );
}


void GaussianColumns::doit( rc_ptr<Volume<float> > & data )
{
  int dX = data->getSizeX();
  int dY = data->getSizeY();
  int dZ = data->getSizeZ();
  int dT = data->getSizeT();

  float *input = new float[ dY + 4 ];
  float *output = new float[ dY + 4 ];
  float *work = new float[ dY + 4 ];

  float *in = NULL, *out = NULL;

  int i, j, k, t;

  for ( t=0; t<dT; t++ )
    for ( k=0; k<dZ; k++ )
    {
      for ( i=dX; i--; )
      {
        in = input;

        for ( j=0; j<dY; ++j )
          *in++ = data->at( i, j, k, t );

        recurse( input, output, work, dY );

        out = output;
        for ( j=0; j<dY; ++j )
          data->at( i, j, k, t ) = *out++;
      }
    }

  delete[] work;
  delete[] output;
  delete[] input;
}


