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
#ifndef USE_SOMA_IO

#include <cartodata/io/gisformatchecker.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/smart/scopedptr.h>
#include <cartobase/object/object.h>
#include <cartobase/object/property.h>
#ifdef USE_SOMA_IO
  #include <soma-io/datasource/filedatasource.h>
  #include <soma-io/reader/itemreader.h>
  #include <soma-io/datasourceinfo/datasourceinfo.h>
  #include <soma-io/utilities/asciidatasourcetraits.h>
#else
  #include <cartobase/datasource/filedatasource.h>
  #include <cartobase/io/itemreader.h>
  #include <cartobase/io/datasourceinfo.h>
  #include <cartobase/io/asciidatasourcetraits.h>
#endif
#include <stdio.h>

using namespace carto;
using namespace std;

namespace
{
  bool initMe()
  {
    vector<string>	exts;
    exts.push_back( "ima" );
    exts.push_back( "dim" );
    DataSourceInfo::registerFormat( "GIS", new GisFormatChecker, exts );

    return true;
  }

  bool	_dummy = initMe();


  //	helper for reading attributes
  template<typename T> 
  static void inHelper( Object & object, const string & semantic, 
                        DataSource & ds )
  {
    if( !ds.isOpen() )
      return;

    T i = 0;
    ds >> i;
    object->setProperty( semantic, i );
  }


  template<typename T> 
  static void inVecHelper( Object & object, const string & semantic, 
                           DataSource & ds )
  {
    vector<T> iv;
    while (1)
      {
        if( !ds.isOpen() )
          return;

        int c = ds.getch();
        while (c != '\n' && c != '\0' && c != EOF 
               && (c == ' ' || c == '\t'))
          c = ds.getch();
        if (c == '\n' || c == '\0' || c == EOF)
          break;
        else
          ds.ungetch( c );
        T i;
        ds >> i;
        iv.push_back(i);
      }
    object->setProperty( semantic, iv );
  }

}


GisFormatChecker::~GisFormatChecker()
{
}


