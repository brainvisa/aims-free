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
#include <cartodata/volume/volumeutil_d.h>
#include <aims/math/mathelem.h>

namespace aims
{

  template <typename T>
  MorphoGreyLevel<T>::MorphoGreyLevel(): radius(1.), int_radius(1,1,1)
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
        std::cout << "WARNING: Radius too small according to voxelsize !!! STOP.\n" << std::endl;
        exit(1);
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
        std::cout << "Number of voxels in the structuring element = " << list.size() << std::endl << std::endl;
        return list;
      }
    else return list;
  }


  template<typename T>
  carto::VolumeRef<T> MorphoGreyLevel<T>::doErosion(
    const carto::Volume< T >& dataIn, float radius )
  {
    std::cout << "EROSION" << std::endl;
    T value = carto::VolumeUtil<T>::max(dataIn);

    std::vector<float> vs = dataIn.getVoxelSize();
    Point4df voxelsize( vs[0], vs[1], vs[2], vs[3] );

    Point4d dim( dataIn.getSizeX(),
                dataIn.getSizeY(),
                dataIn.getSizeZ(),
                dataIn.getSizeT() );

    // FIXME: this copy sometimes gets a shared buffer. WHY ?
    // carto::VolumeRef< T > dataOut( new carto::Volume<T>( dataIn ) );
    carto::VolumeRef< T > dataOut( new carto::Volume<T>( dataIn.getSizeX(), dataIn.getSizeY(), dataIn.getSizeZ(), dataIn.getSizeT() ) );
    dataOut->fill( T(0) );
    dataOut->header().copyProperties(
      carto::Object::reference( dataIn.header() ) );

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
                        tmp = dataIn( coord[0], coord[1], coord[2] );
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
    const carto::Volume< T >& dataIn, float radius )
  {
    std::cout << "DILATION" << std::endl;
    T value = carto::VolumeUtil<T>::min(dataIn);

    std::vector<float> vs = dataIn.getVoxelSize();
    Point4df voxelsize( vs[0], vs[1], vs[2], vs[3] );

    Point4d dim( dataIn.getSizeX(),
                dataIn.getSizeY(),
                dataIn.getSizeZ(),
                dataIn.getSizeT() );

    // FIXME: this copy sometimes gets a shared buffer. WHY ?
//     carto::VolumeRef< T > dataOut( new carto::Volume<T>( dataIn ) );
    carto::VolumeRef< T > dataOut( new carto::Volume<T>( dataIn.getSizeX(), dataIn.getSizeY(), dataIn.getSizeZ(), dataIn.getSizeT() ) );
    dataOut->fill( T(0) );
    dataOut->header().copyProperties(
      carto::Object::reference( dataIn.header() ) );

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
                tmp = dataIn.at( coord[0], coord[1], coord[2] );
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
    const carto::Volume<T>& dataIn, float radius )
  {
    carto::VolumeRef< T > dataOut;
    dataOut = doDilation( dataIn, radius );
    dataOut = doErosion( *dataOut, radius );

    return dataOut;
  }


  template<typename T>
  carto::VolumeRef<T> MorphoGreyLevel<T>::doOpening(
    const carto::Volume<T>& dataIn, float radius )
  {
    carto::VolumeRef< T > dataOut;
    dataOut = doErosion( dataIn, radius );
    dataOut = doDilation( *dataOut, radius );

    return dataOut;
  }

}

#endif


