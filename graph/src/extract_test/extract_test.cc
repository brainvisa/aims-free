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
