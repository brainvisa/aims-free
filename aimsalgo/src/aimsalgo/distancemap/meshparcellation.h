

#ifndef AIMS_DISTANCEMAP_MESHPARCELLATION_H
#define AIMS_DISTANCEMAP_MESHPARCELLATION_H

#include <aims/mesh/texture.h>
#include <aims/mesh/surface.h>
#include <cartodata/volume/volume.h>
#include <aims/distancemap/stlsort.h>
#include <aims/distancemap/meshvoronoi.h>
#include <set>
#include <map>

class Graph;
namespace aims
{
  namespace meshdistance
  {


    /** Extract the boundarie of a voronoi diagram
	i.e. compute the skiz */
    template <class T>
    Texture<std::set<T> > 
    MeshBorderVoronoi( const AimsSurface<3,Void> & mesh,
                       const Texture<T> & inittex,
                       const std::set<T> & setBack,
                       const std::set<T> & setFor);

    /** Define the gyrus seed from the SKIZ */
    template <class T>
    Texture<std::set<T> > 
    gyrusSeedDefinition( const AimsSurface<3,Void> & mesh, 
                         const Texture<T> & inittex,
                         const std::set<T> & setBack,
                         const std::set<T> & setFor,
                         const std::set<std::set<T> > & labelAllowed);
    template <class T>
    Texture<T> 
    gyrusSeedDilationInSKIZ( const AimsSurface<3,Void> & mesh, 
                             const Texture<T> & seed,
                             const Texture<T> & skiz,
                             const T & Back,
                             const T & For );

    /// Convert a multidimensional boundary map to a texture of short
    template <class T>
    Texture<short> border2Texture( const Texture<std::set<T> > &tex,
                                   const AimsSurface<3,Void> & mesh,
                                   const std::set<T> & setBack,
                                   const std::set<T> & setFor);
  
  
    Texture<std::set<short> > 
    MeshVoronoi2Set( const AimsSurface<3,Void> & mesh, 
		     const Texture<std::set<short> > & inittex, 
		     const Texture<short> & sulcitex,
		     bool connexity);


    
    /* Convert a voronoi diagram whose label are multidimensional
       To a texture of short (for the visualisation) */
    template<class T, class U>
    Texture<short> Voronoi2toTexture(const Texture<std::set<T> > & vor, 
				     const AimsSurface<3,Void> & mesh,
				     const std::set<T> & setBack, 
				     const std::set<T> & setFor);


    AimsSurfaceTriangle MakePlaneMesh(unsigned n, Point3df Xi, Point3df Xf);

    /// Give the surface of labelled parcels defined from textures.
    template<class T>
    std::map<T,float>  SurfaceParcel(const Texture<T> & tex,
				     const AimsSurface<3,Void> & mesh);

    /// Give the surface of labelled parcels defined from volume.
    template<class T>
    std::map<T,float> VolumeParcel(
      const carto::rc_ptr<carto::Volume<T> > & vol);
 
    

    
    std::set<std::pair<short,short> > 
    ModelGraph2Boundaries( const Graph & g, 
			   const std::map<std::string,short> & traduction );

    std::set<std::set<short> > 
    gyrusHierarchy2setBoundaries(const std::map<std::string,short> &trans, 
				 const Graph & g);

    //std::set<std::set<short> > GyrusModel2SetOfSetOfSulci(const char *model,
    //const std::map<std::string,short> &trans );

    std::set<std::set<short> > 
    GyrusModel2SetOfSetOfSulci(const std::map<std::string, 
						   std::set<std::string> > 
						   &gyriAndSulci, 
						   const std::map<std::string,
						   short> &trans );
    std::set<std::string> 
    GyrusModel2SetOfSulci(const char *model, 
			  const std::set<std::string> &existingName );
    
    std::set<std::string> 
    GyrusModel2SetOfSulci(const std::map<std::string, std::set<std::string> > 
			  &gyriAndSulci, 
			  const std::set<std::string> &existingName );
    
    std::map<std::string, std::set<std::string> > 
    GyrusModel2GyriAndSulci( const std::string & model );

    /// global variable...
    extern  const short MESHDISTANCE_FORBIDDEN;
    extern  const short MESHDISTANCE_UNREACHED;

  }
}


#endif
