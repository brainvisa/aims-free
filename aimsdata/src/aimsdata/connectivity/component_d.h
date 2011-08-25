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

/*
 *  Connected components
 */
#ifndef AIMS_CONNECTIVITY_COMPONENT_D_H
#define AIMS_CONNECTIVITY_COMPONENT_D_H

#include <aims/connectivity/component.h>
#include <aims/data/data.h>
#include <aims/utility/threshold.h>
#include <aims/bucket/bucket.h>
#include <map>
#include <queue>
#include <iostream>
#include <iomanip>

namespace aims
{

  template<typename T>
  void AimsConnectedComponent( AimsData<T>& data, 
                               Connectivity::Type connectivity, 
                               std::map<T, size_t>& valids, 
                               const T & backg, bool bin, size_t minSize, 
                               size_t numMax, bool verbose )
  {
    std::multimap<size_t, size_t> compSize;
    int x=0, y=0, z=0, t=0, n=0;
    int dimX = data.dimX();
    int dimY = data.dimY();
    int dimZ = data.dimZ();
    int dimT = data.dimT() ;




    for( t = 0 ; t < dimT ; ++t ){


      AimsData<T> cc( dimX, dimY, dimZ );

      cc = 0;

      //
      // boolean volume to say if a voxel is already used
      //

      AimsData<byte> flag( dimX, dimY, dimZ );

      flag = false;
      ForEach3d( flag, x, y, z )
        if ( data( x, y, z, t ) == backg )
          flag( x, y, z ) = true;

      Connectivity cd( data.oLine(), data.oSlice(), connectivity );
      //Connectivity cf( flag.oLine(), flag.oSlice(), connectivity );


      size_t		label = 1;
      Point3d		pos,newpos;
      std::queue<Point3d>	que;
      T			val;
      size_t		sz;



      //
      // track connected components
      //
      for ( z = 0; z < dimZ; ++z )

      {
        if( verbose )

          std::cout << "\rz: " << z << std::flush;

        for ( y = 0; y < dimY; ++y )
          for ( x = 0; x < dimX; ++x )
            if( !flag( x, y, z ) )
              {
                val = data( x, y, z, t );
                que.push( Point3d( x, y, z ) );
                flag( x, y, z ) = true;
                sz = 0;
                while ( !que.empty() )
                  {
                    pos = que.front();
                    que.pop();
                    cc( pos ) = label;
                    ++sz;
                    for ( n = 0; n < cd.nbNeighbors(); n++ )
                      {
                        newpos = pos + cd.xyzOffset( n );
                        if ( newpos[0] >= 0   &&
                             newpos[0] < dimX &&
                             newpos[1] >= 0   &&
                             newpos[1] < dimY &&
                             newpos[2] >= 0   &&
                             newpos[2] < dimZ   )
                   
                          if ( !flag( newpos ) 
                               && ( bin || data( newpos[0], newpos[1], newpos[2], t ) == val ) )
                            {
                              flag( newpos ) = true;
                              que.push( newpos );
                            }
                      }
                  }
                /*std::cout << "comp. " << label << ", val: " << val << " (" 
                  << sz << " voxels)\n";*/

                compSize.insert( std::pair<size_t,size_t>( sz, label ) );
                ++label;
              }
      }

      if( verbose )
        {
          std::cout << std::endl;

          std::cout << label << " components\n";
        } 


      //	filter small comps

      std::multimap<size_t, size_t>::reverse_iterator 
        im, em = compSize.rend();

      label = 1;

      for( im = compSize.rbegin(); 
           im != em && im->first >= minSize 
             && ( numMax == 0 || label <= numMax ); ++im )

        {

          if( verbose )
            std::cout << "component " << std::setw( 4 ) << im->second 
                      << " : " << std::setw( 8 ) << im->first 
                      << " points" << std::endl;

          valids[ im->second ] = label++;
        }


      if( verbose )
        std::cout << "filtering...\n";

      ForEach3d( data, x, y, z )
        data(x, y, z, t) = 0 ;



      typename std::map<T, size_t>::iterator	is, es = valids.end();



      ForEach3d( cc, x, y, z )
        {
          is = valids.find( cc( x, y, z ) );
          if( is != es )
            data( x, y, z, t ) = (T) is->second; // RISK OF OVERFLOW on char types
        }

      if( verbose )
        std::cout << "after filtering: " << valids.size() << " components\n";

    }
  }

