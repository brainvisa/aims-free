// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

/*
 *  ROI writer class
 */
#include <aims/io/roiW.h>

using namespace aims;
using namespace std;

LowLevelRoiWriter::LowLevelRoiWriter() : LowLevelStandardArgWriter()
{
}


LowLevelRoiWriter::~LowLevelRoiWriter()
{
}


void LowLevelRoiWriter::write( const std::string & filename, Graph & graph )
{
  GraphWriter	w( filename, *_syntax );
  AimsGraphWriter	gw( filename );
  gw.writeElements( g );
  w << g;
}



RoiWriter::RoiWriter( const string& name, const SyntaxSet& stx ) 
    : GraphWriter( name, stx ), _name( name )
{
}


RoiWriter::~RoiWriter()
{
}


void RoiWriter::write( AimsRoi& thing )
{ 
  ArgWriter	aw( _name );
  aw.write( thing );
}
