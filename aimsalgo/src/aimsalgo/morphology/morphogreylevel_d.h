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


#ifndef AIMS_MORPHOLOGY_MORPHOGREYLEVEL_D_H
#define AIMS_MORPHOLOGY_MORPHOGREYLEVEL_D_H

#include <aims/morphology/operatormorpho.h>
#include <aims/data/data.h>
#include <cartodata/volume/volumeutil_d.h>
#include <aims/math/mathelem.h>

namespace aims
{

  template <typename T>
  MorphoGreyLevel<T>::MorphoGreyLevel(): radius(1.), int_radius(1,1,1),
    _use_chamfer( true ), _chamfer_factor( 50.F ),
    _chamfer_mask_size( 3, 3, 3 )
  {
  }


  template <typename T>
  MorphoGreyLevel<T>::~MorphoGreyLevel()
  {
  }


  template <typename T>
  Point3d MorphoGreyLevel<T>::computeIntRadius( float radius,
                                                const Point4df & voxelsize )
  {
    int_radius[0] = int(ceil( radius/voxelsize[0] ));
    int_radius[1] = int(ceil( radius/voxelsize[1] ));
    int_radius[2] = int(ceil( radius/voxelsize[2] ));

    std::cout << "Radius = " << radius << " mm" << std::endl;
    std::cout << "Voxel size = " << voxelsize << " mm" << std::endl;
    std::cout << "Integer radius = " << int_radius << " voxel(s)" << std::endl;

    // Test on dimension
    if( (int_radius[0]==0) || (int_radius[1]==0) || (int_radius[2]==0) )
      {
        std::cout << "ERROR: Radius too small according to voxelsize !!! STOP.\n" << std::endl;
        throw std::runtime_error( "Radius too small according to voxelsize" );
      }

    if( (int_radius[0]==1) || (int_radius[1]==1) || (int_radius[2]==1) )
      {
        std::cout << "WARNING: Radius generate a single voxel !!! STOP.\n" << std::endl;
//         exit(2);
      }

    return int_radius;
  }



  template <typename T>
  std::vector<Point3d> MorphoGreyLevel<T>::doStructElement(
    float radius, const Point4df & voxelsize )
  {
  //   AimsData< int16_t > sphere( 2*int_radius[0]-1,
  //                               2*int_radius[1]-1,
  //                               2*int_radius[2]-1 );
  //   sphere.setSizeXYZT( voxelsize[0],
  //                       voxelsize[1],
  //                       voxelsize[2] );
  //   sphere = 0;

    //   Test if list have been already created
    //   List of the voxel of the strust elem
    float sqradius = radius * radius;
    if( !list.size() )
      {
        for(int u = -int_radius[0]; u <= int_radius[0]; ++u)
          for(int v = -int_radius[1]; v <= int_radius[1]; ++v)
            for(int w = -int_radius[2]; w <= int_radius[2]; ++w)
              {
                if( sqr( float(u) * voxelsize[0] ) +
                    sqr( float(v) * voxelsize[1] ) +
                    sqr( float(w) * voxelsize[2] ) <= sqradius )
                  {
                    list.push_back( Point3d(u, v, w) );
                    //               sphere(u, v, w) = 1;
                  }
              }
        //   Writer< AimsData< int16_t > > ws( "sphere.ima" );
        //   ws.write( sphere );
        std::cout << "Number of voxels in the structuring element = "
          << list.size() << std::endl << std::endl;
        return list;
      }
    else return list;
  }


