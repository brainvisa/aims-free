#include <graph/graph/graph.h>
#include <graph/graph/greader.h>
#include <graph/graph/gwriter.h>
#include <graph/graph/gprinter.h>
#include <cartobase/object/sreader.h>
#include <cartobase/exception/parse.h>

using namespace carto;
using namespace std;

int main( int /* argc */, char** /* argv */ )
{
	try {
		SyntaxSet       ss;
		SyntaxReader    sr("/home/appli/graph-main/shared/fold.stx");
		sr >> ss;

		Graph           graph;
		GraphReader     gr("/home/appli/graph-main/shared/dionysos2.arg", ss);
		gr >> graph;

		GraphWriter     gw("/tmp/dionysos2.arg", ss);
		gw << graph;

		cout << graph;
	}
	catch (exception& e) {
		cout << e.what() << endl;
	}
}
