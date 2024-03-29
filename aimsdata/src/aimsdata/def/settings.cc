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

// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/def/settings.h>
#include <aims/getopt/getopt2.h>
#include <aims/def/path.h>
#include <cartobase/stream/fileutil.h>
//#include <soma-io/reader/pythonreader.h>
#include <soma-io/io/reader.h>

//debug
#include <iostream>

using namespace carto;
using namespace std;
using namespace aims;

Settings & Settings::settings()
{
  static Settings	*s = new Settings;
  return *s;
}


Settings::Settings() 
  : AttributedObject( "aims_settings" ), _perso( "aims_settings" )
{
  // read site-wide settings
  try
    {
      carto::Reader<GenericObject>	pr( Path::singleton().dependencies() 
                            + FileUtil::separator() + "aimsrc" );
      pr.read( *this );
    }
  catch( exception & )
    {
      // cerr << "could not read AIMS globl settings: " << e.what() << endl;
    }

  // read personal settings
  try
    {
      carto::Reader<GenericObject> 
        ppr( Path::singleton().home() + FileUtil::separator() + ".aimsrc" );
      ppr.read( _perso );
    }
  catch( exception & )
    {
      // cerr << "could not read AIMS personal settings: " << e.what() << endl;
    }

  // copy personal settings to add / override globals
  copyProperties( Object::reference
                  ( _perso.value<AttributedObject::ContentType>() ) );

  AimsApplication *app = AimsApplication::globalApplication();
  if ( app )
  {
    if ( app->output_4d_volumes >= 0 )
    {
      setProperty( "spm_output_4d_volumes", app->output_4d_volumes );
      setProperty( "nifti_output_4d_volumes", app->output_4d_volumes );
    }
  }
}

