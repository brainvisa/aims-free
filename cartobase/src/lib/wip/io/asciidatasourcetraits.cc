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

#include <cartobase/wip/io/asciidatasourcetraits.h>
#include <stdio.h>

using namespace carto;
using namespace std;


string StreamUtil::readWhile( DataSource & ds, const string & s, 
                              bool ascii )
{
  string	res;

  if( !ascii )
    return res;

  unsigned	n = 0;
  if( !ds.isOpen() )
    return res;

  int c = ds.getch();

  ++n;
  while( c != EOF && s.find_first_of(c) != string::npos )
    {
      res += c;
      c = ds.getch();
      ++n;
    }
  if( c != EOF )
    {
      ds.ungetch( c );
      --n;
    }
  return res;
}


string StreamUtil::readUntil( DataSource & ds, const string & s, 
                              bool ascii )
{
  string	res;

  if( !ascii )
    return res;

  unsigned	n = 0;
  if( !ds.isOpen() )
    return res;

  int c = ds.getch();

  ++n;
  while( c != EOF && s.find_first_of(c) == string::npos )
    {
      res += c;
      c = ds.getch();
      ++n;
    }
  if( c != EOF )
    {
      ds.ungetch( c );
      --n;
    }
  return res;
}


bool StreamUtil::skip( DataSource & ds, const string & s, bool ascii )
{
  if( !ascii )
    return true;

  unsigned	n = 0;
  if( !ds.isOpen() )
    return false;

  int c = ds.getch();

  ++n;
  while( c != EOF && s.find_first_of(c) != string::npos )
    {
      c = ds.getch();
      ++n;
    }
  if( c != EOF )
    {
      ds.ungetch( c );
      --n;
    }
  return ds.isOpen();
}


bool StreamUtil::skipUntil( DataSource & ds, const string & s, 
                            bool ascii )
{
  if( !ascii )
    return true;

  unsigned	n = 0;
  if( !ds.isOpen() )
    return false;

  int c = ds.getch();

  ++n;
  while( c != EOF && s.find_first_of(c) == string::npos )
    {
      c = ds.getch();
      ++n;
    }
  if( c != EOF )
    {
      ds.ungetch( c );
      --n;
    }
  return ds.isOpen();
}


bool StreamUtil::getline( DataSource & ds, string & x )
{
  x.clear();
  int c = ds.getch();
  while( ds.isOpen() && c != '\n' && c != '\r' )
    {
      x += (char) c;
      c = ds.getch();
    }
  return ds.isOpen();
}


bool StreamUtil::skip( istream & is, const string & s, bool ascii )
{
  if( !ascii )
    return true;

  unsigned	n = 0;
  if( is.eof() )
    return false;

  int c = is.get();
  ++n;
  while( c != EOF && s.find_first_of(c) != std::string::npos )
    {
      c = is.get();
      ++n;
    }
  if( c != EOF )
    {
      is.unget();
      --n;
    }
  return !!is;
}


