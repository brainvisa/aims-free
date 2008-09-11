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

#include <aims/graph/graphiterator.h>
#include <cartobase/object/object.h>

using namespace std;
using namespace aims;
using namespace carto;

struct GraphIterator::Private
{
  Private();
  Private( const Private & );

  Graph::iterator	iterator;
  Graph			*graph;
  string		attribute;
};


GraphIterator::Private::Private()
{
}


GraphIterator::Private::Private( const Private & x )
  : iterator( x.iterator ), graph( x.graph ), attribute( x.attribute )
{
}


GraphIterator::GraphIterator() : d( new Private )
{
}


GraphIterator::GraphIterator( const GraphIterator & x )
  : d( new Private( *x.d ) )
{
}


GraphIterator::~GraphIterator()
{
  delete d;
}


Vertex *GraphIterator::vertex()
{
  return *d->iterator;
}


Graph *GraphIterator::graph()
{
  return d->graph;
}


std::string GraphIterator::attribute() const
{
  return d->attribute;
}


GraphIterator & GraphIterator::operator ++ ()
{
  ++d->iterator;
  return *this;
}


GraphIterator & GraphIterator::operator -- ()
{
  --d->iterator;
  return *this;
}


GenObject * GraphIterator::operator * ()
{
  //return TypedObject<rc_ptr<Quoi> >();
  // ici on va avoir besoin de graphes stockant leurs machins 
  // par des GenObject ...
  return 0;
}


