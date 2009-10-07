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

#include <cartobase/datasource/filteringdatasource.h>
#include <stdio.h>

using namespace carto;
using namespace std;

FilteringDataSource::FilteringDataSource( rc_ptr<DataSource> in, int line_num )
  : ChainDataSource( in, in ? in->url() : string() ), 
    _filtercomments( true ), _countlines( true ), _line( line_num ), 
    _commenttag( "#" )
{
}


FilteringDataSource::~FilteringDataSource()
{
}


DataSource* FilteringDataSource::clone() const
{
  FilteringDataSource	*fds
    = new FilteringDataSource( source(), lineNumber() );
  fds->setFiltersComments( filtersComments() );
  fds->setCountsLines( countsLines() );
  return fds;
}


int FilteringDataSource::iterateMode() const
{
  return _source->iterateMode();
}


carto::offset_t FilteringDataSource::size() const
{
  return _source ? _source->size() : 0;
}


carto::offset_t FilteringDataSource::at() const
{
  return _source ? _source->at() : 0;
}


bool FilteringDataSource::at( offset_t pos )
{
  if( _source )
    return _source->at( pos );
  return false;
}


long FilteringDataSource::readBlock( char* data, unsigned long maxlen )
{
  unsigned long i;
  for( i=0; i<maxlen && *_source; ++i )
    *data++ = (char) getch();
  return (long) i;
}


long FilteringDataSource::writeBlock( const char* data, unsigned long len )
{
  return _source->writeBlock( data, len );
}


int FilteringDataSource::getch()
{
  int	c = _source->getch();
  if( c == EOF )
    return c;
  if( _filtercomments && c == _commenttag[0] )
    while( c != '\n' && c != EOF )
      c = _source->getch();
  if( _countlines && c == '\n' )
    ++_line;
  return c;
}


int FilteringDataSource::putch( int ch )
{
  return _source->putch( ch );
}


bool FilteringDataSource::ungetch( int ch )
{
  if( _countlines && ch == '\n' )
    --_line;
  // TODO: handle comment buffer
  return _source->ungetch( ch );
}


bool FilteringDataSource::allowsMemoryMapping() const
{
  return false;
}


bool FilteringDataSource::filtersComments() const
{
  return _filtercomments;
}


bool FilteringDataSource::countsLines() const
{
  return _countlines;
}


void FilteringDataSource::setFiltersComments( bool x )
{
  _filtercomments = x;
}


void FilteringDataSource::setCountsLines( bool x )
{
  _countlines = x;
}


int FilteringDataSource::lineNumber() const
{
  return _line;
}


void FilteringDataSource::setLineNumber( int x )
{
  _line = x;
}


string FilteringDataSource::commentTag() const
{
  return _commenttag;
}


void FilteringDataSource::setCommentTag( const string & x )
{
  _commenttag = x;
  if( x.empty() )
    _filtercomments = false;
}


