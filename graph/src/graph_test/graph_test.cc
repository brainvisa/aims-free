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

using namespace std;

int main()
{
	GraphPrinter visitor(cout);

	Graph graph("aGraph");
	float f= 1.11111111111111111111;
	graph.setProperty("HGHGHGHGHG", f);
	set<Vertex*> vset;
	Vertex* v1 = graph.addVertex("1");
	vset.insert(v1);
	v1->setProperty("VVVVVVVVVV", f);
	Vertex* v2 = graph.addVertex("2");
	vset.insert(v2);
	Vertex* v3 = graph.cloneVertex(*(vset.begin()));
	vset.insert(v3);
	graph.addDirectedEdge(v1, v2, "e");
	cout << "=========================" << endl;
	cout << graph;
	cout << "-------------------------" << endl;
	cout << "isDirected()=" << (graph.isDirected() ? "true" : "false") << endl;
	cout << "isUndirected()=" << (graph.isUndirected() ? "true" : "false") << endl;
	cout << "-------------------------" << endl;

	set<Edge*> eset;
	set<Edge*>::iterator i;
	eset = v1->edgesTo(v3);
	cout << "v1 -> v3 :";
	for (i = eset.begin(); i != eset.end(); ++i)
		cout << ' ' << *i;
	cout << endl;
	eset = v3->edgesTo(v1);
	cout << "v3 -> v1 :";
	for (i = eset.begin(); i != eset.end(); ++i)
		cout << ' ' << *i;
	cout << endl;
	eset = v1->edgesTo(v2);
	cout << "v1 -> v2 :";
	for (i = eset.begin(); i != eset.end(); ++i)
		cout << ' ' << *i;
	cout << endl;
	eset = v2->edgesTo(v1);
	cout << "v2 -> v1 :";
	for (i = eset.begin(); i != eset.end(); ++i)
		cout << ' ' << *i;
	cout << endl;
	eset = v2->edgesTo(v3);
	cout << "v2 -> v3 :";
	for (i = eset.begin(); i != eset.end(); ++i)
		cout << ' ' << *i;
	cout << endl;
	eset = v3->edgesTo(v2);
	cout << "v3 -> v2 :";
	for (i = eset.begin(); i != eset.end(); ++i)
		cout << ' ' << *i;
	cout << endl;
	eset = v1->edgesTo(v1);
	cout << "v1 -> v1 :";
	for (i = eset.begin(); i != eset.end(); ++i)
		cout << ' ' << *i;
	cout << endl;
	eset = v2->edgesTo(v2);
	cout << "v2 -> v2 :";
	for (i = eset.begin(); i != eset.end(); ++i)
		cout << ' ' << *i;
	cout << endl;
	eset = v3->edgesTo(v3);
	cout << "v3 -> v3 :";
	for (i = eset.begin(); i != eset.end(); ++i)
		cout << ' ' << *i;
	cout << endl;

	graph.removeVertex(v2);
}
