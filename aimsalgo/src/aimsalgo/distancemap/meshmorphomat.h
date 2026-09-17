

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
