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

//-------------------------------------------------------------------
#include <aims/io_soma/trm_compose_formatreader.h>
#include <aims/transformation/affinetransformation3d.h>
#include <aims/transformation/transformation_chain.h>
#include <aims/data/pheader.h>
#include <aims/io/reader.h>
#include <soma-io/io/formatdictionary.h>
#include <soma-io/datasource/datasource.h>
//--- debug ------------------------------------------------------------------
#include <cartobase/config/verbose.h>
#define localMsg( message ) cartoCondMsg( 4, message, "TRMCOMPOSEFORMATREADER" )
// localMsg must be undef at end of file
//----------------------------------------------------------------------------

using namespace aims;
using namespace soma;
using namespace carto;
using namespace std;

//============================================================================
//   I N I T
//============================================================================

namespace
{

  bool inittrmcompformat()
  {
    TrmComposeFormatReader  *r = new TrmComposeFormatReader;
    vector<string>  exts;
    exts.push_back( "trmc" );
    FormatDictionary<AffineTransformation3d>::registerFormat( "TRMCOMPOSE", r,
                                                              exts );

    TrmChainFormatReader  *r2 = new TrmChainFormatReader;
    FormatDictionary<Transformation3d>::registerFormat( "TRMCHAIN", r2,
                                                        exts );

    return true;
  }

  bool dummy __attribute__((unused)) = inittrmcompformat();

}


//============================================================================
//   N E W   M E T H O D S
//============================================================================

AffineTransformation3d*
TrmComposeFormatReader::createAndRead( rc_ptr<DataSourceInfo> dsi,
                                       const AllocatorContext & context,
                                       Object options )
{
  // cout << "TrmComposeFormatReader::createAndRead\n";
  rc_ptr<AffineTransformation3d> r( new AffineTransformation3d );
  read( *r, dsi, context, options );
  AffineTransformation3d *result = r.get();
  r.release();

  return result;
}


void TrmComposeFormatReader::read( AffineTransformation3d & result,
                                   rc_ptr<DataSourceInfo> dsi,
                                   const AllocatorContext & /* context */,
                                   Object /* options */ )
{
  // cout << "TrmComposeFormatReader::read\n";
  int i, n = dsi->list().size( "trm_chain" );
  if( n == 0 )
    throw wrong_format_error( "not a transformations chain\n" );
  for( i=0; i<n; ++i )
  {
    rc_ptr<DataSource> ds = dsi->list().dataSource( "trm_chain", i );
    AffineTransformation3d tr;
    Reader<AffineTransformation3d> r( ds->url() );
    r.read( tr );
    // compose
    result *= tr;

    if( i == 0 )  // last applied
    {
      try
      {
        Object ref = tr.header()->getProperty( "destination_referential" );
        string dref = ref->getString();
        if( !dref.empty() )
          result.header()->setProperty( "destination_referential", dref );
      }
      catch( runtime_error & )
      {
      }
    }
    if( i == n - 1 )  // first applied
    {
      try
      {
        Object ref = tr.header()->getProperty( "source_referential" );
        string sref = ref->getString();
        if( !sref.empty() )
          result.header()->setProperty( "source_referential", sref );
      }
      catch( runtime_error & )
      {
      }
    }
  }
}


FormatReader<AffineTransformation3d>* TrmComposeFormatReader::clone() const
{
  return new TrmComposeFormatReader;
}


// ---


Transformation3d*
TrmChainFormatReader::createAndRead( rc_ptr<DataSourceInfo> dsi,
                                     const AllocatorContext & context,
                                     Object options )
{
  rc_ptr<TransformationChain3d> r( new TransformationChain3d );
  read( *r, dsi, context, options );
  TransformationChain3d *result = r.get();
  r.release();

  rc_ptr<Transformation3d> rsimple;
  {
    const_ref<Transformation3d> simple = result->simplify();
    delete result;
    rsimple.reset( const_cast<Transformation3d *>( simple.pointer() ) );
  }
  Transformation3d *sres = rsimple.get();
  rsimple.release(); // ref cont should be 0

  return sres;
}


Transformation3d* TrmChainFormatReader::create( carto::Object,
                                                const AllocatorContext &,
                                                carto::Object )
{
  return new TransformationChain3d;
}


void TrmChainFormatReader::read( Transformation3d & result,
                                 rc_ptr<DataSourceInfo> dsi,
                                 const AllocatorContext & /* context */,
                                 Object /* options */ )
{
  TransformationChain3d *affobj
    = dynamic_cast<TransformationChain3d *>( &result );
  if( !affobj )
    throw wrong_format_error( "Not an transformation chain" );

  int i, n = dsi->list().size( "trm_chain" );
  if( n == 0 )
    throw wrong_format_error( "Not an transformation chain" );

  for( i=0; i<n; ++i )
  {
    rc_ptr<DataSource> ds = dsi->list().dataSource( "trm_chain", i );
    rc_ptr<Transformation3d> tr;
    Reader<Transformation3d> r( ds->url() );
    tr.reset( r.read() );
    // compose
    affobj->push_front( tr );

#if 0
    if( i == 0 )  // last applied
    {
      try
      {
        Object ref = tr.header()->getProperty( "destination_referential" );
        string dref = ref->getString();
        if( !dref.empty() )
          affobj->header()->setProperty( "destination_referential", dref );
      }
      catch( runtime_error & )
      {
      }
    }
    if( i == n - 1 )  // first applied
    {
      try
      {
        Object ref = tr.header()->getProperty( "source_referential" );
        string sref = ref->getString();
        if( !sref.empty() )
          affobj->header()->setProperty( "source_referential", sref );
      }
      catch( runtime_error & )
      {
      }
    }
#endif
  }
}


FormatReader<Transformation3d>* TrmChainFormatReader::clone() const
{
  return new TrmChainFormatReader;
}

#undef localMsg


