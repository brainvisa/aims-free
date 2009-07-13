/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

/*
 *  Paths required for A.I.M.S.
 */
#include <cstdlib>
#include <aims/def/path.h>
#include <aims/def/general.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/stream/directory.h>
#include <cartobase/config/paths.h>
#include <cartobase/config/version.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


using namespace std;
using namespace aims;
using namespace carto;

Path::Path()
{
  const char	*aims_path = getenv( "AIMS_PATH" );
  char		s = FileUtil::separator();
  Directory	d( "/" );
  const string	& _shared = Paths::globalShared();

  if( aims_path )
    {
      _aims = aims_path;
      d.chdir( _aims );
      if( !d.isValid() )
        _aims = "";
    }
  if( _aims.empty() )
    {
      _aims = _shared + s + "aims-" + carto::cartobaseShortVersion();
      d.chdir( _aims );
      if( !d.isValid() )
        {
#ifdef _WIN32
          const string	hardpath = "c:\\share\\aims-";
#else
          const string	hardpath = "/home/appli/aimsdata-";
#endif
          _aims = hardpath + carto::cartobaseShortVersion();
          d.chdir( _aims );
          if( !d.isValid() )
            {
              _aims = _shared + s + "aims-main";
              d.chdir( _aims );
              if( !d.isValid() )
                {
                  _aims = _shared + s + "aims";
                  d.chdir( _aims );
                  if( !d.isValid() )
                    {
                      _aims = hardpath + "main";
                      d.chdir( _aims );
                      if( !d.isValid() )
                        _aims = _shared;
                    }
                }
            }
        }
    }

  _dependencies = _aims + s + "shared";
  d.chdir( _dependencies );
  if( !d.isValid() )
    _dependencies = _aims;

  _nomenclature = Paths::shfjShared() + s + "nomenclature";
  Directory	dir( _nomenclature );
  if( !dir.isValid() )
    _nomenclature = _shared + s + "nomenclature";
  _syntax = _nomenclature + s + "syntax";
  _hierarchy = _nomenclature + s + "hierarchy";
}


const string & Path::home() const
{
  // use carto::Path
  return Paths::home();
}


const string & Path::memmap() const
{
  // use carto::Path
  return Paths::memmap();
}


const string & Path::globalShared() const
{
  // use carto::Path
  return Paths::globalShared();
}


const string & Path::shfjShared() const
{
  // use carto::Path
  return Paths::shfjShared();
}


const Path& Path::singleton()
{
  static Path* path = new Path;
  return *path;
}