  template<typename T>
  void AimsConnectedComponent( AimsData<T>& data, 
                               Connectivity::Type connectivity, 
                               const T & backg, bool bin, size_t minSize, 
                               size_t numMax, bool verbose )
  {
      std::map<T, size_t>	valids;
    
      AimsConnectedComponent( data, 
                               connectivity, 
                               valids, 
                               backg, bin, minSize, 
                               numMax, verbose );
  }


  template <typename T> 
  void AimsConnectedComponent( AimsBucket<Void>& components,
                               const AimsData<T>& data,
                               Connectivity::Type connectivity, 
                               const T & backgrnd, bool bin, size_t minsize, 
                               size_t maxcomp, bool verbose )
  {
    AimsBucket<Void>			*cbk;
    std::auto_ptr<AimsBucket<Void> >	abk;
    if( minsize == 0 && maxcomp == 0 )
      cbk = &components;
    else

      {

        abk.reset( new AimsBucket<Void> );
        cbk = abk.get();
      }
    AimsBucket<Void>	& component = *cbk;

    int x=0, y=0, z=0, t=0, n=0;

    int dimX = data.dimX();
    int dimY = data.dimY();
    int dimZ = data.dimZ();
    int dimT = data.dimT() ;



    //
    // boolean volume to say if a voxel is already used
    //

    for( t = 0 ; t < dimT ; ++t ){
      AimsData<byte> flag( dimX, dimY, dimZ );
      flag = false;
      ForEach3d( flag, x, y, z )
        if ( data( x, y, z, t ) == backgrnd )
          flag( x, y, z ) = true;

      Connectivity cd( data.oLine(), data.oSlice(), connectivity );
      //Connectivity cf( flag.oLine(), flag.oSlice(), connectivity );
      size_t					label = 1;
      AimsBucketItem<Void>			item,newItem;
      std::queue< AimsBucketItem< Void > >	que;
      T						val;



      //
      // track connected components
      //
      for ( z = 0; z < dimZ; ++z )
      {
        if( verbose )
          std::cout << "\rz: " << z << std::flush;
        for ( y = 0; y < dimY; ++y )
          for ( x = 0; x < dimX; ++x )
            {
              if( !flag( x, y, z ) )
                {
   
                  val = data( x, y, z );
                  item.location() = Point3d( x, y, z );
                  que.push( item );
                  flag( x, y, z ) = true;
                  std::list<AimsBucketItem<Void> > & bk = component[ label ];
                  while ( !que.empty() )
                    {
                      item.location() = que.front().location();
                      que.pop();
                      bk.push_back( item );
                      for ( n = 0; n < cd.nbNeighbors(); n++ )
                        {
                          newItem.location() = item.location() 
                            + cd.xyzOffset( n );
                          if ( newItem.location().item( 0 ) >= 0   &&
                               newItem.location().item( 0 ) < dimX &&
                               newItem.location().item( 1 ) >= 0   &&
                               newItem.location().item( 1 ) < dimY &&
                               newItem.location().item( 2 ) >= 0   &&
                               newItem.location().item( 2 ) < dimZ   )
                   
                            if ( flag( newItem.location() ) == false 
                                 && ( bin || data( newItem.location() ) 
                                      == val ) )
                              {
                                flag( newItem.location() ) = true;
                                que.push( newItem );
                              }
                        }
                    }
                  ++label;
                }
            }
      }
      if( verbose )
        std::cout << std::endl;

      // filtering
      if( minsize == 0 && maxcomp == 0 )
        return;

      AimsBucket<Void>::iterator	i, e = component.end();
      std::multimap<unsigned, std::list<AimsBucketItem<Void> > *>	comps;
      for( i=component.begin(); i!=e; ++i )
        if( minsize == 0 || i->second.size() >= minsize )
          comps.insert( std::pair<unsigned, std::list<AimsBucketItem<Void> > *>
                      ( i->second.size(),&i->second ) );
      std::multimap<unsigned, 
        std::list<AimsBucketItem<Void> > *>::reverse_iterator 
        im, em = comps.rend();
      label = 0;
      for( im=comps.rbegin(); im!=em && label < maxcomp; ++im )
        components[ label++ ] = *im->second;
    }
  }

  namespace internal
  {

    template <typename T> inline 
    bool _nulltesttemplate_iseq( const T & x, const T & val )
    {
      return x == val;
    }

    template<> inline 
    bool _nulltesttemplate_iseq( const Void &, const Void & )
    {
      return false;
    }

  }


