// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/io/labelImage.h>
#include <aims/io/finder.h>
#include <aims/utility/converter_volume.h>

using namespace aims;
using namespace carto;
using namespace std;

LabelReader::LabelReader( const string& filename ) 
  : Reader<VolumeRef<int16_t> >( filename )
{
}


bool LabelReader::read( VolumeRef<int16_t> & vol, int border,
                        const string* format )
{
  Finder	f;
  if( ! f.check( _filename ) || f.objectType() != "Volume" )
    return false;

  if( f.dataType() == "S16" )
    return Reader<VolumeRef<int16_t> >::read( vol, border, format );

  // allow on-fly conversion of byte formats
  if( f.dataType() == "U8" )
  {
    Reader<VolumeRef<::byte> >		reader( _filename );
    VolumeRef< ::byte >			tmp;
    Converter< carto::VolumeRef<::byte>, carto::VolumeRef<int16_t> >	conv;
    if( reader.read( tmp, border, format ) )
    {
      carto::VolumeRef<int16_t>	*v = conv( tmp );
      vol = *v;
      delete v;
      return true;
    }
  }

  return false;
}
