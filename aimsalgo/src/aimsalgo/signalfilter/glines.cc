
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/signalfilter/glines.h>

using namespace carto;


void GaussianLines::doit( rc_ptr<Volume<float> >& data, const GCoef& coef )
{
  initialize( coef );
  doit( data );
}


void GaussianLines::doit( rc_ptr<Volume<float> > & data )
{
  int dX = data->getSizeX();
  int dY = data->getSizeY();
  int dZ = data->getSizeZ();
  int dT = data->getSizeT();

  float *output = new float[ dX + 4 ];
  float *work = new float[ dX + 4 ];

  float *out = NULL;
  float *ptr;

  int i, j, k, t;

  for ( t=0; t<dT; ++t )
    for ( k=0; k<dZ; ++k )
      for ( j=0; j<dY; ++j )
      {
        ptr = &data->at( 0, j, k, t );
        recurse( ptr, output, work, dX );

        out = output;
        for ( i=dX; i--; )  *ptr++ = *out++;
      }


  delete[] work;
  delete[] output;
}


