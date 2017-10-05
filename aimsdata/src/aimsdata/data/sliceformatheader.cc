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

#include <aims/data/sliceformatheader.h>
#include <cartobase/stream/fileutil.h>
#include <cartobase/stream/directory.h>
#include <cartobase/stream/sstream.h>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <sys/types.h>
#include <regex.h>
#include <string.h>
#include <stdio.h>

using namespace aims;
using namespace carto;
using namespace std;


SliceFormatHeader::SliceFormatHeader( const std::string & filename, int dimx, 
                                      int dimy, int dimz, int dimt, float vsx,
                                      float vsy, float vsz, float vst )
  : PythonHeader(), _name(filename)
{
  vector<int>	dims(4);
  vector<float>	vs(4);
  dims[0] = dimx;
  dims[1] = dimy;
  dims[2] = dimz;
  dims[3] = dimt;
  vs[0] = vsx;
  vs[1] = vsy;
  vs[2] = vsz;
  vs[3] = vst;
  setProperty( "volume_dimension", dims );
  setProperty( "voxel_size", vs );
}


SliceFormatHeader::~SliceFormatHeader()
{
}


string SliceFormatHeader::filename() const
{
  return _name;
}


vector<string> SliceFormatHeader::inputFilenames()
{
  vector<string>	names;
  vector<Object>	names2;
  string		pattern;
  vector<int>		dims;
  bool			usepat = false;

  getProperty( "volume_dimension", dims );
  while( dims.size() < 4 )
    dims.push_back( 1 );

  unsigned	t, z, nt = (unsigned) dims[3], nz = (unsigned) dims[2];
  unsigned	smin = 0, tmin = 0;

  getProperty( "slice_mode", pattern );
  if( pattern == "auto" )
    {
      unsigned smax, tmax;
      pattern = inputNamepattern( smin, smax, tmin, tmax );
      dims[2] = smax - smin + 1;
      dims[3] = tmax - tmin + 1;
      usepat = true;
      removeProperty( "slice_mode" );
    }
  else if( getProperty( "filenames", names2 ) )
    {
      unsigned	i, cnt = names2.size();
      if( cnt != nz *nt )
        {
          cerr << "filenames count doesn't match volume dimensions. " 
               << "Truncating.\n";
          if( cnt < nz * nt )
            {
              nt = cnt / nz;
              if( cnt > 0 )
                {
                  nt = 1;
                  nz = cnt;
                }
            }
          dims[2] = (int) nz;
          dims[3] = (int) nt;
          cnt = nt * nz;
        }
      names.reserve( cnt );
      for( i=0; i<cnt; ++i )
        names.push_back( names2[i]->getString() );
    }
  else if( getProperty( "input_file_pattern", pattern ) )
    {
      usepat = true;
      removeProperty( "input_file_pattern" );
      int u = 0, v = 0;
      if( getProperty( "slice_min", u ) )
        {
          smin = u;
          removeProperty( "slice_min" );
        }
      if( getProperty( "tile_min", v ) )
        {
          tmin = v;
          removeProperty( "time_min" );
        }
    }
  else
    {
      names.push_back( FileUtil::basename( _name ) );
      dims[2] = 1;
      dims[3] = 1;
    }

  if( usepat )
    {
      enum Mode { Single, Slice, SliceTime };
      Mode	mode;
      char	*fname = new char[ pattern.length() + 1 ];
      string::size_type	p1 = pattern.find( '%' ), p2 = pattern.rfind( '%' );
      if( p1 == string::npos )
        mode = Single;
      else if( p1 == p2 )
        mode = Slice;
      else
        mode = SliceTime;

      names.reserve( nt * nz );
      for( t=0; t<nt; ++t )
        for( z=0; z<nz; ++z )
          {
            switch( mode )
              {
              case Single:
                strcpy( fname, pattern.c_str() );
                break;
              case SliceTime:
                sprintf( fname, pattern.c_str(), t + tmin, z + smin );
                break;
              case Slice:
                sprintf( fname, pattern.c_str(), z + smin );
              }
            names.push_back( fname );
          }
      delete[] fname;

      nz=names.size();
      names2.reserve( nz );
      for( z=0; z<nz; ++z )
        names2.push_back( Object::value( names[z] ) );
      setProperty( "filenames", names2 );
    }

  setProperty( "volume_dimension", dims );

  return names;
}


vector<string> 
SliceFormatHeader::outputFilenames() const
{
  vector<int>		dims;
  vector<string>	names;
  unsigned	z, t, nz = 1, nt = 1;
  if( !getProperty( "volume_dimension", dims ) )
    cerr << "warning: in SliceFormatHeader, volume_dimension attribute not " 
         << "found" << endl;
  if( dims.size() >= 3 )
    {
      nz = dims[2];
      if( dims.size() >= 4 )
        nt = dims[3];
      else
        {
          int	x = 1;
          if( getProperty( "sizeT", x ) )
            nt = x;
        }
    }
  else
    {
      int	x = 1;
      if( getProperty( "sizeZ", x ) )
        nz = x;
    }

  string	dir = FileUtil::dirname( _name );
  if( !dir.empty() )
    dir += FileUtil::separator();
  string	fp = removeExtension( FileUtil::basename( _name ) );
  string	ext = extension();
  if( !ext.empty() && ext[0] != '.' )
    ext = string( "." ) + ext;

  if( nt > 1 )
    {
      fp += "_t";
      for( t=0; t<nt; ++t )
        for( z=0; z<nz; ++z )
          {
            stringstream	ss;
            ss << fp << setfill( '0' ) << setw(4) << t << "_s" << z << ext;
            names.push_back( ss.str() );
          }
    }
  else
  {
    if( nz > 1 )
    {
      fp += "_";
      for( z=0; z<nz; ++z )
        {
          stringstream	ss;
          ss << fp << setfill( '0' ) << setw(4) << z << ext;
          names.push_back( ss.str() );
        }
    }
    else
      names.push_back( fp + ext );
  }

  return names;
}