  template<typename T>
  carto::VolumeRef<T> MorphoGreyLevel<T>::doErosion(
    const carto::VolumeRef< T >& dataIn, float radius )
  {
    std::cout << "EROSION" << std::endl;

    carto::VolumeRef< T > dataOut;

    dataOut = tryChamferErosion( dataIn, radius );
    if( dataOut.get() )
      return dataOut;

    T value = carto::VolumeUtil<T>::max(dataIn);

    std::vector<float> vs = dataIn.getVoxelSize();
    Point4df voxelsize( vs[0], vs[1], vs[2], vs[3] );

    Point4d dim( dataIn->getSizeX(),
                 dataIn->getSizeY(),
                 dataIn->getSizeZ(),
                 dataIn->getSizeT() );

    // FIXME: this copy sometimes gets a shared buffer. WHY ?
    // carto::VolumeRef< T > dataOut( new carto::Volume<T>( dataIn ) );
    dataOut.reset(
      new carto::Volume<T>( dataIn->getSizeX(), dataIn->getSizeY(),
                            dataIn->getSizeZ(), dataIn->getSizeT() ) );
    dataOut->fill( T(0) );
    dataOut->copyHeaderFrom( dataIn->header() );

    int_radius = computeIntRadius( radius, voxelsize );
    list = doStructElement( radius, voxelsize );

    for(int t = 0; t < dim[3]; ++t)
      for(int z = 0; z < dim[2]; ++z)
        {
          //        float pct = float(z*100./dim[2]);
          //         std::cout << "\rPercent done : " << pct << std::flush";
          for(int y = 0; y < dim[1]; ++y)
            for(int x = 0; x < dim[0]; ++x)
              {
                T min = value, tmp;
                for(int l = 0; l < int(list.size()); ++l)
                  {
                    Point3d coord( x + list[l][0],
                                   y + list[l][1],
                                   z + list[l][2] );

                    if( (coord[0]>=0) && (coord[0]<dim[0] )
                        && (coord[1]>=0) && (coord[1]<dim[1] )
                        && (coord[2]>=0) && (coord[2]<dim[2] ) )
                      {
                        tmp = dataIn->at( coord[0], coord[1], coord[2] );
                        if( tmp < min ) min = tmp;
                      }
                  }
                dataOut->at( x, y, z, t ) = min;
              }
        }
    std::cout << std::endl;
    return dataOut;
  }


  template<typename T>
  carto::VolumeRef<T> MorphoGreyLevel<T>::doDilation(
    const carto::VolumeRef< T >& dataIn, float radius )
  {
    std::cout << "DILATION" << std::endl;

    carto::VolumeRef< T > dataOut;

    dataOut = tryChamferDilation( dataIn, radius );
    if( dataOut.get() )
      return dataOut;

    T value = carto::VolumeUtil<T>::min(dataIn);

    std::vector<float> vs = dataIn->getVoxelSize();
    Point4df voxelsize( vs[0], vs[1], vs[2], vs[3] );

    Point4d dim( dataIn->getSizeX(),
                 dataIn->getSizeY(),
                 dataIn->getSizeZ(),
                 dataIn->getSizeT() );

    // FIXME: this copy sometimes gets a shared buffer. WHY ?
//     carto::VolumeRef< T > dataOut( new carto::Volume<T>( dataIn ) );
    dataOut.reset(
      new carto::Volume<T>( dataIn->getSizeX(), dataIn->getSizeY(),
                            dataIn->getSizeZ(), dataIn->getSizeT() ) );
    dataOut->fill( T(0) );
    dataOut->copyHeaderFrom( dataIn.header() );

    int_radius = computeIntRadius( radius, voxelsize );
    list = doStructElement( radius, voxelsize );

    for(int t = 0; t < dim[3]; ++t)
      for(int z = 0; z < dim[2]; ++z)
      {
        float pct = float(z*100./dim[2]);
        std::cout << "\rPercent done : " << pct << std::flush;
        for(int y = 0; y < dim[1]; ++y)
          for(int x = 0; x < dim[0]; ++x)
          {
            T max = value, tmp;
            for(int l = 0; l < int(list.size()); ++l)
            {
              Point3d coord( x + list[l][0],
                              y + list[l][1],
                              z + list[l][2] );

              if( (coord[0]>=0) && (coord[0]<dim[0] )
                  && (coord[1]>=0) && (coord[1]<dim[1] )
                  && (coord[2]>=0) && (coord[2]<dim[2] ) )
              {
                tmp = dataIn->at( coord[0], coord[1], coord[2] );
                if( tmp > max ) max = tmp;
              }
            }
            dataOut->at( x, y, z, t ) = max;
          }
      }
    std::cout << std::endl;
    return dataOut;
  }


  template<typename T>
  carto::VolumeRef<T> MorphoGreyLevel<T>::doClosing(
    const carto::VolumeRef<T>& dataIn, float radius )
  {
    carto::VolumeRef< T > dataOut;

    dataOut = tryChamferClosing( dataIn, radius );
    if( dataOut.get() )
      return dataOut;

    dataOut = doDilation( dataIn, radius );
    dataOut = doErosion( dataOut, radius );

    return dataOut;
  }


  template<typename T>
  carto::VolumeRef<T> MorphoGreyLevel<T>::doOpening(
    const carto::VolumeRef<T>& dataIn, float radius )
  {
    carto::VolumeRef< T > dataOut;

    dataOut = tryChamferOpening( dataIn, radius );
    if( dataOut.get() )
      return dataOut;

    dataOut = doErosion( dataIn, radius );
    dataOut = doDilation( dataOut, radius );

    return dataOut;
  }

