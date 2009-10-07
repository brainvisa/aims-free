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
 *  Texture class
 */
#include <aims/mesh/tex2graph_d.h>
#include <aims/graph/graphmanip.h>
#include <aims/io/datatypecode.h>
#include <graph/graph/graph.h>
#include <stdio.h>

using namespace aims;
using namespace std;
using namespace carto;

template <class T>
Tex2Graph<T>::Tex2Graph()
{
  
}

template <class T>
Tex2Graph<T>::~Tex2Graph()
{
}

namespace aims 
{
  template class Tex2Graph<short>;
  template class Tex2Graph<std::set<short> >;

  //template
  //void Tex2Graph::makeGraph( Graph & g, const AimsSurfaceTriangle & mesh, 
  //                           const Texture<short> & tex,
  //                           const std::map<short,std::string> & lab2name, float epsilon = 0.01);
  //template
  //void Tex2Graph::makeGraph( Graph & g, const AimsSurfaceTriangle & mesh, 
  //                           const Texture<short> & tex, float epsilon = 0.01);
}
