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

#ifndef CARTODATA_IO_VOLUMEFORMATREADER_D_H
#define CARTODATA_IO_VOLUMEFORMATREADER_D_H
//--- cartodata --------------------------------------------------------------
#include <cartodata/io/volumeformatreader.h>
#include <cartodata/io/volumeutilio.h>
#include <cartodata/volume/volume.h>
#include <cartodata/io/carto2aimsheadertranslator.h>
//--- soma io ----------------------------------------------------------------
#include <soma-io/config/soma_config.h>
#include <soma-io/datasourceinfo/datasourceinfo.h>
#include <soma-io/datasource/datasource.h>
#include <soma-io/datasource/filedatasource.h>
#include <soma-io/image/imagereader.h>
#include <soma-io/reader/formatreader.h>
#include <soma-io/allocator/allocator.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/object/object.h>
#include <cartobase/smart/rcptr.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/stream/fileutil.h>
//--- system -----------------------------------------------------------------
#include <vector>
#include <iostream>
#include <string>
//--- debug ------------------------------------------------------------------
#include <cartobase/config/verbose.h>
#include <cartobase/type/string_conversion.h>
#define localMsg( message ) cartoCondMsg( 4, message, "VOLUMEFORMATREADER" )
// localMsg must be undef at end of file
//----------------------------------------------------------------------------

namespace soma
{
//////////////////////////////////////////////////////////////////////////////
////               V O L U M E F O R M A T R E A D E R                    ////
//////////////////////////////////////////////////////////////////////////////

  //==========================================================================
  //   C O N S T R U C T O R S
  //==========================================================================
  template <typename T>
  VolumeFormatReader<T>::~VolumeFormatReader()
  {
  }

  //==========================================================================
  //   U R I   O P T I O N S
  //==========================================================================

  /**** setupAndRead *********************************************************
   * Usually it is not necessary to redefine this method, but in the case of
   * volumes the use of partial reading or borders triggers multiple volume
   * allocations (see volume philosophy). These complex volumes are
   * managed in VolumeUtilIO and setupAndRead/createAndRead must call them
   * when needed.
   **************************************************************************/
  template <typename T> void
  VolumeFormatReader<T>::setupAndRead( Volume<T> & obj,
                                       carto::rc_ptr<DataSourceInfo> dsi,
                                       const AllocatorContext & context,
                                       carto::Object options )
  {
    //=== Reading URI ========================================================
    std::string uri = dsi->list().dataSource()->url();
    std::string url = FileUtil::uriFilename( uri );
    carto::Object urioptions = FileUtil::uriOptions( uri );
    if( urioptions.get() ) {
      if( !options.get() )
        options = carto::Object::value( carto::PropertySet() );
      options->copyProperties( urioptions );
      dsi->list().dataSource().reset( new FileDataSource(  url ) );
    }
    //=== if no options -> classic reading ===================================
    if( !options.get() )
      return FormatReader<Volume<T> >::setupAndRead( obj, dsi, context,
                                                     options );
    //=== else, look for known properties ====================================
    std::set<std::string> prop = VolumeUtilIO<T>::listReadProperties();
    typename std::set<std::string>::iterator p;
    typename std::set<std::string>::iterator plast = prop.end();
    for( p = prop.begin(); p != prop.end(); ++p )
    {
      if( options->hasProperty( *p ) ) {
        VolumeUtilIO<T>::read( &obj, dsi, options );
        bool convert = false;
        options->getProperty( "convert_to_aims", convert );
        if( convert )
        {
          carto::Carto2AimsHeaderTranslator translator;
          translator.translate( carto::Object::reference( obj.header() ) );
        }
        return;
      }
      // for now it's juste unalloc/realloc.
      // it must be possible to design a nice setup, but then changes are
      // needed in VolumeUtilIO
    }
    //=== if no known property -> classic reading ============================
    return FormatReader<Volume<T> >::setupAndRead( obj, dsi, context,
                                                   options );
  }

