// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/getopt/getopt2.h>

using namespace std;
using namespace carto;
using namespace aims;


  ///////////////////////
 //  AimsApplication  //
///////////////////////

//-----------------------------------------------------------------------------
AimsApplication *_globalApplication = NULL;
AimsApplication *AimsApplication::globalApplication()
{
  return _globalApplication;
}


//-----------------------------------------------------------------------------
AimsApplication::AimsApplication( int argc, const char **argv, 
				  const string &documentation ) :
  CartoApplication( argc, argv, documentation )
{
  _globalApplication = this;
}


//-----------------------------------------------------------------------------
AimsApplication::~AimsApplication()
{
}


//-----------------------------------------------------------------------------
void AimsApplication::initialize()
{
  output_4d_volumes = -1;
  addOption( output_4d_volumes, "--output_4d_volumes",
	     "Write 4D images as several 3D files (0 = series of 3D volumes, 1 = one 4D volume, no value = use .aimsrc configuration files)", true );
  CartoApplication::initialize();
}



