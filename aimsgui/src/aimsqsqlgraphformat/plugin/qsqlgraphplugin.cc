/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
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

#include <aims/plugin/qsqlgraphplugin.h>
#include <aims/io/qsqlgraphformat.h>
#include <aims/io/finder.h>
// #include <aims/def/path.h>
// #include <qapplication.h>

using namespace aims;
using namespace carto;
using namespace std;

namespace
{

  /* This sets Qt plugins path for binary packages (actually needed only on
     Mac/Fink Qt), according to an arbitrary path */
//   bool initQtPlugins()
//   {
//     const Path          & p = Path::singleton();
//     const string        & shared = p.globalShared();
// 
//     if( !shared.empty() )
//       {
//         char    sep = FileUtil::separator();
//         string  base = FileUtil::dirname( shared ) + sep
//           + "lib" + sep + "qt3-plugins";
// 
//         QStringList     paths = QApplication::libraryPaths();
//         if( paths.find( base.c_str() ) == paths.end() )
//           QApplication::addLibraryPath( base.c_str() );
//       }
//     return true;
//   }

  bool initqsqlgraphformat()
  {
//#ifdef __APPLE__
//     initQtPlugins();
//#endif

    new QSqlGraphPlugin;
    return true;
  }

  bool qsqlgraphformatinit = initqsqlgraphformat();

}


QSqlGraphPlugin::QSqlGraphPlugin() : Plugin()
{
  QSqlGraphFormat       *df1 = new QSqlGraphFormat;
  vector<string>        exts;
  string                format = "QSqlGraph";
  exts.push_back( "sqlite" );
  // TODO: add other database formats

  FileFormatDictionary<Graph>::registerFormat( format, df1, exts );
  Finder::registerFormat( format, new FinderQSqlGraphFormat, exts );
}


QSqlGraphPlugin::~QSqlGraphPlugin()
{
}


string QSqlGraphPlugin::name() const
{
  return "Qt SQL graph format IO";
}


bool QSqlGraphPlugin::noop()
{
  return true;
}


