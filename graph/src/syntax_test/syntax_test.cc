#include <cstdlib>
#include <cartobase/object/syntax.h>
#include <cartobase/object/sreader.h>
#include <cartobase/object/swriter.h>
#include <cartobase/object/sprinter.h>
#include <iostream>
#include <stdlib.h>

using namespace carto;
using namespace std;

int main()
{
	SyntaxSet data;
	try {
		SyntaxReader reader("/home/appli/graph-main/shared/fold.stx");
		reader >> data;
		SyntaxWriter writer("/tmp/fold.stx");
		writer << data;
	}
	catch (exception& e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
	cout << data;
}
