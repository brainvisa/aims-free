
// activate deprecation warning
#ifdef AIMSDATA_CLASS_NO_DEPREC_WARNING
#undef AIMSDATA_CLASS_NO_DEPREC_WARNING
#endif

#include <aims/distancemap/meshmorphomat_d.h>
#include <aims/distancemap/distancemap_g.h>
#include <aims/morphology/morphology_g.h>

using namespace aims;
using namespace aims::meshdistance;
using namespace std;


TimeTexture<short> aims::meshdistance::MeshDilationStepbyStep( const AimsSurface<3,Void> & mesh, 
							       const Texture<short> & inittex, 
							       const float dist,bool connexity )
{
  
  TimeTexture<short>	tex;
  tex = MeshVoronoiStepbyStep( mesh, inittex, dist, connexity,true );
  return(tex);
  
}
  
TimeTexture<short> aims::meshdistance::MeshErosionStepbyStep( const AimsSurface<3,Void> & mesh, 
							      const Texture<short> & inittex, 
							      const float dist,bool connexity )
{
  
  TimeTexture<short>	tex;
  tex = MeshVoronoiStepbyStep( mesh, inittex, dist, connexity,false );
  return(tex);
  
}  

Texture<float> aims::meshdistance::MeshGreyDilation( const AimsSurface<3,Void> & mesh, 
						     const Texture<float> & inittex,
						     const unsigned dist )
{
  Texture<float>			tex,texTemp = inittex;
  const vector<Point3df>		& vert = mesh.vertex();
  const vector< AimsVector<uint,3> >	& poly = mesh.polygon();
  unsigned				i, n = vert.size();
  
  ASSERT( inittex.nItem() == n );
  tex.reserve( n );
  texTemp.reserve( n );
  for (i=0; i<n; ++i)
    {
      tex.push_back(0);
    }
  
  // neighbours map
  
  map<unsigned, set<unsigned> >	neighbours;
  unsigned			v1, v2, v3;
  
  for( i=0; i<poly.size(); ++i )
    {
      v1 = poly[i][0];
      v2 = poly[i][1];
      v3 = poly[i][2];
      
      neighbours[v1].insert( v2 );
      neighbours[v2].insert( v1 );
      neighbours[v1].insert( v3 );
      neighbours[v3].insert( v1 );
      neighbours[v2].insert( v3 );
      neighbours[v3].insert( v2 );
    }
  
  //
  set<float>		  	values;
  set<unsigned>::iterator	iv,ev;
  unsigned 			j;
  Point3df			pos;
  float 			dl;
  
  for (j=0; j<dist; ++j)
    {
      for (i=0; i<n; ++i)
	{
	  values.clear();
	  for (iv = neighbours[i].begin(), ev = neighbours[i].end(); iv != ev; ++iv)
	    {
	      pos = vert[i] - vert[*iv];
	      dl =  pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2];
	      if (dl < 2)
		values.insert( texTemp.item(*iv) );
	    }
	  if (values.empty())
	    tex.item(i) = texTemp.item(i);
	  else
	    tex.item(i) =  *( values.rbegin() ) ; 
	}
      texTemp = tex;
    }
  
  return( tex );
}





Texture<float> aims::meshdistance::MeshGreyErosion( const AimsSurface<3,Void> & mesh, 
						    const Texture<float> & inittex,
						    const unsigned dist )
{
  Texture<float>			tex,texTemp = inittex;
  const vector<Point3df>		& vert = mesh.vertex();
  const vector< AimsVector<uint,3> >	& poly = mesh.polygon();
  unsigned				i, n = vert.size();

  ASSERT( inittex.nItem() == n );
  tex.reserve( n );
  texTemp.reserve( n );
  for (i=0; i<n; ++i)
    {
      tex.push_back(0);
    }

  // neighbours map

  map<unsigned, set<unsigned> >	neighbours;
  unsigned			v1, v2, v3;

  for( i=0; i<poly.size(); ++i )
    {
      v1 = poly[i][0];
      v2 = poly[i][1];
      v3 = poly[i][2];
     
      neighbours[v1].insert( v2 );
      neighbours[v2].insert( v1 );
      neighbours[v1].insert( v3 );
      neighbours[v3].insert( v1 );
      neighbours[v2].insert( v3 );
      neighbours[v3].insert( v2 );
    }
 
  //
  set<float>		  	values;
  set<unsigned>::iterator	iv,ev;
  unsigned 			j;
  Point3df			pos;
  float 			dl;

  for (j=0; j<dist; ++j)
    {
      for (i=0; i<n; ++i)
	{
	  values.clear();
	  for (iv = neighbours[i].begin(), ev = neighbours[i].end(); iv != ev; ++iv)
	    {
	      pos = vert[i] - vert[*iv];
	      dl =  pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2];
	      if (dl < 2)
		values.insert( texTemp.item(*iv) );
	    }
	  if (values.empty())
	    tex.item(i) = texTemp.item(i);
	  else
	    tex.item(i) =  *( values.begin() ) ; 
	}
      texTemp = tex;
    }
  
  return( tex );
}
   

//Instanciation of template functions

namespace aims
{
  namespace meshdistance 
  {
    template
    Texture<short> MeshDilation( const AimsSurface<3,Void> & mesh, 
				 const Texture<short> & inittex,
				 const short & Back, const short & For,
				 const float dist,bool connexity );

    template
    Texture<float> MeshDilation( const AimsSurface<3,Void> & mesh, 
				 const Texture<float> & inittex,
				 const float & Back, const float & For,
				 const float dist,bool connexity );

    template
    Texture<std::set<short> > MeshDilation( const AimsSurface<3,Void> & mesh, 
                                            const Texture<std::set< short> > & inittex,
                                            const std::set<short> & Back, const std::set<short> & For,
                                            const float dist,bool connexity );
    template
    Texture<short> MeshErosion( const AimsSurface<3,Void> & mesh, 
				const Texture<short> & inittex, 
				const short & Back, const short & For,
				const float dist,bool connexity );

    template
    Texture<float> MeshErosion( const AimsSurface<3,Void> & mesh, 
				const Texture<float> & inittex, 
				const float & Back, const float & For,
				const float dist,bool connexity );

    template
    Texture<short> LineariseSulci( const AimsSurface<3,Void> & mesh, 
				   const Texture<short> & inittex,
				   const short & label,
				   const short & Back, const short & For,
				   unsigned & min, unsigned & max, unsigned nbDil);  

    template Texture<int16_t>
    MeshSkeletization( const AimsSurface<3,Void> & mesh,
                       const Texture<int16_t> & inittex,
                       int16_t label, int16_t Back,
                       const std::vector<std::list<unsigned> >  & neigho );

    template
    Texture<int16_t> CloseSulci(
      const AimsSurface<3,Void> & mesh,
      const Texture<int16_t> & sulctex,
      float nbDilMax,bool connexity,
      const int16_t & Back, const int16_t & For,
      const std::vector<std::list<unsigned> >  & neigho );

    template
    Texture<int16_t> CloseSulci(
      const AimsSurface<3,Void> & mesh,
      const Texture<int16_t> & sulctex,
      float nbDilMax,bool connexity,
      const int16_t & Back, const int16_t & For,
      const std::vector<std::list<unsigned> >  & neigho,
      const std::set<int16_t> & label_forbidden );


  }
}
