#include <graph/graph/graph.h>
#include <graph/graph/gprinter.h>
#include <vector>
#include <exception>

using namespace std;

int main()
{
	try {
		Graph graph;

		Vertex* v1 = graph.addVertex("");
		Vertex* v2 = graph.addVertex("");
		Vertex* v3 = graph.addVertex("");
		Vertex* v4 = graph.addVertex("");

		graph.addUndirectedEdge(v1, v2, "");
		graph.addUndirectedEdge(v1, v3, "");
		graph.addUndirectedEdge(v1, v4, "");
		graph.addUndirectedEdge(v2, v3, "");
		graph.addUndirectedEdge(v2, v4, "");
		graph.addUndirectedEdge(v3, v4, "");

		cout << graph;

		Graph sub;
		vector<Vertex*> tmp;

		tmp.push_back(v1);
		tmp.push_back(v2);
		tmp.push_back(v3);
		tmp.push_back(v4);
		sub.clear();
		graph.extract(sub, tmp.begin(), tmp.end());
		tmp.clear();
		cout << "4 vertices ----------------------------------" << endl;
		cout << sub;

		tmp.push_back(v1);
		tmp.push_back(v2);
		tmp.push_back(v3);
		sub.clear();
		graph.extract(sub, tmp.begin(), tmp.end());
		tmp.clear();
		cout << "3 vertices ----------------------------------" << endl;
		cout << sub;

		tmp.push_back(v1);
		tmp.push_back(v2);
		sub.clear();
		graph.extract(sub, tmp.begin(), tmp.end());
		tmp.clear();
		cout << "2 vertices ----------------------------------" << endl;
		cout << sub;

		tmp.push_back(v1);
		sub.clear();
		graph.extract(sub, tmp.begin(), tmp.end());
		tmp.clear();
		cout << "1 vertex ------------------------------------" << endl;
		cout << sub;
	}
	catch (exception& e) {
		cout << e.what() << endl;
	}
}
