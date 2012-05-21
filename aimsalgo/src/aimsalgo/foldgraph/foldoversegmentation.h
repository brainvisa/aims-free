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


#ifndef AIMS_FOLDGRAPH_FOLDOVERSEGMENTATION_H
#define AIMS_FOLDGRAPH_FOLDOVERSEGMENTATION_H

#include <aims/vector/vector.h>
#include <aims/bucket/bucketMap.h>
#include <cartodata/volume/volume.h>
class Graph;
class Vertex;


namespace aims
{

  class FoldArgOverSegment
  {
  public:
    FoldArgOverSegment( Graph* g );
    ~FoldArgOverSegment();
    Vertex * splitVertex( Vertex* v, const Point3d & pos,
                          size_t minsize = 50 );
    /// split along a path defined by a series of points (dotted line)
    Vertex * splitVertex( Vertex* v, const std::list<Point3d> & pos,
                          size_t minsize = 50 );
    void printSplitInSkeleton( carto::rc_ptr<carto::Volume<int16_t> > skel,
                               const Vertex* v1, const Vertex* v2 );
    /** Splits a vertex into pieces of more or less piecesize voxels.
        @return number of vertices actually created
    */
    int subdivizeVertex( Vertex* v, float piecelength = 20,
                         size_t minsize = 50,
                         std::set<Vertex *> *newvertices = 0 );
    /// same as subdivizeVertex but on all vertices of the graph
    int subdivizeGraph( float piecelength = 20, size_t minsize = 50,
                         std::set<Vertex *> *newvertices = 0 );

    /** Trace a split line along a path defined by a series of points
        (dotted line) on a bucket.
        @return the line as a bucket
    */
    static carto::rc_ptr<BucketMap<Void> > splitLineOnBucket(
      carto::rc_ptr<BucketMap<Void> > bucket, const std::list<Point3d> & pos );
    /** split along a path defined by a series of points (dotted line).
        The initial bucket will be truncated to be one of the split parts.
        splitline should really separate the input bucket into distinct
        connected components (normally the result of splitLineOnBucket() ).
        @return a new bucket with the "other" part of the split
    */
    static carto::rc_ptr<BucketMap<Void> > splitBucket(
      carto::rc_ptr<BucketMap<Void> > bucket,
      carto::rc_ptr<BucketMap<Void> > splitline,
      size_t minsize = 50 );

  private:
    Graph *_graph;
  };

}

#endif

