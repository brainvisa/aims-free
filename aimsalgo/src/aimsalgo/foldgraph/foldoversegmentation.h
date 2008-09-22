/* Copyright (c) 1995-2008 CEA
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


#ifndef AIMS_FOLDGRAPH_FOLDOVERSEGMENTATION_H
#define AIMS_FOLDGRAPH_FOLDOVERSEGMENTATION_H

#include <aims/vector/vector.h>
#include <cartodata/volume/volume.h>
class Graph;
class Vertex;


/*
in case of split:
- duplicate/separate ss, bottom, other: OK
- add a jonction:                       OK
- [ or add a pp (?) ]
- duplicate/separate hull_junction      OK
- separate all existing rels -> how ?   OK junction, hull_junction, pp
                                        not done cortical
*/

namespace aims
{

  class FoldArgOverSegment
  {
  public:
    FoldArgOverSegment( Graph* g );
    ~FoldArgOverSegment();
    Vertex * splitVertex( Vertex* v, const Point3d & pos );
    /** Convert a folds graph back to a skeleton image.
        This is needed after a fold split to rebuild relations: the graph
        structure must be recalculated.
        The old skeleton image is still needed to get the brain hull.
    */
    carto::rc_ptr<carto::Volume<int16_t> >
      argBackToSkeleton( const carto::rc_ptr<carto::Volume<int16_t> >
        oldskel );
    void printSplitInSkeleton( carto::rc_ptr<carto::Volume<int16_t> > skel,
                               const Vertex* v1, const Vertex* v2 );

  private:
    Graph *_graph;
  };

}

#endif