  /**** createAndRead ********************************************************
   * See setupAndRead
   **************************************************************************/
  template <typename T> Volume<T>*
  VolumeFormatReader<T>::createAndRead( carto::rc_ptr<DataSourceInfo> dsi,
                                        const AllocatorContext & context,
                                        carto::Object options )
  {
    localMsg( "Creating and reading object ( " + dsi->url() + " )" );

    //=== Reading URI ========================================================
    std::string uri = dsi->list().dataSource()->url();
    std::string url = FileUtil::uriFilename( uri );
    carto::Object urioptions = FileUtil::uriOptions( uri );
    if( !options.get() )
      options = carto::Object::value( carto::PropertySet() );
    else
      options = options->clone();
    if( urioptions.get() ) {
      options->copyProperties( urioptions );
      dsi->list().dataSource().reset( new FileDataSource(  url ) );
    }
    //=== if no options -> classic reading ===================================
    if( !options.get() )
      return FormatReader<Volume<T> >::createAndRead( dsi, context, options );
    //=== else, look for known properties ====================================
    std::set<std::string> prop = VolumeUtilIO<T>::listReadProperties();
    typename std::set<std::string>::iterator p;
    typename std::set<std::string>::iterator plast = prop.end();
    Volume<T> *volume = 0;
    for( p = prop.begin(); p != plast; ++p )
    {
      if( options->hasProperty( *p ) )
      {
        localMsg( "using VolumeUtilIO<T>::read ( " + dsi->url() + " )" );
        volume = VolumeUtilIO<T>::read( 0, dsi, options );
        break;
      }
    }
    //=== if no known property -> classic reading ============================
    if( !volume )
    {
      localMsg( "using classical Volume reading ( " + dsi->url() + " )" );
      volume = FormatReader<Volume<T> >::createAndRead(
        dsi, context, options );
    }
    if( volume )
    {
      bool convert = false;
      options->getProperty( "convert_to_aims", convert );
      if( convert )
      {
        carto::Carto2AimsHeaderTranslator translator;
        translator.translate( carto::Object::reference( volume->header() ) );
      }
    }
    return volume;
  }

  //==========================================================================
  //   N E W   M E T H O D S
  //==========================================================================