  template <typename T>
  void AimsConnectedComponent( AimsBucket<Void>& components,
                               const BucketMap<T>& data,
                               Connectivity::Type connectivity, 
                               const T & backg, bool bin, size_t minsize, 
                               size_t maxcomp, bool )
  {
    AimsBucket<Void>			*cbk;
    std::auto_ptr<AimsBucket<Void> >	abk;
    if( minsize == 0 && maxcomp == 0 )
      cbk = &components;
    else
      {
        abk.reset( new AimsBucket<Void> );
        cbk = abk.get();
      }
    AimsBucket<Void>	& component = *cbk;


    //
    // boolean bucket to say if a voxel is already used
    //
    BucketMap<byte> 				flags;
    BucketMap<byte>::Bucket			& flag = flags[0];
    BucketMap<byte>::Bucket::iterator		ifl, efl = flag.end();
    const typename  BucketMap<T>::Bucket	& b = data.begin()->second;
    typename BucketMap<T>::Bucket::const_iterator	ib, eb = b.end();
    int 					n;

    for( ib=b.begin(); ib!=eb; ++ib )
      flag[ ib->first ] 
        = internal::_nulltesttemplate_iseq( ib->second, backg );

    Connectivity cd( 0, 0, connectivity );
    //Connectivity cf( flag.oLine(), flag.oSlice(), connectivity );
    size_t					label = 1;
    AimsBucketItem<Void>			item,newItem;
    std::queue< AimsBucketItem< Void > >	que;
    T						val;

    //
    // track connected components
    //
    for( ib=b.begin(); ib!=eb; ++ib )
      {
        ifl = flag.find( ib->first );
        if( !ifl->second )
          {
            val = ib->second;
            item.location() = ib->first;
            que.push( item );
            ifl->second = true;
//             cout << "comp. " << label << ", val: " << val << std::endl;
            std::list<AimsBucketItem<Void> >	& bk = component[ label ];
            while ( !que.empty() )
              {
                item.location() = que.front().location();
                que.pop();
                bk.push_back( item );
                for ( n = 0; n < cd.nbNeighbors(); n++ )
                  {
                    newItem.location() = item.location() + cd.xyzOffset( n );
                    ifl = flag.find( newItem.location() );
                    if( ifl != efl && ifl->second == false 
                        && ( bin 
                             || b.find( newItem.location() )->second == val ) )
                      {
                        ifl->second = true;
                        que.push( newItem );
                      }
                  }
              }
            //std::cout << "(" << bk.size() << " voxels)\n";
            ++label;
          }
      }

    // filtering
    if( minsize == 0 && maxcomp == 0 )
      return;

    AimsBucket<Void>::iterator	i, e = component.end();
    std::multimap<unsigned, std::list<AimsBucketItem<Void> > *>	comps;
    for( i=component.begin(); i!=e; ++i )
      if( minsize == 0 || i->second.size() >= minsize )
        comps.insert( std::pair<unsigned, std::list<AimsBucketItem<Void> > *>
                      ( i->second.size(),&i->second ) );
    std::multimap<unsigned, 
      std::list<AimsBucketItem<Void> > *>::reverse_iterator 
      im, em = comps.rend();
    label = 0;
    for( im=comps.rbegin(); im!=em && label<maxcomp; ++im )
      components[ label++ ] = *im->second;
  }


  template <typename T>
  void AimsConnectedComponent( BucketMap<T>& data,
                               Connectivity::Type connectivity, 
                               const T & backgrnd = 0, bool bin = true, 
                               size_t minsize = 0, size_t maxcomp = 0, 
                               bool verbose = true )
  {
    AimsBucket<Void>	comps;
    AimsConnectedComponent( comps, data, connectivity, backgrnd, bin, minsize, 
                            maxcomp, verbose );
    AimsBucket<Void>::iterator			i, e = comps.end();
    std::list<AimsBucketItem<Void> >::iterator	ic, ec;
    typename BucketMap<T>::iterator		ib, eb = data.end();
    typename BucketMap<T>::Bucket::iterator	ibb, ebb;

    // clear input bucket
    for( ib=data.begin(); ib!=eb; ++ib )
      for( ibb=ib->second.begin(), ebb=ib->second.end(); ibb!=ebb; ++ibb )
        ibb->second = 0;
    // fill it
    typename BucketMap<T>::Bucket	& bk = data.begin()->second;
    for( i=comps.begin(); i!=e; ++i )
      for( ic=i->second.begin(), ec=i->second.end(); ic!=ec; ++ic )
        bk[ ic->location() ] = i->first + 1;
  }


