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



#ifndef AIMS_DISTANCEMAP_MESHMORPHOMAT_H
#define AIMS_DISTANCEMAP_MESHMORPHOMAT_H


#include <set>
#include <list>
#include <map>
#include <aims/connectivity/connectivity.h>
#include <aims/mesh/texture.h>
#include <aims/mesh/surface.h>
class Graph;
namespace aims
{
  namespace meshdistance
  {

    
    template <class T>
    Texture<T> MeshDilation( const AimsSurface<3,Void> & mesh, 
			     const Texture<T> & inittex, 
			     const T & Back, const T & For,
			     const float dist,bool connexity );
    template <class T>
    Texture<T> MeshErosion( const AimsSurface<3,Void> & mesh, 
			    const Texture<T> & inittex, 
			    const T & Back, const T & For,
			    const float dist,bool connexity );

    TimeTexture<short> 
    MeshDilationStepbyStep( const AimsSurface<3,Void> & mesh, 
			    const Texture<short> & inittex, 
			    const float dist,bool connectivity );

    TimeTexture<short> 
    MeshErosionStepbyStep( const AimsSurface<3,Void> & mesh, 
			   const Texture<short> & inittex, 
			   const float dist,bool connectivity );

    Texture<float> MeshGreyDilation( const AimsSurface<3,Void> & mesh, 
				     const Texture<float> & inittex,
				     const unsigned dist );

    Texture<float> MeshGreyErosion( const AimsSurface<3,Void> & mesh, 
				    const Texture<float> & inittex,
				    const unsigned dist );
  
  
    template<class T>
    Texture<T> SplitSulci( const AimsSurface<3,Void> & mesh, 
			   const Texture<T> & inittex,
			   const T & Back, const T & For ,
			   const T label,
			   const unsigned min, const unsigned max);
  
    
    template<class T>
    Texture<T> LineariseSulci( const AimsSurface<3,Void> & mesh, 
			       const Texture<T> & inittex,
			       const T & label,
			       const T & Back, const T & For,
			       unsigned & min, unsigned & max,unsigned nbDil);
  
    /** Close the sulcus map (dilation and skeletization).
	All the labels/components are processed */
    template<class T>
    Texture<T> CloseSulci( const AimsSurface<3,Void> & mesh, 
			   const Texture<T> & sulctex,
			   float nbDilMax,bool connexity,
			   const T & Back, const T & For,
			   const std::vector<std::list<unsigned> >  & neigho, 
			   const std::set<T> & label_forbidden);
    template<class T>
    Texture<T> CloseSulci( const AimsSurface<3,Void> & mesh, 
			   const Texture<T> & sulctex,
			   float nbDilMax,bool connexity,
			   const T & Back, const T & For,
			   const std::vector<std::list<unsigned> >  & neigho);
    template<class T>
    unsigned NbOfPoint(const Texture<T> tex, const T label);
    
    template<class T>
    bool HasHole(const Texture<T> &tex,
		       const AimsSurface<3,Void> & mesh, const T label );

#if 0  // these are internal functions implemented as static functions
    /// Tell if a point (whose ordonned neighbourhood is neigho) is simple   
    template<class T>
    inline bool SimplePoint( const Texture<T> & tex,
			     const std::list<unsigned>  & neigho);

    /** Tell if a point (whose ordonned neighbourhood is neigho) is immortal 
	(squeletization) */
    template<class T>
    inline bool ImmortalPoint( const Texture<T> & tex,
			       const std::list<unsigned>  & neigho);
#endif

    /** Skeletization of the components of the innitex with label label.
	neigho is the ordonned neighbourhood for each point. 
	The output texture has only two labels :
	The background (0) and the object (label) */
    template<class T> Texture<T> 
    MeshSkeletization( const AimsSurface<3,Void> & mesh, 
		       const Texture<T> & inittex,
		       T label,T Back,
		       const std::vector<std::list<unsigned> >  & neigho );




    /// global variable...
    extern  const short MESHDISTANCE_FORBIDDEN;
    extern  const short MESHDISTANCE_UNREACHED;

  }
}


#endif