  /**** Reading to a Volume<T> ***********************************************
   * This method depends deeply on the data structure (Volume<T>). It is
   * declared in FormatReader but only defined here.
   **************************************************************************/
  template <typename T>
  void VolumeFormatReader<T>::read( carto::Volume<T> & obj,
                                    carto::rc_ptr<DataSourceInfo> dsi,
                                    const AllocatorContext & context,
                                    carto::Object options )
  {
    localMsg( "Reading object ( " + dsi->url() + " )" );

    //=== Test data compability ==============================================
    std::string otype = dsi->header()->getProperty( "object_type" )->getString();
    std::string dtype = dsi->header()->getProperty( "data_type" )->getString();
    std::string ttype = DataTypeCode<T>::dataType();
    if( otype != "Volume" )
      throw datatype_format_error( "unsupported data type - " + otype + " != Volume", dsi->url() );
    if( dtype != ttype )
    {
      if( dsi->header()->hasProperty( "possible_data_types" ) )
      {
        size_t i;
        for( i=0;
            i<dsi->header()->getProperty( "possible_data_types" )->size();
            ++i )
        {
          if( dsi->header()->getProperty( "possible_data_types" )
              ->getArrayItem( i )->getString()
              == ttype )
            break;
        }
        if( i == dsi->header()->getProperty( "possible_data_types" )->size() )
          throw datatype_format_error( dsi->url() );
      } else
        throw datatype_format_error( "unsupported data type - " + dtype + " != " + ttype, dsi->url() );
    }

    //=== test for memory mapping ============================================
    localMsg( "checking for memory mapping..." );
    if( obj.allocatorContext().allocatorType()
        == AllocatorStrategy::ReadOnlyMap )
    {
      localMsg( " -> Memory Mapping : nothing to read." );
      return;
    }

    //=== volume is a view ? =================================================
    localMsg( "checking if object is a view..." );
    carto::Volume<T>* parent1( 0 );
    carto::Volume<T>* parent2( 0 );
    parent1 = obj.refVolume().get();
    if( parent1 )
      parent2 = parent1->refVolume().get();
    else if( !obj.allocatorContext().isAllocated() ) {
      localMsg( " -> Unallocated Volume : nothing to read." );
      return;
    }
    localMsg( std::string(" -> object ") + ( parent1 ? "is" : "isn't" )
              + " a view and "
              + ( obj.allocatorContext().isAllocated() ? "is" : "isn't" )
              + " allocated." );
    if( parent1 )
      localMsg( std::string(" -> parent exists and ")
                + ( parent1->allocatorContext().isAllocated() ? "is" : "isn't" )
                + " allocated." );
    if( parent2 )
      localMsg( std::string(" -> grandparent exists and ")
                + ( parent2->allocatorContext().isAllocated() ? "is" : "isn't" )
                + " allocated." );

    //=== view size ==========================================================
    localMsg( "reading view size..." );
    std::vector<int>  viewsize( 4, 0 );
    viewsize[ 0 ] = obj.getSizeX();
    viewsize[ 1 ] = obj.getSizeY();
    viewsize[ 2 ] = obj.getSizeZ();
    viewsize[ 3 ] = obj.getSizeT();
    localMsg( " -> view size ( "
              + carto::toString( viewsize[ 0 ] ) + ", "
              + carto::toString( viewsize[ 1 ] ) + ", "
              + carto::toString( viewsize[ 2 ] ) + ", "
              + carto::toString( viewsize[ 3 ] ) + " )" );

    //=== multiresolution level ==============================================
    localMsg( "reading multiresolution level..." );
    int level = 0;
    if( options->hasProperty( "resolution_level" ) ) {
      options->getProperty( "resolution_level", level );
      if (level < 0) {
        try {
          // Try to solve negative level values
          level += dsi->header()->getProperty( "resolutions_dimension" )
                                ->size();
        }
        catch(...){}
      }
    }
    localMsg( " -> level to read : " + carto::toString( level ) );

    //=== full volume size ===================================================
    localMsg( "reading full volume size..." );
    std::vector<int>  imagesize( 4, 0 );
    try {
      // first we look for "resolutions_dimension" property
      imagesize[ 0 ] = (int) rint(
        dsi->header()->getProperty( "resolutions_dimension" )
                          ->getArrayItem( level )->getArrayItem( 0 )
                          ->getScalar() );
      imagesize[ 1 ] = (int) rint(
        dsi->header()->getProperty( "resolutions_dimension" )
                          ->getArrayItem( level )->getArrayItem( 1 )
                          ->getScalar() );
      imagesize[ 2 ] = (int) rint(
        dsi->header()->getProperty( "resolutions_dimension" )
                          ->getArrayItem( level )->getArrayItem( 2 )
                          ->getScalar() );
      imagesize[ 3 ] = (int) rint(
        dsi->header()->getProperty( "resolutions_dimension" )
                          ->getArrayItem( level )->getArrayItem( 3 )
                          ->getScalar() );
      localMsg( " -> found \"resolutions_dimension\"." );
    } catch( ... ) {
      try {
        // if it doesn't work, we look for "volume_dimension"
        imagesize[ 0 ] = (int) rint(
          dsi->header()->getProperty( "volume_dimension" )
                            ->getArrayItem( 0 )->getScalar() );
        imagesize[ 1 ] = (int) rint(
          dsi->header()->getProperty( "volume_dimension" )
                            ->getArrayItem( 1 )->getScalar() );
        imagesize[ 2 ] = (int) rint(
          dsi->header()->getProperty( "volume_dimension" )
                            ->getArrayItem( 2 )->getScalar() );
        imagesize[ 3 ] = (int) rint(
            dsi->header()->getProperty( "volume_dimension" )
                            ->getArrayItem( 3 )->getScalar() );
        localMsg( " -> found \"volume_dimension\"." );
      } catch( ... ) {
        // if still nothing, we look for parent volumes
        if( parent1 && !parent1->allocatorContext().isAllocated() ) {
          imagesize[ 0 ] = parent1->getSizeX();
          imagesize[ 1 ] = parent1->getSizeY();
          imagesize[ 2 ] = parent1->getSizeZ();
          imagesize[ 3 ] = parent1->getSizeT();
          localMsg( " -> found unallocated parent." );
        } else if( parent2 ) {
          imagesize[ 0 ] = parent2->getSizeX();
          imagesize[ 1 ] = parent2->getSizeY();
          imagesize[ 2 ] = parent2->getSizeZ();
          imagesize[ 3 ] = parent2->getSizeT();
          localMsg( " -> found grandparent." );
        } else {
          imagesize = viewsize;
          localMsg( " -> full volume is self." );
        }
      }
    }
    localMsg( " -> full volume size ( "
              + carto::toString( imagesize[ 0 ] ) + ", "
              + carto::toString( imagesize[ 1 ] ) + ", "
              + carto::toString( imagesize[ 2 ] ) + ", "
              + carto::toString( imagesize[ 3 ] ) + " )" );

    //=== allocated volume size ==============================================
    localMsg( "reading allocated size..." );
    std::vector<int> allocsize( 4, 0 );
    if( !parent1 ) {
      allocsize = viewsize;
      localMsg( " -> allocated volume is self (full volume)." );
    } else if( !parent1->allocatorContext().isAllocated() ) {
      allocsize = viewsize;
      localMsg( " -> allocated volume is self (partial volume)." );
    } else {
      allocsize[0] = parent1->getSizeX();
      allocsize[1] = parent1->getSizeY();
      allocsize[2] = parent1->getSizeZ();
      allocsize[3] = parent1->getSizeT();
      localMsg( " -> allocated volume is parent "
                "(borders or partially loading in full volume)." );
    }
    localMsg( " -> allocated volume size ( "
              + carto::toString( allocsize[ 0 ] ) + ", "
              + carto::toString( allocsize[ 1 ] ) + ", "
              + carto::toString( allocsize[ 2 ] ) + ", "
              + carto::toString( allocsize[ 3 ] ) + " )" );

    //=== strides ============================================================
    std::vector<long> strides(4);
    strides[0] = &obj(1,0,0,0) - &obj(0,0,0,0);
    strides[1] = &obj(0,1,0,0) - &obj(0,0,0,0);
    strides[2] = &obj(0,0,1,0) - &obj(0,0,0,0);
    strides[3] = &obj(0,0,0,1) - &obj(0,0,0,0);

    //=== region's origine ===================================================
    localMsg( "reading view position in reference to full volume..." );
    std::vector<int>  pos ( 4 , 0 );
    if( parent1 && !parent1->allocatorContext().isAllocated() ) {
      pos[ 0 ] = obj.posInRefVolume()[ 0 ];
      pos[ 1 ] = obj.posInRefVolume()[ 1 ];
      pos[ 2 ] = obj.posInRefVolume()[ 2 ];
      pos[ 3 ] = obj.posInRefVolume()[ 3 ];
    } else if( parent2 ) {
      pos[ 0 ] = obj.posInRefVolume()[ 0 ] + parent1->posInRefVolume()[ 0 ];
      pos[ 1 ] = obj.posInRefVolume()[ 1 ] + parent1->posInRefVolume()[ 1 ];
      pos[ 2 ] = obj.posInRefVolume()[ 2 ] + parent1->posInRefVolume()[ 2 ];
      pos[ 3 ] = obj.posInRefVolume()[ 3 ] + parent1->posInRefVolume()[ 3 ];
    }
    localMsg( " -> view position ( "
              + carto::toString( pos[ 0 ] ) + ", "
              + carto::toString( pos[ 1 ] ) + ", "
              + carto::toString( pos[ 2 ] ) + ", "
              + carto::toString( pos[ 3 ] ) + " )" );

    //=== possibilities : with borders, partial reading ======================
    bool withborders = allocsize[0] > viewsize[0] ||
                       allocsize[1] > viewsize[1] ||
                       allocsize[2] > viewsize[2] ||
                       allocsize[3] > viewsize[3];
    bool partialreading = imagesize[0] > viewsize[0] ||
                          imagesize[1] > viewsize[1] ||
                          imagesize[2] > viewsize[2] ||
                          imagesize[3] > viewsize[3];
    localMsg( "With Borders : "
              + std::string( ( withborders ? "yes" : "no" ) ) );
    localMsg( "Partial Reading : "
              + std::string( ( partialreading ? "yes" : "no" ) ) );

    //=== reading volume =====================================================
    localMsg( "reading volume..." );
    int y, z, t;
    if( !withborders || dsi->capabilities().canHandleStrides() )
    {
      // we can read the volume/region into a contiguous buffer
      _imr->read( ( T * ) &obj(0,0,0,0), *dsi, pos,
                  viewsize, strides, options );
    }
    else
    {
      // we are in a "border" context. The volume/region must be read
      // line by line
      std::vector<int> posline ( pos );
      std::vector<int> sizeline ( 4, 1 );
      sizeline[ 0 ] = viewsize[ 0 ];
      for ( t=0; t<viewsize[ 3 ]; ++t )
        for ( z=0; z<viewsize[ 2 ]; ++z )
          for ( y=0; y<viewsize[ 1 ]; ++y ) {
            posline[ 1 ] = pos[ 1 ] + y;
            posline[ 2 ] = pos[ 2 ] + z;
            posline[ 3 ] = pos[ 3 ] + t;
            _imr->read( ( T * ) &obj(0,y,z,t), *dsi, posline,
                        sizeline, strides, options );
          }
    }

    // we reset at 0 the ImageReader's members (sizes, binary, ...) so that
    // they are recomputed at the next reading.
    _imr->resetParams();
  }