  // chamfer-specific implementations

  template<typename T>
  carto::VolumeRef<T> MorphoGreyLevel<T>::tryChamferErosion(
    const carto::VolumeRef< T >& dataIn, float radius )
  {
    return 0;
  }


  template<typename T>
  carto::VolumeRef<T> MorphoGreyLevel<T>::tryChamferDilation(
    const carto::VolumeRef< T >& dataIn, float radius )
  {
    return 0;
  }


  template<typename T>
  carto::VolumeRef<T> MorphoGreyLevel<T>::tryChamferClosing(
    const carto::VolumeRef< T >& dataIn, float radius )
  {
    return 0;
  }


  template<typename T>
  carto::VolumeRef<T> MorphoGreyLevel<T>::tryChamferOpening(
    const carto::VolumeRef< T >& dataIn, float radius )
  {
    return 0;
  }


  template<>
  carto::VolumeRef<int16_t> MorphoGreyLevel<int16_t>::tryChamferErosion(
    const carto::VolumeRef< int16_t >& dataIn, float radius )
  {
    if( _use_chamfer && isBinary( dataIn ) )
    {
      carto::VolumeRef<int16_t> dataIn2 = checkBorderWidth( dataIn );
      return AimsMorphoChamferErosion(
        AimsData<int16_t>(
          const_cast<carto::VolumeRef<int16_t> &>( dataIn2 ) ),
        radius, _chamfer_mask_size[0], _chamfer_mask_size[0],
        _chamfer_mask_size[0], _chamfer_factor ).volume();
    }
    return 0;
  }


  template<>
  carto::VolumeRef<int16_t> MorphoGreyLevel<int16_t>::tryChamferDilation(
    const carto::VolumeRef< int16_t >& dataIn, float radius )
  {
    if( _use_chamfer && isBinary( dataIn ) )
    {
      carto::VolumeRef<int16_t> dataIn2 = checkBorderWidth( dataIn );
      return AimsMorphoChamferDilation(
        AimsData<int16_t>(
          const_cast<carto::VolumeRef<int16_t> &>( dataIn2 ) ),
        radius, _chamfer_mask_size[0], _chamfer_mask_size[0],
        _chamfer_mask_size[0], _chamfer_factor ).volume();
    }
    return 0;
  }


  template<>
  carto::VolumeRef<int16_t> MorphoGreyLevel<int16_t>::tryChamferClosing(
    const carto::VolumeRef< int16_t >& dataIn, float radius )
  {
    if( _use_chamfer && isBinary( dataIn ) )
    {
      carto::VolumeRef<int16_t> dataIn2 = checkBorderWidth( dataIn );
      return AimsMorphoChamferClosing(
        AimsData<int16_t>(
          const_cast<carto::VolumeRef<int16_t> &>( dataIn2 ) ),
        radius, _chamfer_mask_size[0], _chamfer_mask_size[0],
        _chamfer_mask_size[0], _chamfer_factor ).volume();
    }
    return 0;
  }


  template<>
  carto::VolumeRef<int16_t> MorphoGreyLevel<int16_t>::tryChamferOpening(
    const carto::VolumeRef< int16_t >& dataIn, float radius )
  {
    if( _use_chamfer && isBinary( dataIn ) )
    {
      carto::VolumeRef<int16_t> dataIn2 = checkDistanceToBorder( dataIn,
                                                                 radius );
      int size_diff = ( dataIn2->getSizeX() - dataIn->getSizeX() ) / 2;
      carto::VolumeRef<int16_t> dataOut = AimsMorphoChamferOpening(
        AimsData<int16_t>(
          const_cast<carto::VolumeRef<int16_t> &>( dataIn2 ) ),
        radius, 3, 3, 3, _chamfer_factor ).volume();
      if( size_diff != 0 )
        dataOut = reallocateVolume( dataOut, -size_diff, 1 ); // FIXME border
      return dataOut;
    }
    return 0;
  }

  // chamfer-specific sanity checks

