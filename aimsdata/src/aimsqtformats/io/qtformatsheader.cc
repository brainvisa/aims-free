/* Copyright (c) 1995-2005 CEA
 *
 *  This software and supporting documentation were developed by
 *      CEA/DSV/SHFJ
 *      4 place du General Leclerc
 *      91401 Orsay cedex
 *      France
 *
 * This software is governed by the CeCILL license version 2 under 
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the 
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#include <aims/io/qtformatsheader.h>
#include <aims/rgb/rgb.h>
#include <aims/io/datatypecode.h>
#include <cartobase/exception/ioexcept.h>
#include <cartobase/stream/directory.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/type/string_conversion.h>
#include <qimage.h>
#include <regex.h>
#if QT_VERSION >= 0x040000
#include <QString>
#include <QImageReader>
#include <QImageWriter>
typedef QImageReader QImageIO;
#endif

using namespace aims;
using namespace carto;
using namespace std;


struct QtFormatsHeader::Private
{
  Private() : hasread( false ) {}
  ~Private()
  {
  }

  void clear()
  {
    qimage = QImage();
    hasread = false;
  }

  QImage	qimage;
  bool		hasread;
};


QtFormatsHeader::QtFormatsHeader( const string & name, const string & type,
                                  int x, int y, int z, int t,
                                  float sx, float sy, float sz, float st ) :
  SliceFormatHeader( name, x, y, z, t, sx, sy, sz, st ), 
  _type(type), _dimX(x), _dimY(y), _dimZ(z), _dimT(t), 
  _sizeX(sx), _sizeY(sy), _sizeZ(sz), _sizeT(st), 
  d( new QtFormatsHeader::Private )
{
}


QtFormatsHeader::~QtFormatsHeader()
{
  delete d;
}


string QtFormatsHeader::extension() const
{
  string	bname = FileUtil::basename( _name );
  string::size_type	pos = bname.rfind( '.' );
  if( pos == string::npos )
    return "";
  return bname.substr( pos + 1, bname.length() - pos - 1 );
}


set<string> QtFormatsHeader::extensions() const
{
  static set<string>	exts;
  if( exts.empty() )
    {
#if QT_VERSION >= 0x040000
      QList<QByteArray>	iform = QImageReader::supportedImageFormats();
      QList<QByteArray>	oform = QImageWriter::supportedImageFormats();
      QList<QByteArray>::iterator	c, ce = iform.end();

      for( c=iform.begin(); c!=ce; ++c )
        exts.insert( QString( *c ).upper().utf8().data() );
      for( c=oform.begin(),ce=oform.end(); c!=ce; ++c )
        exts.insert( QString( *c ).upper().utf8().data() );

#else
      QStrList	iform = QImageIO::inputFormats();
      QStrList	oform = QImageIO::outputFormats();
      char	*c;

      for( c=iform.first(); c; c=iform.next() )
        exts.insert( QString( c ).upper().utf8().data() );
      for( c=oform.first(); c; c=oform.next() )
        exts.insert( QString( c ).upper().utf8().data() );
#endif
    }

  return exts;
}


void QtFormatsHeader::reset()
{
  d->clear();
}


void QtFormatsHeader::read()
{
  // don't do things twice
  if( d->hasread )
    return;

  string	fname = _name;
  const char	*fmt = QImageIO::imageFormat( fname.c_str() );
  if( !fmt )
    {
      set<string>		exts = extensions();
      set<string>::iterator	ie, ee = exts.end();

      for( ie=exts.begin(); ie!=ee && !fmt; ++ie )
        {
          fname = removeExtension( _name ) + '.' + *ie;
          fmt = QImageIO::imageFormat( fname.c_str() );
        }
    }
  if( !fmt )
    throw format_error( _name );

#if QT_VERSION >= 0x040000

  QImageReader	qio( fname.c_str(), fmt );
  d->qimage = qio.read();

  if( d->qimage.isNull() )
    {
      d->clear();
      throw format_mismatch_error( fname );
    }

#else // Qt 3

  QImageIO	qio( fname.c_str(), fmt );
  bool	ok = qio.read();

  if( !ok )
    {
      d->clear();
      throw format_mismatch_error( fname );
    }

  d->qimage = qio.image();

#endif

  const QImage	& im = d->qimage;

  _dimX = im.width();
  _dimY = im.height();
  _dimZ = 1;
  _dimT = 1;
  if( im.dotsPerMeterX() != 0 )
    _sizeX = 1000. / im.dotsPerMeterX();
  else
    _sizeX = 1;
  if( im.dotsPerMeterY() != 0 )
    _sizeY = 1000. / im.dotsPerMeterY();
  else
    _sizeY = 1;
  _sizeZ = 1;
  _sizeT = 1;

  string		dt;
  vector<string>	pdt;

  bool gray = im.allGray();
  if( gray )
    {
      //cout << "gray image, numCol: " << im.numColors() << endl;
      if( im.numColors() == 0 )
        switch( im.depth() )
          {
          case 1:
          case 8:
            dt = DataTypeCode<uint8_t>().dataType();
            pdt.push_back( DataTypeCode<uint8_t>().dataType() );
            pdt.push_back( DataTypeCode<uint16_t>().dataType() );
            pdt.push_back( DataTypeCode<int16_t>().dataType() );
            pdt.push_back( DataTypeCode<uint32_t>().dataType() );
            pdt.push_back( DataTypeCode<int32_t>().dataType() );
            break;
          case 16:
            dt = DataTypeCode<uint16_t>().dataType();
            pdt.push_back( DataTypeCode<uint16_t>().dataType() );
            pdt.push_back( DataTypeCode<int16_t>().dataType() );
            pdt.push_back( DataTypeCode<uint32_t>().dataType() );
            pdt.push_back( DataTypeCode<int32_t>().dataType() );
            pdt.push_back( DataTypeCode<uint8_t>().dataType() );
            break;
          case 32:
            dt = DataTypeCode<uint32_t>().dataType();
            pdt.push_back( DataTypeCode<uint32_t>().dataType() );
            pdt.push_back( DataTypeCode<int32_t>().dataType() );
            pdt.push_back( DataTypeCode<uint16_t>().dataType() );
            pdt.push_back( DataTypeCode<int16_t>().dataType() );
            pdt.push_back( DataTypeCode<uint8_t>().dataType() );
            break;
          default:
            break;
          }
      else
        {
          if( im.numColors() <= 256 )
            {
              dt = DataTypeCode<byte>().dataType();
              pdt.push_back( DataTypeCode<uint8_t>().dataType() );
            }
          if( im.numColors() <= 65536 )
            {
              if( dt.empty() )
                dt = DataTypeCode<uint16_t>().dataType();
              pdt.push_back( DataTypeCode<uint16_t>().dataType() );
              pdt.push_back( DataTypeCode<int16_t>().dataType() );
            }
          if( dt.empty() )
            dt = DataTypeCode<uint32_t>().dataType();
          pdt.push_back( DataTypeCode<uint32_t>().dataType() );
          pdt.push_back( DataTypeCode<int32_t>().dataType() );
        }
    }
  if( im.hasAlphaBuffer() )
    {
      if( dt.empty() )
        dt = DataTypeCode<AimsRGBA>().dataType();
      pdt.push_back( DataTypeCode<AimsRGBA>().dataType() );
      pdt.push_back( DataTypeCode<AimsRGB>().dataType() );
    }
  else
    {
      if( dt.empty() )
        dt = DataTypeCode<AimsRGB>().dataType();
      pdt.push_back( DataTypeCode<AimsRGB>().dataType() );
      pdt.push_back( DataTypeCode<AimsRGBA>().dataType() );
    }
  if( !gray )
    {
      pdt.push_back( DataTypeCode<uint32_t>().dataType() );
      pdt.push_back( DataTypeCode<int32_t>().dataType() );
      pdt.push_back( DataTypeCode<uint16_t>().dataType() );
      pdt.push_back( DataTypeCode<int16_t>().dataType() );
      pdt.push_back( DataTypeCode<uint8_t>().dataType() );
    }

  if( dt.empty() && !pdt.empty() )
    dt = pdt[0];
  if( !dt.empty() )
    setProperty( "data_type", dt );
  _type = dt;
  if( !pdt.empty() )
    setProperty( "possible_data_types", pdt );

  setProperty( "object_type", string( "Volume" ) );
  setProperty( "file_type", string( fmt ) );

  d->hasread = true;

  vector<int>	dims( 4 );
  dims[0] = _dimX;
  dims[1] = _dimY;
  dims[2] = _dimZ;
  dims[3] = _dimT;
  setProperty( "volume_dimension", dims );
  vector<float>	vs( 4 );
  vs[0] = _sizeX;
  vs[1] = _sizeY;
  vs[2] = _sizeZ;
  vs[3] = _sizeT;
  setProperty( "voxel_size", vs );

  // add meta-info to header
  readMinf( fname + ".minf" );

  inputFilenames();

  // if .minf has been modified
  if( getProperty( "volume_dimension", dims ) )
    if( dims.size() >= 1 )
      {
        _dimX = dims[0];
        if( dims.size() >= 2 )
          {
            _dimY = dims[1];
            if( dims.size() >= 3 )
              {
                _dimZ = dims[2];
                if( dims.size() >= 4 )
                  {
                    _dimT = dims[3];
                  }
              }
          }
      }

  getProperty( "voxel_size", vs );
  if( vs.size() >= 3 )
    {
      _sizeX = vs[0];
      _sizeY = vs[1];
      _sizeZ = vs[2];
      if( vs.size() >= 4 )
        _sizeT = vs[3];
    }
}


vector<string> QtFormatsHeader::possibleDataTypes() const
{
  vector<string>	pt;
  getProperty( "possible_data_types", pt );
  return( pt );
}


void QtFormatsHeader::setType( const string & t )
{
  _type = t;
  setProperty( "data_type", t );
}


string QtFormatsHeader::removeExtension( const string & name ) const
{
  string	ext = extension();
  string	ext2 = stringUpper( ext );
  if( ext2 == "JPG" )
    ext2 = "JPEG";
  set<string>	exs = extensions();
  if( exs.find( ext2 ) == exs.end() )
    return name;
  return name.substr( 0, name.length() - ext.length() - 1 );
}


namespace
{

  string _escape( string name )
  {
    // escape some chars in filename
    unsigned	ist, nst = name.length();
    string	toescape = "\\+()[].*?";
    for( ist=0; ist<nst; ++ist )
      if( toescape.find( name[ist] ) != string::npos )
        {
          name.insert( name.begin() + ist, '\\' );
          ++ist;
          ++nst;
        }
    return name;
  }

}


bool QtFormatsHeader::hasRead() const
{
  return d->hasread;
}


QImage & QtFormatsHeader::qimage()
{
  return d->qimage;
}

