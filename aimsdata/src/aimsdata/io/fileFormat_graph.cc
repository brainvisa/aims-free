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

/*
 *  IO classes
 */

#include <aims/io/fileFormat_d.h>
#include <aims/io/baseFormats_graph.h>
#include <aims/io/baseFormats_syntax.h>
#include <aims/io/argR.h>
#include <graph/graph/graph.h>


using namespace aims;
using namespace std;
using namespace carto;

namespace aims
{

  // The following line is required for compilation on gcc 3.3
  template<> void FileFormatDictionary<Graph>::registerBaseFormats();
  template<> void FileFormatDictionary<SyntaxSet>::registerBaseFormats();
  
  template<> const map<string, list<string> >
  & FileFormatDictionary<Graph>::extensions()
  {
    init();
    map<string, list<string> > & ext = _extensions();
    const map<string, list<string> >
      & volext = FileFormatDictionary<AimsData<short> >::extensions();
    map<string, list<string> >::const_iterator	ive, eve = volext.end();
    list<string>::const_iterator                ie, ee;

    mutex().lock();
    for( ive=volext.begin(); ive!=eve; ++ive )
    {
      list<string> & gext = ext[ ive->first ];
      for( ie=gext.begin(), ee=gext.end(); ie!=ee; ++ie )
        if( *ie == "GraphVolume" )
          break;
      if( ie == ee )
        gext.push_back( "GraphVolume" );
    }
    mutex().unlock();

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


  template<> void FileFormatDictionary<SyntaxSet>::registerBaseFormats()
  {
    vector<string>      ext;
    ext.push_back( "stx" );
    StxFormat           *af = new StxFormat;
    registerFormat( "STX", af, ext );
  }

}


template class FileFormatDictionary<Graph>;
template class FileFormat<Graph>;
template class FileFormatDictionary<SyntaxSet>;
template class FileFormat<SyntaxSet>;


static bool _graphdic()
{
  FileFormatDictionary<Graph>::init();
  FileFormatDictionary<SyntaxSet>::init();
  ArgReader::initLowLevelReaders();
  return true;
}

static bool graphdic = _graphdic();