  template<typename T>
  carto::VolumeRef<T> MorphoGreyLevel<T>::checkDistanceToBorder(
    const carto::VolumeRef<T>& dataIn, float radius ) const
  {
    std::set<float> f;
    std::vector<float> vs = dataIn->getVoxelSize();
    f.insert( vs[0] );
    f.insert( vs[1] );
    f.insert( vs[2] );
    int offset ;
    offset = (int) ( radius / *f.rbegin() );
    float dist = distanceToBorder( dataIn );

    if( dist <= offset )
    {
      std::cout << "The distance between the object and the border is less "
        "than the radius. Copying it.\n";

      int border_width = 1; // for now.
      carto::VolumeRef<T> outvol = reallocateVolume( dataIn, offset,
                                                     border_width );
      return outvol;
    }
    return dataIn;
  }


  template<typename T>
  carto::VolumeRef<T> MorphoGreyLevel<T>::checkBorderWidth(
    const carto::VolumeRef<T>& dataIn ) const
  {
    int needed_border = neededBorderWidth();
    int border = 0;

    if( !dataIn->refVolume().isNull() ) // is a view to another volume
    {
      std::set<int> b;
      b.insert( dataIn->refVolume()->getSizeX() - dataIn->getSizeX() );
      b.insert( dataIn->refVolume()->getSizeY() - dataIn->getSizeY() );
      b.insert( dataIn->refVolume()->getSizeZ() - dataIn->getSizeZ() );
      border = *b.rbegin() / 2;
    }
    if( border < needed_border )
      return reallocateVolume( dataIn, 0, needed_border );
    return dataIn;
  }


  template<typename T>
  float MorphoGreyLevel<T>::distanceToBorder( const carto::VolumeRef<T> &vol )
  {
    int x,y,z;
    int dx = vol->getSizeX() ,dy = vol->getSizeY(), dz = vol->getSizeZ();
    int distance = dx + dy + dz;

    for(x=0; x < dx; ++x)
      for(y=0; y < dy; ++y)
        for(z=0; z < dz; ++z)
          if ( vol->at(x,y,z) == 32767 )
          {
            if (x < distance)
              distance = x ;
            else
              if ( (dx - x) < distance && (dx - x) >= 0 )
                distance = dx - x;

            if (y < distance)
              distance = y ;
            else
              if ( (dy - y) < distance && (dy - y) >= 0 )
                distance = dy - y;

            if (z < distance)
              distance = z ;
            else
              if ( (dz - z) < distance && (dz - z) >= 0 )
                distance = dz - z;
          }

    return distance;
  }


  template<typename T>
  carto::VolumeRef<T> MorphoGreyLevel<T>::reallocateVolume(
    const carto::VolumeRef<T>& dataIn, int added_width, int border_width )
  {
    int x, y, z, ox = 0, oy = 0, oz = 0;
    int dx = dataIn->getSizeX(), dy = dataIn->getSizeY(),
      dz = dataIn->getSizeZ();

    carto::VolumeRef<T> volBig( dx + 2 * added_width,
                                dy + 2 * added_width,
                                dz + 2 * added_width,
                                1, border_width );

    if( added_width < 0 )
    {
      dx += added_width;
      dy += added_width;
      dz += added_width;
      ox = -added_width;
      oy = -added_width;
      oz = -added_width;
    }

    std::vector<float> voxelsize = dataIn->getVoxelSize();
    volBig->copyHeaderFrom( dataIn->header() );

    volBig->fill( 0 );

    for(z=oz; z < dz; ++z)
      for(y=oy; y < dy; ++y)
        for(x=ox; x < dx; ++x)
          volBig->at( x + added_width, y + added_width, z+added_width )
            = dataIn->at( x, y, z );

    return volBig;
  }


  template<typename T>
  bool MorphoGreyLevel<T>::isBinary( const carto::VolumeRef<T>& dataIn )
  {
    int x,y,z;
    int dx = dataIn->getSizeX(), dy = dataIn->getSizeY(),
      dz = dataIn->getSizeZ();
    T value = 0, v;

    for(z=0; z < dz; ++z)
      for(y=0; y < dy; ++y)
        for(x=0; x < dx; ++x)
        {
          v = dataIn->at( x, y, z );
          if( v != 0 )
          {
            if( value == 0 )
            {
              value = v;
              if( v != 32767 ) // for now
                return false;
            }
            else if( value != v )
              return false;
          }
        }
    return true;
  }


  template<typename T>
  int MorphoGreyLevel<T>::neededBorderWidth() const
  {
    std::set<unsigned> s;
    s.insert( _chamfer_mask_size[0] );
    s.insert( _chamfer_mask_size[1] );
    s.insert( _chamfer_mask_size[2] );
    int dimMax = *s.rbegin();
    return (int) (dimMax - 1) / 2;
  }

}

#endif


