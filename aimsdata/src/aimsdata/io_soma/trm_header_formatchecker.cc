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

#include <aims/io_soma/trm_header_formatchecker.h>
#include <aims/transformation/affinetransformation3d.h>
#include <aims/io/finder.h>
#include <aims/data/pheader.h>
#include <cartobase/stream/fileutil.h>


using namespace aims;
using namespace soma;
using namespace std;

DataSourceInfo TrmHeaderFormatChecker::check( DataSourceInfo dsi,
                                              DataSourceInfoLoader &,
                                              carto::Object options )
                                              const
{
  // cout << "TrmHeaderFormatChecker::check\n";

  string url = FileUtil::uriFilename( dsi.list().dataSource()->url() );
  if( url.length() > 7 && url.substr( url.length() - 7, 7 ) == ".trmhdr" )
    url = url.substr( 0, url.length() - 7 );
  else
    throw wrong_format_error( "Not a transform-in-header",
                              dsi.list().dataSource()->url() );

  Finder f;
  if( !f.check( url ) )
    throw wrong_format_error( "Not a transform-in-header",
                              dsi.list().dataSource()->url() );

  const PythonHeader *ph = dynamic_cast<const PythonHeader *>( f.header() );
  if( !ph )
    throw wrong_format_error( "Not a transform-in-header",
                              dsi.list().dataSource()->url() );
  int trans_i = -1;

  Object trans = ph->getProperty( "transformations" );

  try
  {
    Object trans_n = options->getProperty( "index" );
    trans_i = int( trans_n->getScalar() );
    if( trans_i < 0 )
      trans_i = trans->size() + trans_i;
  }
  catch( runtime_error & e )
  {
  }

  if( trans_i >= int( trans->size() ) )
    throw runtime_error( "transformation index does not exist in header" );

  Object refs;

  if( trans_i < 0 )
  {
    string trans_t;

    try
    {
      Object trans_n = options->getProperty( "target" );
      trans_t = trans_n->getString();
      refs = ph->getProperty( "referentials" );
    }
    catch( runtime_error & e )
    {
      trans_i = 0;
    }
    if( trans_i < 0 && refs.get() )
    {
      Object it = refs->objectIterator();
      int i;

      for( i=0; it->isValid(); it->next(), ++i )
      {
        string tname = it->currentValue()->getString();
        if( tname == trans_t )
        {
          trans_i = i;
          break;
        }
      }
    }
  }

  if( trans_i < 0 )
    throw runtime_error( "transformation not found in header" );

  Object tr = trans->getArrayItem( trans_i );
  AffineTransformation3d t( tr );
  // cout << t << endl;

  Object hdr = Object::value( PropertySet() );  // header

  if( !refs )
    try
    {
      refs = ph->getProperty( "referentials" );
    }
    catch( runtime_error & )
    {
    }
  if( refs.get() )
  {
    Object ref = refs->getArrayItem( trans_i );
    hdr->setProperty( "destination_referential", ref->getString() );
  }

  try
  {
    Object sr = ph->getProperty( "referential" );
    hdr->setProperty( "source_referential", sr->getString() );
  }
  catch( runtime_error & )
  {
  }

  dsi.header() = hdr;

  try
  {
    Object oinv = options->getProperty( "inv" );
    bool inv = bool( oinv->getScalar() );
    if( inv )
    {
      t = *t.inverse();
      Object s;
      if( hdr->hasProperty( "source_referential" ) )
      {
        s = hdr->getProperty( "source_referential" );
        hdr->removeProperty( "source_referential" );
      }
      if( hdr->hasProperty( "destination_referential" ) )
      {
        hdr->setProperty( "source_referential",
                          hdr->getProperty( "destination_referential" ) );
        hdr->removeProperty( "destination_referential" );
      }
      if( s )
        hdr->setProperty( "destination_referential", s );
    }
  }
  catch( runtime_error & e )
  {
  }

  hdr->setProperty( "format", "TRMHEADER" );
  hdr->setProperty( "object_type", "AffineTransformation3d" );
  hdr->setProperty( "data_type", "VOID" );
  hdr->setProperty( "transformation", t.toVector() );

  bool docapa = !dsi.capabilities().isInit();
  if( docapa )
  {
    dsi.capabilities().setMemoryMapping( false );
//     dsi.capabilities().setDataSource( dsi.list().dataSource( "ima" ) );
    dsi.capabilities().setThreadSafe( true ); /* TODO */
    dsi.capabilities().setOrdered( true );
    dsi.capabilities().setSeekVoxel( false );
    dsi.capabilities().setSeekLine( false );
    dsi.capabilities().setSeekSlice( false );
    dsi.capabilities().setSeekVolume( false );
    dsi.capabilities().setRandomAccessEfficient( false );
    dsi.capabilities().setHandleStrides( false );
  }

  return dsi;
}

