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

#include <aims/getopt/getopt2.h>
#include <aims/io/reader.h>
#include <aims/io/writer.h>
#include <aims/transformation/affinetransformation3d.h>
#include <aims/transformation/transformation_chain.h>
#include <aims/def/path.h>
#include <cartobase/plugin/plugin.h>
#include <cartobase/stream/fileutil.h>

using namespace aims;
using namespace carto;
using namespace std;


int main( int argc, const char** argv )
{
    try
    {
        // load aimsalgo, if possible.
        // this command is part of aimsdata, however if aimsalgo is loaded,
        // we can support FFD deformation fields for free using the same
        // API, transparently.

        list<PluginLoader::PluginFile>
            & plugins = PluginLoader::pluginFiles();
        char	s = FileUtil::separator();
        plugins.push_back( PluginLoader::PluginFile
                        ( Path::singleton().home() + s + ".aims"
                            + s + "plugins" + s + "aimsdata_algo.plugins",
                            aimsVersion() ) );
        plugins.push_back( PluginLoader::PluginFile
                        ( Path::singleton().dependencies()
                            + s + "plugins" + s + "aimsdata_algo.plugins",
                            aimsVersion() ) );

        list< string > list_r1;
        string r2="";
        string w;

        AimsApplication app( argc, argv, "Composes two or more Aims/anatomist "
                    "transformations: M1 x M2. "
                    "It is possibility to use a list of transformations with "
                    "-i options. If the aimsalgo library is available, "
                    "the command also supports FFD deformation fields and is "
                    "able to combine them into a single FFD transform file "
                    "(then use .ima format for the output)");
        app.addOptionSeries( list_r1, "-i", "M1: 1st input transformation" );
        app.addOption( r2, "-j", "M2: 2nd input transformation [option]", true );
        app.addOption( w, "-o", "output transformation" );
        app.initialize();

        list<string>::const_iterator it = list_r1.begin();
        
        TransformationChain3d chain;
        rc_ptr<Transformation3d> m;
        for( it=list_r1.begin(); it != list_r1.end(); ++it )
        {
            Reader<Transformation3d> mr1( *it );
            m.reset( mr1.read() );
            chain.push_front( m );
        }
        
        if( !r2.empty() )
        {
            Reader<Transformation3d> mr2( r2 );
            m.reset( mr2.read() );
            chain.push_front( m );
        }

        const_ref<Transformation3d> composed = chain.simplify( true );

        Writer<Transformation3d> mw( w );
        mw.write( *composed );

    }
    catch( user_interruption & )
    {
    }
    catch( exception & e )
    {
        cerr << argv[ 0 ] << ": " << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