  template <typename T>
  FormatReader<carto::Volume<T> >* VolumeFormatReader<T>::clone() const
  {
    VolumeFormatReader<T> *reader = new VolumeFormatReader<T>;
    reader->attach( carto::rc_ptr<ImageReader<T> >( _imr->cloneReader() ) );
    return reader;
  }

  /***************************************************************************
   * Attaching a specific ImageReader to the FormatReader
   **************************************************************************/
  template <typename T>
  void VolumeFormatReader<T>::attach( carto::rc_ptr<ImageReader<T> > imr )
  {
    _imr = imr;
  }


  template <typename T>
  std::string VolumeFormatReader<T>::formatID() const
  {
    // delegate to ImageReader
    if( !_imr )
      return "";
    return _imr->formatID();
  }

//////////////////////////////////////////////////////////////////////////////
////            V O L U M E R E F F O R M A T R E A D E R                 ////
//////////////////////////////////////////////////////////////////////////////

  //==========================================================================
  //   C O N S T R U C T O R S
  //==========================================================================
  template <typename T>
  VolumeRefFormatReader<T>::~VolumeRefFormatReader()
  {
  }

  //==========================================================================
  //   U R I   O P T I O N S
  //==========================================================================
  /*** setupAndRead **********************************************************
   * see VolumeFormatReader
   **************************************************************************/
  template <typename T> void
  VolumeRefFormatReader<T>::setupAndRead( VolumeRef<T> & obj,
                                       carto::rc_ptr<DataSourceInfo> dsi,
                                       const AllocatorContext & context,
                                       carto::Object options )
  {
    //=== Reading URI ========================================================
    std::string uri = dsi->list().dataSource()->url();
    std::string url = FileUtil::uriFilename( uri );
    carto::Object urioptions = FileUtil::uriOptions( uri );
    if( urioptions.get() ) {
      if( !options.get() )
        options = carto::Object::value( carto::PropertySet() );
      options->copyProperties( urioptions );
      dsi->list().dataSource().reset( new FileDataSource(  url ) );
    }
    //=== if no options -> classic reading ===================================
    if( !options.get() )
      return FormatReader<VolumeRef<T> >::setupAndRead( obj, dsi, context,
                                                        options );
    //=== else, look for known properties ====================================
    std::set<std::string> prop = VolumeUtilIO<T>::listReadProperties();
    typename std::set<std::string>::iterator p;
    typename std::set<std::string>::iterator plast = prop.end();
    for( p = prop.begin(); p != plast; ++p )
    {
      if( options->hasProperty( *p ) )
      {
        VolumeFormatReader<T> vrf;
        vrf.attach( _imr );
        vrf.setupAndRead( *obj, dsi, context, options );
        bool convert = false;
        options->getProperty( "convert_to_aims", convert );
        if( convert )
        {
          carto::Carto2AimsHeaderTranslator translator;
          translator.translate( carto::Object::reference( obj->header() ) );
        }
        return; // return as soon as one read prop has been found.
      }
    }
    //=== if no known property -> classic reading ============================
    return FormatReader<VolumeRef<T> >::setupAndRead( obj, dsi, context,
                                                      options );
  }