  template <typename T> 
  AimsData<int16_t> AimsLabeledConnectedComponent( AimsBucket<Void>& components,
                                                   const AimsData<T>& data,
                                                   Connectivity::Type connectivity, 
                                                   const T & backgrnd, bool bin, size_t minsize, 
                                                   size_t maxcomp, bool verbose )
  {
    AimsBucket<Void>      *cbk;
//     ajout  pour retourner l'image de labels des composantes
    AimsData<int16_t> labelImage (data.dimX(), data.dimY(), data.dimZ());
    labelImage = 0;
    std::auto_ptr<AimsBucket<Void> >  abk;
    if( minsize == 0 && maxcomp == 0 )
      cbk = &components;
    else

    {

      abk.reset( new AimsBucket<Void> );
      cbk = abk.get();
    }
    AimsBucket<Void>  & component = *cbk;

    int x=0, y=0, z=0, t=0, n=0;

    int dimX = data.dimX();
    int dimY = data.dimY();
    int dimZ = data.dimZ();
    int dimT = data.dimT() ;



    
//     boolean volume to say if a voxel is already used
    

    for( t = 0 ; t < dimT ; ++t ){
      AimsData<byte> flag( dimX, dimY, dimZ );
      flag = false;
      ForEach3d( flag, x, y, z )
          if ( data( x, y, z, t ) == backgrnd )
          flag( x, y, z ) = true;

      Connectivity cd( data.oLine(), data.oSlice(), connectivity );
      Connectivity cf( flag.oLine(), flag.oSlice(), connectivity );
      size_t          label = 1;
      AimsBucketItem<Void>      item,newItem;
      std::queue< AimsBucketItem< Void > >  que;
      T           val;


//       track connected components
      
      for ( z = 0; z < dimZ; ++z )
      {
        if( verbose )
          std::cout << "\rz: " << z << std::flush;
        for ( y = 0; y < dimY; ++y )
          for ( x = 0; x < dimX; ++x )
        {

          if( !flag( x, y, z ) )
          {

            val = data( x, y, z );
            item.location() = Point3d( x, y, z );

            labelImage (x, y, z) = label;
            que.push( item );
            flag( x, y, z ) = true;
//                   std::cout << "comp. " << label << ", val: " << val 
//                   << std::endl;
            std::list<AimsBucketItem<Void> > & bk = component[ label ];
            while ( !que.empty() )
            {
              item.location() = que.front().location();
              que.pop();
              bk.push_back( item );
              labelImage (item.location()) =label;
              for ( n = 0; n < cd.nbNeighbors(); n++ )
              {
                newItem.location() = item.location() 
                    + cd.xyzOffset( n );
                if ( newItem.location().item( 0 ) >= 0   &&
                     newItem.location().item( 0 ) < dimX &&
                     newItem.location().item( 1 ) >= 0   &&
                     newItem.location().item( 1 ) < dimY &&
                     newItem.location().item( 2 ) >= 0   &&
                     newItem.location().item( 2 ) < dimZ   )
                   
                  if ( flag( newItem.location() ) == false 
                       && ( bin || data( newItem.location() ) 
                       == val ) )
                {
                  flag( newItem.location() ) = true;
                  que.push( newItem );
                  labelImage (newItem.location()) = label;
                }
              }
            }
//                   std::cout << "(" << bk.size() << " voxels)\n";
            ++label;
          }
        }
      }
      if( verbose )
        std::cout << std::endl;

//       filtering probleme filtrage
      if( minsize == 0 && maxcomp == 0 )
        return labelImage;

      AimsBucket<Void>::iterator  i, e = component.end();
      std::multimap<unsigned, std::list<AimsBucketItem<Void> > *> comps;
      for( i=component.begin(); i!=e; ++i )
        if( minsize == 0 || i->second.size() >= minsize )
          comps.insert( std::pair<unsigned, std::list<AimsBucketItem<Void> > *>
              ( i->second.size(),&i->second ) );
      
      std::multimap<unsigned, 
      std::list<AimsBucketItem<Void> > *>::reverse_iterator 
          im, em = comps.rend();
      label = 0;
      labelImage = 0;
      for( im=comps.rbegin(); im!=em && (label < maxcomp || maxcomp == 0); ++im ){
          std::list<AimsBucketItem<Void> > bucket = *im->second;
          std::list<AimsBucketItem<Void> >::iterator ite, iteend = bucket.end();
          for (ite = bucket.begin(); ite != iteend ; ite ++ ) {
              labelImage ((*ite).location()) = label;
          }
        components[ label++ ] = *im->second;
      }
    }
    return labelImage;
  }

}

#endif

