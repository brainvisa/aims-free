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