  /*** createAndRead *********************************************************
   * see VolumeFormatReader
   **************************************************************************/
  template <typename T> VolumeRef<T>*
  VolumeRefFormatReader<T>::createAndRead( carto::rc_ptr<DataSourceInfo> dsi,
                                        const AllocatorContext & context,
                                        carto::Object options )
  {
    VolumeFormatReader<T> vrf;
    vrf.attach( _imr );
    return new VolumeRef<T>( vrf.createAndRead( dsi, context, options ) );
  }

  //==========================================================================
  //   N E W   M E T H O D S
  //==========================================================================

  /*** Reading to a Volume<T> ************************************************
   * This method depends deeply on the data structure (Volume<T>). It is
   * declared in FormatReader but only defined here.
   **************************************************************************/
  template <typename T>
  void VolumeRefFormatReader<T>::read( carto::VolumeRef<T> & obj,
                                       carto::rc_ptr<DataSourceInfo> dsi,
                                       const AllocatorContext & context,
                                       carto::Object options )
  {
    VolumeFormatReader<T> vrf;
    vrf.attach( _imr );
    vrf.read( *obj, dsi, context, options );
  }

  /***************************************************************************
   * Attaching a specific ImageReader to the FormatReader
   **************************************************************************/
  template <typename T>
  void VolumeRefFormatReader<T>::attach( carto::rc_ptr<ImageReader<T> > imr )
  {
    _imr = imr;
  }


  template <typename T>
  FormatReader<carto::VolumeRef<T> >*
  VolumeRefFormatReader<T>::clone() const
  {
    VolumeRefFormatReader<T> *reader = new VolumeRefFormatReader<T>;
    reader->attach( carto::rc_ptr<ImageReader<T> >( _imr->cloneReader() ) );
    return reader;
  }


  template <typename T>
  std::string VolumeRefFormatReader<T>::formatID() const
  {
    // delegate to ImageReader
    if( !_imr )
      return "";
    return _imr->formatID();
  }

}

#undef localMsg
#endif