string 
SliceFormatHeader::inputNamepattern( unsigned & smin, unsigned & smax, 
                                     unsigned & tmin, unsigned & tmax ) const
{
  string	name = removeExtension( _name ), regname;
  string	dirname = FileUtil::dirname( name );
  name = FileUtil::basename( name );
  char	sep = FileUtil::separator();

  //cout << "scan dir " << dirname << endl;

  smin = smax = tmin = tmax = 0;

  // find slices / times numbers pattern in filename string
  regex_t	regst;
  regex_t	regt;
  regcomp( &regst, "^(.*)t([0-9]+)_s([0-9]+)$", 
	   REG_EXTENDED | REG_ICASE );
  regcomp( &regt, "^(.*[^0-9])([0-9]+)$", REG_EXTENDED );
  regmatch_t	rmatch[4];
  enum numtype { None, Slice, SliceTime };
  numtype	nt = None;

  if( !regexec( &regst, name.c_str(), 4, rmatch, 0 ) )
    {
      /*cout << "regex match sl + time\n";
      cout << "match 0 : " << rmatch[0].rm_so << "-"<<rmatch[0].rm_eo<<endl;
      cout << "match 1 : " << rmatch[1].rm_so << "-"<<rmatch[1].rm_eo<<endl;
      cout << "match 2 : " << rmatch[2].rm_so << "-"<<rmatch[2].rm_eo<<endl;
      cout << "match 3 : " << rmatch[3].rm_so << "-"<<rmatch[3].rm_eo<<endl;*/
      unsigned	lens = rmatch[2].rm_eo - rmatch[2].rm_so;
      unsigned	lent = rmatch[3].rm_eo - rmatch[3].rm_so;
      char		num1[10], num2[10];
      sprintf( num1, "%d", lens );
      sprintf( num2, "%d", lent );
      regname = string( "^" ) + name.substr( 0, rmatch[1].rm_eo + 1 ) 
        + "([0-9]+)_s([0-9]+)\\.jpg$";
      name = dirname + sep + name.substr( 0, rmatch[1].rm_eo + 1 ) + "%0" 
        + num1 + "u_s%0" + num2 + "u.jpg";
      nt = SliceTime;
    }
  else if( !regexec( &regt, name.c_str(), 3, rmatch, 0 ) )
    {
      /*cout << "regex match slice\n";
      cout << "match 0 : " << rmatch[0].rm_so << "-"<<rmatch[0].rm_eo<<endl;
      cout << "match 1 : " << rmatch[1].rm_so << "-"<<rmatch[1].rm_eo<<endl;
      cout << "match 2 : " << rmatch[2].rm_so << "-"<<rmatch[2].rm_eo<<endl;*/
      unsigned	lent = rmatch[2].rm_eo - rmatch[2].rm_so;
      char		num[10];
      sprintf( num, "%u", lent );
      regname = string( "^" ) + name.substr( 0, rmatch[1].rm_eo ) 
        + "([0-9]+)\\.jpg$";
      name = dirname + sep + name.substr( 0, rmatch[1].rm_eo ) + "%0" 
        + num + "u.jpg";
      nt = Slice;
    }
  else
    name += FileUtil::extension( _name );
  //cout << "name : " << name << endl;

  regfree( &regst );
  regfree( &regt );

  if( nt == None )
    return( dirname + sep + name );

  // scan directory for matching files

  Directory	dir( dirname );
  set<string>	files = dir.files();
  set<string>::iterator	is, es = files.end();

  std::string	num;
  unsigned	n;
  bool	hast = false, hass = false;

  //cout << "regex fiter : " << regname << endl;
  regcomp( &regst, regname.c_str(), REG_EXTENDED | REG_ICASE );

  for( is=files.begin(); is!=es; ++is )
    {
      if( !regexec( &regst, is->c_str(), 3, rmatch, 0 ) )
        {
          //cout << "match\n";
          switch( nt )
            {
            case SliceTime:
              num = is->substr( rmatch[1].rm_so, rmatch[1].rm_eo );
              sscanf( num.c_str(), "%u", &n );
              if( n < tmin || !hast )
                tmin = n;
              if( n > tmax || !hast )
                tmax = n;
              if( !hast )
                hast = true;

              num = is->substr( rmatch[2].rm_so, rmatch[2].rm_eo );
              sscanf( num.c_str(), "%u", &n );
              if( n < smin || !hass )
                smin = n;
              if( n > smax || !hass )
                smax = n;
              if( !hass )
                hass = true;
              break;

            default:
              num = is->substr( rmatch[1].rm_so, rmatch[1].rm_eo );
              sscanf( num.c_str(), "%u", &n );
              //cout << "num: " << n << endl;
              if( n < smin || !hass )
                smin = n;
              if( n > smax || !hass )
                smax = n;
              if( !hass )
                hass = true;
            }
        }
    }

  /* cout << "sl : " << smin << " - " << smax << endl;
  cout << "t  : " << tmin << " - " << tmax << endl;
  cout << "pattern : " << name << endl; */
  regfree( &regst );
  return name;
}


