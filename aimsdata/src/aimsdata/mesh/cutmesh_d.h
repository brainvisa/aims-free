#ifndef AIMS_MESH_CUTMESH_D_H
#define AIMS_MESH_CUTMESH_D_H

#include <aims/mesh/cutmesh.h>
#include <aims/mesh/surfacemanip.h>

namespace aims
{

  template <typename T>
  CutTexturedMesh<T>::CutTexturedMesh(
    const std::vector<const AimsSurfaceTriangle *> & insurf,
    const std::vector<std::vector<const TimeTexture<T> *> > & intex,
    const Point4df & plane )
    : CutMesh( insurf, plane ), _intex( intex )
  {
  }


  template <typename T>
  CutTexturedMesh<T>::CutTexturedMesh(
    const std::vector<const AimsSurfaceTriangle *> & insurf,
    const std::vector<std::vector<carto::rc_ptr<TimeTexture<T> > > > & intex,
    const Point4df & plane )
    : CutMesh( insurf, plane )
  {
    setTextures( intex );
  }


  template <typename T>
  CutTexturedMesh<T>::~CutTexturedMesh()
  {
  }


  template <typename T>
  void CutTexturedMesh<T>::setTextures(
      const std::vector<std::vector<carto::rc_ptr<TimeTexture<T> > > >
      & intex )
  {
    _intexrc = intex;
    _intex.clear();
    _intex.resize( intex.size(), std::vector<const TimeTexture<T> *>() );
    typename std::vector<std::vector<
      carto::rc_ptr<TimeTexture<T> > > >::const_iterator
        it, et = intex.end();
    typename std::vector<std::vector<const TimeTexture<T> *> >::iterator ipt;
    typename std::vector<carto::rc_ptr<TimeTexture<T> > >::const_iterator
      iit, eit;

    for( it=intex.begin(), ipt=_intex.begin(); it!=et; ++it, ++ipt )
      for( iit=it->begin(), eit=it->end(); iit!=eit; ++iit )
        ipt->push_back( iit->get() );
  }


  template <typename T>
  void CutTexturedMesh<T>::setTextures(
      const std::vector<std::vector<const TimeTexture<T> *> > & intex )
  {
    _intex = intex;
    _intexrc.clear();
  }


  template <typename T>
  void CutTexturedMesh<T>::initializeOutputTextures()
  {
    _cuttex.clear();
    _cuttex.resize( _insurf.size() );
    unsigned i, n = _intex.size();
    if( n > _insurf.size() )
      n = _insurf.size();
    for( i=0; i<n; ++i )
      _cuttex[i].resize( _intex[i].size(),
                         carto::rc_ptr<TimeTexture<T> >( new TimeTexture<T> )
                       );
  }


  template <typename T>
  void CutTexturedMesh<T>::addTexturePoint( int mesh, uint v )
  {
    std::vector<const TimeTexture<T> *> & itexs = _intex[ mesh ];
    std::vector<carto::rc_ptr<TimeTexture<T> > > & otexs = _cuttex[ mesh ];
    typename std::vector<const TimeTexture<T> *>::const_iterator
      itex, etex = itexs.end();
    typename std::vector<carto::rc_ptr<TimeTexture<T> > >::iterator otex;

    // add one texture point for each input texture
    for( itex=itexs.begin(), otex=otexs.begin(); itex!=etex; ++itex, ++otex )
    {
      const Texture<T> & tex = (*itex)->begin()->second;
      (**otex)[0].push_back( tex[v] );
    }
  }


  namespace
  {

    template <typename T>
    inline T _interpol( const T & v1, float w1, const T & v2, float w2 )
    {
      return ( v1 * w1 + v2 * w2 ) / ( w1 + w2 );
    }


    template <>
    inline int32_t _interpol( const int32_t & v1, float w1,
                              const int32_t & v2, float w2 )
    {
      return int32_t( rint( ( v1 * w1 + v2 * w2 ) / ( w1 + w2 ) ) );
    }


    template <>
    inline int16_t _interpol( const int16_t & v1, float w1,
                              const int16_t & v2, float w2 )
    {
      return int16_t( rint( ( v1 * w1 + v2 * w2 ) / ( w1 + w2 ) ) );
    }


    template <>
    inline uint32_t _interpol( const uint32_t & v1, float w1,
                               const uint32_t & v2, float w2 )
    {
      return uint32_t( rint( ( v1 * w1 + v2 * w2 ) / ( w1 + w2 ) ) );
    }


    template <>
    inline uint16_t _interpol( const uint16_t & v1, float w1,
                               const uint16_t & v2, float w2 )
    {
      return uint16_t( rint( ( v1 * w1 + v2 * w2 ) / ( w1 + w2 ) ) );
    }


    template <>
    inline Point2df _interpol( const Point2df & v1, float w1,
                      const Point2df & v2, float w2 )
    {
      return ( v1 * w1 + v2 * w2 ) / ( w1 + w2 );
    }

  }


  template <typename T>
  void CutTexturedMesh<T>::addTextureInterpolPoint( int mesh, uint v, float w1,
                                                    uint w, float w2 )
  {
    std::vector<const TimeTexture<T> *> & itexs = _intex[ mesh ];
    std::vector<carto::rc_ptr<TimeTexture<T> > > & otexs = _cuttex[ mesh ];
    typename std::vector<const TimeTexture<T> *>::const_iterator
      itex, etex = itexs.end();
    typename std::vector<carto::rc_ptr<TimeTexture<T> > >::iterator otex;

    // add one texture point for each input texture
    for( itex=itexs.begin(), otex=otexs.begin(); itex!=etex; ++itex, ++otex )
    {
      const Texture<T> & tex = (*itex)->begin()->second;
      (**otex)[0].push_back( _interpol( tex[v], w1, tex[w], w2 ) );
    }
  }


}

#endif
