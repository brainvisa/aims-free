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
 *  IO classes
 */

#include <aims/io/fileFormat_d.h>
#include <aims/io/baseFormats_graph.h>
#include <graph/graph/graph.h>


using namespace aims;
using namespace std;

namespace aims
{

  // The following line is required for compilation on gcc 3.3
  template<> void FileFormatDictionary<Graph>::registerBaseFormats();

  template<> const map<string, list<string> >
  & FileFormatDictionary<Graph>::extensions()
  {
    init();
    map<string, list<string> >	& ext = _extensions();
    const map<string, list<string> >
      & volext = FileFormatDictionary<AimsData<short> >::extensions();
    map<string, list<string> >::const_iterator	ive, eve = volext.end();
    list<string>::const_iterator                ie, ee;

    for( ive=volext.begin(); ive!=eve; ++ive )
    {
      for( ie=ive->second.begin(), ee=ive->second.end(); ie!=ee; ++ie )
        if( *ie == "GraphVolume" )
          break;
      if( ie == ee )
        ext[ ive->first ].push_back( "GraphVolume" );
    }

    return ext;
  }


  template<> void FileFormatDictionary<Graph>::registerBaseFormats()
  {
    vector<string>	ext;
    ext.push_back( "arg" );
    ArgFormat		*af = new ArgFormat;
    registerFormat( "ARG", af, ext );
    _formats()[ "GraphVolume" ] = new GraphVolumeFormat;
  }

}

template class FileFormatDictionary<Graph>;
template class FileFormat<Graph>;


static bool _graphdic()
{
  FileFormatDictionary<Graph>::init();
  return true;
}

static bool graphdic = _graphdic();

