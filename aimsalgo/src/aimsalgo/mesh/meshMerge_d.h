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



#include <aims/mesh/meshMerge.h>


template<int D, class T>
void aims::meshMerge( AimsTimeSurface<D,T> & dst, 
		      const AimsTimeSurface<D,T> & add )
{
  std::vector<Point3df> & vert = dst.vertex();
  std::vector<Point3df> & norm = dst.normal();
  std::vector<AimsVector<uint,D> > & poly = dst.polygon();
  const std::vector<Point3df>  & vert2 = add.vertex();
  const std::vector<Point3df>  & norm2 = add.normal();
  const std::vector<AimsVector<uint,D> >  & poly2 = add.polygon();
  unsigned i, j, n = vert.size(), m = vert2.size(), p = poly2.size();

  // copy vertices & normals
  for( i=0; i<m; ++i )
    {
      vert.push_back( vert2[i] );
      norm.push_back( norm2[i] );
    }

  // translate & copy polygons
  for( i=0; i<p; ++i )
    {
      AimsVector<uint,D>  pol;
      for( j=0; j<D; ++j )
	pol[j] = poly2[i][j] + n;
      poly.push_back( pol );
    }
}

template<int D, class T>
void aims::meshMerge( AimsTimeSurface<D,T> & dst, 
		      const std::list<AimsTimeSurface<D,T> > & src )
{
  dst.erase();
  typename std::list<AimsTimeSurface<D,T> >::const_iterator
    im, em = src.end();
  for( im=src.begin(); im!=em; ++im )
    meshMerge( dst, *im );
}