Object GisFormatChecker::check( DataSource & ds, DataSourceInfo & ) const
{
  //cout << "GisFormatChecker::check\n";

  FileDataSource		*fds = dynamic_cast<FileDataSource *>( &ds );
  DataSource			*hds = &ds;
  scoped_ptr<DataSource>	realds;
  string			fname, imaname;

  fname = ds.url();
  if( !fname.empty() )
    {
      string	ext = FileUtil::extension( fname );
      imaname = fname;
      if( ext == "ima" )
        {
          fname = FileUtil::removeExtension( fname ) + ".dim";
        }
      else if( ext == "dim" )
        {
          imaname = FileUtil::removeExtension( fname ) + ".ima";
        }
      else
        ext.clear();
      if( ext.empty() )
        {
          fname += ".dim";
          imaname += ".ima";
        }
      if( fname != ds.url() )
        {
          if( FileUtil::fileStat( fname ).find( '+' ) != string::npos )
            {
              realds.reset( new FileDataSource( fname ) );
              hds = realds.get();
              fds = static_cast<FileDataSource *>( hds );
            }
          else
            fname = ds.url();
        }
    }

  if( !hds->isOpen() )
    hds->open( DataSource::Read );
  if( !hds->isOpen() )
    io_error::launchErrnoExcept( fname );

  // read header

  DefaultAsciiItemReader<int>		iir;
  DefaultAsciiItemReader<string>	sir;
  DefaultAsciiItemReader<float>		fir;

  string	token, type, byteord, binar;
  int		sizex = 1, sizey = 1, sizez = 1, sizet = 1;
  Object	hdr = Object::value( PropertySet() );
  int		c;
  vector<float>	vs(4, 1.);

  if( !StreamUtil::skip( *hds, " \t\n\r" ) )
    throw wrong_format_error( fname );
  if( iir.read( *hds, &sizex ) != 1 )
    throw wrong_format_error( fname );
  if( !StreamUtil::skip( *hds, " \t" ) )
    throw wrong_format_error( fname );
  c = hds->getch();
  hds->ungetch( c );
  if( c != '\n' && c != '\r' )
    {
      if( iir.read( *hds, &sizey ) != 1 )
        throw wrong_format_error( fname );
      if( !StreamUtil::skip( *hds, " \t" ) )
        throw wrong_format_error( fname );
      c = hds->getch();
      hds->ungetch( c );
      if( c != '\n' && c != '\r' )
        {
          if( iir.read( *hds, &sizez ) != 1 )
            throw wrong_format_error( fname );
          if( !StreamUtil::skip( *hds, " \t" ) )
            throw wrong_format_error( fname );
          c = hds->getch();
          hds->ungetch( c );
          if( c != '\n' && c != '\r' )
            {
              if( iir.read( *hds, &sizet ) != 1 )
                throw wrong_format_error( fname );
            }
        }
    }

  /* cout << "size: " << sizex << ", " << sizey << ", " << sizez << ", " 
     << sizet << endl; */

  const Syntax	&sx = DataSourceInfo::minfSyntax()[ "__generic__" ];

  while( hds->isOpen() )
    {
      if( !StreamUtil::skip( *hds, " \t\n\r" ) )
        break;
      c = hds->getch();
      if( c != '-' )
        {
          hds->ungetch( c );
          break;
        }
      if( sir.read( *hds, &token ) != 1 )
      {
        if( token.empty() )
          break;
        else
          throw wrong_format_error( fname );
      }
      if( !StreamUtil::skip( *hds, " \t" ) )
        throw wrong_format_error( fname );

      if( token == "type" )
        sir.read( *hds, &type );
      else if (token == "dx" )
        fir.read( *hds, &vs[0] );
      else if (token == "dy" )
        fir.read( *hds, &vs[1] );
      else if (token == "dz" )
        fir.read( *hds, &vs[2] );
      else if (token == "dt" )
        fir.read( *hds, &vs[3] );
      else if (token == "bo" )
        sir.read( *hds, &byteord );
      else if (token == "om" )
        sir.read( *hds, &binar );
      else if( !token.empty() )
        {
          Syntax::const_iterator	ix = sx.find( token );
          if( ix == sx.end() )
            {
              string	attval;
              StreamUtil::getline( *hds, attval );
              hdr->setProperty( token, attval );
            }
          else
            {
              typedef void (*Helper)( Object &, const string &, 
                                      DataSource & );
              typedef map<string, Helper> HelperSet;

              static HelperSet			hs;
              if( hs.empty() )
                {
                  hs[ "int" ] = &inHelper<int>;
                  hs[ "float" ] = &inHelper<float>;
                  hs[ "bool" ] = &inHelper<bool>;
                  hs[ "string" ] = &inHelper<string>;
                  hs[ "int_vector" ] = &inVecHelper<int>;
                  hs[ "float_vector" ] = &inVecHelper<float>;
                  //hs[ "string_vector" ] = &inVecHelper<string>;
                }

              HelperSet::const_iterator	ih = hs.find( ix->second.type );
              if( ih == hs.end() )
                {
                  string	attval;
                  StreamUtil::getline( *hds, attval );
                  hdr->setProperty( token, attval );
                }
              else
                ih->second( hdr, token, *hds );
            }
        }
      else
        break;
    }

  if( type.empty() )
    throw format_mismatch_error( "Not a GIS header", fname );

  if( fds && fds->at() != fds->size() - 1 )
    throw format_mismatch_error( "GIS header not entirely read - garbage at " 
                                 "end ?", fname );

  /*
  vector<int>	sz(4);
  sz[0] = sizex;
  sz[1] = sizey;
  sz[2] = sizez;
  sz[3] = sizet;
  hdr->setProperty( "volume_dimension", sz );
  */
  hdr->setProperty( "sizeX", sizex );
  hdr->setProperty( "sizeY", sizey );
  hdr->setProperty( "sizeZ", sizez );
  hdr->setProperty( "sizeT", sizet );
  hdr->setProperty( "format", string( "GIS" ) );
  hdr->setProperty( "voxel_size", vs );
  //if( !type.empty() )
  // hdr->setProperty( "data_type", type );
  bool	ascii = ( binar == "ascii" );
  hdr->setProperty( "ascii", (int) ascii );
  if( !ascii )
    if( byteord.length() >= 4 )
      {
        uint32_t	bo = *( (uint32_t*) byteord.c_str() );
        hdr->setProperty( "byte_swapping", (int) ( bo != 0x41424344 ) );
      }

  // add meta-info to header
  if( !imaname.empty() )
    {
      FileDataSource	minfds( imaname + ".minf" );
      DataSourceInfo::readMinf( minfds, hdr );
    }
  else
    DataSourceInfo::readMinf( *hds, hdr );
  hdr->setProperty( "object_type", string( "Volume of " ) + type );

  return hdr;
}


#endif // USE_SOMA_IO
