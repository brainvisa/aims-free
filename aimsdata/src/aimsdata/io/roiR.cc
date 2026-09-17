// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 *  ROI reader class
 */
#include <aims/io/roiR.h>
#include <aims/io/aimsGraphR.h>
#include <graph/graph/greader.h>

using namespace aims;
using namespace std;


LowLevelRoiReader::LowLevelRoiReader() : LowLevelStandardArgReader()
{
}


LowLevelRoiReader::~LowLevelRoiReader()
{
}


Graph* LowLevelRoiReader::read( const string & filename, 
				int subobjectsfilter )
{
  GraphReader	r( filename, *_syntax );
  AimsRoi	*g = new AimsRoi;
  try
    {
      r >> *g;
    }
  catch( exception & )
    {
      delete g;
      throw;
    }

  AimsGraphReader	gr( filename );
  if( subobjectsfilter < 0 )
    gr.readElements( *g, 3 );
  else if( subobjectsfilter > 0 )
    gr.readElements( *g, 1 );

  return( g );
}
